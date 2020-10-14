//----------------------------------------------------------------------
//   Copyright 2016-2019 NXP B.V.
//   Copyright 2007-2010 Mentor Graphics Corporation
//   Copyright 2007-2010 Cadence Design Systems, Inc.
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

//This is dummy DUT.

#ifndef DUT_DUMMY_H_
#define DUT_DUMMY_H_

#include <systemc>

class dut_dummy : public sc_core::sc_module
{
 public:
  sc_core::sc_in<sc_dt::sc_logic>     ubus_req_master_0;
  sc_core::sc_out<sc_dt::sc_logic>    ubus_gnt_master_0;
  sc_core::sc_in<sc_dt::sc_logic>     ubus_req_master_1;
  sc_core::sc_out<sc_dt::sc_logic>    ubus_gnt_master_1;
  sc_core::sc_in<sc_dt::sc_logic>     ubus_clock;
  sc_core::sc_in<sc_dt::sc_logic>     ubus_reset;
  sc_core::sc_in<sc_dt::sc_lv<16> >   ubus_addr;
  sc_core::sc_in<sc_dt::sc_lv<2> >    ubus_size;
  sc_core::sc_out<sc_dt::sc_logic>    ubus_read;
  sc_core::sc_out<sc_dt::sc_logic>    ubus_write;
  sc_core::sc_out<sc_dt::sc_logic>    ubus_start;
  sc_core::sc_in<sc_dt::sc_logic>     ubus_bip;
  sc_core::sc_inout<sc_dt::sc_lv<8> > ubus_data;
  sc_core::sc_in<sc_dt::sc_logic>     ubus_wait;
  sc_core::sc_in<sc_dt::sc_logic>     ubus_error;

  // Basic arbiter, supports two masters, 0 has priority over 1

  SC_HAS_PROCESS(dut_dummy);

  dut_dummy(sc_core::sc_module_name name)
  : sc_module(name),
    ubus_req_master_0("ubus_req_master_0"),
    ubus_gnt_master_0("ubus_gnt_master_0"),
    ubus_req_master_1("ubus_req_master_1"),
    ubus_gnt_master_1("ubus_gnt_master_1"),
    ubus_clock("ubus_clock"),
    ubus_reset("ubus_reset"),
    ubus_addr("ubus_addr"),
    ubus_size("ubus_size"),
    ubus_read("ubus_read"),
    ubus_write("ubus_write"),
    ubus_start("ubus_start"),
    ubus_bip("ubus_bip"),
    ubus_data("ubus_data"),
    ubus_wait("ubus_wait"),
    ubus_error("ubus_error")
  {
    SC_THREAD(proc0); //@(posedge ubus_clock or posedge ubus_reset)

    SC_THREAD(proc1); // @(negedge ubus_clock or posedge ubus_reset)

    SC_THREAD(proc2); // @(posedge ubus_clock or posedge ubus_reset)
  }

  void proc0()
  {
    while(true)
    {
      wait( ubus_clock.posedge_event() | ubus_reset.posedge_event() );

      if(ubus_reset.read() == sc_dt::SC_LOGIC_1)
      {
        ubus_start = sc_dt::SC_LOGIC_0;
        state = 0;
      }
      else
      {
        switch(state)
        {
        case 0: // Begin out of Reset
          ubus_start = sc_dt::SC_LOGIC_1;
          state = 3;
          break;
        case 3: // Start state
          ubus_start = sc_dt::SC_LOGIC_0;
          if((ubus_gnt_master_0.read() == sc_dt::SC_LOGIC_0) && (ubus_gnt_master_1.read() == sc_dt::SC_LOGIC_0))
            state = 4;
          else
            state = 1;
          break;
        case 4: // No-op state
          ubus_start = sc_dt::SC_LOGIC_1;
          state = 3;
          break;
        case 1: // Addr state
          state = 2;
          ubus_start = sc_dt::SC_LOGIC_0;
          break;
        case 2: // Data state
          if( (ubus_error == sc_dt::SC_LOGIC_1) || ((ubus_bip == sc_dt::SC_LOGIC_0) &&
              (ubus_wait == sc_dt::SC_LOGIC_0))) {
            state = 3;
            ubus_start = sc_dt::SC_LOGIC_1;
          }
          else {
            state = 2;
            ubus_start = sc_dt::SC_LOGIC_0;
          }
          break;
        default: break; // TODO default?
        } // switch state
      } // else
    } // while
  }

  void proc1()
  {
    while(true) // forever
    {
      wait( ubus_clock.negedge_event() | ubus_reset.posedge_event() );

      if(ubus_reset.read() == sc_dt::SC_LOGIC_1)
      {
        ubus_gnt_master_0 = sc_dt::SC_LOGIC_0;
        ubus_gnt_master_1 = sc_dt::SC_LOGIC_0;
      }
      else
      {
        if( (ubus_start.read() == sc_dt::SC_LOGIC_1) &&
            (ubus_req_master_0.read() == sc_dt::SC_LOGIC_1) )
        {
          ubus_gnt_master_0 = sc_dt::SC_LOGIC_1;
          ubus_gnt_master_1 = sc_dt::SC_LOGIC_0;
        }
        else
          if( (ubus_start.read() == sc_dt::SC_LOGIC_1) &&
              !(ubus_req_master_0.read() == sc_dt::SC_LOGIC_1) &&
              (ubus_req_master_1.read() == sc_dt::SC_LOGIC_1) )
        {
          ubus_gnt_master_0 = sc_dt::SC_LOGIC_0;
          ubus_gnt_master_1 = sc_dt::SC_LOGIC_1;
        }
        else {
          ubus_gnt_master_0 = sc_dt::SC_LOGIC_0;
          ubus_gnt_master_1 = sc_dt::SC_LOGIC_0;
        }
      }
    }

  }

  void proc2()
  {
    while(true) // forever
    {
      wait( ubus_clock.posedge_event() | ubus_reset.posedge_event() );

      if(ubus_reset.read() == sc_dt::SC_LOGIC_1)
      {
        ubus_read = sc_dt::SC_LOGIC_Z;
        ubus_write = sc_dt::SC_LOGIC_Z;
      }
      else
      {
        if( (ubus_start.read() == sc_dt::SC_LOGIC_1) &&
            !(ubus_gnt_master_0.read() == sc_dt::SC_LOGIC_1) &&
            !(ubus_gnt_master_1.read() == sc_dt::SC_LOGIC_1))
        {
          ubus_read = sc_dt::SC_LOGIC_0;
          ubus_write = sc_dt::SC_LOGIC_0;
        }
        else
        {
          ubus_read = sc_dt::SC_LOGIC_Z;
          ubus_write = sc_dt::SC_LOGIC_Z;
        }
      }

    }
  }

private:
  // internal state
  int state;

}; // class dut_dummy

#endif /* DUT_DUMMY_H_ */
