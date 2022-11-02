// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include <sysc/datatypes/bit/sc_bv.h>
#include <sysc/datatypes/bit/sc_lv.h>
#include <sysc/datatypes/int/sc_int.h>
#include <sysc/datatypes/int/sc_uint.h>

#include "bitsize_traits.hpp"

namespace crave {

/**
 * define various type traits for SystemC data types
 */
template <int N>
struct is_signed<sc_dt::sc_bv<N> > : std::false_type {};

template <int N>
struct is_signed<sc_dt::sc_int<N> > : std::true_type {};

template <int N>
struct is_signed<sc_dt::sc_uint<N> > : std::false_type {};

template <int N>
struct is_sysc_dt<sc_dt::sc_bv<N> > : std::true_type {};

template <int N>
struct is_sysc_dt<sc_dt::sc_int<N> > : std::true_type {};

template <int N>
struct is_sysc_dt<sc_dt::sc_uint<N> > : std::true_type {};

template <class SCDT>
struct sc_dt_width {};

template <template <int> class SCDT, int N>
struct sc_dt_width<SCDT<N> > : std::integral_constant<std::size_t, N> {};

template <typename T>
struct bitsize_traits<T, typename std::enable_if<is_sysc_dt<T>::value>::type> : sc_dt_width<T> {};

}  // namespace crave
