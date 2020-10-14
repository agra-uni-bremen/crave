//----------------------------------------------------------------------
//   Copyright 2016 NXP B.V.
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

#ifndef UBUS_EXAMPLE_TB_H_
#define UBUS_EXAMPLE_TB_H_

#include <systemc>
#include <uvm>

#include "ubus_example_scoreboard.h"
#include "ubus_example_master_seq_lib.h"

#include "vip/ubus.h"
#include "vip/ubus_master_seq_lib.h"
#include "vip/ubus_slave_seq_lib.h"


//------------------------------------------------------------------------------
//
// CLASS: ubus_example_tb
//
//------------------------------------------------------------------------------

class ubus_example_tb : public uvm::uvm_env
{
public:

  UVM_COMPONENT_UTILS(ubus_example_tb);

  // ubus environment
  ubus_env* ubus0;

  // Scoreboard to check the memory operation of the slave.
  ubus_example_scoreboard* scoreboard0;

  // constructor
  ubus_example_tb( uvm::uvm_component_name name)
  : uvm::uvm_env(name),
    ubus0(0),
    scoreboard0(0)
  {}
  
  // build_phase
  virtual void build_phase(uvm::uvm_phase& phase)
  {
    uvm::uvm_env::build_phase(phase);

    uvm::uvm_config_db<int>::set(this,"ubus0", "num_masters", 1);
    uvm::uvm_config_db<int>::set(this,"ubus0", "num_slaves", 1);
    
    ubus0 = ubus_env::type_id::create("ubus0", this);
    assert(ubus0);

    scoreboard0 = ubus_example_scoreboard::type_id::create("scoreboard0", this);
    assert(scoreboard0);
  }

  void connect_phase(uvm::uvm_phase& phase)
  {
    // Connect slave0 monitor to scoreboard
    ubus0->slaves[0]->monitor->item_collected_port.connect(
      scoreboard0->item_collected_export);
  }

  void end_of_elaboration_phase(uvm::uvm_phase& phase)
  {
    // Set up slave address map for ubus0 (basic default)
    ubus0->set_slave_address_map("slaves[0]", 0, 0xffff);
  }

}; // class ubus_example_tb


#endif /* UBUS_EXAMPLE_TB_H_ */
