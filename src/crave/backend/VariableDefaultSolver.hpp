// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/scoped_ptr.hpp>
#include <vector>
#include <map>
#include <string>

#include "VariableSolver.hpp"

namespace crave {

struct VariableDefaultSolver : VariableSolver {
  static bool bypass_constraint_analysis;

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
