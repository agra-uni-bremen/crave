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
#include "../ir/visitor/metaSMTNodeVisitor.hpp"

namespace crave {

enum SolverTypes {
  UNDEFINED_SOLVER,
  BOOLECTOR,
  SWORD,
  STP,
  YICES2,
  Z3,
  CVC4,
  CUDD,
};

struct FactoryMetaSMT {
  static void setSolverType(std::string const&);
  /**
    * creates a new metaSMTVisitor with a specified backend or if unspecified,
    * metaSMTNodeVisitor::solver_type. Caller is responsible for deleting the
    * visitor.
    **/
  static metaSMTVisitor* getNewInstance(SolverTypes type = solver_type_);

  static SolverTypes solver_type_;
};

template <SolverTypes solver_type>
struct FactorySolver {
  static bool isDefined() { return false; }
  static metaSMTVisitor* getNewInstance() { return NULL; }
};
}  // namespace crave
