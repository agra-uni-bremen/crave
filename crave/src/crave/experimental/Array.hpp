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
#include <vector>

#include "Object.hpp"
#include "Variable.hpp"

namespace crave {

/**
 * \ingroup newAPI
 * \brief Class for an N-element array with random values of type T.
 * 
 * This class can be used as an array of random variables and should be preferred to the
 * definition 'crv_variable<T> array[N]' as it comes with CRAVE functionality.
 */
template <typename T, unsigned N>
class crv_array : public crv_object {
 public:
  /**
   * \brief Constructor using name.
   */
  crv_array(crv_object_name = "arr") : arr_() {
    for (unsigned i = 0; i < N; i++) {
      std::string s = std::string("element[") + std::to_string(i) + "]";
      arr_.emplace_back(new crv_variable<T>{s.c_str()});
    }
  }
  /**
   * \brief Destructor.
   */
  ~crv_array() {
    for (crv_variable<T>* arr : arr_) delete arr;
  }

  /**
   * \brief Deleted copy constructor.
   */
  crv_array(crv_array const&) = delete;

  std::string obj_kind() const override { return "crv_array"; }

  /**
   * \brief Gets the sum of the array elements as a CRAVE expression.
   * 
   * The expression can be used in a constraint.
   * 
   * \return sum expression of the elements in the array
   */
  expression sum() {
    expression result = value_to_expression(0);
    for (crv_variable<T>* v : arr_) result = crave::make_expression(result + (*v)());
    return result;
  }

  /**
   * \brief Access operator [] for array element.
   * \return The value of type T
   */
  crv_variable<T>& operator[](unsigned pos) { return *arr_[pos]; }

 private:
  std::vector<crv_variable<T>*> arr_; /** < elements of array */
};

}  // namespace crave
