#pragma once

#include "VariableGeneratorType.hpp"

namespace crave {
class VariableGeneratorMT : public VariableGenerator {
 public:
  explicit VariableGeneratorMT(VariableContainer const& vcon);
  virtual ~VariableGeneratorMT() {}

  virtual void reset(std::vector<ConstraintPartition>& partitions);
  virtual bool solve();

 private:
  void createNewSolver(ConstraintPartition& partition, unsigned int index);
  void solve(VarSolverPtr vs);
  bool result_;
};
}
