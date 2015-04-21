#pragma once

#include "../frontend/bitsize_traits.hpp"
#include "../frontend/RandomBase.hpp"
#include "../ir/Context.hpp"
#include "../ir/Node.hpp"
#include "../ir/VariableContainer.hpp"
#include "../ir/visitor/EvalVisitor.hpp"

#include <boost/proto/eval.hpp>
#include <boost/type_traits/is_signed.hpp>

namespace crave {

class Evaluator {
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
    return evaluate(make_expression(expr));
  }
  bool evaluate(expression const & expr) {
    return visitor_.evaluate(*boost::proto::value(expr));
  }

  template<typename Integer>
  Integer result() const {
    return static_cast<Integer>(visitor_.result().value());
  }
  unsigned long result() const {
    return result<unsigned long>();
  }

private:
  eval_map assignments_;
  EvalVisitor visitor_;
};

} // end namespace crave
