//----------------------------------------------------------------------
//   Copyright 2016 NXP B.V.
//   Copyright 2007-2011 Mentor Graphics Corporation
//   Copyright 2007-2010 Cadence Design Systems, Inc.
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

#ifndef UBUS_MASTER_SEQUENCER_H_
#define UBUS_MASTER_SEQUENCER_H_

#include <systemc>
#include <uvm>

//------------------------------------------------------------------------------
// Class: ubus_master_sequencer
//------------------------------------------------------------------------------

class ubus_master_sequencer : public uvm::uvm_sequencer<ubus_transfer>
{
 public:
  UVM_COMPONENT_UTILS(ubus_master_sequencer);
     
  ubus_master_sequencer( uvm::uvm_component_name name)
  : uvm::uvm_sequencer<ubus_transfer>(name) {}

};

#endif /* UBUS_MASTER_SEQUENCER_H_ */

