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

#include "Variable.hpp"

#include "../frontend/Distribution.hpp"
#include "../frontend/bitsize_traits_sysc.hpp"

namespace crave {

template <typename T>
struct to_constant_expr<T, typename std::enable_if<is_sysc_dt<T>::value>::type> {
  Constant operator()(T value) {
    return Constant(value.to_uint64(), bitsize_traits<T>::value, crave::is_signed<T>::value);
  }
};

/**
 * crv_variable for SystemC data types
 */
template <typename T>
class crv_variable<T, typename std::enable_if<is_sysc_dt<T>::value>::type> : public crv_variable_base<T> {
  CRV_VARIABLE_COMMON_CONSTRUCTORS(T);
  CRV_VARIABLE_ASSIGNMENT_INTERFACE(T);
  CRV_VARIABLE_ARITHMETIC_INTERFACE(T);
  CRV_VARIABLE_BITWISE_INTERFACE(T);

 public:
  /**
   * randomize variable
   * @return true if success
   */
  bool randomize() override {
    static distribution<uint64_t> dist;
    this->value = dist.nextValue();
    return true;
  }
};

}  // namespace crave
