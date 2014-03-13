#pragma once

#include "../bitsize_traits.hpp"
#include "../Context.hpp"
#include "../RandomBase.hpp"
#include "../VariableContainer.hpp"
#include "EvalVisitor.hpp"
#include "Node.hpp"

#include <boost/proto/eval.hpp>
#include <boost/type_traits/is_signed.hpp>

namespace crave {

class Evaluator {

  typedef NodePtr expression;
  typedef EvalVisitor::eval_map eval_map;

public:
  Evaluator() : assignments_(), visitor_(assignments_) { }

  template<typename var_type, typename value_type>
  void assign(var_type const& var, value_type const& value) {
    unsigned width = bitsize_traits<typename var_type::value_type>::value;
    bool sign = crave::is_signed<typename var_type::value_type>::value;
    assignments_[var.id()] = Constant(value, width, sign);
  }

  template<typename Expr>
  bool evaluate(Expr expr) {
    return evaluate(*get_expression(expr));
  }
  bool evaluate(Node const& expr) {
    return visitor_.evaluate(expr);
  }

  template<typename Expr>
  expression get_expression(Expr expr) const {
    return LWGenerator()(expr);
  }

  template<typename Integer>
  Integer get_result() const {
    return static_cast<Integer>(visitor_.get_result().value());
  }
  unsigned long get_result() const {
    return get_result<unsigned long>();
  }

private:
  eval_map assignments_;
  EvalVisitor visitor_;

  struct LWGenerator {
    LWGenerator()
      : ctx_(crave::variables) { }

    template<typename Expr>
    expression operator()(Expr expr) {
      return boost::proto::eval(expr, ctx_);
    }

  private:
    Context ctx_;
  };
};

} // end namespace crave
