// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#include "../crave/ir/visitor/metaSMTNodeVisitor.hpp"
#include "../crave/backend/FactoryMetaSMT.hpp"
#include "../crave/utils/Logging.hpp"
#include "metaSMTNodeVisitorImpl.hpp"

#include <metaSMT/BitBlast.hpp>
#include <metaSMT/DirectSolver_Context.hpp>

#include <string>

#define DEFINE_SOLVER(SOLVER_ENUM, SOLVER_T)                                                     \
  namespace crave {                                                                              \
  template <>                                                                                    \
  struct FactorySolver<SOLVER_ENUM> {                                                            \
    typedef metaSMT::DirectSolver_Context<SOLVER_T> SolverType;                                  \
    static bool isDefined() { return true; }                                                     \
    static metaSMTVisitor* getNewInstance() { return new metaSMTVisitorImpl<SolverType>(); }     \
  };                                                                                             \
  }  // namespace crave

#ifdef metaSMT_USE_Boolector
#include <metaSMT/backend/Boolector.hpp>
namespace crave {
class BoolectorSolver : public metaSMT::solver::Boolector {
 public:
  BoolectorSolver() {
#ifndef metaSMT_BOOLECTOR_1_API
    boolector_set_opt(_btor, "rewrite_level", 1);
//    assert(boolector_set_sat_solver_minisat(_btor));
#endif
  }
};
}
DEFINE_SOLVER(crave::BOOLECTOR, crave::BoolectorSolver);
#endif

#ifdef metaSMT_USE_CVC4
#include <metaSMT/backend/CVC4.hpp>
DEFINE_SOLVER(crave::CVC4, metaSMT::solver::CVC4);
#endif

#ifdef metaSMT_USE_STP
#include <metaSMT/backend/STP.hpp>
DEFINE_SOLVER(crave::STP, metaSMT::solver::STP);
#endif

#ifdef metaSMT_USE_SWORD
#include <metaSMT/backend/SWORD_Backend.hpp>
DEFINE_SOLVER(crave::SWORD, metaSMT::solver::SWORD_Backend);
#endif

#ifdef metaSMT_USE_Z3
#include <metaSMT/backend/Z3_Backend.hpp>
DEFINE_SOLVER(crave::Z3, metaSMT::solver::Z3_Backend);
#endif

#ifdef metaSMT_USE_CUDD
#include <metaSMT/backend/CUDD_Distributed.hpp>
#include "../crave/RandomSeedManager.hpp"
namespace crave {
extern RandomSeedManager rng;
class CUDD_Solver : public metaSMT::solver::CUDD_Distributed {
 public:
  CUDD_Solver() { gen.seed((*crave::rng.get())()); }
};
}
DEFINE_SOLVER(crave::CUDD, metaSMT::BitBlast<crave::CUDD_Solver>);
#endif

#undef DEFINE_SOLVER

namespace crave {

SolverTypes FactoryMetaSMT::solver_type_ = UNDEFINED_SOLVER;  // default solver

void FactoryMetaSMT::setSolverType(std::string const& type) {
  if (type == "Boolector")
    solver_type_ = BOOLECTOR;
  else if (type == "CVC4")
    solver_type_ = CVC4;
  else if (type == "STP")
    solver_type_ = STP;
  else if (type == "SWORD")
    solver_type_ = SWORD;
  else if (type == "Yices2")
    solver_type_ = YICES2;
  else if (type == "Z3")
    solver_type_ = Z3;
  else if (type == "Cudd")
    solver_type_ = CUDD;
}

#define TRY_GET_SOLVER(solver)                                                        \
  if (!FactorySolver<solver>::isDefined()) {                                          \
    solver_type_ = UNDEFINED_SOLVER;                                                  \
    LOG(INFO) << #solver << " has not been defined, another solver will be selected"; \
    return getNewInstance();                                                          \
  } else {                                                                            \
    return FactorySolver<solver>::getNewInstance();                                   \
  }

#define TRY_GET_SOLVER_WHEN_UNDEFINED(solver)                   \
  if (FactorySolver<solver>::isDefined()) {                     \
    solver_type_ = solver;                                      \
    LOG(INFO) << #solver << " has been automatically selected"; \
    return getNewInstance();                                    \
  }

metaSMTVisitor* FactoryMetaSMT::getNewInstance(SolverTypes type) {
  switch (type) {
    case BOOLECTOR:
      TRY_GET_SOLVER(BOOLECTOR);
    case SWORD:
      TRY_GET_SOLVER(SWORD);
    case STP:
      TRY_GET_SOLVER(STP);
    case YICES2:
      TRY_GET_SOLVER(YICES2);
    case Z3:
      TRY_GET_SOLVER(Z3);
    case CVC4:
      TRY_GET_SOLVER(CVC4);
    case CUDD:
      TRY_GET_SOLVER(CUDD);
    default:  // UNDEFINED_SOLVER
      TRY_GET_SOLVER_WHEN_UNDEFINED(BOOLECTOR);
      TRY_GET_SOLVER_WHEN_UNDEFINED(SWORD);
      TRY_GET_SOLVER_WHEN_UNDEFINED(STP);
      TRY_GET_SOLVER_WHEN_UNDEFINED(YICES2);
      TRY_GET_SOLVER_WHEN_UNDEFINED(Z3);
      TRY_GET_SOLVER_WHEN_UNDEFINED(CVC4);
      TRY_GET_SOLVER_WHEN_UNDEFINED(CUDD);
      throw std::runtime_error("No solver has been defined.");  
  }
}

}  // namespace crave
