// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <sysc/datatypes/int/sc_int.h>
#include <sysc/datatypes/int/sc_uint.h>
#include <sysc/datatypes/bit/sc_bv.h>
#include <vector>
#include "bitsize_traits.hpp"
#include "Distribution.hpp"
#include "RandomBase.hpp"

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

#define RANDV_SCDT_PRIM_INTERFACE(Typename)                                                       \
 public:                                                                                          \
  void gather_values(std::vector<int64_t>* ch) { ch->insert(ch->end(), this->value.to_int64()); } \
  bool next() {                                                                                   \
    static distribution<int64_t> dist;                                                            \
    this->value = dist.nextValue();                                                               \
    return true;                                                                                  \
  }                                                                                               \
  template <typename T>                                                                           \
  randv<Typename>& operator=(T i) {                                                               \
    this->value = ((Typename)i);                                                                  \
    return *this;                                                                                 \
  }                                                                                               \
  template <typename T>                                                                           \
  randv<Typename>& operator=(randv<T> const& i) {                                                 \
    this->value = (T)i;                                                                           \
    return *this;                                                                                 \
  }

#define RANDV_SCDT_BINARY_OPERATOR(Typename, BinOp)                                 \
  template <int N, typename T>                                                      \
  sc_dt::int64 operator BinOp(randv<Typename<N> > const& r, T const& i) {           \
    return ((Typename<N>)r)BinOp i;                                                 \
  }                                                                                 \
  template <int N>                                                                  \
  sc_dt::int64 operator BinOp(sc_dt::int64 const i, randv<Typename<N> > const& r) { \
    return i BinOp((Typename<N>)r);                                                 \
  }

#define RANDV_SCDT_VALUE_OPERATORS(Typename)             \
  RANDV_SCDT_BINARY_OPERATOR(Typename, +);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, -);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, *);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, / );              \
  RANDV_SCDT_BINARY_OPERATOR(Typename, % );              \
  RANDV_SCDT_BINARY_OPERATOR(Typename, &);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, | );              \
  RANDV_SCDT_BINARY_OPERATOR(Typename, ^);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, << );             \
  RANDV_SCDT_BINARY_OPERATOR(Typename, >> );             \
  template <int N>                                       \
  sc_dt::int64 operator~(randv<Typename<N> > const& r) { \
    return ~((Typename<N>)r);                            \
  }

#define RANDV_SCDT_COMPARISON_OPERATORS(Typename) \
  RANDV_SCDT_BINARY_OPERATOR(Typename, == );      \
  RANDV_SCDT_BINARY_OPERATOR(Typename, != );      \
  RANDV_SCDT_BINARY_OPERATOR(Typename, > );       \
  RANDV_SCDT_BINARY_OPERATOR(Typename, >= );      \
  RANDV_SCDT_BINARY_OPERATOR(Typename, < );       \
  RANDV_SCDT_BINARY_OPERATOR(Typename, <= );

template <int N>
class randv<sc_dt::sc_bv<N> > : public randv_base<sc_dt::sc_bv<N> > {
  typedef sc_dt::sc_bv<N> sc_bv;
  RANDV_COMMON_INTERFACE(sc_bv);
  RANDV_SCDT_PRIM_INTERFACE(sc_bv);
};
RANDV_SCDT_BINARY_OPERATOR(sc_dt::sc_bv, == );
RANDV_SCDT_BINARY_OPERATOR(sc_dt::sc_bv, != );

template <int N>
class randv<sc_dt::sc_int<N> > : public randv_base<sc_dt::sc_int<N> > {
  typedef sc_dt::sc_int<N> sc_int;
  RANDV_COMMON_INTERFACE(sc_int);
  RANDV_ARITHMETIC_INTERFACE(sc_int);
  RANDV_BITWISE_INTERFACE(sc_int);
  RANDV_SCDT_PRIM_INTERFACE(sc_int);
};
RANDV_SCDT_VALUE_OPERATORS(sc_dt::sc_int);
RANDV_SCDT_COMPARISON_OPERATORS(sc_dt::sc_int);

template <int N>
class randv<sc_dt::sc_uint<N> > : public randv_base<sc_dt::sc_uint<N> > {
  typedef sc_dt::sc_uint<N> sc_uint;
  RANDV_COMMON_INTERFACE(sc_uint);
  RANDV_ARITHMETIC_INTERFACE(sc_uint);
  RANDV_BITWISE_INTERFACE(sc_uint);
  RANDV_SCDT_PRIM_INTERFACE(sc_uint);
};
RANDV_SCDT_VALUE_OPERATORS(sc_dt::sc_uint);
RANDV_SCDT_COMPARISON_OPERATORS(sc_dt::sc_uint);

}  // namespace crave

//  vim: ft=cpp:ts=2:sw=2:expandtab
