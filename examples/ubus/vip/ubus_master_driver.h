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

#ifndef UBUS_MASTER_DRIVER_H_
#define UBUS_MASTER_DRIVER_H_

#include <systemc>
#include <uvm>

#include "ubus_transfer.h"

// forward class declaration
class ubus_if;

//------------------------------------------------------------------------------
// Class: ubus_master_driver
//------------------------------------------------------------------------------

class ubus_master_driver : public uvm::uvm_driver<ubus_transfer>
{
public:

  UVM_COMPONENT_UTILS(ubus_master_driver);

  ubus_master_driver(uvm::uvm_component_name name);

  void build_phase(uvm::uvm_phase& phase);
  virtual void run_phase(uvm::uvm_phase& phase);

protected:

  virtual void get_and_drive();
  virtual void reset_signals();
  virtual void drive_transfer(ubus_transfer& trans);
  virtual void arbitrate_for_bus();
  virtual void drive_address_phase(const ubus_transfer& trans);

  virtual void drive_data_phase(ubus_transfer& trans);
  virtual void read_byte(sc_dt::sc_uint<8>& data, bool& error);
  virtual void write_byte(sc_dt::sc_uint<8> data, bool& error);
  virtual void drive_size (unsigned int size);
  virtual void drive_read_write(ubus_read_write_enum rw);

private:
  // data members

  // The interface used to drive and view signals.
  ubus_if* vif;

  // Master Id
  int master_id;
};

#endif /* UBUS_MASTER_DRIVER_H_ */
