/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2011-2016 University of Bremen, Germany.
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

#define BOOST_TEST_MODULE direct_Yices2
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/backend/Yices2.hpp>

using namespace metaSMT;
struct Solver_Fixture {
  typedef DirectSolver_Context<metaSMT::solver::Yices2> ContextType;
  ContextType ctx;
};

#include "test_solver.cpp"
#include "test_unsat.cpp"
#include "test_QF_UF.cpp"
#include "test_QF_BV.cpp"
#include "test_Array.cpp"
