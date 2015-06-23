// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <vector>

#include "Object.hpp"

#include "../frontend/Constraint.hpp"
#include "../frontend/Distribution.hpp"
#include "../ir/UserExpression.hpp"

namespace crave {

template <typename T>
class crv_variable;

struct crv_variable_base_ : public crv_object {
  virtual Constant constant_expr() = 0;
  virtual expression bound_expr() = 0;
  virtual unsigned id() = 0;
  std::string kind() override { return "crv_variable"; }
};

template <typename T>
class crv_variable_base : public crv_variable_base_ {
 public:
  operator T() const { return actual_value(); }
  friend std::ostream& operator<<(std::ostream& os, const crv_variable_base<T>& e) {
    os << e.actual_value();
    return os;
  }
  WriteReference<T> const& operator()() const { return var; }
  unsigned id() override { return var.id(); }
  Constant constant_expr() override {
    unsigned width = bitsize_traits<T>::value;
    bool sign = crave::is_signed<T>::value;
    return Constant(actual_value(), width, sign);
  }
  void bind(crv_variable_base& other) { bound_var = &other; }
  void unbind() { bound_var = nullptr; }
  expression bound_expr() { return bound_var ? make_expression(var == bound_var->var) : value_to_expression(true); }

 protected:
  crv_variable_base() : var(&value), value(), bound_var() {}
  crv_variable_base(const crv_variable_base& other)
      : var(&value), value(other.value), bound_var(other.bound_var), crv_variable_base_(other) {}

  T actual_value() const { return bound_var ? bound_var->value : value; }

  WriteReference<T> var;
  T value;
  crv_variable_base<T>* bound_var;
};

#define CRV_VARIABLE_COMMON_INTERFACE(Typename)                                   \
 public:                                                                          \
  crv_variable(crv_object_name name = "var") {}                                   \
  crv_variable(const crv_variable& other) : crv_variable_base<Typename>(other) {} \
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

#define CRV_VARIABLE_PRIM_INTERFACE(Typename) \
 public:                                      \
  bool randomize() override {                 \
    static distribution<Typename> dist;       \
    value = dist.nextValue();                 \
    return true;                              \
  }

template <>
class crv_variable<bool> : public crv_variable_base<bool> {
  CRV_VARIABLE_COMMON_INTERFACE(bool);
  CRV_VARIABLE_PRIM_INTERFACE(bool);
  CRV_VARIABLE_BITWISE_INTERFACE(bool);
};

// for all C/C++ built-in integer types
#define CRV_VARIABLE_INTEGER_TYPE(typename)                           \
  template <>                                                         \
  class crv_variable<typename> : public crv_variable_base<typename> { \
    CRV_VARIABLE_COMMON_INTERFACE(typename);                          \
    CRV_VARIABLE_PRIM_INTERFACE(typename);                            \
    CRV_VARIABLE_ARITHMETIC_INTERFACE(typename);                      \
    CRV_VARIABLE_BITWISE_INTERFACE(typename);                         \
  };

CRV_VARIABLE_INTEGER_TYPE(int);
CRV_VARIABLE_INTEGER_TYPE(unsigned int);
CRV_VARIABLE_INTEGER_TYPE(char);
CRV_VARIABLE_INTEGER_TYPE(signed char);
CRV_VARIABLE_INTEGER_TYPE(unsigned char);
CRV_VARIABLE_INTEGER_TYPE(short);
CRV_VARIABLE_INTEGER_TYPE(unsigned short);
CRV_VARIABLE_INTEGER_TYPE(long);
CRV_VARIABLE_INTEGER_TYPE(unsigned long);
CRV_VARIABLE_INTEGER_TYPE(long long);
CRV_VARIABLE_INTEGER_TYPE(unsigned long long);

}  // namespace crave
