//----------------------------------------------------------------------
//   Copyright (c) 2019-2020 University of Bremen, Germany. 
//   Copyright (c) 2019-2020 DFKI GmbH Bremen, Germany.
//   Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//   Copyright 2016-2019 NXP B.V.
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

#ifndef UBUS_TRANSFER_H_
#define UBUS_TRANSFER_H_

#include <systemc>
#include <uvm>
#include <crave2uvm.h>
//#include <crave/SystemC.hpp>
//#include <crave/experimental/SystemC.hpp>
#include <crave/ConstrainedRandom.hpp>
#include <crave/experimental/Experimental.hpp>
#include "ubus_defines.h"

//----------------------------------------------------------------------
// ubus transfer enums, parameters, and events
//----------------------------------------------------------------------

CRAVE_BETTER_ENUM(ubus_read_write_enum, NOP = 0, READ = 1, WRITE = 2);
//----------------------------------------------------------------------
// Class: ubus_transfer
//----------------------------------------------------------------------

class ubus_transfer : public uvm_randomized_sequence_item
{
public:

  crave::crv_variable< sc_dt::sc_uint<16> > addr;
  crave::crv_variable< ubus_read_write_enum> read_write;
  crave::crv_variable< unsigned int> size;
  crave::crv_variable< sc_dt::sc_uint<8> > data [MAXSIZE];
  crave::crv_variable< sc_dt::sc_uint<4> > wait_state[MAXSIZE];
  //crave::crv_vector< unsigned int > wait_state;
  crave::crv_variable< unsigned int> error_pos;
  crave::crv_variable< unsigned int> transmit_delay;

  std::string master;
  std::string slave;


  crave::crv_constraint c_read_write{crave::inside(read_write(), std::set<ubus_read_write_enum> {
              ubus_read_write_enum::READ, ubus_read_write_enum::WRITE
          })};

  crave::crv_constraint c_size{crave::inside(size(), std::set<int> {
              1, 2, 4, 8
          })};

/*  crave::crv_constraint c_data_wait_size{
//          data().size() == size(),
          wait_state().size() == size()
      };
*/
 crave::crv_constraint c_transmit_delay{transmit_delay() <= 10};

  UVM_OBJECT_UTILS(ubus_transfer);

  /* TODO no field macros; add specific pack/unpack methods
    `uvm_field_int      (addr,           UVM_DEFAULT)
    `uvm_field_enum     (ubus_read_write_enum, read_write, UVM_DEFAULT)
    `uvm_field_int      (size,           UVM_DEFAULT)
    `uvm_field_array_int(data,           UVM_DEFAULT)
    `uvm_field_array_int(wait_state,     UVM_DEFAULT)
    `uvm_field_int      (error_pos,      UVM_DEFAULT)
    `uvm_field_int      (transmit_delay, UVM_DEFAULT)
    `uvm_field_string   (master,         UVM_DEFAULT|UVM_NOCOMPARE)
    `uvm_field_string   (slave,          UVM_DEFAULT|UVM_NOCOMPARE)
  */

  ubus_transfer(crave::crv_object_name name = "ubus_transfer_inst");
  virtual void do_print(const uvm::uvm_printer& printer) const;
  //virtual void do_pack(uvm::uvm_packer& p) const;
  //virtual void do_unpack(uvm::uvm_packer& p);
  virtual void do_copy(const uvm::uvm_object& rhs);
  virtual bool do_compare(const uvm_object& rhs) const;
  std::string convert2string() const;

};

#endif /* UBUS_TRANSFER_H_ */
