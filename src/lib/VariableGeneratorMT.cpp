#include "../crave/backend/VariableGeneratorMT.hpp"
#include "../crave/backend/VariableDefaultSolver.hpp"
#include <boost/thread/thread.hpp>

namespace crave {
VariableGeneratorMT::VariableGeneratorMT(VariableContainer const& vcon) : VariableGenerator(vcon) {}

void VariableGeneratorMT::createNewSolver(ConstraintPartition& partition, unsigned int index) {
  solvers_[index] = boost::make_shared<VariableDefaultSolver>(var_ctn_, partition);
}

void VariableGeneratorMT::reset(std::vector<ConstraintPartition>& partitions) {
  solvers_.resize(partitions.size());
  boost::thread_group threads;
  for (unsigned i = 0; i < partitions.size(); i++) {
    boost::thread* thread(
        new boost::thread(boost::bind(&VariableGeneratorMT::createNewSolver, this, boost::ref(partitions.at(i)), i)));
    threads.add_thread(thread);
  }
  threads.join_all();
}

void VariableGeneratorMT::solve(VarSolverPtr vs) {
  if (!result_) return;
  if (!vs->solve()) result_ = false;
}

bool VariableGeneratorMT::solve() {
  result_ = true;
  boost::thread_group threads;
  for (unsigned i = 0; i < solvers_.size(); i++) {
    if (!result_) break;
    boost::thread* thread(new boost::thread(boost::bind(&VariableGeneratorMT::solve, this, solvers_.at(i))));
    threads.add_thread(thread);
  }
  threads.join_all();
  return result_;
}
}
