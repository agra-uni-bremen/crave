// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include <vector>

#include "Distribution.hpp"
#include "RandvInterface.hpp"
#include "bitsize_traits_sysc.hpp"

namespace crave {

/**
 * create constrant expression from a value of SystemC data types
 */
template <typename T>
struct to_constant_expr<T, typename std::enable_if<is_sysc_dt<T>::value>::type> {
  Constant operator()(T value) {
    return Constant(value.to_uint64(), bitsize_traits<T>::value, crave::is_signed<T>::value);
  }
};

/**
 * Implements crave::randv for SystemC data types
 */
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

/**
 * binary operators for randv
 */
#define RANDV_SCDT_BINARY_OPERATOR(Typename, BinOp)                                 \
  template <int N, typename T>                                                      \
  sc_dt::int64 operator BinOp(randv<Typename<N> > const& r, T const& i) {           \
    return ((Typename<N>)r)BinOp i;                                                 \
  }                                                                                 \
  template <int N>                                                                  \
  sc_dt::int64 operator BinOp(sc_dt::int64 const i, randv<Typename<N> > const& r) { \
    return i BinOp((Typename<N>)r);                                                 \
  }

/**
 * arithmetic operators for randv
 */
#define RANDV_SCDT_VALUE_OPERATORS(Typename)             \
  RANDV_SCDT_BINARY_OPERATOR(Typename, +);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, -);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, *);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, /);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, %);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, &);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, |);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, ^);               \
  RANDV_SCDT_BINARY_OPERATOR(Typename, <<);              \
  RANDV_SCDT_BINARY_OPERATOR(Typename, >>);              \
  template <int N>                                       \
  sc_dt::int64 operator~(randv<Typename<N> > const& r) { \
    return ~((Typename<N>)r);                            \
  }

/**
 * comparison operators for randv
 */
#define RANDV_SCDT_COMPARISON_OPERATORS(Typename) \
  RANDV_SCDT_BINARY_OPERATOR(Typename, ==);       \
  RANDV_SCDT_BINARY_OPERATOR(Typename, !=);       \
  RANDV_SCDT_BINARY_OPERATOR(Typename, >);        \
  RANDV_SCDT_BINARY_OPERATOR(Typename, >=);       \
  RANDV_SCDT_BINARY_OPERATOR(Typename, <);        \
  RANDV_SCDT_BINARY_OPERATOR(Typename, <=);

/**
 * Define randv variable for sc_bv types
 */
template <int N>
struct randv<sc_dt::sc_bv<N> > : public randv_base<sc_dt::sc_bv<N> > {
  typedef sc_dt::sc_bv<N> sc_bv;
  RANDV_COMMON_INTERFACE(sc_bv);
  RANDV_SCDT_PRIM_INTERFACE(sc_bv);
};
RANDV_SCDT_BINARY_OPERATOR(sc_dt::sc_bv, ==);
RANDV_SCDT_BINARY_OPERATOR(sc_dt::sc_bv, !=);

/**
 * Define randv variables for sc_int types
 */
template <int N>
struct randv<sc_dt::sc_int<N> > : public randv_base<sc_dt::sc_int<N> > {
  typedef sc_dt::sc_int<N> sc_int;
  RANDV_COMMON_INTERFACE(sc_int);
  RANDV_ARITHMETIC_INTERFACE(sc_int);
  RANDV_BITWISE_INTERFACE(sc_int);
  RANDV_SCDT_PRIM_INTERFACE(sc_int);
};
RANDV_SCDT_VALUE_OPERATORS(sc_dt::sc_int);
RANDV_SCDT_COMPARISON_OPERATORS(sc_dt::sc_int);

/**
 * Define randv variables for sc_uint types
 */
template <int N>
struct randv<sc_dt::sc_uint<N> > : public randv_base<sc_dt::sc_uint<N> > {
  typedef sc_dt::sc_uint<N> sc_uint;
  RANDV_COMMON_INTERFACE(sc_uint);
  RANDV_ARITHMETIC_INTERFACE(sc_uint);
  RANDV_BITWISE_INTERFACE(sc_uint);
  RANDV_SCDT_PRIM_INTERFACE(sc_uint);
};
RANDV_SCDT_VALUE_OPERATORS(sc_dt::sc_uint);
RANDV_SCDT_COMPARISON_OPERATORS(sc_dt::sc_uint);

}  // namespace crave
