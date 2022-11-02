// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include "../frontend/bitsize_traits.hpp"
#include "ReferenceExpressionType.hpp"

namespace crave {

template <typename Integer>
struct ReferenceExpressionImpl : public ReferenceExpression {
 public:
  ReferenceExpressionImpl(Integer const& value, ReferenceExpression::result_type expr) : value_(value), expr_(expr) {}

  virtual ~ReferenceExpressionImpl() {}

 public:
  virtual ReferenceExpression::result_type expr() const {
    unsigned width = bitsize_traits<Integer>::value;
    bool sign = crave::is_signed<Integer>::value;
    return std::make_shared<EqualOpr>(expr_, std::make_shared<Constant>(value_, width, sign));
  }

 private:
  Integer const& value_;
  ReferenceExpression::result_type expr_;
};
}  // namespace crave
