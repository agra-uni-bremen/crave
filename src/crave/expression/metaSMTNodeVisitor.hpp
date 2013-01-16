#ifndef METASMTNODEVISITOR_HPP_
#define METASMTNODEVISITOR_HPP_

#include "Node.hpp"
#include "NodeVisitor.hpp"

namespace crave {

class metaSMTVisitor : public NodeVisitor {
public:

  virtual void makeAssertion( Node const &);
  virtual void makeAssumption( Node const&);
  virtual bool solve();
};

} //end namespace crave

#endif /* METASMTNODEVISITOR_HPP_ */
