// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

// FIXME this is a workaround for the issue that STP header and Yices2 header cannnot be included
// in the same metaSMTNodeVisitor.cpp

#ifdef metaSMT_USE_YICES2

#include <metaSMT/DirectSolver_Context.hpp>

#include "../crave/backend/FactoryMetaSMT.hpp"
#include "../crave/ir/visitor/metaSMTNodeVisitor.hpp"
#include "metaSMTNodeVisitorImpl.hpp"

#define DEFINE_SOLVER(SOLVER_ENUM, SOLVER_T)                                                 \
  namespace crave {                                                                          \
  template <>                                                                                \
  struct FactorySolver<SOLVER_ENUM> {                                                        \
    typedef metaSMT::DirectSolver_Context<SOLVER_T> SolverType;                              \
    static bool isDefined() { return true; }                                                 \
    static metaSMTVisitor* getNewInstance() { return new metaSMTVisitorImpl<SolverType>(); } \
  };                                                                                         \
  }  // namespace crave

#include <metaSMT/backend/Yices2.hpp>
DEFINE_SOLVER(crave::YICES2, metaSMT::solver::Yices2);

#undef DEFINE_SOLVER

#endif
