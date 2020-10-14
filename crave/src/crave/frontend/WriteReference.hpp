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
// WriteReference
namespace crave {

/**
 * tag class for WriteReference
 */
template <typename value_type>
struct write_ref_tag : public var_tag<value_type> {
  /**
   * constructor
   * @param id_ id of object
   * @param ref_ reference
   */
  write_ref_tag(int id_, value_type *ref_) : var_tag<value_type>(id_), ref(ref_) {}
  value_type *ref;
};

/**
 * WriteReference<T> is an important internal class for randomization.
 * It contains a symbolic variable to be used in constraints and a pointer to value of type T, which will be updated after constraint solving.
 */
template <typename value_type_>
struct WriteReference : public Constraint<typename boost::proto::terminal<write_ref_tag<value_type_> >::type> {
  typedef Constraint<typename boost::proto::terminal<write_ref_tag<value_type_> >::type> base_type;

  /**
   * constructor
   * @param ref reverence to value
   */
  explicit WriteReference(value_type_ *ref)
      : base_type(boost::proto::make_expr<boost::proto::tag::terminal>(write_ref_tag<value_type_>(new_var_id(), ref))) {
  }

  typedef value_type_ value_type;

  /**
   * id of this object
   * @return id
   */
  int id() const { return boost::proto::value(*this).id; }

  /**
   * assignment operator, only to be used in constraints.
   * @param e value for assignment
   * @return boost::proto expression
   */
  typename boost::proto::result_of::make_expr<boost::proto::tag::assign, Constraint_Domain,
                                              WriteReference<value_type_> const &, value_type_ const &>::type const
  operator=(value_type_ const &e) const {
    return boost::proto::make_expr<boost::proto::tag::assign, Constraint_Domain>(boost::cref(*this), boost::cref(e));
  }
};
}  // namespace crave
