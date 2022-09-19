// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <string>
#include <vector>

#include "../ir/UserConstraint.hpp"
#include "../ir/VariableContainer.hpp"
#include "FactoryMetaSMT.hpp"

namespace crave {
struct VariableSolver {
  friend struct VariableGenerator;

  VariableSolver(const VariableContainer& vcon, const ConstraintPartition& cp);
  virtual ~VariableSolver() {}
  virtual bool solve() = 0;

  template <typename T>
  bool read(Variable<T> const& var, T* value) {
    if (var_ctn_.variables.find(var.id()) == var_ctn_.variables.end()) return false;
    AssignResultToRef<T> result(value);
    solver_->read(*var_ctn_.variables[var.id()], result);
    return true;
  }

  std::vector<std::vector<std::string> > getContradictions() const;

  std::vector<std::string> getInactiveSofts() const;

 protected:
  VariableContainer var_ctn_;
  const ConstraintPartition& constr_pttn_;
  SolverPtr solver_;

  std::vector<std::vector<std::string> > contradictions_;
  std::vector<std::string> inactive_softs_;
};
}  // namespace crave
