//----------------------------------------------------------------------
//   Copyright (c) 2019-2020 University of Bremen, Germany. 
//   Copyright (c) 2019-2020 DFKI GmbH Bremen, Germany.
//   Copyright (c) 2020 Johannes Kepler University Linz, Austria.
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

#include <systemc>
#include <uvm>

#include <map>
#include <sstream>

#include "ubus_example_scoreboard.h"

//----------------------------------------------------------------------
// constructor
//----------------------------------------------------------------------

ubus_example_scoreboard::ubus_example_scoreboard(uvm::uvm_component_name name)
: uvm::uvm_scoreboard(name),
  item_collected_export("item_collected_export", this)
{
  disable_scoreboard = false;
  num_writes = 0;
  num_init_reads = 0;
  num_uninit_reads = 0;
  sbd_error = 0;
}

//----------------------------------------------------------------------
// member function: write (virtual)
//----------------------------------------------------------------------

void ubus_example_scoreboard::write(const ubus_transfer& trans)
{
  if(!disable_scoreboard)
    memory_verify(trans);
}

//----------------------------------------------------------------------
// member function: report_phase (virtual)
//----------------------------------------------------------------------

void ubus_example_scoreboard::report_phase(uvm::uvm_phase& phase)
{
  if(!disable_scoreboard)
    UVM_INFO(get_type_name(), "Reporting scoreboard information...\n" +
        this->sprint(), uvm::UVM_LOW);
}

//----------------------------------------------------------------------
// member function: memory_verify
//----------------------------------------------------------------------
void ubus_example_scoreboard::memory_verify(const ubus_transfer& trans)
{
  sc_dt::sc_uint<8> data;
  sc_dt::sc_uint<8> exp;

  for (unsigned int i = 0; i < trans.size; i++)
  {
    // Check to see if entry in associative array for this address when read
    // If so, check that transfer data matches associative array data.
   	sc_dt::sc_uint<16> trans_addr (trans.addr);
	sc_dt::sc_uint<8> trans_data (trans.data[i]);
    if (m_mem_expected.find(trans_addr + i) != m_mem_expected.end()) // exists
    {
      if (trans.read_write == ubus_read_write_enum::READ)
      {
        data = trans.data[i];
        std::ostringstream msg;
        msg << trans.read_write._to_string()
            << " to existing address...Checking address : 0x"
            << std::hex << trans.addr
            << " with data : 0x"
            << std::hex << data;
        UVM_INFO(get_type_name(), msg.str(), uvm::UVM_LOW);

        // TODO make assertion
        if(m_mem_expected[trans_addr + i] != trans_data)
        {
          std::ostringstream errmsg;
          errmsg << "Read data mismatch.  Expected : 0x"
              << std::hex << m_mem_expected[trans_addr + i]
              << " Actual : 0x"
              << std::hex << data;

          UVM_ERROR("ASSERT-" + get_type_name(), errmsg.str());
          sbd_error = 1;
        }

        num_init_reads++;
      }

      if (trans.read_write == ubus_read_write_enum::WRITE)
      {
        data = trans.data[i];
        std::ostringstream msg;
        msg << trans.read_write._to_string()
                                    << " to existing address...Updating address : 0x"
                                    << std::hex << (trans_addr + i)
                                    << " with data : 0x"
                                    << std::hex << data;
        UVM_INFO(get_type_name(), msg.str(), uvm::UVM_LOW);

        m_mem_expected[trans_addr + i] = trans_data;
        num_writes++;
      }
    } // if m_mem_expected

    // Check to see if entry in associative array for this address
    // If not, update the location regardless if read or write.
    else
    {
      data = trans.data[i];
      std::ostringstream msg;
      msg << trans.read_write._to_string()
                                  << " to empty address...Updating address : 0x"
                                  << std::hex << (trans_addr + i)
                                  << " with data : 0x"
                                  << std::hex << data;
      UVM_INFO(get_type_name(), msg.str(), uvm::UVM_LOW);

      m_mem_expected[trans_addr + i] = trans_data;

      if(trans.read_write == ubus_read_write_enum::READ) num_uninit_reads++;
      else
        if (trans.read_write == ubus_read_write_enum::WRITE) num_writes++;
    }
  } // for
}

