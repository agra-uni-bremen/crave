#include "../crave/backend/VariableCoverageGenerator.hpp"

#include "../crave/backend/VariableCoverageSolver.hpp"

namespace crave {

VariableCoverageGenerator::VariableCoverageGenerator(const VariableContainer& vcon) : VariableGenerator(vcon) {}

void VariableCoverageGenerator::reset(std::vector<ConstraintPartition>& partitions) {
  solvers_.clear();

  for (ConstraintPartition& partition : partitions) {
    VarSolverPtr vs(new VariableCoverageSolver(var_ctn_, partition));
    solvers_.push_back(vs);
  }
}

bool VariableCoverageGenerator::solve() {
  bool res = false;

  for (VarSolverPtr vs : solvers_) {
    if (vs->solve())
      res = true;  // one solver has hitted an uncovered constraint
                   // -> everything ok,
                   // from covered solvers, the last
                   // solutions will be reused.
  }
  return res;
}
}  // namespace crave
