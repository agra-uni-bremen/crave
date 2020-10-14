//----------------------------------------------------------------------
//   Copyright 2016 NXP B.V.
//   Copyright 2007-2010 Mentor Graphics Corporation
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

#ifndef UBUS_EXAMPLE_SCOREBOARD_H_
#define UBUS_EXAMPLE_SCOREBOARD_H_

#include <systemc>
#include <uvm>

#include <map>
#include <sstream>

#include "vip/ubus.h"

//------------------------------------------------------------------------------
// CLASS: ubus_example_scoreboard
//------------------------------------------------------------------------------

class ubus_example_scoreboard : public uvm::uvm_scoreboard
{
public:

  uvm::uvm_analysis_imp<ubus_transfer, ubus_example_scoreboard> item_collected_export;

  // Provide implementations of virtual methods such as get_type_name and create
  UVM_COMPONENT_UTILS(ubus_example_scoreboard);

  // constructor
  ubus_example_scoreboard(uvm::uvm_component_name name);

  virtual void write(const ubus_transfer& trans);

  // report_phase
  virtual void report_phase(uvm::uvm_phase& phase);

  int sbd_error;

protected:

  // memory_verify
  void memory_verify(const ubus_transfer& trans);

  bool disable_scoreboard;
  int num_writes;
  int num_init_reads;
  int num_uninit_reads;
  std::map<unsigned int, unsigned int> m_mem_expected;

}; // class ubus_example_scoreboard


#endif /* UBUS_EXAMPLE_SCOREBOARD_H_ */
