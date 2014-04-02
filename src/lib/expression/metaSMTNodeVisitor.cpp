#include "../../crave/ir/visitor/metaSMTNodeVisitor.hpp"
#include "../../crave/backend/FactoryMetaSMT.hpp"
#include "metaSMTNodeVisitorImpl.hpp"

#include <metaSMT/BitBlast.hpp>
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/UnpackFuture_Context.hpp>

#ifdef metaSMT_USE_Boolector
  #include <metaSMT/backend/Boolector.hpp>
  typedef metaSMT::DirectSolver_Context <
    metaSMT::UnpackFuture_Context <
    metaSMT::solver::Boolector
    >
  > BoolectorSolverType;
#endif

#ifdef metaSMT_USE_SWORD
  #include <metaSMT/backend/SWORD_Backend.hpp>
  typedef metaSMT::DirectSolver_Context <
    metaSMT::UnpackFuture_Context <
    metaSMT::solver::SWORD_Backend
    >
  > SWORDSolverType;
#endif

#ifdef metaSMT_USE_Z3
  #include <metaSMT/backend/Z3_Backend.hpp>
  typedef metaSMT::DirectSolver_Context <
    metaSMT::UnpackFuture_Context <
    metaSMT::solver::Z3_Backend
    >
  > Z3SolverType;
#endif

#ifdef metaSMT_USE_CUDD
  #include <metaSMT/backend/CUDD_Distributed.hpp>
  typedef metaSMT::DirectSolver_Context <
    metaSMT::UnpackFuture_Context <
    metaSMT::BitBlast <
    metaSMT::solver::CUDD_Distributed
    > >
  > CUDDSolverType;
#endif

#if defined(metaSMT_USE_CUDD) && defined(metaSMT_USE_Boolector)
  #include <metaSMT/Priority_Context.hpp>
  typedef metaSMT::Priority_Context< BoolectorSolverType, CUDDSolverType > SolverType;
#endif

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
metaSMTVisitor* FactoryMetaSMT::getNewInstance() {
#ifdef metaSMT_USE_SWORD
  if (0 == metaSMTVisitor::solver_type.compare("SWORD"))
    return new metaSMTVisitorImpl<SWORDSolverType>();
#endif
#if defined metaSMT_USE_Boolector
  if (0 == metaSMTVisitor::solver_type.compare("Boolector"))
    return new metaSMTVisitorImpl<BoolectorSolverType>();
#endif
#ifdef metaSMT_USE_Z3
  if (0 == metaSMTVisitor::solver_type.compare("Z3"))
    return new metaSMTVisitorImpl<Z3SolverType>();
#endif
#ifdef metaSMT_USE_CUDD
  if (0 == metaSMTVisitor::solver_type.compare("Cudd"))
    return new metaSMTVisitorImpl<CUDDSolverType>();
#endif
#if defined(metaSMT_USE_CUDD) && defined(metaSMT_USE_Boolector)
  if (0 == metaSMTVisitor::solver_type.compare("Priority"))
    return new metaSMTVisitorImpl<SolverType>();
#endif
  assert("Unsupported backend is choosen.");
  return NULL;
}

} // end crave namespace
