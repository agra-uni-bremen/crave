#include "../crave/backend/VariableGeneratorMT.hpp"

#include <thread>

#include "../crave/backend/VariableDefaultSolver.hpp"

namespace crave {
VariableGeneratorMT::VariableGeneratorMT(VariableContainer const& vcon) : VariableGenerator(vcon) {}

void VariableGeneratorMT::createNewSolver(ConstraintPartition& partition, unsigned int index) {
  solvers_[index] = std::make_shared<VariableDefaultSolver>(var_ctn_, partition);
}

void joinAll(std::vector<std::thread*> threads) {
  for (std::thread* thread : threads) thread->join();
}

void VariableGeneratorMT::reset(std::vector<ConstraintPartition>& partitions) {
  solvers_.resize(partitions.size());
  std::vector<std::thread*> threads;
  for (unsigned i = 0; i < partitions.size(); i++) {
    std::thread* thread(new std::thread(&VariableGeneratorMT::createNewSolver, this, std::ref(partitions.at(i)), i));
    threads.push_back(thread);
  }
  joinAll(threads);
}

void VariableGeneratorMT::solve(VarSolverPtr vs) {
  if (!result_) return;
  if (!vs->solve()) result_ = false;
}

bool VariableGeneratorMT::solve() {
  result_ = true;
  std::vector<std::thread*> threads;
  for (unsigned i = 0; i < solvers_.size(); i++) {
    if (!result_) break;
    std::thread* thread(new std::thread([i, this]() { this->solve(this->solvers_.at(i)); }));
    threads.push_back(thread);
  }
  joinAll(threads);
  return result_;
}
}  // namespace crave
