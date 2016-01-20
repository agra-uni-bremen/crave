// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "Object.hpp"

#include "../frontend/Constraint.hpp"
#include "../frontend/Distribution.hpp"
#include "../ir/UserExpression.hpp"

namespace crave {

template<typename T>
distribution_tag<T> make_distribution(weighted_range<T> const& range) { return distribution<T>::create(range); }

template<typename T, typename... Args>
distribution_tag<T> make_distribution(weighted_range<T> const& range, Args... args) {
  return make_distribution(args...)(range);
}

struct crv_variable_base_ : public crv_object {
  virtual Constant constant_expr() = 0;
  virtual expression bound_expr() = 0;
  virtual unsigned id() = 0;
  std::string kind() override final { return "crv_variable"; }
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
  ReadReference<T> const& prev() const { return ref; }
  unsigned id() override { return var.id(); }
  void bind(crv_variable_base& other) { bound_var = &other; }
  void unbind() { bound_var = nullptr; }
  expression bound_expr() { return bound_var ? make_expression(var == bound_var->var) : value_to_expression(true); }
  Constant constant_expr() override { return to_constant_expr<T>()(actual_value()); }

 protected:
  crv_variable_base() : var(&value), ref(value), value(), bound_var() {}
  crv_variable_base(const crv_variable_base& other)
      : var(&value), ref(value), value(other.value), bound_var(other.bound_var), crv_variable_base_(other) {}

  T actual_value() const { return bound_var ? bound_var->value : value; }

  WriteReference<T> var;
  ReadReference<T> ref;
  T value;
  crv_variable_base* bound_var;
};

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

}  // namespace crave
