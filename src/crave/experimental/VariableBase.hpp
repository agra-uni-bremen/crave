// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

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
}  // namespace crave
