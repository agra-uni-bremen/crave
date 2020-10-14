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

#include "VariableSolver.hpp"

namespace crave {

/**
     *
     */
struct VariableGenerator {
  typedef std::shared_ptr<VariableSolver> VarSolverPtr;

  explicit VariableGenerator(const VariableContainer& vcon);
  
  virtual ~VariableGenerator(){}

  virtual void reset(std::vector<ConstraintPartition>& partitions);

  virtual bool solve();

  template <typename T>
  bool read(const Variable<T>& var, T* value) const {
    for(VarSolverPtr vs : solvers_) {
      if (vs->read(var, value)) return true;
    }
    return false;
  }

  std::vector<std::vector<std::string> > analyseContradiction();

  std::vector<std::string> getInactiveSofts() const;

 protected:
  const VariableContainer& var_ctn_;
  std::vector<VarSolverPtr> solvers_;
};
}  // namespace crave
