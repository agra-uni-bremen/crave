// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <string>
#include <set>

#include "VariableSolver.hpp"

namespace crave {

struct VariableCoverageSolver : VariableSolver {
  VariableCoverageSolver(VariableContainer *vcon,
                         const ConstraintPartition& cp)
      : VariableSolver(vcon, cp) {
    LOG(INFO) << "Create coverage solver for partition " << constr_pttn;

    BOOST_FOREACH(ConstraintPtr c, constr_pttn) {
      if (c->isSoft()) {
        continue;  // coverage solver ignores soft constraints for now
      }
      if (!c->isCover()) solver->makeAssertion(*c->expr());
    }
  }

  virtual bool solve() {
    BOOST_FOREACH(ConstraintPtr c, constr_pttn) {
      if (!c->isCover()) continue;
      if (covered_set_.find(c->name()) != covered_set_.end()) {
        continue;  // alread covered
      }
      // try solve
      BOOST_FOREACH(VariableContainer::ReadRefPair pair,
                     var_ctn->read_references) {
        if (constr_pttn.containsVar(pair.first)) {
          solver->makeAssumption(*pair.second->expr());
        }
      }
      solver->makeAssumption(*c->expr());
      if (solver->solve()) {
        LOG(INFO) << "Solve partition " << constr_pttn << " hitting constraint "
                  << c->name();
        covered_set_.insert(c->name());
        BOOST_FOREACH(VariableContainer::WriteRefPair pair,
                       var_ctn->write_references) {
          if (constr_pttn.containsVar(pair.first)) {
            solver->read(*var_ctn->variables[pair.first], *pair.second);
          }
        }
        return true;
      }
    }
    return false;
  }

 private:
  std::set<std::string> covered_set_;
};
}  // namespace crave
