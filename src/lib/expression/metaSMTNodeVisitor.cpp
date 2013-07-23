#include "../../crave/expression/metaSMTNodeVisitor.hpp"
#include "../../crave/expression/FactoryMetaSMT.hpp"
#include "metaSMTNodeVisitorImpl.hpp"

#include <metaSMT/backend/Boolector.hpp>
#include <metaSMT/backend/SWORD_Backend.hpp>
#include <metaSMT/backend/Z3_Backend.hpp>
#include <metaSMT/backend/CUDD_Distributed.hpp>

#include <metaSMT/BitBlast.hpp>
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/Priority_Context.hpp>
#include <metaSMT/UnpackFuture_Context.hpp>

namespace crave {

std::string metaSMTVisitor::solver_type = "Boolector";

void FactoryMetaSMT::setSolverType(std::string const& type) {
  metaSMTVisitor::solver_type = type;
}

typedef metaSMT::DirectSolver_Context <
  metaSMT::UnpackFuture_Context <
  metaSMT::BitBlast <
  metaSMT::solver::CUDD_Distributed
  > >
> CUDDSolverType;

typedef metaSMT::DirectSolver_Context <
  metaSMT::UnpackFuture_Context <
  metaSMT::solver::SWORD_Backend
  >
> SWORDSolverType;

typedef metaSMT::DirectSolver_Context <
  metaSMT::UnpackFuture_Context <
  metaSMT::solver::Boolector
  >
> BoolectorSolverType;

typedef metaSMT::DirectSolver_Context <
  metaSMT::UnpackFuture_Context <
  metaSMT::solver::Z3_Backend
  >
> Z3SolverType;

metaSMTVisitor* newVisitorSWORD() {
  return new metaSMTVisitorImpl<SWORDSolverType>();
}

metaSMTVisitor* newVisitorBoolector() {
  return new metaSMTVisitorImpl<BoolectorSolverType>();
}

metaSMTVisitor* newVisitorZ3() {
  return new metaSMTVisitorImpl<Z3SolverType>();
}

metaSMTVisitor* newVisitorCudd() {
  return new metaSMTVisitorImpl<CUDDSolverType>();
}

metaSMTVisitor* newVisitorPriority() {
  typedef metaSMT::Priority_Context< SWORDSolverType, CUDDSolverType > SolverType;
  return new metaSMTVisitorImpl<SolverType>();
}

metaSMTVisitor* FactoryMetaSMT::getNewInstance() {
  if (0 == metaSMTVisitor::solver_type.compare("SWORD"))
    return newVisitorSWORD();
  else if (0 == metaSMTVisitor::solver_type.compare("Boolector"))
    return newVisitorBoolector();
  else if (0 == metaSMTVisitor::solver_type.compare("Z3"))
    return newVisitorZ3();
  else if (0 == metaSMTVisitor::solver_type.compare("Cudd"))
    return newVisitorCudd();
  else if (0 == metaSMTVisitor::solver_type.compare("Priority"))
    return newVisitorPriority();
  assert("Unsupported backend is choosen.");
  return NULL;
}

} // end crave namespace
