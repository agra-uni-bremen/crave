#ifndef METASMTNODEVISITOR_HPP_
#define METASMTNODEVISITOR_HPP_

#include "../AssignResult.hpp"
#include "Node.hpp"
#include "NodeVisitor.hpp"

#include <boost/function.hpp>

#include <map>
#include <vector>

namespace crave {

class metaSMTVisitor : public NodeVisitor {
public:
  virtual void makeAssertion( Node const & ) = 0;
  virtual void makeSoftAssertion( Node const & ) = 0;
  virtual void makeAssumption( Node const & ) = 0;
  virtual void addPreHook( boost::function0<bool> ) = 0;
  virtual std::vector<std::vector<unsigned int> > analyseContradiction(
              std::map<unsigned int, boost::intrusive_ptr<Node> > const & ) = 0;
  virtual bool solve() = 0;
  virtual bool read(Node const&, AssignResult&) = 0;
};

} //end namespace crave

#endif /* METASMTNODEVISITOR_HPP_ */
