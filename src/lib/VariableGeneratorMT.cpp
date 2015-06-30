#include "../crave/backend/VariableGeneratorMT.hpp"

namespace crave {
VariableGeneratorMT::VariableGeneratorMT(VariableContainer* vcon) : VariableGenerator(vcon){
}


VariableGeneratorMT::~VariableGeneratorMT() {
}

void VariableGeneratorMT::reset(std::vector<ConstraintPartition>& partitions) {
  solvers_.clear();

  BOOST_FOREACH(ConstraintPartition & partition, partitions) {
    VarSolverPtr vs(new VariableDefaultSolver(var_ctn_, partition));
    solvers_.push_back(vs);
  }
}

bool VariableGeneratorMT::solve(VarSolverPtr vs)
{
    return vs->solve();
}

bool VariableGeneratorMT::solve() {
  
  BOOST_FOREACH(VarSolverPtr vs, solvers_) {
    if (!solve(vs)) return false;
  }
  return true;
}

}
