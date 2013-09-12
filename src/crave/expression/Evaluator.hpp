#pragma once

#include "../Context.hpp"
#include "../ConstrainedRandom.hpp"
#include "../VariableContainer.hpp"
#include "EvalVisitor.hpp"
#include "Node.hpp"

namespace crave {

class Evaluator {

  typedef NodePtr expression;
  typedef EvalVisitor::eval_map eval_map;

public:
  Evaluator() : assignments_(), visitor_(assignments_) { }

  template<typename var_type, typename value_type>
  void assign(var_type const& var, value_type const& value) {
    unsigned width = bitsize_traits<value_type>::value;
    bool sign = boost::is_signed<value_type>::value;
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
      : vars_(), ctx_(vars_) { }

    template<typename Expr>
    expression operator()(Expr expr) {
      return boost::proto::eval(FixWidth()(expr), ctx_);
    }

  private:
    VariableContainer vars_;
    Context ctx_;
  };
};

} // end namespace crave
