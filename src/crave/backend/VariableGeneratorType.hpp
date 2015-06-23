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

  explicit VariableGenerator(VariableContainer* vcon);

  virtual void reset(std::vector<ConstraintPartition>& partitions);

  virtual bool solve();

  template <typename T>
  bool read(const Variable<T>& var, T* value) const {
    BOOST_FOREACH(VarSolverPtr vs, solvers_) {
      if (vs->read(var, value)) return true;
    }
    return false;
  }

  std::vector<std::vector<std::string> > analyseContradiction();

  std::vector<std::string> getInactiveSofts() const;

 protected:
  VariableContainer* var_ctn_;
  std::vector<VarSolverPtr> solvers_;
};
}  // namespace crave
