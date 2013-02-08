#include "../../crave/expression/metaSMTNodeVisitor.hpp"
#include "../../crave/expression/FactoryMetaSMT.hpp"
#include "metaSMTNodeVisitorImpl.hpp"

#include <metaSMT/backend/SWORD_Backend.hpp>
#include <metaSMT/BitBlast.hpp>
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/Priority_Context.hpp>
#include <metaSMT/UnpackFuture_Context.hpp>
#include <metaSMT/backend/CUDD_Distributed.hpp> // FIXME: hidden dependencies in CUDD_Distributed.hpp

namespace crave {

metaSMTVisitor* FactoryMetaSMT::newVisitorSWORD() {
  return new metaSMTVisitorImpl< metaSMT::DirectSolver_Context< metaSMT::solver::SWORD_Backend > > ();
};

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

} // end crave namespace
