#pragma once

#include "expression/Node.hpp"

#include <boost/intrusive_ptr.hpp>

#include <ostream>
#include <string>

namespace crave {

struct Statement {

  typedef boost::intrusive_ptr<Node> expression;

  Statement(expression const expr) : expr_(expr) { }

  inline expression const & get_expression() const {
    return expr_;
  }

 private:
  expression expr_;
};

struct VectorStatement : Statement {

  VectorStatement(boost::intrusive_ptr<VectorExpr> vec, Placeholder const &pl, expression expr)
  : Statement(expr), vec_expr_(vec), placeholder_(pl) { }

  inline boost::intrusive_ptr<VectorExpr> get_vector_expr() {
    return vec_expr_;
  }
  inline Placeholder const & get_placeholder() const {
    return placeholder_;
  }

private:
  boost::intrusive_ptr<VectorExpr> vec_expr_;
  Placeholder placeholder_;
};

struct NamedStatement : Statement {

  NamedStatement(std::string const name, int id, expression const expr)
  : Statement(expr), id_(id),  name_(name) { }

  inline std::string const& get_name() const {
    return name_;
  }
  inline int get_var_id() const {
    return id_;
  }

private:
  int id_;
  std::string name_;
};

struct NamedVectorStatement : VectorStatement {

  NamedVectorStatement (std::string name, boost::intrusive_ptr< VectorExpr > vec,
                        const Placeholder& pl, expression expr)
  : VectorStatement(vec, pl, expr), name_(name) { }

  inline std::string const& get_name() const {
    return name_;
  }
private:
  std::string name_;
};

} // end namespace crave
