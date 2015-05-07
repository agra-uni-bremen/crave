// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include "Node.hpp"

namespace crave {
    
    struct ReferenceExpression {
 public:
  typedef NodePtr result_type;
  virtual ~ReferenceExpression() {}
  virtual result_type expr() const = 0;
};

}
