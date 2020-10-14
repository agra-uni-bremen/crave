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

#ifndef UBUS_SLAVE_MONITOR_H_
#define UBUS_SLAVE_MONITOR_H_

#include <systemc>
#include <uvm>

#include "ubus_transfer.h"
#include "ubus_if.h"

//------------------------------------------------------------------------------
// Class: ubus_slave_monitor
//------------------------------------------------------------------------------

class ubus_slave_monitor : public uvm::uvm_monitor
{
public:
  uvm::uvm_analysis_port<ubus_transfer> item_collected_port;
  uvm::uvm_blocking_peek_imp<ubus_transfer, ubus_slave_monitor> addr_ph_imp;

  // new - constructor
  ubus_slave_monitor(uvm::uvm_component_name name);

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

  // TODO Transfer collected data covergroup
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
  UVM_COMPONENT_UTILS(ubus_slave_monitor);

  void build_phase(uvm::uvm_phase& phase);

  void set_addr_range( unsigned int min_addr_, unsigned int max_addr_);

  unsigned int get_min_addr();
  unsigned int get_max_addr();
  virtual void run_phase(uvm::uvm_phase& phase);
  virtual void report_phase(uvm::uvm_phase& phase);

  //void peek(ubus_transfer& trans);
  ubus_transfer peek();

protected:

  virtual void collect_transactions();
  virtual bool check_addr_range();
  virtual void collect_address_phase();
  virtual void collect_data_phase();
  void perform_transfer_checks();
  void check_transfer_size();
  void check_transfer_data_size();
  void perform_transfer_coverage();

  // This property is the virtual interface needed for this component to drive
  // and view HDL signals.
  ubus_if* vif;

  // The following two unsigned integer properties are used by
  // check_addr_range() method to detect if a transaction is for this target.
  unsigned int min_addr;
  unsigned int max_addr;

  // The following two bits are used to control whether checks and coverage are
  // done both in the monitor class and the interface.
  bool checks_enable;
  bool coverage_enable;

  // The following property holds the transaction information currently
  // begin captured (by the collect_address_phase and data_phase methods).
  ubus_transfer trans_collected;

  // monitor notifier that the address phase (and full item) has been collected
  sc_core::sc_event address_phase_grabbed;

  // Events needed to trigger covergroups
  sc_core::sc_event cov_transaction;
  sc_core::sc_event cov_transaction_beat;

  // Fields to hold trans data and wait_state.  No coverage of dynamic arrays.
  sc_dt::sc_bv<16> addr;
  sc_dt::sc_bv<8> data;
  unsigned int wait_state;


}; // class ubus_slave_monitor

#endif /* UBUS_SLAVE_MONITOR_H_ */

