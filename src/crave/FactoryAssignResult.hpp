#ifndef FACTORYASSIGNRESULT_HPP_
#define FACTORYASSIGNRESULT_HPP_

#include "../lib/AssignResultImpl.hpp"
#include "AssignResult.hpp"

namespace crave {

  template<typename Integral>
  class FactoryAssignResult {
  public:
    /**
      * creates a new metaSMTVisitor with SWORD as the backend.
      * Caller is responsible for deleting the visitor.
      **/
    static AssignResult* newAssignResult();
    static AssignResult* newAssignResult(Integral n);
  };

  template<typename Integral>
  AssignResult* FactoryAssignResult<Integral>::newAssignResult() {
    return newAssignResult(Integral(0));
  };

  template<typename Integral>
  AssignResult* FactoryAssignResult<Integral>::newAssignResult(Integral n) {
    return new AssignResultImpl<Integral>(n);
  };
} // namespace crave;


#endif /* FACTORYASSIGNRESULT_HPP_ */
