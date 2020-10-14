//----------------------------------------------------------------------
//   Copyright 2016-2019 NXP-2019 B.V.
//   Copyright 2007-2011 Mentor Graphics Corporation
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

#ifndef UBUS_TEST_LIB_H_
#define UBUS_TEST_LIB_H_

#include <systemc>
#include <uvm>

#include "vip/ubus.h"

#include "ubus_example_tb.h"


// Base Test
class ubus_example_base_test : public uvm::uvm_test
{
public:

  UVM_COMPONENT_UTILS(ubus_example_base_test);

  ubus_example_tb* ubus_example_tb0 {nullptr};
  uvm::uvm_table_printer* printer {nullptr};
  bool test_pass;

  ubus_example_base_test( uvm::uvm_component_name name = "ubus_example_base_test")
  : uvm::uvm_test(name)
  {
    test_pass = true;
  }

  virtual void build_phase(uvm::uvm_phase& phase)
  {
    uvm::uvm_test::build_phase(phase);

    // Enable transaction recording for everything
    uvm::uvm_config_db<int>::set(this, "*", "recording_detail", uvm::UVM_FULL);

    // Create the test bench
    ubus_example_tb0 = ubus_example_tb::type_id::create("ubus_example_tb0", this);
    assert(ubus_example_tb0);

    // Create a specific depth printer for printing the created topology
    printer = new uvm::uvm_table_printer();
    printer->knobs.depth = 3;
  }

  void end_of_elaboration_phase(uvm::uvm_phase& phase)
  {
    // Set verbosity for the bus monitor for this demo
     if(ubus_example_tb0->ubus0->bus_monitor != NULL)
       ubus_example_tb0->ubus0->bus_monitor->set_report_verbosity_level(uvm::UVM_FULL);

    UVM_INFO(get_type_name(), "Printing the test topology :\n" +
      this->sprint(printer), uvm::UVM_LOW);
  }

  void run_phase(uvm::uvm_phase& phase)
  {
    //set a drain-time for the environment if desired
    sc_core::sc_time drain_time = sc_core::sc_time(50.0, sc_core::SC_NS);
    phase.get_objection()->set_drain_time(this, drain_time);
  }

  void extract_phase(uvm::uvm_phase& phase)
  {
    if(ubus_example_tb0->scoreboard0->sbd_error)
      test_pass = false;
  }
  
  void report_phase(uvm::uvm_phase& phase)
  {
    if(test_pass)
    {
      UVM_INFO(get_type_name(), "** UVM TEST PASSED **", uvm::UVM_NONE);
    }
    else
    {
      UVM_ERROR(get_type_name(), "** UVM TEST FAIL **");
    }
  }

  void final_phase(uvm::uvm_phase& phase)
  {
    delete printer;
  }

}; // class ubus_example_base_test


// Read Modify Write Read Test
class test_read_modify_write : public ubus_example_base_test
{
public:

  UVM_COMPONENT_UTILS(test_read_modify_write);

  test_read_modify_write( uvm::uvm_component_name name = "test_read_modify_write")
  : ubus_example_base_test(name)
  {}

  virtual void build_phase(uvm::uvm_phase phase)
  {
    uvm::uvm_config_db<uvm::uvm_object_wrapper*>::set(this,
      "ubus_example_tb0.ubus0.masters[0].sequencer.run_phase",
      "default_sequence", read_modify_write_seq::type_id::get());

    uvm::uvm_config_db<uvm::uvm_object_wrapper*>::set(this,
      "ubus_example_tb0.ubus0.slaves[0].sequencer.run_phase",
      "default_sequence", slave_memory_seq::type_id::get());

    // Create the test bench
    ubus_example_base_test::build_phase(phase);
  }
}; // class test_read_modify_write


// Large word read/write test
class test_r8_w8_r4_w4 : public ubus_example_base_test
{
public:
  UVM_COMPONENT_UTILS(test_r8_w8_r4_w4);

  test_r8_w8_r4_w4( uvm::uvm_component_name name = "test_r8_w8_r4_w4")
  : ubus_example_base_test(name)
  {}

  virtual void build_phase(uvm::uvm_phase& phase)
  {
    ubus_example_base_test::build_phase(phase);

    uvm::uvm_config_db<uvm::uvm_object_wrapper*>::set(this,
      "ubus_example_tb0.ubus0.masters[0].sequencer.run_phase",
      "default_sequence", r8_w8_r4_w4_seq::type_id::get());

    uvm::uvm_config_db<uvm::uvm_object_wrapper*>::set(this,
      "ubus_example_tb0.ubus0.slaves[0].sequencer.run_phase",
      "default_sequence", slave_memory_seq::type_id::get());
  }

}; // class test_r8_w8_r4_w4


// 2 Masters, 4 Slaves test
class test_2m_4s : public ubus_example_base_test
{
public:
  UVM_COMPONENT_UTILS(test_2m_4s);

  test_2m_4s(uvm::uvm_component_name name = "test_2m_4s")
  : ubus_example_base_test(name)
  {}

  virtual void build_phase(uvm::uvm_phase& phase)
  {
    // Overides to the ubus_example_tb build_phase()
    // Set the topology to 2 masters, 4 slaves
    uvm::uvm_config_db<int>::set(this, "ubus_example_tb0.ubus0",
			         "num_masters", 2);
    uvm::uvm_config_db<int>::set(this, "ubus_example_tb0.ubus0",
			         "num_slaves", 4);
     
    // Control the number of read-modified-write loops
    uvm::uvm_config_db<int>::set(this,
      "ubus_example_tb0.ubus0.masters[0].sequencer.loop_read_modify_write_seq", "itr", 6);
    uvm::uvm_config_db<int>::set(this,
      "ubus_example_tb0.ubus0.masters[1].sequencer.loop_read_modify_write_seq", "itr", 8);

    // Define the sequences to run in the run phase
    uvm::uvm_config_db<uvm::uvm_object_wrapper*>::set(this,
      "*.ubus0.masters[0].sequencer.main_phase",
      "default_sequence",
      loop_read_modify_write_seq::type_id::get() );

    lrmw_seq = loop_read_modify_write_seq::type_id::create("loop_read_modify_write_seq");

    uvm::uvm_config_db<uvm::uvm_sequence_base*>::set(this,
      "ubus_example_tb0.ubus0.masters[1].sequencer.main_phase",
      "default_sequence",
      lrmw_seq );

    for(int i = 0; i < 4; i++)
    {
      std::ostringstream slname;
      slname << "ubus_example_tb0.ubus0.slaves["
             << i << "].sequencer";
      uvm::uvm_config_db<uvm::uvm_object_wrapper*>::set(this,
        slname.str() + ".run_phase",
        "default_sequence", slave_memory_seq::type_id::get());
    }
     
    // Create the test bench
    ubus_example_base_test::build_phase(phase);
  }

  void connect_phase(uvm::uvm_phase& phase)
  {
    // Connect other slaves monitor to scoreboard
    ubus_example_tb0->ubus0->slaves[1]->monitor->item_collected_port.connect(
      ubus_example_tb0->scoreboard0->item_collected_export);

    ubus_example_tb0->ubus0->slaves[2]->monitor->item_collected_port.connect(
      ubus_example_tb0->scoreboard0->item_collected_export);

    ubus_example_tb0->ubus0->slaves[3]->monitor->item_collected_port.connect(
      ubus_example_tb0->scoreboard0->item_collected_export);
  }
  
  void end_of_elaboration_phase(uvm::uvm_phase& phase)
  {
    // Set up slave address map for ubus0 (slaves[0] is overwritten here)
    ubus_example_tb0->ubus0->set_slave_address_map("slaves[0]", 0x0000, 0x3fff);
    ubus_example_tb0->ubus0->set_slave_address_map("slaves[1]", 0x4000, 0x7fff);
    ubus_example_tb0->ubus0->set_slave_address_map("slaves[2]", 0x8000, 0xBfff);
    ubus_example_tb0->ubus0->set_slave_address_map("slaves[3]", 0xC000, 0xFfff);

    ubus_example_base_test::end_of_elaboration_phase(phase);
  }

  virtual void final_phase(uvm::uvm_phase& phase)
  {
    loop_read_modify_write_seq::type_id::destroy(lrmw_seq); // clean-up
  }

 private:
  loop_read_modify_write_seq* lrmw_seq{nullptr};

}; // class test_2m_4s

#endif /* UBUS_TEST_LIB_H_ */
