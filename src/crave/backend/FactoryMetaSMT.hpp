#pragma once

#include "../ir/visitor/metaSMTNodeVisitor.hpp"

#include <string>

namespace crave {

  enum solver_t {
    UNDEFINED_SOLVER,
    BOOLECTOR, 
    CVC4, 
    Z3, 
    SWORD, 
    CUDD
  };

  class FactoryMetaSMT {
  public:
    static void setSolverType(std::string const&);
    /**
      * creates a new metaSMTVisitor with a backend specified by
      * metaSMTNodeVisitor::solver_type. Caller is responsible for deleting the visitor.
      **/
    static metaSMTVisitor* getNewInstance();

    static solver_t solver_type;
  };

} // namespace crave;

