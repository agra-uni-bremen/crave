// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "Object.hpp"
#include "../ir/UserExpression.hpp"
#include "../frontend/Constraint.hpp"

namespace crave
{
struct crv_variable_base_ : public crv_object {
  virtual Constant constant_expr() = 0;
  virtual expression bound_expr() = 0;
  virtual unsigned id() = 0;
  std::string obj_kind() const override final { return "crv_variable"; }
};

struct prev_ { };
extern prev_ prev;

template <typename T>
class crv_variable_base : public crv_variable_base_ {
 public:
  operator T() const { return actual_value(); }
  friend std::ostream& operator<<(std::ostream& os, const crv_variable_base<T>& e) {
    os << e.actual_value();
    return os;
  }
  WriteReference<T> const& operator()() const { return var; }
  
  template <class Q>
  ReadReference<T> const& operator()(Q const&) const {
    static_assert(std::is_same<Q, prev_>::value, "only prev is supported");
    return ref; 
  }

  unsigned id() override { return var.id(); }
  void bind(crv_variable_base& other) { bound_var = &other; }
  void unbind() { bound_var = nullptr; }
  expression bound_expr() { return bound_var ? make_expression(var == bound_var->var) : value_to_expression(true); }
  Constant constant_expr() override { return to_constant_expr<T>()(actual_value()); }

 protected:
  crv_variable_base() : var(&value), ref(value), value(), bound_var() {}
  crv_variable_base(const crv_variable_base& other)
      : crv_variable_base_(other), var(&value), ref(value), value(other.value), bound_var(other.bound_var) {}

  T actual_value() const { return bound_var ? bound_var->value : value; }

  WriteReference<T> var;
  ReadReference<T> ref;
  T value;
  crv_variable_base* bound_var;
};
}