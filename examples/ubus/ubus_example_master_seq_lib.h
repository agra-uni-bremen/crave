//----------------------------------------------------------------------
//   Copyright (c) 2019-2020 University of Bremen, Germany. 
//   Copyright (c) 2019-2020 DFKI GmbH Bremen, Germany.
//   Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//   Copyright 2016-2020 NXP B.V.
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

#ifndef UBUS_EXAMPLE_MASTER_SEQ_LIB_H_
#define UBUS_EXAMPLE_MASTER_SEQ_LIB_H_

#include <systemc>
#include <uvm>

#include <sstream>

#include "vip/ubus.h"
#include "vip/ubus_master_seq_lib.h"

//------------------------------------------------------------------------------
//
// SEQUENCE: incr_read_byte_seq
//
//------------------------------------------------------------------------------

class incr_read_byte_seq : public ubus_base_sequence
{
 public:
  incr_read_byte_seq(const std::string& name = "incr_read_byte_seq")
  : ubus_base_sequence(name)
  {
    incr_transmit_del = 0;
  }

  UVM_OBJECT_UTILS(incr_read_byte_seq);

  read_byte_seq read_byte_seq0;

  /* rand */ unsigned int count;
  /* rand */ sc_dt::sc_uint<16> start_address;
  /* rand */ unsigned int incr_transmit_del;

  /*
  constraint count_ct { (count < 20); }
  constraint transmit_del_ct { (incr_transmit_del <= 10); }
  */

  virtual void body()
  {
    std::ostringstream str;
    str << get_sequence_path()
        << " starting with count = "
        << count;
    UVM_INFO(get_type_name(), str.str(), uvm::UVM_MEDIUM);

    for(unsigned int i = 0; i < count; i++)
    {
      /*
      UVM_DO_WITH(read_byte_seq0,
        { read_byte_seq0.start_addr == start_address;
          read_byte_seq0.transmit_del == incr_transmit_del; } )
      */
      start_address++;
    }
  }
 
}; // class incr_read_byte_seq


//------------------------------------------------------------------------------
//
// SEQUENCE: incr_write_byte_seq
//
//------------------------------------------------------------------------------

class incr_write_byte_seq : public ubus_base_sequence
{
 public:
  incr_write_byte_seq(const std::string& name = "incr_write_byte_seq")
  : ubus_base_sequence(name)
  {
    incr_transmit_del = 0;
  }

  UVM_OBJECT_UTILS(incr_write_byte_seq);
    
  write_byte_seq write_byte_seq0;

  /* rand */ unsigned int count;
  /* rand */ sc_dt::sc_uint<16> start_address;
  /* rand */ unsigned int incr_transmit_del;

  /*
  constraint count_ct { (count < 20); }
  constraint transmit_del_ct { (incr_transmit_del <= 10); }
  */

  virtual void body()
  {
    std::ostringstream str;
    str << get_sequence_path()
        << " starting with count = "
        << count;
    UVM_INFO(get_type_name(), str.str(), uvm::UVM_MEDIUM);

    for(unsigned int i = 0; i < count; i++)
    {
      /*
      UVM_DO_WITH(write_byte_seq0,
          { write_byte_seq0.start_addr == start_address;
          write_byte_seq0.transmit_del == incr_transmit_del; } )
          start_address++;
      */
    }
  }
}; // class incr_write_byte_seq


//------------------------------------------------------------------------------
//
// SEQUENCE: incr_read_write_read_seq
//
//------------------------------------------------------------------------------

class incr_read_write_read_seq : public ubus_base_sequence
{
 public:

  incr_read_write_read_seq(const std::string& name = "incr_read_write_read_seq")
  : ubus_base_sequence(name)
  {}

  UVM_OBJECT_UTILS(incr_read_write_read_seq);

  incr_read_byte_seq*  read0;
  incr_write_byte_seq* write0;

  virtual void body()
  {
    std::ostringstream str;
    str << get_sequence_path()
        << " starting sequence";
    UVM_INFO(get_type_name(), str.str(), uvm::UVM_MEDIUM);
    UVM_DO(read0);
    UVM_DO(write0);
    UVM_DO(read0);
  }
}; // class incr_read_write_read_seq


//------------------------------------------------------------------------------
//
// SEQUENCE: r8_w8_r4_w4_seq
//
//------------------------------------------------------------------------------

class r8_w8_r4_w4_seq : public ubus_base_sequence
{
 public:
  r8_w8_r4_w4_seq(const std::string& name = "r8_w8_r4_w4_seq")
  : ubus_base_sequence(name)
  {}

  UVM_OBJECT_UTILS(r8_w8_r4_w4_seq);

  read_word_seq         read_word_seq0;
  read_double_word_seq  read_double_word_seq0;
  write_word_seq        write_word_seq0;
  write_double_word_seq write_double_word_seq0;

  /* rand */ sc_dt::sc_uint<16> start_address;

  //constraint start_address_ct { (start_address == 16'h4000); }

  virtual void body()
  {
    std::ostringstream str;
    str << get_sequence_path()
        << " starting...";
    UVM_INFO(get_type_name(), str.str(), uvm::UVM_MEDIUM);
/*
    UVM_DO_WITH(read_double_word_seq0,
      { read_double_word_seq0.start_addr == start_address;
        read_double_word_seq0.transmit_del == 2; } )

    UVM_DO_WITH(write_double_word_seq0,
      { write_double_word_seq0.start_addr == start_address;
        write_double_word_seq0.transmit_del == 4; } )

    start_address = start_address + 8;

    UVM_DO_WITH(read_word_seq0,
      { read_word_seq0.start_addr == start_address;
        read_word_seq0.transmit_del == 6; } )

    UVM_DO_WITH(write_word_seq0,
      { write_word_seq0.start_addr == start_address;
        write_word_seq0.transmit_del == 8; } )
*/
  }
}; // class r8_w8_r4_w4_seq
  

//------------------------------------------------------------------------------
//
// SEQUENCE: read_modify_write_seq
//
//------------------------------------------------------------------------------

class read_modify_write_seq : public ubus_base_sequence
{
 public:
  read_modify_write_seq(const std::string& name = "read_modify_write_seq")
  : ubus_base_sequence(name)
  {
	  read_byte_seq0 = read_byte_seq::type_id::create();
	  write_byte_seq0 = write_byte_seq::type_id::create();
  }

  UVM_OBJECT_UTILS(read_modify_write_seq);

  read_byte_seq*  read_byte_seq0 {nullptr};
  write_byte_seq* write_byte_seq0 {nullptr};

  /* rand */ sc_dt::sc_uint<16> addr_check;
  sc_dt::sc_uint<8> m_data0_check;

  virtual void body()
  {
    std::ostringstream str;
    str << get_sequence_path()
        << " starting...";
    UVM_INFO(get_type_name(), str.str(), uvm::UVM_MEDIUM);


read_byte_seq0->randomize_with(
    		//read_byte_seq0->start_addr() == 0x1111,
			read_byte_seq0->transmit_del() == 0
			);

    read_byte_seq0->start(m_sequencer);

    addr_check = read_byte_seq0->rsp->addr;
    sc_dt::sc_uint<8> rsp_data (read_byte_seq0->rsp->data[0]);
    m_data0_check = rsp_data + 1;


	write_byte_seq0->randomize_with(
    			write_byte_seq0->data0() == m_data0_check,
				write_byte_seq0->start_addr() == addr_check
				);

    write_byte_seq0->start(m_sequencer);

    // review read sequence

	 read_byte_seq0->randomize_with(
    			read_byte_seq0->start_addr() == addr_check
				);

    read_byte_seq0->start(m_sequencer);

    if(m_data0_check != read_byte_seq0->rsp->data[0])
    {
      std::ostringstream str;
      str << get_sequence_path()
          << " Read Modify Write Read error!"
          << std::endl
          << "ADDR: " << addr_check
          << " EXP: " << m_data0_check
          << " ACT: " << read_byte_seq0->rsp->data[0];
      UVM_ERROR(get_type_name(), str.str());
    }
    read_byte_seq::type_id::destroy(read_byte_seq0);
    write_byte_seq::type_id::destroy(write_byte_seq0);

    UVM_INFO(get_type_name(), "sequence finished.", uvm::UVM_MEDIUM);
  }
}; // class read_modify_write_seq


//------------------------------------------------------------------------------
//
// SEQUENCE: loop_read_modify_write_seq
//
//------------------------------------------------------------------------------

class loop_read_modify_write_seq : public ubus_base_sequence
{
 public:

  int itr;

  loop_read_modify_write_seq(const std::string& name = "loop_read_modify_write_seq")
  : ubus_base_sequence(name)
  {}

  UVM_OBJECT_UTILS(loop_read_modify_write_seq);

  read_modify_write_seq* rmw_seq;

  virtual void body()
  {
    uvm::uvm_config_db<int>::get(NULL, get_full_name(), "itr", itr);

    std::ostringstream str;
    str << get_sequence_path()
        << " starting...itr =" << itr;
    UVM_INFO(get_type_name(), str.str(), uvm::UVM_NONE);

    for(int i = 0; i < itr; i++)
    {
      UVM_DO(rmw_seq);
    }
  }
}; // class loop_read_modify_write_seq



#endif /* UBUS_EXAMPLE_MASTER_SEQ_LIB_H_ */
