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


#pragma once

#include <string>
#include <vector>

#include "../ir/UserConstraint.hpp"
#include "../ir/VariableContainer.hpp"
#include "FactoryMetaSMT.hpp"

namespace crave {
struct VariableSolver {
  friend struct VariableGenerator;

  VariableSolver(const VariableContainer& vcon, const ConstraintPartition& cp);
  virtual ~VariableSolver(){}
  virtual bool solve() = 0;

  template <typename T>
  bool read(Variable<T> const& var, T* value) {
    if (var_ctn_.variables.find(var.id()) == var_ctn_.variables.end()) return false;
    AssignResultToRef<T> result(value);
    solver_->read(*var_ctn_.variables[var.id()], result);
    return true;
  }

  std::vector<std::vector<std::string> > getContradictions() const;

  std::vector<std::string> getInactiveSofts() const;

 protected:
  VariableContainer var_ctn_;
  const ConstraintPartition& constr_pttn_;
  SolverPtr solver_;

  std::vector<std::vector<std::string> > contradictions_;
  std::vector<std::string> inactive_softs_;
};
}  // namespace crave
