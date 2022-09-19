// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include <vector>

#include "../frontend/bitsize_traits.hpp"
#include "../ir/UserExpression.hpp"

namespace crave {

/**
 * \ingroup newAPI
 * \brief Class for list of expressions (constraints)
 */
class expression_list {
  typedef std::vector<expression> container_type;

 public:
  /**
   * \brief Constructor with expressions.
   * \param exprs Comma seperated list of expressions
   */
  template <typename... Exprs>
  expression_list(Exprs... exprs) : exprs_() {
    add_exprs(exprs...);
  }

  /**
   * \brief Assignment operator
   * \param list other expression_list to assign to this list.
   */
  void operator=(expression_list const& list);

  /**
   * \brief Merges to expressions lists by appending the other list to this list.
   * \param list Other expression_list to join
   */
  void join(expression_list const& list);

  /**
   * \brief Add an expression to this object.
   * \param expr The expression to add.
   */
  template <typename Expr>
  void add_expr(Expr expr) {
    exprs_.push_back(make_expression(expr));
  }

  /**
   * \brief Get the begin iterator.
   *
   * Returns the same iterator a std::vector would return.
   *
   * \return Begin iterator of the list
   */
  container_type::iterator begin() { return exprs_.begin(); }

  /**
   * \brief Get the end iterator.
   *
   * Returns the same iterator a std::vector would return.
   *
   * \return End iterator of the list
   */
  container_type::iterator end() { return exprs_.end(); }

  /**
   * \brief Get the const begin iterator.
   *
   * Returns the same iterator a std::vector would return.
   *
   * \return Const begin iterator of the list
   */
  container_type::const_iterator begin() const { return exprs_.begin(); }

  /**
   * \brief Get the const end iterator.
   *
   * Returns the same iterator a std::vector would return.
   *
   * \return Const end iterator of the list
   */
  container_type::const_iterator end() const { return exprs_.end(); }

 private:
  void add_exprs() {}

  template <typename Expr, typename... Exprs>
  void add_exprs(Expr expr, Exprs... exprs) {
    add_expr(expr);
    add_exprs(exprs...);
  }

  container_type exprs_;
};

/**
 * A pre-defined expression for constraining an integer value to be one-hot
 */
template <typename IntegerType>
expression onehot(IntegerType const& x) {
  return crave::make_expression((x > 0) && ((x & (x - 1)) == 0));
}

/**
 * A pre-defined expression for counting bit in the binary representation of a variable
 */
template <typename IntegerType>
expression bitcount(IntegerType const& x) {
  expression result = value_to_expression(0);
  for (unsigned i = 0; i < bitsize_traits<IntegerType>::value; i++)
    result = crave::make_expression(result + ((x >> i) & 1));
  return result;
}

}  // namespace crave
