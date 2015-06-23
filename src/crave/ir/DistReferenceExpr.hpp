// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include "../frontend/Distribution.hpp"

namespace crave {

template <typename Integer>
struct DistReferenceExpr : ReferenceExpression {
  DistReferenceExpr(distribution<Integer> dist, ReferenceExpression::result_type expr) : dist_(dist), expr_(expr) {}

  virtual ReferenceExpression::result_type expr() const {
    unsigned width = bitsize_traits<Integer>::value;
    bool sign = crave::is_signed<Integer>::value;
    return new EqualOpr(expr_, new Constant(dist_.nextValue(), width, sign));
  }

 private:
  distribution<Integer> dist_;
  ReferenceExpression::result_type expr_;
};
}  // namespace crave
