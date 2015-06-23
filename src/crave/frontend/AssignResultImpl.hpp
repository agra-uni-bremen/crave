// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <string>
#include "AssignResult.hpp"
#include "bitsize_traits.hpp"
#include "AssignResultToRef.hpp"

namespace crave {

template <typename T>
struct AssignResultImpl : AssignResult {
 public:
  AssignResultImpl() {}

  explicit AssignResultImpl(T& value) : value_(value) {}

 private:
  typedef boost::optional<boost::function0<bool> > Random;

  bool random_bit() const { return (*random_)(); }

 public:
  virtual T value() const { return value_; }

  virtual void set_value(std::string const& str) {
    value_ = ((crave::is_signed<T>::value && str[0] == '1') ? -1 : 0);
    for (std::string::const_iterator ite = str.begin(); ite != str.end(); ++ite) {
      value_ <<= 1;
      switch (*ite) {
        case '0':
          value_ &= T(-2);
          break;
        case '1':
          value_ |= T(1);
          break;
        default:
          if (random_ && random_bit())
            value_ |= T(1);
          else
            value_ &= T(-2);
          break;
      }
    }
  }

 protected:
  T value_;
  Random random_;
};
} /* namespace crave */
