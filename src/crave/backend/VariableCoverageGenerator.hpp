// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <vector>

#include "VariableGeneratorType.hpp"

namespace crave {

struct VariableCoverageGenerator : VariableGenerator {
  explicit VariableCoverageGenerator(const VariableContainer& vcon);

  virtual void reset(std::vector<ConstraintPartition>& partitions);

  virtual bool solve();
};
}  // namespace crave
