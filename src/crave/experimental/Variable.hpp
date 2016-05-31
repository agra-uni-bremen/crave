// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once


#include "VariableBase.hpp"

#include "../frontend/Distribution.hpp"
#include "../ir/UserExpression.hpp"

namespace crave {

template<typename T>
distribution_tag<T> make_distribution(weighted_range<T> const& range) { return distribution<T>::create(range); }

template<typename T, typename... Args>
distribution_tag<T> make_distribution(weighted_range<T> const& range, Args... args) {
  return make_distribution(args...)(range);
}


#define CRV_VARIABLE_COMMON_CONSTRUCTORS(Typename)                                \
 public:                                                                          \
  crv_variable(crv_object_name name = "var") {}                                   \
  crv_variable(const crv_variable& other) : crv_variable_base<Typename>(other) {} \

#define CRV_VARIABLE_ASSIGNMENT_INTERFACE(Typename)                               \
 public:                                                                          \
  crv_variable<Typename>& operator=(const crv_variable<Typename>& i) {            \
    this->value = i.value;                                                        \
    return *this;                                                                 \
  }                                                                               \
  crv_variable<Typename>& operator=(Typename i) {                                 \
    this->value = i;                                                              \
    return *this;                                                                 \
  }

#define CRV_VARIABLE_ARITHMETIC_INTERFACE(Typename) \
 public:                                            \
  crv_variable<Typename>& operator++() {            \
    ++(this->value);                                \
    return *this;                                   \
  }                                                 \
  Typename operator++(int) {                        \
    Typename tmp = this->value;                     \
    ++(this->value);                                \
    return tmp;                                     \
  }                                                 \
  crv_variable<Typename>& operator--() {            \
    --(this->value);                                \
    return *this;                                   \
  }                                                 \
  Typename operator--(int) {                        \
    Typename tmp = this->value;                     \
    --(this->value);                                \
    return tmp;                                     \
  }                                                 \
  crv_variable<Typename>& operator+=(Typename i) {  \
    this->value += i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator-=(Typename i) {  \
    this->value -= i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator*=(Typename i) {  \
    this->value *= i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator/=(Typename i) {  \
    this->value /= i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator%=(Typename i) {  \
    this->value %= i;                               \
    return *this;                                   \
  }

#define CRV_VARIABLE_BITWISE_INTERFACE(Typename)    \
 public:                                            \
  crv_variable<Typename>& operator&=(Typename i) {  \
    this->value &= i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator|=(Typename i) {  \
    this->value |= i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator^=(Typename i) {  \
    this->value ^= i;                               \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator<<=(Typename i) { \
    this->value <<= i;                              \
    return *this;                                   \
  }                                                 \
  crv_variable<Typename>& operator>>=(Typename i) { \
    this->value >>= i;                              \
    return *this;                                   \
  }

template <typename T, typename Enable = void> 
class crv_variable { };

template <typename T>
class crv_variable<T, typename std::enable_if<std::is_integral<T>::value>::type> : public crv_variable_base<T> {
  CRV_VARIABLE_COMMON_CONSTRUCTORS(T);
  CRV_VARIABLE_ASSIGNMENT_INTERFACE(T);                      
  CRV_VARIABLE_ARITHMETIC_INTERFACE(T);                      
  CRV_VARIABLE_BITWISE_INTERFACE(T);

 public:                                            \
  bool randomize() override {
    static distribution<T> dist;
    this->value = dist.nextValue();  
    return true;
  }
};

template <typename T>
struct bitsize_traits<crv_variable<T>> : public bitsize_traits<T> {};

}  // namespace crave
