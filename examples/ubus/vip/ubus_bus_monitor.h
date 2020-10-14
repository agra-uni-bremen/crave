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

#ifndef UBUS_BUS_MONITOR_H_
#define UBUS_BUS_MONITOR_H_

#include <systemc>
#include <uvm>

#include "ubus_bus_status.h"
#include "ubus_transfer.h"

//forward class declaration
class ubus_if;
class slave_address_map_info;

//----------------------------------------------------------------------
// CLASS: ubus_bus_monitor
//----------------------------------------------------------------------

class ubus_bus_monitor : public uvm::uvm_monitor
{
public:
  // Analysis ports for the item_collected and state notifier.
  uvm::uvm_analysis_port<ubus_transfer> item_collected_port;
  uvm::uvm_analysis_port<ubus_status> state_port;

  // Transfer collected covergroup
  /* TODO
  covergroup cov_trans @cov_transaction;
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

  // Transfer collected data covergroup
  covergroup cov_trans_beat @cov_transaction_beat;
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

  UVM_COMPONENT_UTILS(ubus_bus_monitor);

  /*
    `uvm_field_int(checks_enable, UVM_DEFAULT)
    `uvm_field_int(coverage_enable, UVM_DEFAULT)
    `uvm_field_int(num_transactions, UVM_DEFAULT)
    `uvm_field_aa_object_string(slave_addr_map, UVM_DEFAULT)
  */

  ubus_bus_monitor(uvm::uvm_component_name name);
  void set_slave_configs(std::string slave_name, unsigned int min_addr, unsigned int max_addr);
  void build_phase(uvm::uvm_phase& phase);
  void run_phase(uvm::uvm_phase& phase);
  void observe_reset();
  void check_reset_on_posedge();
  void check_reset_on_negedge();

protected:

  virtual void collect_transactions();

  void collect_arbitration_phase();
  void collect_address_phase();
  void collect_data_phase();
  void check_which_slave();
  void perform_transfer_checks();
  void check_transfer_size();
  void check_transfer_data_size();
  void perform_transfer_coverage();

  // The virtual interface used to view HDL signals.
  ubus_if* vif;

  // Property indicating the number of transactions occuring on the ubus.
  unsigned int num_transactions; // default = 0;

  // The following two bits are used to control whether checks and coverage are
  // done both in the bus monitor class and the interface.
  bool checks_enable;  // default true
  bool coverage_enable; // default true

  // The state of the ubus
  ubus_status status;

  // The following property is used to store slave address map
  std::map<std::string, slave_address_map_info*> slave_addr_map;
  typedef std::map<std::string, slave_address_map_info*>::iterator slave_addr_map_it;

  // The following property holds the transaction information currently
  // being captured (by the collect_address_phase and data_phase methods).
  ubus_transfer trans_collected;

  // Events needed to trigger covergroups
  sc_core::sc_event cov_transaction;
  sc_core::sc_event cov_transaction_beat;

  // Fields to hold trans data and wait_state. No coverage of dynamic arrays.
  sc_dt::sc_bv<16> addr;
  sc_dt::sc_bv<8> data;

  unsigned int wait_state;
};

#endif /* UBUS_BUS_MONITOR_H_ */
