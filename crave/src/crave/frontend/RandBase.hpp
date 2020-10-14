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

#include <iostream>
#include <string>
#include <vector>
#include "Constraint.hpp"

namespace crave {

/*!
 * \ingroup oldAPI
 * \brief Base class of all CRAVE randomizable components in old API.
 */
class rand_base {
 protected:
  rand_base() {}

  ~rand_base() {}

 public:
  /**
   * \brief Tries to find the next solution.
   * 
   * The next solution can be found if there is a combination of values that meet all defined constraints.
   * It is possible to create contradicts with constraints.
   * In that case false is returned.
   * If a solution is found, the variables are set according to the solution and true is returned.
   * \return true if a solution is found, false otherwise
   */
  virtual bool next() = 0;
  /**
   * Retrieves all values from children objects to the given vector.
   * @param vector of all values
   */
  virtual void gather_values(std::vector<int64_t>*) = 0;
  
  /**
   * \brief Gets the name of this object as a string. 
   * 
   * The name of the object is most likely the name of the class the object is made of.
   * 
   * \return Name of this object as a string
   */
  virtual std::string obj_kind() const = 0;
};

/**
 * \brief Base class for class rand_obj
 */
class rand_obj_base : public rand_base {
 public:
  /**
  * Adds a child object, should not be called by users
  * @param rb child object
  */
  virtual void add_base_child(rand_base* rb) = 0;

  virtual std::string obj_kind() const { return "rand_obj"; }
};

/**
 * \brief Base class for class randv
 */
template <typename T>
class randv_base : public rand_base {
 public:
  /**
   * get value
   * @return value
   */
  operator T() const { return value; }

  /**
   * output stream operator for a randv_base
   * @param os outputstream
   * @param e randv_base object to output
   * @return outputstream
   */
  friend std::ostream& operator<<(std::ostream& os, const randv_base<T>& e) {
    os << e.value;
    return os;
  }

  /**
   * get WriteReference for this object
   * @return WriteReference for this object
   */
  WriteReference<T> const& operator()() const { return var; }

  /**
   * kind of this object as string
   * @return kind of this object
   */
  virtual std::string obj_kind() const { return "randv"; }

 protected:
  explicit randv_base(rand_obj_base* parent) : var(&value) {
    if (parent != 0) parent->add_base_child(this);
  }

  randv_base(const randv_base& other) : var(&value), value(other.value) {}
  WriteReference<T> var;
  T value;
};
}  // namespace crave
