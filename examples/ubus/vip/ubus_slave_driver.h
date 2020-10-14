//----------------------------------------------------------------------
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

#ifndef UBUS_SLAVE_DRIVER_H_
#define UBUS_SLAVE_DRIVER_H_

#include <systemc>
#include <uvm>

// forward class declaration
#include "ubus_transfer.h"
#include "ubus_if.h"

//----------------------------------------------------------------------
// Class: ubus_slave_driver
//----------------------------------------------------------------------

class ubus_slave_driver : public uvm::uvm_driver<ubus_transfer>
{
public:

  UVM_COMPONENT_UTILS(ubus_slave_driver);

  ubus_slave_driver(uvm::uvm_component_name name);

  void build_phase(uvm::uvm_phase& phase);
  virtual void run_phase(uvm::uvm_phase& phase);

protected:

  virtual void get_and_drive();
  virtual void reset_signals();
  virtual void respond_to_transfer(ubus_transfer& resp);

private:
  // The virtual interface used to drive and view HDL signals.
  ubus_if* vif;

};

#endif /* UBUS_SLAVE_DRIVER_H_ */

