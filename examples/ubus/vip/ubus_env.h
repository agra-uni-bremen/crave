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

#ifndef UBUS_ENV_H_
#define UBUS_END_H_

#include <systemc>
#include <uvm>

// forward class declaration
class ubus_bus_monitor;
class ubus_master_agent;
class ubus_slave_agent;
class ubus_if;

//------------------------------------------------------------------------------
// Class: ubus_env
//------------------------------------------------------------------------------

class ubus_env : public uvm::uvm_env
{
public:

  ubus_bus_monitor* bus_monitor;
  std::vector<ubus_master_agent*> masters;
  std::vector<ubus_slave_agent*> slaves;

  // Provide implementations of virtual methods such as get_type_name and create
  UVM_COMPONENT_UTILS(ubus_env);

  /*
    `uvm_field_int(has_bus_monitor, UVM_DEFAULT)
    `uvm_field_int(num_masters, UVM_DEFAULT)
    `uvm_field_int(num_slaves, UVM_DEFAULT)
    `uvm_field_int(intf_checks_enable, UVM_DEFAULT)
    `uvm_field_int(intf_coverage_enable, UVM_DEFAULT)
  */

  ubus_env(uvm::uvm_component_name name);

  void build_phase(uvm::uvm_phase& phase);

  void set_slave_address_map( std::string slave_name, int min_addr, int max_addr);
  void run_phase(uvm::uvm_phase& phase);

protected:

  // update_vif_enables
  void update_vif_enables();

  // Interface variable
  ubus_if* vif;

  // Control properties
  bool has_bus_monitor;
  int num_masters;
  int num_slaves;

public:
  // The following two bits are used to control whether checks and coverage are
  // done both in the bus monitor class and the interface.
  bool intf_checks_enable;
  bool intf_coverage_enable;

  sc_core::sc_event intf_checks_enable_ev;
  sc_core::sc_event intf_coverage_enable_ev;
};

#endif /* UBUS_END_H_ */
