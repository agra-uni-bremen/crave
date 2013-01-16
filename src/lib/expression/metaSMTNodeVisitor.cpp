#include "../../crave/expression/Node.hpp"
#include "../../crave/expression/NodeVisitor.hpp"
#include "metaSMTNodeVisitorImpl.hpp"

#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/backend/SWORD_Backend.hpp>

namespace crave {

class FactoryMetaSMT {

  /**
    * creates a new metaSMTVisitor with SWORD as the backend.
    * Caller is responsible for deleting the visitor.
    **/
  metaSMTVisitor* newVisitorSWORD() const {
  	return new metaSMTVisitorImpl< metaSMT::DirectSolver_Context< metaSMT::solver::SWORD_Backend > > ();
  }
};


} // end crave namespace
