// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <vector>

#include "RandBase.hpp"
namespace crave {
#define RANDV_COMMON_INTERFACE(Typename)                             \
 public:                                                             \
  randv(rand_obj_base* parent = 0) : randv_base<Typename>(parent) {} \
  randv(const randv& other) : randv_base<Typename>(other) {}         \
  randv<Typename>& operator=(const randv<Typename>& i) {             \
    this->value = i.value;                                           \
    return *this;                                                    \
  }                                                                  \
  randv<Typename>& operator=(Typename i) {                           \
    this->value = i;                                                 \
    return *this;                                                    \
  }

#define RANDV_ARITHMETIC_INTERFACE(Typename) \
 public:                                     \
  randv<Typename>& operator++() {            \
    ++(this->value);                         \
    return *this;                            \
  }                                          \
  Typename operator++(int) {                 \
    Typename tmp = this->value;              \
    ++(this->value);                         \
    return tmp;                              \
  }                                          \
  randv<Typename>& operator--() {            \
    --(this->value);                         \
    return *this;                            \
  }                                          \
  Typename operator--(int) {                 \
    Typename tmp = this->value;              \
    --(this->value);                         \
    return tmp;                              \
  }                                          \
  randv<Typename>& operator+=(Typename i) {  \
    this->value += i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator-=(Typename i) {  \
    this->value -= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator*=(Typename i) {  \
    this->value *= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator/=(Typename i) {  \
    this->value /= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator%=(Typename i) {  \
    this->value %= i;                        \
    return *this;                            \
  }

#define RANDV_BITWISE_INTERFACE(Typename)    \
  randv<Typename>& operator&=(Typename i) {  \
    this->value &= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator|=(Typename i) {  \
    this->value |= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator^=(Typename i) {  \
    this->value ^= i;                        \
    return *this;                            \
  }                                          \
  randv<Typename>& operator<<=(Typename i) { \
    this->value <<= i;                       \
    return *this;                            \
  }                                          \
  randv<Typename>& operator>>=(Typename i) { \
    this->value >>= i;                       \
    return *this;                            \
  }

#define RANDV_PRIM_INTERFACE(Typename)           \
 public:                                         \
  void gather_values(std::vector<int64_t>& ch) { \
    ch.push_back(static_cast<int64_t>(value));   \
  }                                              \
  bool next() {                                  \
    static distribution<Typename> dist;          \
    value = dist.nextValue();                    \
    return true;                                 \
  }

template <>
class randv<bool> : public randv_base<bool> {
  RANDV_COMMON_INTERFACE(bool)
  RANDV_PRIM_INTERFACE(bool)
  RANDV_BITWISE_INTERFACE(bool)
};

// for all C/C++ built-in integer types
#define RANDV_INTEGER_TYPE(typename)                    \
  template <>                                           \
  class randv<typename> : public randv_base<typename> { \
    RANDV_COMMON_INTERFACE(typename)                    \
    RANDV_PRIM_INTERFACE(typename)                      \
    RANDV_ARITHMETIC_INTERFACE(typename)                \
    RANDV_BITWISE_INTERFACE(typename)                   \
  };

RANDV_INTEGER_TYPE(int)
RANDV_INTEGER_TYPE(unsigned int)
RANDV_INTEGER_TYPE(char)
RANDV_INTEGER_TYPE(signed char)
RANDV_INTEGER_TYPE(unsigned char)
RANDV_INTEGER_TYPE(short)
RANDV_INTEGER_TYPE(unsigned short)
RANDV_INTEGER_TYPE(long)
RANDV_INTEGER_TYPE(unsigned long)
RANDV_INTEGER_TYPE(long long)
RANDV_INTEGER_TYPE(unsigned long long)
}  // namespace crave
