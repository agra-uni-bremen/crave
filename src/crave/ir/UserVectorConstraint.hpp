// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <string>
#include <set>
#include <boost/make_shared.hpp>

#include "UserConstraintType.hpp"

namespace crave {

struct UserVectorConstraint : UserConstraint {
  friend struct ConstraintManager;
  friend struct ConstraintPartitioner;

  //protected: // to make make_shared_posible
  UserVectorConstraint(unsigned const id, expression const expr,
                       std::string const& name, std::set<int> support_vars,
                       bool const unique, bool const soft = false,
                       bool const cover = false, bool const enabled = true);

 public:
  bool isUnique();
  int getVectorId();
  bool isVectorConstraint();

 protected:
  bool unique_;
};
}  // namespace crave
