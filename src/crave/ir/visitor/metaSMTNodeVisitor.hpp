#pragma once

#include "../../frontend/RandomBase.hpp"
#include "../../frontend/AssignResult.hpp"
#include "../Node.hpp"
#include "NodeVisitor.hpp"

#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <string>
#include <vector>

namespace crave {

class metaSMTVisitor : public NodeVisitor {

 protected:
  typedef boost::intrusive_ptr<VariableExpr> VariablePtr;

 public:
  virtual void makeAssertion(Node const&) = 0;
  virtual void makeSoftAssertion(Node const&) = 0;
  virtual void makeSuggestion(Node const&) = 0;
  virtual void makeAssumption(Node const&) = 0;
  virtual std::vector<unsigned int> analyseSofts(bool exact = false) = 0;
  virtual std::vector<std::vector<unsigned int> > analyseContradiction(
      std::map<unsigned int, NodePtr> const&) = 0;
  virtual bool solve(bool ignoreSofts = true) = 0;
  virtual bool read(Node const&, AssignResult&) = 0;
  virtual bool readVector(std::vector<VariablePtr>& vec,
                          __rand_vec_base& rand_vec) = 0;
};

typedef boost::shared_ptr<metaSMTVisitor> SolverPtr;

}  // end namespace crave
