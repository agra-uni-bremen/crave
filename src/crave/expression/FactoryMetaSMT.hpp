#pragma once

#include "metaSMTNodeVisitor.hpp"

namespace crave {

  class FactoryMetaSMT {
  public:
    /**
      * creates a new metaSMTVisitor with SWORD as the backend.
      * Caller is responsible for deleting the visitor.
      **/
    static metaSMTVisitor* newVisitorSWORD();

    static metaSMTVisitor* newVisitorPriority();
  };

} // namespace crave;
