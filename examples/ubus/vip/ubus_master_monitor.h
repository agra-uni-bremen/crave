//----------------------------------------------------------------------
//   Copyright 2016 NXP B.V.
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

#ifndef UBUS_MASTER_MONITOR_H_
#define UBUS_MASTER_MONITOR_H_

#include <systemc>
#include <uvm>

#include "ubus_if.h"
#include "ubus_transfer.h"

// forward class declaration


//------------------------------------------------------------------------------
// Class: ubus_master_monitor
//------------------------------------------------------------------------------

class ubus_master_monitor : public uvm::uvm_monitor
{
public:

  uvm::uvm_analysis_port<ubus_transfer> item_collected_port;

  // TODO Transfer collected covergroup
  /*
  covergroup cov_trans;
    option.per_instance = 1;
    trans_start_addr : coverpoint trans_collected.addr {
      option.auto_bin_max = 16; }
    trans_dir : coverpoint trans_collected.read_write;
    trans_size : coverpoint trans_collected.size {
      bins sizes[] = {1, 2, 4, 8};
      illegal_bins invalid_sizes = default; }
    trans_addrXdir : cross trans_start_addr, trans_dir;
    trans_dirXsize : cross trans_dir, trans_size;
  endgroup : cov_trans
  */

  // TODO Transfer collected beat covergroup
  /*
  covergroup cov_trans_beat;
    option.per_instance = 1;
    beat_addr : coverpoint addr {
      option.auto_bin_max = 16; }
    beat_dir : coverpoint trans_collected.read_write;
    beat_data : coverpoint data {
      option.auto_bin_max = 8; }
    beat_wait : coverpoint wait_state {
      bins waits[] = { [0:9] };
      bins others = { [10:$] }; }
    beat_addrXdir : cross beat_addr, beat_dir;
    beat_addrXdata : cross beat_addr, beat_data;
  endgroup : cov_trans_beat

  */
  // Provide implementations of virtual methods such as get_type_name and create
  UVM_COMPONENT_UTILS(ubus_master_monitor);

  // constructor
  ubus_master_monitor( uvm::uvm_component_name name);

  void build_phase(uvm::uvm_phase& phase);
  void run_phase(uvm::uvm_phase& phase);

protected:

  virtual void collect_transactions();
  virtual void collect_arbitration_phase();
  virtual void collect_address_phase();
  virtual void collect_data_phase();
  virtual void perform_transfer_checks();
  virtual void check_transfer_size();
  virtual void check_transfer_data_size();
  virtual void perform_transfer_coverage();
  virtual void report_phase(uvm::uvm_phase& phase);

  // data members
private:

  // This property is the virtual interfaced needed for this component to drive
  // and view HDL signals.
  ubus_if* vif;

  // Master Id
  int master_id;

  // The following property holds the transaction information currently
  // begin captured (by the collect_address_phase and data_phase methods).
  ubus_transfer trans_collected;

  // Fields to hold trans addr, data and wait_state.
  sc_dt::sc_uint<16> addr;
  sc_dt::sc_uint<8> data;
  unsigned int wait_state;

  // The following two bits are used to control whether checks and coverage are
  // done both in the monitor class and the interface.
  bool checks_enable;
  bool coverage_enable;

}; // class ubus_master_monitor

#endif /* UBUS_MASTER_MONITOR_H_ */
