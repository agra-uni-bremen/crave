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

#include "../crave/backend/VariableGeneratorType.hpp"
#include "../crave/backend/VariableDefaultSolver.hpp"

namespace crave {
VariableGenerator::VariableGenerator(const VariableContainer& vcon) : var_ctn_(vcon), solvers_() {}

void VariableGenerator::reset(std::vector<ConstraintPartition>& partitions) {
  solvers_.clear();

  for(ConstraintPartition & partition : partitions) {
    VarSolverPtr vs(new VariableDefaultSolver(var_ctn_, partition));
    solvers_.push_back(vs);
  }
}

bool VariableGenerator::solve() {
  for(VarSolverPtr vs : solvers_) {
    if (!vs->solve()) return false;
  }
  return true;
}

std::vector<std::vector<std::string> > VariableGenerator::analyseContradiction() {
  std::vector<std::vector<std::string> > str_vec;

  for(VarSolverPtr vs : solvers_) {
    std::vector<std::vector<std::string> > c = vs->getContradictions();
    if (!c.empty()) str_vec.insert(str_vec.end(), c.begin(), c.end());
  }
  return str_vec;
}

std::vector<std::string> VariableGenerator::getInactiveSofts() const {
  std::vector<std::string> str_vec;

  for(VarSolverPtr vs : solvers_) {
    std::vector<std::string> c = vs->getInactiveSofts();
    if (!c.empty()) str_vec.insert(str_vec.end(), c.begin(), c.end());
  }
  return str_vec;
}
}
