// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/foreach.hpp>
#include <string>
#include <vector>

#include "VariableDefaultSolver.hpp"

namespace crave {

/**
     *
     */
struct VariableGenerator {
  typedef boost::shared_ptr<VariableSolver> VarSolverPtr;

  explicit VariableGenerator(VariableContainer* vcon)
      : var_ctn(vcon), solvers() {}

  virtual void reset(std::vector<ConstraintPartition>& partitions) {
    solvers.clear();

    BOOST_FOREACH(ConstraintPartition& partition, partitions) {
      VarSolverPtr vs(new VariableDefaultSolver(var_ctn, partition));
      solvers.push_back(vs);
    }
  }

  virtual bool solve() {
    BOOST_FOREACH(VarSolverPtr vs, solvers) {
      if (!vs->solve()) return false;
    }
    return true;
  }

  template <typename T>
  bool read (const Variable<T> &var, T* value) const {
    BOOST_FOREACH(VarSolverPtr vs, solvers) {
      if (vs->read(var, value)) return true;
    }
    return false;
  }

  std::vector<std::vector<std::string> > analyseContradiction() {
    std::vector<std::vector<std::string> > str_vec;

    BOOST_FOREACH(VarSolverPtr vs, solvers) {
      std::vector<std::vector<std::string> > c = vs->getContradictions();
      if (!c.empty()) str_vec.insert(str_vec.end(), c.begin(), c.end());
    }
    return str_vec;
  }

  std::vector<std::string> getInactiveSofts() const {
    std::vector<std::string> str_vec;

    BOOST_FOREACH(VarSolverPtr vs, solvers) {
      std::vector<std::string> c = vs->getInactiveSofts();
      if (!c.empty()) str_vec.insert(str_vec.end(), c.begin(), c.end());
    }
    return str_vec;
  }

 protected:
  VariableContainer* var_ctn;
  std::vector<VarSolverPtr> solvers;
};
}  // namespace crave
