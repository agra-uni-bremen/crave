// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <vector>

#include "VariableGeneratorType.hpp"

namespace crave {

struct VariableCoverageGenerator : VariableGenerator {
  explicit VariableCoverageGenerator(VariableContainer* vcon)
      : VariableGenerator(vcon) {}

  virtual void reset(std::vector<ConstraintPartition>& partitions) {
    solvers_.clear();

    BOOST_FOREACH(ConstraintPartition& partition, partitions) {
      VarSolverPtr vs(new VariableCoverageSolver(var_ctn_, partition));
      solvers_.push_back(vs);
    }
  }

  virtual bool solve() {
    bool res = false;
    BOOST_FOREACH(VarSolverPtr vs, solvers_) {
      if (vs->solve())
        res = true;  // one solver has hitted an uncovered constraint
                     // -> everything ok,
                     // from covered solvers, the last
                     // solutions will be reused.
    }
    return res;
  }
};
}  // namespace crave
