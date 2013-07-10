#pragma once

#include "metaSMTNodeVisitor.hpp"

#include <string>

namespace crave {

  class FactoryMetaSMT {
  public:
    /**
      * creates a new metaSMTVisitor with SWORD as the backend.
      * Caller is responsible for deleting the visitor.
      **/
    static metaSMTVisitor* getInstanceOf(std::string const&);
  };

} // namespace crave;
