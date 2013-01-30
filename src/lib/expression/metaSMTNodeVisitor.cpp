#include "../../crave/expression/Node.hpp"
#include "../../crave/expression/NodeVisitor.hpp"
#include "../../crave/expression/FactoryMetaSMT.hpp"
#include "metaSMTNodeVisitorImpl.hpp"

#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/backend/SWORD_Backend.hpp>

namespace crave {

metaSMTVisitor* FactoryMetaSMT::newVisitorSWORD() {
  return new metaSMTVisitorImpl< metaSMT::DirectSolver_Context< metaSMT::solver::SWORD_Backend > > ();
};

} // end crave namespace
