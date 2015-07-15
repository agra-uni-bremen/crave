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
struct AssignResultToRef : AssignResult {
 public:
  explicit AssignResultToRef(T* ref) : value_(ref) {}

 public:
  Constant value() const {
    return Constant(crave::to_uint64<T>()(*value_), bitsize_traits<T>::value, crave::is_signed<T>::value);
  }

  void set_value(std::string const& str) {
    *value_ = ((crave::is_signed<T>::value && str[0] == '1') ? -1 : 0);
    for (std::string::const_iterator ite = str.begin(); ite != str.end(); ++ite) {
      *value_ <<= 1;
      switch (*ite) {
        case '0':
          *value_ &= T(-2);
          break;
        case '1':
          *value_ |= T(1);
          break;
        default:
          if (random_bit && random_bit())
            *value_ |= T(1);
          else
            *value_ &= T(-2);
          break;
      }
    }
  }

 protected:
  T* value_;
};
}  // namespace crave
