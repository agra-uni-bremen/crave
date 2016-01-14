// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "Variable.hpp"

#include <sysc/datatypes/int/sc_int.h>
#include <sysc/datatypes/int/sc_uint.h>
#include <sysc/datatypes/bit/sc_bv.h>

#include "../frontend/bitsize_traits.hpp"
#include "../frontend/Distribution.hpp"

namespace boost {

template <int N>
struct is_signed<sc_dt::sc_bv<N> > : boost::mpl::false_ {};

template <int N>
struct is_signed<sc_dt::sc_int<N> > : boost::mpl::true_ {};

template <int N>
struct is_signed<sc_dt::sc_uint<N> > : boost::mpl::false_ {};
}  // namespace boost

namespace crave {

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

template <typename T>
struct to_uint64<T, typename boost::enable_if<is_sysc_dt<T> >::type> {
  uint64_t operator()(T const& value) { return value.to_uint64(); }
};

template <typename T>
struct to_constant_expr<T, typename std::enable_if<is_sysc_dt<T>::value>::type> {
  Constant operator()(T value) { 
    constexpr unsigned width = bitsize_traits<T>::value;
    constexpr bool sign = crave::is_signed<T>::value;
    return Constant(value.to_uint64(), width, sign);
  }
};

template <typename T>
class crv_variable<T, typename std::enable_if<is_sysc_dt<T>::value>::type> : public crv_variable_base<T> {
  CRV_VARIABLE_COMMON_CONSTRUCTORS(T);
  CRV_VARIABLE_ASSIGNMENT_INTERFACE(T);                      
  CRV_VARIABLE_ARITHMETIC_INTERFACE(T);                      
  CRV_VARIABLE_BITWISE_INTERFACE(T);

 public:                                  
  bool randomize() override {             
    static distribution<int64_t> dist;    
    this->value = dist.nextValue();       
    return true;                          
  }
};

}  // namespace crave

//  vim: ft=cpp:ts=2:sw=2:expandtab
