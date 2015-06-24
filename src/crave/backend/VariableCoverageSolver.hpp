// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <string>
#include <set>

#include "VariableSolver.hpp"

namespace crave {

struct VariableCoverageSolver : VariableSolver {
  VariableCoverageSolver(const VariableContainer& vcon, const ConstraintPartition& cp);

  virtual bool solve();

 private:
  std::set<std::string> covered_set_;
};
}  // namespace crave
