#pragma once

#include "expression/Node.hpp"

#include <boost/intrusive_ptr.hpp>

namespace crave {

struct Statement {

  typedef boost::intrusive_ptr<Node> expression;

  Statement(expression const expr) : expr_(expr) { }

 private:
  expression expr_;
};

struct ForeachStatement : public Statement {
  ForeachStatement(VectorExpr const &vec, Placeholder const &pl, expression expr) : Statement(expr), vec_expr_(vec), placeholder_(pl) { }
 private:
  VectorExpr vec_expr_;
  Placeholder placeholder_;
};

} // end namespace crave
