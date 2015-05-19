// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <vector>

#include "VariableGeneratorType.hpp"

namespace crave {

struct VariableCoverageGenerator : VariableGenerator {
  VariableCoverageGenerator(VariableContainer* vcon)
      : VariableGenerator(vcon) {}

  virtual void reset(std::vector<ConstraintPartition>& partitions) {
    solvers.clear();

    BOOST_FOREACH (ConstraintPartition& partition, partitions) {
      VarSolverPtr vs(new VariableCoverageSolver(var_ctn, partition));
      solvers.push_back(vs);
    }
  }

  virtual bool solve() {
    bool res = false;
    BOOST_FOREACH (VarSolverPtr vs, solvers) {
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
