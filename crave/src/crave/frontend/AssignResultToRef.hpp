/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/

#pragma once

#include <string>

#include <functional>

#include "AssignResult.hpp"
#include "bitsize_traits.hpp"

#include "../ir/Node.hpp"

namespace crave {

extern std::function<bool(void)> random_bit;

/**
 * Convert a value to an instance of Constant
 * @param value
 * @return an instance of Constant
 */
template <typename T>
struct to_constant_expr<T, typename boost::enable_if<boost::is_integral<T> >::type> {
  Constant operator()(T value) { return Constant(value, bitsize_traits<T>::value, crave::is_signed<T>::value); }
};

/**
 * Implements AssignResult to assign value to a pointer
 */
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
