// Copyright 2014 The CRAVE developers. All rights reserved.
#include "../../crave/experimental/Expression.hpp"

namespace crave
{
  void expression_list::operator=(expression_list const& list) { exprs_ = list.exprs_; }

  void expression_list::join(expression_list const& list) { exprs_.insert(end(), list.begin(), list.end()); }
}