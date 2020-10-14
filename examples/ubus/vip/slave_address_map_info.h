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

#ifndef SLAVE_ADDRESS_MAP_INFO_H_
#define SLAVE_ADDRESS_MAP_INFO_H_

#include <systemc>
#include <uvm>

#include <sstream>

#include "ubus.h"

//----------------------------------------------------------------------
// CLASS: slave_address_map_info
//
// The following class is used to determine which slave should respond 
// to a transfer on the bus
//----------------------------------------------------------------------

class slave_address_map_info : public uvm::uvm_object
{
public:
  slave_address_map_info(const std::string& name = "slave_address_map_info")
  : uvm::uvm_object(name)
  {}

  UVM_OBJECT_UTILS(slave_address_map_info);

  void set_address_map(unsigned int min_addr_, unsigned int max_addr_)
  {
    min_addr = min_addr_;
    max_addr = max_addr_;
  }

  // get the min addr
  unsigned int get_min_addr()
  {
    return min_addr;
  }

  // get the max addr
  unsigned int get_max_addr()
  {
    return max_addr;
  }

  // added mandatory field functions
  virtual void do_print(uvm::uvm_printer& printer)
  {
    printer.print_field_int("min_addr", min_addr);
    printer.print_field_int("max_addr", max_addr);
  }

  virtual void do_copy( const uvm::uvm_object& rhs )
  {
    const slave_address_map_info* rhs_ =
      dynamic_cast<const slave_address_map_info*>(&rhs);

    if(rhs_ == NULL)
      UVM_ERROR("do_copy", "cast failed, check type compatibility");

    min_addr = rhs_->min_addr;
    max_addr = rhs_->max_addr;
  }

  virtual bool do_compare(const uvm::uvm_object& rhs,
                          const uvm::uvm_comparer* comparer ) const
  {
    const slave_address_map_info* rhs_ =
      dynamic_cast<const slave_address_map_info*>(&rhs);

    if(rhs_ == NULL)
      UVM_FATAL("do_compare", "cast failed, check type compatibility");

    return ((min_addr == rhs_->min_addr)
        && (max_addr == rhs_->max_addr));
  }

  std::string convert2string()
  {
    std::ostringstream str;
    str << "min_addr=" << min_addr
        << "max_addr=" << max_addr;
    return str.str();
  }

protected:
  unsigned int min_addr;
  unsigned int max_addr;
};


#endif /* SLAVE_ADDRESS_MAP_INFO_H_ */
