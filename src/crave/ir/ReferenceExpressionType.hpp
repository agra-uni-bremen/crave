// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include "Node.hpp"

namespace crave {

struct ReferenceExpression {
 public:
  typedef NodePtr result_type;
  virtual ~ReferenceExpression() {}
  virtual result_type expr() const = 0;
};
}  // namespace crave
