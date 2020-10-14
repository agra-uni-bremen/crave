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

#include "../frontend/Constraint.hpp"
#include "../ir/UserExpression.hpp"
#include "Object.hpp"

namespace crave {
/**
 * base class for crv_variable
 */
struct crv_variable_base_ : public crv_object {
  /**
  * get an instance of Constant, which captures the current value of this variable.
  * @return a Constant
  */
  virtual Constant constant_expr() = 0;

  /**
   * get an expression, which bounds the symbolic value to be equal to another symbolic value.
   * @return bound expression
   */
  virtual expression bound_expr() = 0;

  /**
   * get id
   * @return id
   */
  virtual unsigned id() = 0;

  std::string obj_kind() const override final { return "crv_variable"; }
};


struct prev_ {};
/*!
 * \ingroup newAPI
 * \brief A symbol to refer the previously generated value.
 *
 * crave::prev allows you to access the value the crv_variable had before calling randomize().
 * You can use this variable in the parenthese operator () as an argument. 
 */
extern prev_ prev;

template <typename T>
class crv_variable_base : public crv_variable_base_ {
 public:
  operator T() const { return actual_value(); }

  /**
   * stream operator for crv_variable
   */
  friend std::ostream& operator<<(std::ostream& os, const crv_variable_base<T>& e) {
    os << e.actual_value();
    return os;
  }

  /**
   * get WriteReference
   * @return WriteReference
   */
  WriteReference<T> const& operator()() const { return var; }

  /**
   * get ReadReference to the current (i.e. last generated) value, the only supported argument is crave::prev.
   * @return ReadReference
   */
  template <class Q>
  ReadReference<T> const& operator()(Q const&) const {
    static_assert(std::is_same<Q, prev_>::value, "only prev is supported");
    return ref;
  }

  /**
   * get id
   * @return id
   */
  unsigned id() override { return var.id(); }

  /**
   * Bind this variable to another variable.
   * The binding influences the functions bound_expr() and actual_value().
   */
  void bind(crv_variable_base& other) { bound_var = &other; }

  /**
   * Unbind
   */
  void unbind() { bound_var = nullptr; }

  /**
   * get the current value of the crv_variable
   * @return value
   */
  T get() const {
	  return this->actual_value();
  }

  expression bound_expr() override {
    return bound_var ? make_expression(var == bound_var->var) : value_to_expression(true);
  }

  Constant constant_expr() override { return to_constant_expr<T>()(actual_value()); }

 protected:
  crv_variable_base() : var(&value), ref(value), value(), bound_var() {}
  crv_variable_base(const crv_variable_base& other)
      : crv_variable_base_(other), var(&value), ref(value), value(other.value), bound_var(other.bound_var) {}

  /**
   * get the current value
   * @return value
   */
  T actual_value() const { return bound_var ? bound_var->value : value; }

  WriteReference<T> var;
  ReadReference<T> ref;
  T value;
  crv_variable_base* bound_var;
};
}
