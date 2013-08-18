#include "../../crave/expression/metaSMTNodeVisitor.hpp"
#include "../../crave/expression/FactoryMetaSMT.hpp"
#include "metaSMTNodeVisitorImpl.hpp"

#ifdef metaSMT_USE_Boolector
  #include <metaSMT/backend/Boolector.hpp>
#endif
#ifdef metaSMT_USE_SWORD
  #include <metaSMT/backend/SWORD_Backend.hpp>
#endif
#ifdef metaSMT_USE_Z3
  #include <metaSMT/backend/Z3_Backend.hpp>
#endif
#ifdef metaSMT_USE_CUDD
  #include <metaSMT/backend/CUDD_Distributed.hpp>
#endif

#include <metaSMT/BitBlast.hpp>
#include <metaSMT/DirectSolver_Context.hpp>

#if defined(metaSMT_USE_CUDD) && defined(metaSMT_USE_Boolector)
  #include <metaSMT/Priority_Context.hpp>
#endif

#include <metaSMT/UnpackFuture_Context.hpp>

namespace crave {

#ifdef metaSMT_USE_Boolector
  std::string metaSMTVisitor::solver_type = "Boolector";
#elif metaSMT_USE_Z3
  std::string metaSMTVisitor::solver_type = "Z3";
#elif metaSMT_USE_SWORD
  std::string metaSMTVisitor::solver_type = "SWORD";
#elif metaSMT_USE_CUDD
  std::string metaSMTVisitor::solver_type = "Cudd";
#endif

void FactoryMetaSMT::setSolverType(std::string const& type) {
  metaSMTVisitor::solver_type = type;
}

#ifdef metaSMT_USE_SWORD
typedef metaSMT::DirectSolver_Context <
  metaSMT::UnpackFuture_Context <
  metaSMT::solver::SWORD_Backend
  >
> SWORDSolverType;

metaSMTVisitor* newVisitorSWORD() {
  return new metaSMTVisitorImpl<SWORDSolverType>();
}
#endif

#ifdef metaSMT_USE_Boolector
typedef metaSMT::DirectSolver_Context <
  metaSMT::UnpackFuture_Context <
  metaSMT::solver::Boolector
  >
> BoolectorSolverType;

metaSMTVisitor* newVisitorBoolector() {
  return new metaSMTVisitorImpl<BoolectorSolverType>();
}
#endif

#ifdef metaSMT_USE_Z3
typedef metaSMT::DirectSolver_Context <
  metaSMT::UnpackFuture_Context <
  metaSMT::solver::Z3_Backend
  >
> Z3SolverType;

metaSMTVisitor* newVisitorZ3() {
  return new metaSMTVisitorImpl<Z3SolverType>();
}
#endif

#ifdef metaSMT_USE_CUDD
typedef metaSMT::DirectSolver_Context <
  metaSMT::UnpackFuture_Context <
  metaSMT::BitBlast <
  metaSMT::solver::CUDD_Distributed
  > >
> CUDDSolverType;

metaSMTVisitor* newVisitorCudd() {
  return new metaSMTVisitorImpl<CUDDSolverType>();
}
#endif

#if defined(metaSMT_USE_CUDD) && defined(metaSMT_USE_Boolector)
typedef metaSMT::Priority_Context< BoolectorSolverType, CUDDSolverType > SolverType;

metaSMTVisitor* newVisitorPriority() {
  return new metaSMTVisitorImpl<SolverType>();
}
#endif

metaSMTVisitor* FactoryMetaSMT::getNewInstance() {
#ifdef metaSMT_USE_SWORD
  if (0 == metaSMTVisitor::solver_type.compare("SWORD"))
    return newVisitorSWORD();
#endif
#if defined metaSMT_USE_Boolector
  if (0 == metaSMTVisitor::solver_type.compare("Boolector"))
    return newVisitorBoolector();
#endif
#ifdef metaSMT_USE_Z3
  if (0 == metaSMTVisitor::solver_type.compare("Z3"))
    return newVisitorZ3();
#endif
#ifdef metaSMT_USE_CUDD
  if (0 == metaSMTVisitor::solver_type.compare("Cudd"))
    return newVisitorCudd();
#endif
#if defined(metaSMT_USE_CUDD) && defined(metaSMT_USE_Boolector)
  if (0 == metaSMTVisitor::solver_type.compare("Priority"))
    return newVisitorPriority();
#endif
  assert("Unsupported backend is choosen.");
  return NULL;
}

} // end crave namespace
