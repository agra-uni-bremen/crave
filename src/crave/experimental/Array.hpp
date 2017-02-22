// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

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
