#include "../crave/backend/VariableSolver.hpp"

namespace crave {
    VariableSolver::VariableSolver(VariableContainer* vcon, const ConstraintPartition& cp)
      : var_ctn_(vcon),
        constr_pttn_(cp),
        solver_(FactoryMetaSMT::getNewInstance()) {}
    
    std::vector<std::vector<std::string> > VariableSolver::getContradictions() {
    return contradictions_;
  }
    
    std::vector<std::string> VariableSolver::getInactiveSofts() const { return inactive_softs_; }
}