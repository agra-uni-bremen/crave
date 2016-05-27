// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <string>
#include <vector>

#include "VariableSolver.hpp"

namespace crave {

/**
     *
     */
struct VariableGenerator {
  typedef boost::shared_ptr<VariableSolver> VarSolverPtr;

  explicit VariableGenerator(const VariableContainer& vcon);
  
  virtual ~VariableGenerator(){}

  virtual void reset(std::vector<ConstraintPartition>& partitions);

  virtual bool solve();

  template <typename T>
  bool read(const Variable<T>& var, T* value) const {
    for(VarSolverPtr vs : solvers_) {
      if (vs->read(var, value)) return true;
    }
    return false;
  }

  std::vector<std::vector<std::string> > analyseContradiction();

  std::vector<std::string> getInactiveSofts() const;

 protected:
  const VariableContainer& var_ctn_;
  std::vector<VarSolverPtr> solvers_;
};
}  // namespace crave
