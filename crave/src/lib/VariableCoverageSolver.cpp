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

#include "../crave/backend/VariableCoverageSolver.hpp"
#include "../crave/utils/Logging.hpp"

namespace crave {

VariableCoverageSolver::VariableCoverageSolver(const VariableContainer& vcon, const ConstraintPartition& cp)
    : VariableSolver(vcon, cp) {
  LOG(INFO) << "Create coverage solver for partition " << constr_pttn_;

  for(ConstraintPtr c : constr_pttn_) {
    if (c->isSoft()) {
      continue;  // coverage solver ignores soft constraints for now
    }
    if (!c->isCover()) solver_->makeAssertion(*c->expr());
  }
}

bool VariableCoverageSolver::solve() {

  for(ConstraintPtr c : constr_pttn_) {
    if (!c->isCover()) continue;
    if (covered_set_.find(c->name()) != covered_set_.end()) {
      continue;  // alread covered
    }
    // try solve

    for(VariableContainer::ReadRefPair & pair : var_ctn_.read_references) {
      solver_->makeAssumption(*pair.second->expr());
    }
    solver_->makeAssumption(*c->expr());
    if (solver_->solve()) {
      LOG(INFO) << "Solve partition " << constr_pttn_ << " hitting constraint " << c->name();
      covered_set_.insert(c->name());

      for(VariableContainer::WriteRefPair & pair : var_ctn_.write_references) {
        solver_->read(*var_ctn_.variables[pair.first], *pair.second);
      }
      return true;
    }
  }
  return false;
}
}
