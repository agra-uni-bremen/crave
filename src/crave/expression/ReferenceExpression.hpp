#pragma once

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_01.hpp>

#include "../bitsize_traits.hpp"

namespace crave {

  extern boost::mt19937 rng;

  struct ReferenceExpression {
  public:
    typedef boost::intrusive_ptr<Node> result_type;
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

  struct DistReferenceExpr : ReferenceExpression {
    DistReferenceExpr(float prob, ReferenceExpression::result_type expr)
    : value_(), probability_(prob), expr_(expr) { }

    virtual ReferenceExpression::result_type expr() const {
      float res = boost::uniform_01<float>()(rng);
      value_ = res <= probability_;
      return new EqualOpr(expr_, new Constant(value_));
    }
  private:
    mutable bool value_;
    float probability_;
    ReferenceExpression::result_type expr_;
  };
} /* namespace crave */
