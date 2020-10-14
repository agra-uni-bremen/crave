/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/


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
struct is_signed<sc_dt::sc_bv<N> > : boost::mpl::false_ {};

template <int N>
struct is_signed<sc_dt::sc_int<N> > : boost::mpl::true_ {};

template <int N>
struct is_signed<sc_dt::sc_uint<N> > : boost::mpl::false_ {};

template <typename T, int N = 0>
struct is_sysc_dt : boost::mpl::false_ {};

template <int N>
struct is_sysc_dt<sc_dt::sc_bv<N> > : boost::mpl::true_ {};

template <int N>
struct is_sysc_dt<sc_dt::sc_int<N> > : boost::mpl::true_ {};

template <int N>
struct is_sysc_dt<sc_dt::sc_uint<N> > : boost::mpl::true_ {};

template <class SCDT>
struct sc_dt_width {};

template <template <int> class SCDT, int N>
struct sc_dt_width<SCDT<N> > : boost::mpl::int_<N> {};

template <typename T>
struct bitsize_traits<T, typename boost::enable_if<is_sysc_dt<T> >::type> : sc_dt_width<T> {};

}  // namespace crave
