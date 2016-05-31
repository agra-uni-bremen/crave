// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <string>
#include "../ir/visitor/metaSMTNodeVisitor.hpp"

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
    * creates a new metaSMTVisitor with a specified backend or if unspecified,
    * metaSMTNodeVisitor::solver_type. Caller is responsible for deleting the
    * visitor.
    **/
  static metaSMTVisitor* getNewInstance(SolverTypes type = solver_type_);

  static SolverTypes solver_type_;
};

template <SolverTypes solver_type>
struct FactorySolver {
  static bool isDefined() { return false; }
  static metaSMTVisitor* getNewInstance() { return NULL; }
};
}  // namespace crave
