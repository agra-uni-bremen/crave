// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/scoped_ptr.hpp>
#include <vector>
#include <map>
#include <string>

#include "VariableSolver.hpp"

namespace crave {

struct VariableDefaultSolver : VariableSolver {
  VariableDefaultSolver(VariableContainer* vcon, const ConstraintPartition& cp);

  virtual bool solve();

 private:
  void analyseHards();

  void analyseSofts();
};
}  // namespace crave
