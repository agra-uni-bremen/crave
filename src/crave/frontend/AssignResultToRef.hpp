// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.
#pragma once

#include <functional>
#include <string>

#include "../ir/Node.hpp"
#include "AssignResult.hpp"
#include "bitsize_traits.hpp"

namespace crave {

extern std::function<bool(void)> random_bit;

/**
 * Convert a value to an instance of Constant
 * @param value
 * @return an instance of Constant
 */
template <typename T>
struct to_constant_expr<T, typename std::enable_if<std::is_integral<T>::value>::type> {
  Constant operator()(T value) { return Constant(value, bitsize_traits<T>::value, crave::is_signed<T>::value); }
};

/**
 * Implements AssignResult to assign value to a pointer
 */
template <typename T>
struct AssignResultToRef : AssignResult {
 public:
  explicit AssignResultToRef(T* ref, size_t width, bool sign) : value_(ref), width_(width), sign_(sign) {}

 public:
  Constant value_as_constant() const { return value_as_constant_impl(); }

  Constant to_constant(std::string const& str) const { return to_constant_impl(str); }

  void set_value(std::string const& str) { set_value(str, value_); }

 private:
  void set_value(std::string const& str, T* val) const {
    *val = ((sign_ && str[0] == '1') ? -1 : 0);
    for (std::string::const_iterator ite = str.begin(); ite != str.end(); ++ite) {
      *val <<= 1;
      switch (*ite) {
        case '0':
          *val &= T(-2);
          break;
        case '1':
          *val |= T(1);
          break;
        default:
          if (random_bit && random_bit())
            *val |= T(1);
          else
            *val &= T(-2);
          break;
      }
    }
  }

  template <typename U = T>
  typename std::enable_if<is_sysc_dt<U>::value, Constant>::type value_as_constant_impl() const {
    return Constant(value_->to_uint(), width_, sign_);
  }

  template <typename U = T>
  typename std::enable_if<!is_sysc_dt<U>::value, Constant>::type value_as_constant_impl() const {
    return Constant(*value_, width_, sign_);
  }

  template <typename U = T>
  typename std::enable_if<is_sysc_dt<U>::value, Constant>::type to_constant_impl(std::string const& str) const {
    U v;
    set_value(str, &v);
    return Constant(v.to_uint(), width_, sign_);
  }

  template <typename U = T>
  typename std::enable_if<!is_sysc_dt<U>::value, Constant>::type to_constant_impl(std::string const& str) const {
    U v;
    set_value(str, &v);
    return Constant(v, width_, sign_);
  }

 protected:
  T* value_;
  size_t width_;
  bool sign_;
};
}  // namespace crave
