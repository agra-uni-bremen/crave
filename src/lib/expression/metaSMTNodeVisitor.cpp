#include "../../crave/expression/metaSMTNodeVisitor.hpp"
#include "../../crave/expression/FactoryMetaSMT.hpp"
#include "metaSMTNodeVisitorImpl.hpp"

#include <metaSMT/backend/SWORD_Backend.hpp>
#include <metaSMT/BitBlast.hpp>
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/Priority_Context.hpp>
#include <metaSMT/UnpackFuture_Context.hpp>
#include <metaSMT/backend/CUDD_Distributed.hpp>

namespace crave {

metaSMTVisitor* FactoryMetaSMT::newVisitorSWORD() {
  return new metaSMTVisitorImpl< metaSMT::DirectSolver_Context< metaSMT::solver::SWORD_Backend > > ();
};

metaSMTVisitor* FactoryMetaSMT::newVisitorCudd() {
  return new metaSMTVisitorImpl<
      metaSMT::DirectSolver_Context< metaSMT::BitBlast< metaSMT::solver::CUDD_Distributed > > > ();
}

metaSMTVisitor* FactoryMetaSMT::newVisitorPriority() {
  typedef metaSMT::DirectSolver_Context <
    metaSMT::UnpackFuture_Context <
    metaSMT::BitBlast <
    metaSMT::solver::CUDD_Distributed
    > >
  > SolverType1;
  typedef metaSMT::DirectSolver_Context <
    metaSMT::UnpackFuture_Context <
    metaSMT::solver::SWORD_Backend
    >
  > SolverType2;

  typedef metaSMT::Priority_Context< SolverType1, SolverType2 > SolverType;
  return new metaSMTVisitorImpl<SolverType>();
}

metaSMTVisitor* FactoryMetaSMT::getInstanceOf(std::string const& type) {
  if (0 == type.compare("SWORD"))
    return newVisitorSWORD();
  else if (0 == type.compare("Cudd"))
    return newVisitorCudd();
  else if (0 == type.compare("Priority"))
    return newVisitorPriority();
  assert("Unsupported backend is choosen.");
  return NULL;
}

} // end crave namespace
