//----------------------------------------------------------------------
//   Copyright 2019 NXP B.V.
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

#ifndef UBUS_DEFINES_H_
#define UBUS_DEFINES_H_

#include <systemc>
#include <uvm>

// defines and macros

// TODO maximum size for arrays used.
// Replace by dynamic solution
#define MAXSIZE 64

// typedefs

//typedef uvm::uvm_config_db<ubus_if*> ubus_vif_config;
//typedef ubus_if* ubus_vif;

typedef sc_dt::sc_logic logic;
typedef sc_dt::sc_lv<2> logic2;
typedef sc_dt::sc_lv<8> logic8;
typedef sc_dt::sc_lv<16> logic16;

typedef sc_core::sc_signal< sc_dt::sc_logic > s_logic;
typedef sc_core::sc_signal< sc_dt::sc_logic, sc_core::SC_UNCHECKED_WRITERS > s_logic_nchk;
typedef sc_core::sc_signal< sc_dt::sc_logic, sc_core::SC_MANY_WRITERS > s_logic_md;

typedef sc_core::sc_signal< sc_dt::sc_lv<2>, sc_core::SC_MANY_WRITERS > s_logic2_md;
typedef sc_core::sc_signal< sc_dt::sc_lv<8>, sc_core::SC_MANY_WRITERS > s_logic8_md;
typedef sc_core::sc_signal< sc_dt::sc_lv<16>, sc_core::SC_MANY_WRITERS > s_logic16_md;

typedef sc_core::sc_signal< sc_dt::sc_uint<2> > s_uint2;
typedef sc_core::sc_signal< sc_dt::sc_uint<8> > s_uint8;
typedef sc_core::sc_signal< sc_dt::sc_uint<16> > s_uint16;

typedef sc_core::sc_signal< sc_dt::sc_lv<2> > s_logic2;
typedef sc_core::sc_signal< sc_dt::sc_lv<8> > s_logic8;
typedef sc_core::sc_signal< sc_dt::sc_lv<16> > s_logic16;

typedef sc_core::sc_signal_resolved s_logic_res; // incl signal resolution
typedef sc_core::sc_signal_rv<2> s_logic2_res;
typedef sc_core::sc_signal_rv<8> s_logic8_res;
typedef sc_core::sc_signal_rv<16> s_logic16_res;

typedef sc_core::sc_vector<s_logic_md > vs_logic_md;
typedef sc_core::sc_vector<s_logic > vs_logic ;


#endif /* UBUS_DEFINES_H_ */
