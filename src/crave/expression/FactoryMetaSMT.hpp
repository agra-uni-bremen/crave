#pragma once

#include "metaSMTNodeVisitor.hpp"

#include <string>

namespace crave {

  class FactoryMetaSMT {
  public:
    static void setSolverType(std::string const&);
    /**
      * creates a new metaSMTVisitor with a backend specified by
      * metaSMTNodeVisitor::solver_type. Caller is responsible for deleting the visitor.
      **/
    static metaSMTVisitor* getNewInstance();
  };

} // namespace crave;
