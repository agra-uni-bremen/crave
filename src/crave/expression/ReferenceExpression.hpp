#pragma once

#include "../Distribution.hpp"
#include "../bitsize_traits.hpp"
#include "Node.hpp"

#include <boost/type_traits/is_signed.hpp>

namespace crave {

  struct ReferenceExpression {
  public:
    typedef NodePtr result_type;
    virtual ~ReferenceExpression() { }
    virtual result_type expr() const = 0;
  };

  template<typename Integer>
  struct ReferenceExpressionImpl : public ReferenceExpression {
  public:
    ReferenceExpressionImpl(Integer const& value, ReferenceExpression::result_type expr ) : value_(value), expr_(expr) { }
    virtual ~ReferenceExpressionImpl() { }

  public:
    virtual ReferenceExpression::result_type expr() const {
      unsigned width = bitsize_traits<Integer>::value;
      bool sign = boost::is_signed<Integer>::value;
      return new EqualOpr(expr_, new Constant(value_, width, sign));
    }

  private:
    Integer const & value_;
    ReferenceExpression::result_type expr_;
  };

  template<typename Integer>
  struct DistReferenceExpr : ReferenceExpression {
    DistReferenceExpr(distribution<Integer> dist, ReferenceExpression::result_type expr)
    : dist_(dist), expr_(expr) { }

    virtual ReferenceExpression::result_type expr() const {
      unsigned width = bitsize_traits<Integer>::value;
      bool sign = boost::is_signed<Integer>::value;
      return new EqualOpr(expr_, new Constant(dist_.nextValue(), width, sign));
    }
  private:
    distribution<Integer> dist_;
    ReferenceExpression::result_type expr_;
  };  

} /* namespace crave */
