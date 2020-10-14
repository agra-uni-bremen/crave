//----------------------------------------------------------------------
//   Copyright (c) 2019-2020 University of Bremen, Germany. 
//   Copyright (c) 2019-2020 DFKI GmbH Bremen, Germany.
//   Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//   Copyright 2016-2019 NXP B.V.
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
//   permissions and limitations under the License.
//----------------------------------------------------------------------

#include <systemc>
#include <uvm>

#include "ubus_slave_driver.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------


ubus_slave_driver::ubus_slave_driver(uvm::uvm_component_name name)
: uvm::uvm_driver<ubus_transfer>(name)
{}

//----------------------------------------------------------------------
// member function: build_phase
//----------------------------------------------------------------------

void ubus_slave_driver::build_phase(uvm::uvm_phase& phase)
{
  if(!uvm::uvm_config_db<ubus_if*>::get(this, "", "vif", vif))
    UVM_FATAL("NOVIF", "Virtual interface must be set for: " + get_full_name() + ".vif");
}

//----------------------------------------------------------------------
// member function: run_phase
//----------------------------------------------------------------------

void ubus_slave_driver::run_phase(uvm::uvm_phase& phase)
{
  SC_FORK
    sc_core::sc_spawn(sc_bind(&ubus_slave_driver::get_and_drive, this)),
    sc_core::sc_spawn(sc_bind(&ubus_slave_driver::reset_signals, this))
  SC_JOIN
}

//----------------------------------------------------------------------
// member function: get_and_drive
//----------------------------------------------------------------------

void ubus_slave_driver::get_and_drive()
{
  sc_core::wait(vif->sig_reset.negedge_event());

  while(true) // forever
  {
    ubus_transfer req;
    ubus_transfer rsp;

    sc_core::wait(vif->sig_clock.posedge_event());

    this->seq_item_port.get_next_item(req);
    respond_to_transfer(req);
    this->seq_item_port.item_done();
    this->seq_item_port.put_response(req); // return changed request to enable pre_do
  }
}

//----------------------------------------------------------------------
// member function: reset_signals
//----------------------------------------------------------------------

void ubus_slave_driver::reset_signals()
{
  while(true) // forever
  {
    sc_core::wait(vif->sig_reset.posedge_event());
    vif->sig_error = sc_dt::SC_LOGIC_Z;
    vif->sig_wait  = sc_dt::SC_LOGIC_Z;
    vif->rw        = sc_dt::SC_LOGIC_0;
  }
}

//----------------------------------------------------------------------
// member function: respond_to_transfer
//----------------------------------------------------------------------

void ubus_slave_driver::respond_to_transfer(ubus_transfer& resp)
{
  if (resp.read_write != ubus_read_write_enum::NOP)
  {
    vif->sig_error = sc_dt::SC_LOGIC_0;

    for (unsigned int i = 0; i < resp.size; i++)
    {
    sc_dt::sc_uint<8> resp_data (resp.data[i]);
      switch (resp.read_write)
      {
      case ubus_read_write_enum::READ :
        vif->rw = sc_dt::SC_LOGIC_1;
        vif->sig_data_out = resp_data;
        break;
      case ubus_read_write_enum::WRITE : break;
      default: break;
      }

	  sc_dt::sc_uint<4> resp_wait_state (resp.wait_state[i]);
      if (resp_wait_state > 0)
      {
        vif->sig_wait = sc_dt::SC_LOGIC_1;
        for (unsigned int j = 0; j < resp_wait_state; j++)
          sc_core::wait(vif->sig_clock.posedge_event());
      }
      vif->sig_wait = sc_dt::SC_LOGIC_0;

      sc_core::wait(vif->sig_clock.posedge_event());
      resp.data[i] = vif->sig_data.read().to_uint();
    }

    // release signals
    vif->rw        = sc_dt::SC_LOGIC_Z;
    vif->sig_wait  = sc_dt::SC_LOGIC_Z;
    vif->sig_error = sc_dt::SC_LOGIC_Z;
    vif->sig_data_out = "zzzzzzzz";
  }
}
