// Copyright 2014 The CRAVE developers. All rights reserved.
#pragma once

#include <string>
#include "AssignResult.hpp"

namespace crave {

template <typename T>
struct AssignResultToRef : AssignResult {
 public:
  explicit AssignResultToRef(T *ref) : value_(ref) {}

 private:
  typedef boost::optional<boost::function0<bool> > Random;

  bool random_bit() const { return (*random_)(); }

 public:
  virtual T const& value() const { return *value_; }

  virtual void set_value(std::string const& str) {
    *value_ = ((crave::is_signed<T>::value && str[0] == '1') ? -1 : 0);
    for (std::string::const_iterator ite = str.begin(); ite != str.end();
         ++ite) {
      *value_ <<= 1;
      switch (*ite) {
        case '0':
          *value_ &= T(-2);
          break;
        case '1':
          *value_ |= T(1);
          break;
        default:
          if (random_ && random_bit())
            *value_ |= T(1);
          else
            *value_ &= T(-2);
          break;
      }
    }
  }

 protected:
  T *value_;
  Random random_;
};
}  // namespace crave
