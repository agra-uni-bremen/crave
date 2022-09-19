// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <set>
#include <string>

#include "VariableSolver.hpp"

namespace crave {

struct VariableCoverageSolver : VariableSolver {
  VariableCoverageSolver(const VariableContainer& vcon, const ConstraintPartition& cp);

  virtual bool solve();
  virtual ~VariableCoverageSolver() {}

 private:
  std::set<std::string> covered_set_;
};
}  // namespace crave
