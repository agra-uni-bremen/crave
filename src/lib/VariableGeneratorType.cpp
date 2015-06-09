#include "../crave/backend/VariableGeneratorType.hpp"

namespace crave
{
    VariableGenerator::VariableGenerator(VariableContainer* vcon)
      : var_ctn_(vcon), solvers_() {}
    
    void VariableGenerator::reset(std::vector<ConstraintPartition>& partitions) {
    solvers_.clear();

    BOOST_FOREACH(ConstraintPartition& partition, partitions) {
      VarSolverPtr vs(new VariableDefaultSolver(var_ctn_, partition));
      solvers_.push_back(vs);
    }
  }
    
    bool VariableGenerator::solve() {
    BOOST_FOREACH(VarSolverPtr vs, solvers_) {
      if (!vs->solve()) return false;
    }
    return true;
  }
    
     std::vector<std::vector<std::string> > VariableGenerator::analyseContradiction() {
    std::vector<std::vector<std::string> > str_vec;

    BOOST_FOREACH(VarSolverPtr vs, solvers_) {
      std::vector<std::vector<std::string> > c = vs->getContradictions();
      if (!c.empty()) str_vec.insert(str_vec.end(), c.begin(), c.end());
    }
    return str_vec;
  }

  std::vector<std::string> VariableGenerator::getInactiveSofts() const {
    std::vector<std::string> str_vec;

    BOOST_FOREACH(VarSolverPtr vs, solvers_) {
      std::vector<std::string> c = vs->getInactiveSofts();
      if (!c.empty()) str_vec.insert(str_vec.end(), c.begin(), c.end());
    }
    return str_vec;
  }
}
