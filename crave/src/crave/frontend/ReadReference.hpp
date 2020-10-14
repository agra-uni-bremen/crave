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
// ReadReference
/**
 * tag class for read_ref
 */
template <typename value_type>
struct read_ref_tag : public var_tag<value_type> {
  read_ref_tag(int id_, value_type const& ref_) : var_tag<value_type>(id_), ref(ref_) {}

  value_type const& ref;
};

/**
 * ReadReference<T> contains a symbolic variable and a reference to a variable of type T.
 * The value of the symbolic variable will be constrained to be the value of the reference when constrain solving is invoked.
 */
template <typename value_type_>
struct ReadReference : public Constraint<typename boost::proto::terminal<read_ref_tag<value_type_> >::type> {
  typedef Constraint<typename boost::proto::terminal<read_ref_tag<value_type_> >::type> base_type;

  /**
   * constructor
   * @param ref reference to object
   */
  explicit ReadReference(value_type_ const& ref)
      : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(read_ref_tag<value_type_>(new_var_id(), ref))) {}

  typedef value_type_ value_type;

  /**
   * get id
   * @return id
   */
  int id() const { return boost::proto::value(*this).id; }
};

/**
 * \ingroup operators
 * \brief Creates a read reference to a non-CRAVE variable.
 * 
 * This operator creates a reference to normal C++ variable.
 * The difference of using var and crave::reference(var) in constraint definition is the value of var that will be used in constraint solving.
 * In the former case, the value at the time of constraint definition will be used.
 * In the latter case, the current value at the time of constraint solving will be used.
 * \param ref Variable to refer to
 * \return ReadReference for constraint solving to this variable
 */
template <typename T>
typename boost::proto::result_of::make_expr<boost::proto::tag::terminal, Constraint_Domain, read_ref_tag<T> >::type
reference(T const& ref) {
  return boost::proto::make_expr<boost::proto::tag::terminal, Constraint_Domain>(read_ref_tag<T>(new_var_id(), ref));
}
}  // namespace crave
