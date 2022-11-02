// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include "../frontend/bitsize_traits.hpp"
#include "../ir/visitor/EvalVisitor.hpp"
#include "stdint.h"

namespace crave {

template <typename Integer>
Integer get_result(EvalVisitor const& v) {
  return static_cast<Integer>(v.result().value());
}

template <>
bool get_result(EvalVisitor const& v);

class Evaluator {
  typedef EvalVisitor::eval_map eval_map;

 public:
  Evaluator() : assignments_(), visitor_(&assignments_) {}

  template <typename var_type, typename value_type>
  void assign(var_type const& var, value_type const& value) {
    unsigned width = bitsize_traits<typename var_type::value_type>::value;
    bool sign = crave::is_signed<typename var_type::value_type>::value;
    assignments_[var.id()] = Constant(value, width, sign);
  }

  void assign(unsigned id, Constant c);

  template <typename Integer>
  Integer result() const {
    return get_result<Integer>(visitor_);
  }

 private:
  eval_map assignments_;
  EvalVisitor visitor_;
};

}  // end namespace crave
