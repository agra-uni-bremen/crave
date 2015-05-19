// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <string>
#include <vector>

#include "../ir/UserConstraint.hpp"
#include "../ir/VariableContainer.hpp"
#include "FactoryMetaSMT.hpp"

namespace crave {
struct VariableSolver {
  friend struct VariableGenerator;

  VariableSolver(VariableContainer *vcon,const ConstraintPartition& cp)
      : var_ctn(vcon), constr_pttn(cp),
        solver(FactoryMetaSMT::getNewInstance()) {}

  virtual bool solve() = 0;

  template <typename T>
  bool read(Variable<T> const& var, T& value) {
    if (var_ctn->variables.find(var.id()) == var_ctn->variables.end()) {
        return false;
    }
    if (!constr_pttn.containsVar(var.id())) return false;
    AssignResultImpl<T> result;
    solver->read(*var_ctn->variables[var.id()], result);
    value = result.value();
    return true;
  }

  std::vector<std::vector<std::string> > getContradictions() {
      return contradictions_;
  }

  std::vector<std::string> getInactiveSofts() const { 
      return inactive_softs_;
  }

 protected:
  VariableContainer *var_ctn;
  const ConstraintPartition& constr_pttn;
  SolverPtr solver;

  std::vector<std::vector<std::string> > contradictions_;
  std::vector<std::string> inactive_softs_;
};
}  // namespace crave
