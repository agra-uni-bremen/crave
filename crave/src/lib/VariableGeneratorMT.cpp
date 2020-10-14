/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/

#include "../crave/backend/VariableGeneratorMT.hpp"
#include "../crave/backend/VariableDefaultSolver.hpp"
#include <thread>

namespace crave {
VariableGeneratorMT::VariableGeneratorMT(VariableContainer const& vcon) : VariableGenerator(vcon) {}

void VariableGeneratorMT::createNewSolver(ConstraintPartition& partition, unsigned int index) {
  solvers_[index] = std::make_shared<VariableDefaultSolver>(var_ctn_, partition);
}

void joinAll(std::vector<std::thread*> threads) {
  for(std::thread* thread : threads) thread->join();
}

void VariableGeneratorMT::reset(std::vector<ConstraintPartition>& partitions) {
  solvers_.resize(partitions.size());
  std::vector<std::thread*> threads;
  for (unsigned i = 0; i < partitions.size(); i++) {
    std::thread* thread(
        new std::thread(&VariableGeneratorMT::createNewSolver, this, std::ref(partitions.at(i)), i));
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
    std::thread* thread(new std::thread([i,this](){this->solve(this->solvers_.at(i));}));
    threads.push_back(thread);
  }
  joinAll(threads);
  return result_;
}
}
