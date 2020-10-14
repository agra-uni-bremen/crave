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


// FIXME this is a workaround for the issue that STP header and Yices2 header cannnot be included
// in the same metaSMTNodeVisitor.cpp

#ifdef metaSMT_USE_YICES2

#include "../crave/ir/visitor/metaSMTNodeVisitor.hpp"
#include "../crave/backend/FactoryMetaSMT.hpp"
#include "metaSMTNodeVisitorImpl.hpp"

#include <metaSMT/DirectSolver_Context.hpp>

#define DEFINE_SOLVER(SOLVER_ENUM, SOLVER_T)                                                     \
  namespace crave {                                                                              \
  template <>                                                                                    \
  struct FactorySolver<SOLVER_ENUM> {                                                            \
    typedef metaSMT::DirectSolver_Context<SOLVER_T> SolverType;                                  \
    static bool isDefined() { return true; }                                                     \
    static metaSMTVisitor* getNewInstance() { return new metaSMTVisitorImpl<SolverType>(); }     \
  };                                                                                             \
  }  // namespace crave


#include <metaSMT/backend/Yices2.hpp>
DEFINE_SOLVER(crave::YICES2, metaSMT::solver::Yices2);

#undef DEFINE_SOLVER

#endif


