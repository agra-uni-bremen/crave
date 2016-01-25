// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/make_shared.hpp>

#include <string>
#include <set>

#include "UserConstraintType.hpp"

namespace crave {

class UserVectorConstraint : public UserConstraint {
  friend struct ConstraintManager;
  friend struct ConstraintPartitioner;

 public:
  UserVectorConstraint(unsigned const id, expression const expr, std::string const& name, std::set<int> support_vars,
                       bool const unique, bool const soft = false, bool const cover = false, bool const enabled = true);

  bool isUnique();
  int getVectorId();
  bool isVectorConstraint();

 protected:
  bool unique_;
};
}  // namespace crave
