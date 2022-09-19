// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <map>
#include <string>
#include <vector>

#include "VariableSolver.hpp"

namespace crave {

struct VariableDefaultSolver : VariableSolver {
  static bool bypass_constraint_analysis;
  static unsigned complexity_limit_for_bdd;

  VariableDefaultSolver(const VariableContainer& vcon, const ConstraintPartition& cp);

  virtual bool solve();

 private:
  void analyseConstraints();
  void analyseHards();
  void analyseSofts();

  std::map<int, SolverPtr> bdd_solvers_;
  std::vector<VariableContainer::WriteRefPair> random_write_refs_;
};
}  // namespace crave
