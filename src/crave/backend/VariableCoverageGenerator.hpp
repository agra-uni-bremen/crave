// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <vector>

#include "VariableGeneratorType.hpp"

namespace crave {

struct VariableCoverageGenerator : VariableGenerator {
  explicit VariableCoverageGenerator(VariableContainer* vcon);

  virtual void reset(std::vector<ConstraintPartition>& partitions);

  virtual bool solve();
};
}  // namespace crave
