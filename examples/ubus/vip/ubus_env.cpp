//----------------------------------------------------------------------
//   Copyright 2016 NXP B.V
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

#include "ubus_env.h"
#include "ubus_if.h"
#include "ubus_bus_monitor.h"
#include "ubus_master_agent.h"
#include "ubus_slave_agent.h"
#include "ubus_slave_monitor.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------

ubus_env::ubus_env( uvm::uvm_component_name name)
: uvm::uvm_env(name)
{
  has_bus_monitor = true;
  num_masters = 0;
  num_slaves = 0;
  vif = NULL;
  intf_checks_enable = true;
  intf_coverage_enable = true;
}

//----------------------------------------------------------------------
// member function: build_phase
//----------------------------------------------------------------------

void ubus_env::build_phase(uvm::uvm_phase& phase)
{
  std::string inst_name;

  // set_phase_domain("uvm");

  uvm_env::build_phase(phase);

  if(! uvm::uvm_config_db<ubus_if*>::get(this, "", "vif", vif))
    UVM_FATAL("NOVIF", "virtual interface must be set for: " + get_full_name() + ".vif");

  if (has_bus_monitor)
  {
    bus_monitor = ubus_bus_monitor::type_id::create("bus_monitor", this);
  }

  uvm::uvm_config_db<int>::get(this, "", "num_masters", num_masters);

  for(int i = 0; i < num_masters; i++)
  {
    char inst_name[20];
    std::sprintf(inst_name, "masters[%d]", i);
    ubus_master_agent* master = ubus_master_agent::type_id::create(inst_name);
    masters.push_back(master);

    uvm::uvm_config_db<int>::set(this, (std::string)inst_name + ".monitor", "master_id", i);
    uvm::uvm_config_db<int>::set(this, (std::string)inst_name + ".driver", "master_id", i);
  }

  uvm::uvm_config_db<int>::get(this, "", "num_slaves", num_slaves);

  for(int i = 0; i < num_slaves; i++)
  {
    char inst_name[20];
    std::sprintf(inst_name, "slaves[%d]", i);
    ubus_slave_agent* slave = ubus_slave_agent::type_id::create((std::string)(inst_name));
    slaves.push_back(slave);
  }
}

//----------------------------------------------------------------------
// member function: set_slave_address_map
//----------------------------------------------------------------------

void ubus_env::set_slave_address_map(std::string slave_name, int min_addr, int max_addr)
{
  ubus_slave_monitor* tmp_slave_monitor;

  if( bus_monitor != NULL )
  {
    // Set slave address map for bus monitor
    bus_monitor->set_slave_configs(slave_name, min_addr, max_addr);
  }

  // Set slave address map for slave monitor
  tmp_slave_monitor = dynamic_cast<ubus_slave_monitor*>(lookup(slave_name + ".monitor"));

  if (tmp_slave_monitor==NULL)
    UVM_FATAL("Address Map", "Cannot set slave address map for slave monitor '" + slave_name + ".monitor'.");

  tmp_slave_monitor->set_addr_range(min_addr, max_addr);
}

//----------------------------------------------------------------------
// member function: run_phase
//----------------------------------------------------------------------

void ubus_env::run_phase(uvm::uvm_phase& phase)
{
  SC_FORK
    sc_core::sc_spawn(sc_bind(&ubus_env::update_vif_enables, this))
  SC_JOIN
}


//----------------------------------------------------------------------
// member function: update_vif_enables
//----------------------------------------------------------------------

void ubus_env::update_vif_enables()
{
  while(true) // forever
  {
    sc_core::sc_event_or_list list = intf_checks_enable_ev | intf_coverage_enable_ev;
    sc_core::wait(list);
    vif->has_checks = intf_checks_enable;
    vif->has_coverage = intf_coverage_enable;
  }
}
