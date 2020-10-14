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

#include "ConstraintType.hpp"
#include "Variable.hpp"

namespace crave {
// Vector

/**
 * tag class for vector
 */
template <typename value_type>
struct vector_tag {
  /**
   * constructor
   * @param id id of this object
   */
  explicit vector_tag(int id) : id_(id) {}
  int id_;
};

/**
 * class for internal symbolic vector variables
 */
template <typename value_type_>
struct Vector : public Constraint<typename boost::proto::terminal<vector_tag<value_type_> >::type> {
  typedef Constraint<typename boost::proto::terminal<vector_tag<value_type_> >::type> base_type;

  /**
   * constructor
   */
  Vector() : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(vector_tag<value_type_>(new_var_id()))) {}

  typedef value_type_ value_type;

  /**
   * get id of this symbolic vector
   * @return id
   */
  int id() const { return boost::proto::value(*this).id_; }

  /**
   * number of elements in vector as a symbolic variable to used in constraints
   * @return symbolic size
   */
  const Variable<unsigned int>& size() const { return size_; }

 private:
  Variable<unsigned int> size_;
};
}  // namespace crave
