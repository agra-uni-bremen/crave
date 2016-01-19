// Copyright 2014 The CRAVE developers. All rights reserved.
#pragma once

#include <string>

#include <boost/function.hpp>

#include "AssignResult.hpp"
#include "bitsize_traits.hpp"

#include "../ir/Node.hpp"

namespace crave {

extern boost::function0<bool> random_bit;

template <typename T>
struct to_constant_expr<T, typename boost::enable_if<boost::is_integral<T> >::type> {
  Constant operator()(T value) { 
    return Constant(value, bitsize_traits<T>::value, crave::is_signed<T>::value);
  }
};

template <typename T>
struct AssignResultToRef : AssignResult {
 public:
  explicit AssignResultToRef(T* ref) : value_(ref) {}

 public:
  Constant value_as_constant() const { return to_constant_expr<T>()(*value_); }

  Constant to_constant(std::string const& str) const { 
    T v;
    set_value(str, &v);
    return to_constant_expr<T>()(v);
  }

  void set_value(std::string const& str) { set_value(str, value_); }

 private:
  void set_value(std::string const& str, T* val) const {
    *val = ((crave::is_signed<T>::value && str[0] == '1') ? -1 : 0);
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

 protected:
  T* value_;
};
}  // namespace crave
