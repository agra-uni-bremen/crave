// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include "../frontend/Distribution.hpp"
#include "ReferenceExpressionType.hpp"

namespace crave {

template <typename Integer>
struct DistReferenceExpr : ReferenceExpression {
  DistReferenceExpr(distribution<Integer> dist, ReferenceExpression::result_type expr) : dist_(dist), expr_(expr) {}

  virtual ReferenceExpression::result_type expr() const {
    unsigned width = bitsize_traits<Integer>::value;
    bool sign = crave::is_signed<Integer>::value;
    return std::make_shared<EqualOpr>(expr_, std::make_shared<Constant>(dist_.nextValue(), width, sign));
  }

 private:
  distribution<Integer> dist_;
  ReferenceExpression::result_type expr_;
};
}  // namespace crave
