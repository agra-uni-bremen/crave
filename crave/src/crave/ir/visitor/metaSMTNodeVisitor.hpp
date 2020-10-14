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

#include <boost/intrusive_ptr.hpp>

#include <map>
#include <string>
#include <vector>
#include <memory>

#include "../../frontend/RandomBase.hpp"
#include "../../frontend/AssignResult.hpp"
#include "../Node.hpp"
#include "NodeVisitor.hpp"

namespace crave {

class metaSMTVisitor : public NodeVisitor {
 protected:
  typedef boost::intrusive_ptr<VariableExpr> VariablePtr;

 public:
  virtual void makeAssertion(Node const&) = 0;
  virtual void makeSoftAssertion(Node const&) = 0;
  virtual void makeSuggestion(Node const&) = 0;
  virtual void makeAssumption(Node const&) = 0;
  virtual std::vector<unsigned int> analyseSofts() = 0;
  virtual std::vector<std::vector<unsigned int> > analyseContradiction(std::map<unsigned int, NodePtr> const&) = 0;
  virtual bool solve(bool ignoreSofts = true) = 0;
  virtual bool read(Node const&, AssignResult&) = 0;
  virtual bool read(Node const&, std::string&) = 0;
  virtual bool readVector(const std::vector<VariablePtr>& vec, __rand_vec_base* rand_vec) = 0;
};

typedef std::shared_ptr<metaSMTVisitor> SolverPtr;

}  // end namespace crave
