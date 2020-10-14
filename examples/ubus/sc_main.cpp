//----------------------------------------------------------------------
//   Copyright (c) 2019-2020 University of Bremen, Germany. 
//   Copyright (c) 2019-2020 DFKI GmbH Bremen, Germany.
//   Copyright (c) 2020 Johannes Kepler University Linz, Austria.
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

#include <systemc>
#include <uvm>

#include "vip/ubus.h"
#include "vip/ubus_if.h"

#include "dut_dummy.h"
#include "test_lib.h" 

#define UBUS_ADDR_WIDTH 16

class stim : public sc_core::sc_module
{
 public:
  sc_core::sc_out<sc_dt::sc_logic> clock;
  sc_core::sc_out<sc_dt::sc_logic> reset;

  SC_HAS_PROCESS(stim);

  stim(sc_core::sc_module_name nm)
  : sc_module(nm), clock("clock"), reset("reset")
  {
    m_clk = sc_dt::SC_LOGIC_0;
    SC_THREAD(run_reset);
    SC_THREAD(run_clk);
  }

  void run_reset()
  {
    reset.write(sc_dt::SC_LOGIC_0);
    sc_core::wait(sc_core::SC_ZERO_TIME);
    reset.write(sc_dt::SC_LOGIC_1);
    sc_core::wait(51.0, sc_core::SC_NS);
    reset.write(sc_dt::SC_LOGIC_0);
  }

  void run_clk()
  {
    while (true) // forever
    {
      m_clk = ~m_clk;
      clock.write(m_clk);
      sc_core::wait(5.0, sc_core::SC_NS);
    }
  }

 private:
  sc_dt::sc_logic m_clk;
};


int sc_main(int, char*[])
{
  //uvm::uvm_set_verbosity_level(uvm::UVM_FULL);

  crave::init("crave.cfg");
  // Interface instantiation
  ubus_if* vif = new ubus_if("vif");
  assert(vif);

  // Add interface to configuration database
  uvm::uvm_config_db<ubus_if*>::set(uvm::uvm_root::get(), "*", "vif", vif);

  // Instantiate the DUT and bind ports to interface
  dut_dummy dut("dut");
  dut.ubus_req_master_0.bind(vif->sig_request[0]);
  dut.ubus_gnt_master_0.bind(vif->sig_grant[0]);
  dut.ubus_req_master_1.bind(vif->sig_request[1]);
  dut.ubus_gnt_master_1.bind(vif->sig_grant[1]);
  dut.ubus_clock.bind(vif->sig_clock);
  dut.ubus_reset.bind(vif->sig_reset);
  dut.ubus_addr.bind(vif->sig_addr);
  dut.ubus_size.bind(vif->sig_size);
  dut.ubus_read.bind(vif->sig_read);
  dut.ubus_write.bind(vif->sig_write);
  dut.ubus_start.bind(vif->sig_start);
  dut.ubus_bip.bind(vif->sig_bip);
  dut.ubus_data.bind(vif->sig_data);
  dut.ubus_wait.bind(vif->sig_wait);
  dut.ubus_error.bind(vif->sig_error);

  // Clock generator and bind ports to interface
  stim tb_stimuli("stim");
  tb_stimuli.clock.bind(vif->sig_clock);
  tb_stimuli.reset.bind(vif->sig_reset);

  sc_core::sc_trace_file* tf = sc_core::sc_create_vcd_trace_file("traces_sc");
  sc_core::sc_trace(tf, vif->sig_request[0], "vif->sig_request_0");
  sc_core::sc_trace(tf, vif->sig_grant[0],   "vif->sig_grant_0");
  sc_core::sc_trace(tf, vif->sig_request[1], "vif->sig_request_1");
  sc_core::sc_trace(tf, vif->sig_grant[1],   "vif->sig_grant_1");
  sc_core::sc_trace(tf, vif->sig_clock,      "vif->sig_clock");
  sc_core::sc_trace(tf, vif->sig_reset,      "vif->sig_reset");
  sc_core::sc_trace(tf, vif->sig_addr,       "vif->sig_addr");
  sc_core::sc_trace(tf, vif->sig_size,       "vif->sig_size");
  sc_core::sc_trace(tf, vif->sig_read,       "vif->sig_read");
  sc_core::sc_trace(tf, vif->sig_write,      "vif->sig_write");
  sc_core::sc_trace(tf, vif->sig_start,      "vif->sig_start");
  sc_core::sc_trace(tf, vif->sig_bip,        "vif->sig_bip");
  sc_core::sc_trace(tf, vif->sig_data,       "vif->sig_data");
  sc_core::sc_trace(tf, vif->sig_wait,       "vif->sig_wait");
  sc_core::sc_trace(tf, vif->sig_error,      "vif->sig_error");
  sc_core::sc_trace(tf, vif->rw,             "vif->rw");

  // Run test
  uvm::run_test("test_2m_4s");

  //sc_core::sc_close_vcd_trace_file(tf);

  return 0;
}
