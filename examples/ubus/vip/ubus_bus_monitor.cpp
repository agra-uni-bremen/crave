//----------------------------------------------------------------------
//   Copyright (c) 2019-2020 University of Bremen, Germany. 
//   Copyright (c) 2019-2020 DFKI GmbH Bremen, Germany.
//   Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//   Copyright 2016-2019 NXP B.V
//   Copyright 2007-2010 Mentor Graphics Corporation
//   Copyright 2007-2011 Cadence Design Systems, Inc.
//   Copyright 2010 Synopsys, Inc.
//   All Rights Reserved Worldwide
//
//   Licensed under the Apache License, Version 2.0 (the
//   "License"); you may not use this file except in
//   compliance with the License.  You may obtain a copy of
//   the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in
//   writing, software distributed under the License is
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
//   CONDITIONS OF ANY KIND, either express or implied.  See
//   the License for the specific language governing
//   permissions and limitaions under the License.
//----------------------------------------------------------------------

#include <systemc>
#include <uvm>

#include "ubus_bus_monitor.h"
#include "slave_address_map_info.h"
#include "ubus_transfer.h"

using namespace sc_core;
using namespace uvm;
using namespace std;

//----------------------------------------------------------------------
// Class implementation  ubus_bus_monitor
//----------------------------------------------------------------------

ubus_bus_monitor::ubus_bus_monitor(uvm_component_name name)
: uvm_monitor(name)
{
  num_transactions = 0;

  /* TODO
    cov_trans = new();
    cov_trans.set_inst_name({get_full_name(), ".cov_trans"});
    cov_trans_beat = new();
    cov_trans_beat.set_inst_name({get_full_name(), ".cov_trans_beat"});
    trans_collected = new();
    item_collected_port = new("item_collected_port", this);
    state_port = new("state_port", this);
    status = new("status");
   */
}

//----------------------------------------------------------------------
// member function: set_slave_configs
//----------------------------------------------------------------------

void ubus_bus_monitor::set_slave_configs(string slave_name, unsigned int min_addr, unsigned int max_addr)
{
  slave_addr_map[slave_name] = new slave_address_map_info();
  slave_addr_map[slave_name]->set_address_map(min_addr, max_addr);
}

//----------------------------------------------------------------------
// member function: build_phase
//----------------------------------------------------------------------

void ubus_bus_monitor::build_phase(uvm_phase& phase)
{
  if(!uvm_config_db<ubus_if*>::get(this, "", "vif", vif))
    UVM_FATAL("NOVIF", "virtual interface must be set for: "
        + get_full_name() +".vif");
}

//----------------------------------------------------------------------
// member function: run_phase
//----------------------------------------------------------------------

void ubus_bus_monitor::run_phase(uvm_phase& phase)
{
  SC_FORK
    sc_spawn(sc_bind(&ubus_bus_monitor::observe_reset, this)),
    sc_spawn(sc_bind(&ubus_bus_monitor::collect_transactions, this))
  SC_JOIN
}

//----------------------------------------------------------------------
// member function: run_phase
//----------------------------------------------------------------------

void ubus_bus_monitor::observe_reset()
{
  SC_FORK
    sc_spawn(sc_bind(&ubus_bus_monitor::check_reset_on_posedge, this)),
    sc_spawn(sc_bind(&ubus_bus_monitor::check_reset_on_negedge, this))
  SC_JOIN
}

//----------------------------------------------------------------------
// member function: check_reset_on_posedge
//----------------------------------------------------------------------

void ubus_bus_monitor::check_reset_on_posedge()
{
  while(true) // forever
  {
    sc_core::wait(vif->sig_reset.posedge_event());
    status.bus_state = RST_START;
    state_port.write(status);
  }
}

//----------------------------------------------------------------------
// member function: check_reset_on_posedge
//----------------------------------------------------------------------

void ubus_bus_monitor::check_reset_on_negedge()
{
  while(true) // forever
  {
    sc_core::wait(vif->sig_reset.negedge_event());
    status.bus_state = RST_STOP;
    state_port.write(status);
  }
}

//----------------------------------------------------------------------
// member function: collect_transactions (protected)
//----------------------------------------------------------------------

void ubus_bus_monitor::collect_transactions()
{
  while(true) // forever
  {
    collect_arbitration_phase();
    collect_address_phase();
    collect_data_phase();

    UVM_INFO(get_type_name(), "Transfer collected :\n" +
      trans_collected.sprint(), uvm::UVM_HIGH);

    if (checks_enable) perform_transfer_checks();
    if (coverage_enable) perform_transfer_coverage();

    item_collected_port.write(trans_collected);
  }
}

//----------------------------------------------------------------------
// member function: collect_arbitration_phase (protected)
//----------------------------------------------------------------------

void ubus_bus_monitor::collect_arbitration_phase()
{
  // @(posedge vif->sig_clock iff (vif->sig_grant != 0));
  do
  {
    sc_core::wait(vif->sig_clock.posedge_event());
  }
  while(!((vif->sig_grant[0] != sc_dt::SC_LOGIC_0) || (vif->sig_grant[1] != sc_dt::SC_LOGIC_0)));

  status.bus_state = ARBI;
  state_port.write(status);

  this->begin_tr(trans_collected);

  // Check which grant is asserted to determine which master is performing
  // the transfer on the bus.
  for (int j = 0; j < 2; j++) // note: only 2 grant signals connected
  {
    if (vif->sig_grant[j] == sc_dt::SC_LOGIC_1)
    {
      std::ostringstream tmpstr;
      tmpstr << "masters[" << j << "]";
      trans_collected.master = tmpstr.str();

      break;
    }
  }
}

//----------------------------------------------------------------------
// member function: collect_address_phase (protected)
//----------------------------------------------------------------------

void ubus_bus_monitor::collect_address_phase()
{
  sc_core::wait(vif->sig_clock.posedge_event());

  sc_dt::sc_uint<16> trans_collected_addr( vif->sig_addr);
  trans_collected.addr = trans_collected_addr;

  switch (vif->sig_size.read().to_uint())
  {
    case 0 /* 0b00 */ : trans_collected.size = 1; break;
    case 1 /* 0b01 */ : trans_collected.size = 2; break;
    case 2 /* 0b10 */ : trans_collected.size = 4; break;
    case 3 /* 0b11 */ : trans_collected.size = 8; break;
    default: break;
  }

  sc_dt::sc_logic read_state = vif->sig_read.read();
  sc_dt::sc_logic write_state = vif->sig_write.read();

  if( read_state==sc_dt::SC_LOGIC_0 && write_state==sc_dt::SC_LOGIC_0)
  {
    trans_collected.read_write = ubus_read_write_enum::NOP;
    status.bus_state = NO_OP;
    state_port.write(status);
  }

  if( read_state==sc_dt::SC_LOGIC_1 && write_state==sc_dt::SC_LOGIC_0)
  {
    trans_collected.read_write = ubus_read_write_enum::READ;
    status.bus_state = ADDR_PH;
    state_port.write(status);
  }

  if( read_state==sc_dt::SC_LOGIC_0 && write_state==sc_dt::SC_LOGIC_1)
  {
    trans_collected.read_write = ubus_read_write_enum::WRITE;
    status.bus_state = ADDR_PH;
    state_port.write(status);
  }

  if( read_state==sc_dt::SC_LOGIC_1 && write_state==sc_dt::SC_LOGIC_1)
  {
    status.bus_state = ADDR_PH_ERROR;
    state_port.write(status);
    if (checks_enable)
      UVM_ERROR(get_type_name(),
          "Read and Write true at the same time");
  }
}

//----------------------------------------------------------------------
// member function: collect_data_phase (protected)
//----------------------------------------------------------------------

void ubus_bus_monitor::collect_data_phase()
{
  if (trans_collected.read_write != ubus_read_write_enum::NOP)
  {
    check_which_slave();

    for (unsigned int i = 0; i < trans_collected.size; i++)
    {
      status.bus_state = DATA_PH;
      state_port.write(status);

      // TODO check: @(posedge vif->sig_clock iff vif->sig_wait === 0);
      do
      {
        sc_core::wait(vif->sig_clock.posedge_event());
      }
      while (!(vif->sig_wait.read() == sc_dt::SC_LOGIC_0));

      trans_collected.data[i] = vif->sig_data.read().to_uint();
    }
    num_transactions++;

    this->end_tr(trans_collected);
  }
}

//----------------------------------------------------------------------
// member function: check_which_slave (protected)
//----------------------------------------------------------------------

void ubus_bus_monitor::check_which_slave()
{
  std::string slave_name;
  bool slave_found = false;

  slave_addr_map_it it = slave_addr_map.begin();
  slave_name = it->first;
  sc_dt::sc_uint<16> trans_collected_addr (trans_collected.addr);
  if(!slave_name.empty())
  {
    do
    {
      if ( (slave_addr_map[slave_name]->get_min_addr() <= trans_collected_addr)
          && (trans_collected_addr <= slave_addr_map[slave_name]->get_max_addr() ) )
      {
        trans_collected.slave = slave_name;
        slave_found = true;
      }
      if (slave_found) break;
      it++;
    }
    while (it != slave_addr_map.end());
    assert(slave_found);
  }
  else
  {
    std::ostringstream msg;
    msg << "Master attempted a transfer at illegal address 0x"
        << std::hex << trans_collected_addr;
    UVM_ERROR(get_type_name(), msg.str());
  }
}

//----------------------------------------------------------------------
// member function: perform_transfer_checks (protected)
//----------------------------------------------------------------------

void ubus_bus_monitor::perform_transfer_checks()
{
  check_transfer_size();
  check_transfer_data_size();
}

//----------------------------------------------------------------------
// member function: check_transfer_size (protected)
//----------------------------------------------------------------------

void ubus_bus_monitor::check_transfer_size()
{
  if (trans_collected.read_write != ubus_read_write_enum::NOP)
  {
    // TODO assert "assert_transfer_size"
    if (trans_collected.size == 1 ||
        trans_collected.size == 2 ||
        trans_collected.size == 4 ||
        trans_collected.size == 8) /*ok*/ ;
    else
      UVM_ERROR(get_type_name(), "Invalid transfer size!");
  }
}

//----------------------------------------------------------------------
// member function: check_transfer_data_size (protected)
//----------------------------------------------------------------------

void ubus_bus_monitor::check_transfer_data_size()
{
  //TODO monitor size data array
  //if (trans_collected.size != trans_collected.data.size())
  //  UVM_ERROR(get_type_name(), "Transfer size field / data size mismatch.");
}

//----------------------------------------------------------------------
// member function: perform_transfer_coverage (protected)
//----------------------------------------------------------------------

void ubus_bus_monitor::perform_transfer_coverage()
{
  if (trans_collected.read_write != ubus_read_write_enum::NOP)
  {
    cov_transaction.notify();

    for (int unsigned i = 0; i < trans_collected.size; i++)
    {
      sc_dt::sc_uint<16> trans_collected_addr (trans_collected.addr);
      addr = trans_collected_addr + i;
      data = trans_collected.data[i];
      //wait_state = trans_collected.wait_state[i];
      cov_transaction_beat.notify();
    }
  }
}
