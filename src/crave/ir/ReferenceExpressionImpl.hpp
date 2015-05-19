// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include "ReferenceExpressionType.hpp"
#include "../frontend/bitsize_traits.hpp"

namespace crave {

template <typename Integer>
struct ReferenceExpressionImpl : public ReferenceExpression {
 public:
  ReferenceExpressionImpl(Integer const& value,
                          ReferenceExpression::result_type expr)
      : value_(value), expr_(expr) {}

  virtual ~ReferenceExpressionImpl() {}

 public:
  virtual ReferenceExpression::result_type expr() const {
    unsigned width = bitsize_traits<Integer>::value;
    bool sign = crave::is_signed<Integer>::value;
    return new EqualOpr(expr_, new Constant(value_, width, sign));
  }

 private:
  Integer const& value_;
  ReferenceExpression::result_type expr_;
};
}  // namespace crave
