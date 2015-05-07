// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include "../ir/visitor/metaSMTNodeVisitor.hpp"

#include <string>

namespace crave {

enum SolverTypes {
  UNDEFINED_SOLVER,
  BOOLECTOR,
  CVC4,
  Z3,
  SWORD,
  CUDD
};

struct FactoryMetaSMT {
  static void setSolverType(std::string const&);
  /**
    * creates a new metaSMTVisitor with a backend specified by
    * metaSMTNodeVisitor::solver_type. Caller is responsible for deleting the
    *visitor.
    **/
  static metaSMTVisitor* getNewInstance();

  static SolverTypes solver_type;
};

template <SolverTypes solver_type>
struct FactorySolver {
  static bool isDefined() { return false; }
  static metaSMTVisitor* getNewInstance() { return NULL; }
};

}  // namespace crave;
