//----------------------------------------------------------------------
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

#ifndef UBUS_IF_H_
#define UBUS_IF_H_

#include <systemc>
#include <uvm>

#include "ubus_defines.h"

//----------------------------------------------------------------------
//  Class: ubus_if
//----------------------------------------------------------------------

class ubus_if : sc_core::sc_module 
{
 public:

  SC_HAS_PROCESS(ubus_if);

  ubus_if(sc_core::sc_module_name name)
  : sc_core::sc_module(name),
    sig_clock("sig_clock"),
    sig_reset("sig_reset"),
    sig_request("sig_request",2),
    sig_grant("sig_grant",2),
    sig_addr("sig_addr"),
    sig_size("sig_size"),
    sig_read("sig_read"),
    sig_write("sig_write"),
    sig_start("sig_start"),
    sig_bip("sig_bip"),
    sig_data("sig_data"),
    sig_data_out("sig_data_out"),
    sig_wait("sig_wait"),
    sig_error("sig_error"),
    rw("rw"),
    has_checks(true),
    has_coverage(true)
    {
      SC_THREAD(proc);

      SC_THREAD(sync);
      sensitive << sig_clock.default_event()
                << sig_reset.default_event()
                << sig_request[0].default_event() << sig_request[1].default_event()
                << sig_grant[0].default_event() << sig_grant[1].default_event()
                << sig_addr.default_event()
                << sig_size.default_event()
                << sig_read.default_event()
                << sig_write.default_event()
                << sig_start.default_event()
                << sig_bip.default_event()
                << sig_data.default_event()
                << sig_data_out.default_event()
                << sig_wait.default_event()
                << sig_error.default_event()
                << rw.default_event();
    }
  
  // Actual Signals
  s_logic sig_clock;
  s_logic sig_reset;
  
  vs_logic_md sig_request;
  vs_logic sig_grant;

  s_logic16_res sig_addr; // TODO: check: requires resolution as multiple master drivers write to this signal at the same time
  s_logic2_res sig_size; // TODO: check: requires resolution as multiple master drivers write to this signal at the same time

  s_logic_res sig_read; // TODO: check: requires resolution as multiple master drivers write to this signal at the same time
  s_logic_res sig_write; // TODO: check: requires resolution as multiple master drivers write to this signal at the same time
  s_logic sig_start;
  s_logic_res sig_bip;

  s_logic8 sig_data;
  s_logic8_res sig_data_out; // TODO: check: requires resolution as multiple master drivers write to this signal at the same time

  s_logic_res sig_wait; // TODO: check: requires resolution as multiple slave drivers write to this signal at the same time
  s_logic_res sig_error; // TODO: check: requires resolution as multiple slave drivers write to this signal at the same time

  s_logic_res rw; // TODO: check: requires resolution as multiple slave drivers write to this signal at the same time

  // Control flags
  bool has_checks;
  bool has_coverage;

  //assign sig_data = rw ? sig_data_out : 8'bz;
  void proc()
  {
    while(true) // forever
    {
      sc_core::wait(rw.default_event());

      if ((rw == sc_dt::SC_LOGIC_1) || (rw == sc_dt::SC_LOGIC_X)) // 1 or X
        sig_data = sig_data_out;
      else
        if(rw == sc_dt::SC_LOGIC_0) // 0
        sig_data = "zzzzzzzz";
    }
  }

  void sync()
  {
    sc_core::wait(sc_core::SC_ZERO_TIME);
  }

/* Coverage and assertions to be implemented here.

always @(negedge sig_clock)
begin

// Address must not be X or Z during Address Phase
assertAddrUnknown:assert property (
                  disable iff(!has_checks) 
                  ($onehot(sig_grant) |-> !$isunknown(sig_addr)))
                  else
                    $error("ERR_ADDR_XZ\n Address went to X or Z \
                            during Address Phase");

// Read must not be X or Z during Address Phase
assertReadUnknown:assert property ( 
                  disable iff(!has_checks) 
                  ($onehot(sig_grant) |-> !$isunknown(sig_read)))
                  else
                    $error("ERR_READ_XZ\n READ went to X or Z during \
                            Address Phase");

// Write must not be X or Z during Address Phase
assertWriteUnknown:assert property ( 
                   disable iff(!has_checks) 
                   ($onehot(sig_grant) |-> !$isunknown(sig_write)))
                   else
                     $error("ERR_WRITE_XZ\n WRITE went to X or Z during \
                             Address Phase");

// Size must not be X or Z during Address Phase
assertSizeUnknown:assert property ( 
                  disable iff(!has_checks) 
                  ($onehot(sig_grant) |-> !$isunknown(sig_size)))
                  else
                    $error("ERR_SIZE_XZ\n SIZE went to X or Z during \
                            Address Phase");


// Wait must not be X or Z during Data Phase
assertWaitUnknown:assert property ( 
                  disable iff(!has_checks) 
                  ($onehot(sig_grant) |=> !$isunknown(sig_wait)))
                  else
                    $error("ERR_WAIT_XZ\n WAIT went to X or Z during \
                            Data Phase");


// Error must not be X or Z during Data Phase
assertErrorUnknown:assert property ( 
                   disable iff(!has_checks) 
                   ($onehot(sig_grant) |=> !$isunknown(sig_error)))
                   else
                    $error("ERR_ERROR_XZ\n ERROR went to X or Z during \
                            Data Phase");


//Reset must be asserted for at least 3 clocks each time it is asserted
assertResetFor3Clocks: assert property (
                       disable iff(!has_checks) 
                       ($rose(sig_reset) |=> sig_reset[*2]))
                       else 
                         $error("ERR_SHORT_RESET_DURING_TEST\n",
                                "Reset was asserted for less than 3 clock \
                                 cycles");

// Only one grant is asserted
//assertSingleGrant: assert property (
//                   disable iff(!has_checks)
//                   (sig_start |=> $onehot0(sig_grant)))
//                   else
//                     $error("ERR_GRANT\n More that one grant asserted");

// Read and write never true at the same time
assertReadOrWrite: assert property (
                   disable iff(!has_checks) 
                   ($onehot(sig_grant) |-> !(sig_read && sig_write)))
                   else
                     $error("ERR_READ_OR_WRITE\n Read and Write true at \
                             the same time");

end

*/

}; // class ubus_if


#endif /* UBUS_IF_H_ */
