// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "Variable.hpp"

#include "../frontend/Distribution.hpp"
#include "../frontend/bitsize_traits_sysc.hpp"

namespace crave {

template <typename T>
struct to_constant_expr<T, typename std::enable_if<is_sysc_dt<T>::value && bitsize_traits<T>::value <= 64>::type> {
  Constant operator()(T value) {
    return Constant(value.to_uint64(), bitsize_traits<T>::value, crave::is_signed<T>::value);
  }
};

template <typename T>
struct to_constant_expr<T, typename std::enable_if<is_sysc_dt<T>::value && bitsize_traits<T>::value >= 65>::type> {
  Constant operator()(T value) {
    // XXX conversion over strings might not be portable
    return Constant(unsigned_int<bitsize_traits<T>::value>(value.to_string(sc_dt::SC_HEX)), bitsize_traits<T>::value, crave::is_signed<T>::value);
  }
};

/**
 * crv_variable for SystemC data types
 */
template <typename T>
class crv_variable<T, typename std::enable_if<is_sysc_dt<T>::value && bitsize_traits<T>::value <= 64>::type> : public crv_variable_base<T> {
  CRV_VARIABLE_COMMON_CONSTRUCTORS(T);
  CRV_VARIABLE_ASSIGNMENT_INTERFACE(T);
  CRV_VARIABLE_ARITHMETIC_INTERFACE(T);
  CRV_VARIABLE_BITWISE_INTERFACE(T);

 public:
  /**
   * randomize variable
   * @return true if success
   */
  bool randomize() override {
    static distribution<uint64_t> dist;
    this->value = dist.nextValue();
    return true;
  }
};

template <typename T>
class crv_variable<T, typename std::enable_if<is_sysc_dt<T>::value && bitsize_traits<T>::value >= 65>::type> : public crv_variable_base<T> {
  CRV_VARIABLE_COMMON_CONSTRUCTORS(T);
  CRV_VARIABLE_ASSIGNMENT_INTERFACE(T);
  CRV_VARIABLE_ARITHMETIC_INTERFACE(T);
  CRV_VARIABLE_BITWISE_INTERFACE(T);

  static_assert(bitsize_traits<T>::value <= bitsize_traits<underlying_type>::value, "requested bit width is too big");

 public:
  /**
   * randomize variable
   * @return true if success
   */
  bool randomize() override {
    using unsigned_type = unsigned_int<bitsize_traits<T>::value>;
    static distribution<unsigned_type> dist;
    unsigned_type val = dist.nextValue();
    std::string bin(bitsize_traits<T>::value, '0');
    for (unsigned i = 0; i < bin.length(); i++)
      if ((val >> i) & 1) bin[bin.length() - i - 1] = '1';
    // XXX conversion over strings might not be portable
    this->value = bin.c_str();
    return true;
  }
};

}  // namespace crave
