// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <string>
#include <set>

#include "UserConstraintType.hpp"

namespace crave {

struct UserVectorConstraint : UserConstraint {
  friend struct ConstraintManager;
  friend struct ConstraintPartitioner;

 protected:
  UserVectorConstraint(unsigned const id, expression const expr,
                       std::string const& name, std::set<int> support_vars,
                       bool const unique, bool const soft = false,
                       bool const cover = false, bool const enabled = true)
      : UserConstraint(id, expr, name, support_vars, soft, cover, enabled),
        unique_(unique) {}

 public:
  bool isUnique() { return unique_; }
  int getVectorId() { return *support_vars_.begin(); }
  bool isVectorConstraint() { return true; }

 protected:
  bool unique_;
};
}  // namespace crave
