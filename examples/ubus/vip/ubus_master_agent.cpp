//----------------------------------------------------------------------
//   Copyright 2016 NXP B.V.
//   Copyright 2007-2011 Mentor Graphics Corporation
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

#include "ubus_master_agent.h"
#include "ubus_transfer.h"
#include "ubus_master_driver.h"
#include "ubus_master_monitor.h"
#include "ubus_master_sequencer.h"

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------

ubus_master_agent::ubus_master_agent(uvm::uvm_component_name name)
: uvm::uvm_agent(name),
  driver(0), sequencer(0), monitor(0), master_id(0)
{}

//------------------------------------------------------------------------------
// member function: build_phase
//------------------------------------------------------------------------------

void ubus_master_agent::build_phase(uvm::uvm_phase& phase)
{
  uvm::uvm_agent::build_phase(phase);

  monitor = ubus_master_monitor::type_id::create("monitor", this);
  assert(monitor);

  if(get_is_active() == uvm::UVM_ACTIVE)
  {
    sequencer = ubus_master_sequencer::type_id::create("sequencer", this);
    assert(sequencer);
    driver = ubus_master_driver::type_id::create("driver", this);
    assert(driver);
  }
}

//------------------------------------------------------------------------------
// member function: connect_phase
//------------------------------------------------------------------------------

void ubus_master_agent::connect_phase(uvm::uvm_phase& phase)
{
  if(get_is_active() == uvm::UVM_ACTIVE)
    driver->seq_item_port.connect(sequencer->seq_item_export);
}
