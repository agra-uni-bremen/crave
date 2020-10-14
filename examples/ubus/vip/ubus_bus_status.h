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
//   permissions and limitaions under the License.
//----------------------------------------------------------------------

#ifndef UBUS_BUS_STATUS_H_
#define UBUS_BUS_STATUS_H_

#include <systemc>
#include <uvm>

//----------------------------------------------------------------------
// Enumerated for ubus bus state
//----------------------------------------------------------------------

typedef enum {
  RST_START,
  RST_STOP,
  NO_OP,
  ARBI,
  ADDR_PH,
  ADDR_PH_ERROR,
  DATA_PH
} ubus_bus_state;


//----------------------------------------------------------------------
// CLASS: ubus_status
//----------------------------------------------------------------------

class ubus_status : public uvm::uvm_object
{
public:
  ubus_bus_state bus_state;

  ubus_status( const std::string& name = "ubus_status")
  : uvm::uvm_object(name)
  {}

  UVM_OBJECT_UTILS(ubus_status);

  // TODO add field methods do_print, do_copy, do_compare
};


#endif /* UBUS_BUS_STATUS_H_ */
