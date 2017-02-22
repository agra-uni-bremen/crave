// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "../ir/UserExpression.hpp"
#include "../utils/Evaluator.hpp"
#include "Expression.hpp"
#include "Object.hpp"
#include "Variable.hpp"

#include <string>
#include <vector>

namespace crave {

class crv_covergroup;
class crv_coverpoint;
class crv_bin;

/**
 * \ingroup newAPI
 * \brief A crv_bin represents a set of values for a variable within a coverpoint.
 * 
 * A crv_bin is hit if a value in the bin has been sampled.
 * A crv_bin is considered as covered if it has been hit at least a pre-defined number of times.
 */
class crv_bin {
 public:
  /**
   * \brief Constructor with bin expression and minimal hit count.
   * \param expr The expression that must be true to hit this bin.
   * \param min The minimum hit count that is needed to count this bin as covered.
   */
  template <typename Expr>
  crv_bin(Expr expr, unsigned min = 1) : bin_expr_(make_expression(expr)), hit_minimum_(min), hit_count_(0) {}

  /**
   * \brief Returns the expression this bin must met for a hit.
   * \return The bin expression
   */
  expression bin_expr() { return bin_expr_; }
  /**
   * \brief Returns the minimal hit count.
   * \return The minimal hit count
   */
  unsigned hit_minimum() { return hit_minimum_; }
  /**
   * \brief Sets the minimal hit count to a new value.
   * \param min New hit minimum.
   */
  void hit_minimum(unsigned min) { hit_minimum_ = min; }
  /**
   * \brief Returns the current amount of hits.
   * \return Counter how often this bin has been hit.
   */
  unsigned hit_count() { return hit_count_; }
  /**
   * \brief Increases the hit counter.
   */
  void hit() { hit_count_++; }
  /**
   * \brief Checks if the bin is covered or not.
   * \return true if the bin is covered, false otherwise.
   */
  bool covered() { return hit_count_ >= hit_minimum_; }

 private:
  expression bin_expr_;
  unsigned hit_minimum_;
  unsigned hit_count_;
};

/**
 * \ingroup newAPI
 * \brief A covergroup contains a set of bins for a variable
 * 
 * A covergroup contains a set of bins for a variable.
 * A coverpoint is considered as covered if its bins are covered.
 * A coverpoint is typically bound to a variable and measures its coverage.
 */
class crv_coverpoint : public crv_object {
 public:
  /**
   * \brief Constructor by name.
   * \param name of the coverpoint, covpt by default.
   */
  crv_coverpoint(crv_object_name = "covpt") : bins_() {}

  /**
   * \brief Deleted copy constructor.
   */
  crv_coverpoint(crv_coverpoint const&) = delete;

  std::string obj_kind() const override final { return "crv_coverpoint"; }

  /**
   * \brief Returns a vector of all bins of this coverpoint.
   * \return std::vector& of crv_bin
   */
  std::vector<crv_bin>& bins() { return bins_; }

  /**
   * \brief Adds new bins to the bin list.
   * \param elist List of bin expressions. Each expression will create a new bin with minimum hit count equal to 1.
   */
  void bins(expression_list elist) {
    for (auto e : elist) bins_.push_back(crv_bin(e));
  }

  /**
   * \brief Sets new bins for the expression list.
   * 
   * All currently existing bins are replaced with the given bin expressions.
   * 
   * \param list List of bin expressions.
   */
  void operator=(expression_list list) {
    bins_.clear();
    bins(list);
  }

  /**
   * \brief Creates a cross of two coverpoints cp1 and cp2.
   * 
   * Every bin of cp1 will be paired with every bin of cp2 to create a new bin.
   * 
   * \param cp1 First coverpoint of the cross
   * \param cp2 Second coverpoint of the cross
   * \return a std::vector of bins of the crossed product of cp1 bins and cp2 bins.
   */
  static std::vector<crv_bin> cross(crv_coverpoint& cp1, crv_coverpoint& cp2) {
    std::vector<crv_bin> result;
    for (crv_bin& cb1 : cp1.bins())
      for (crv_bin& cb2 : cp2.bins()) result.push_back(crv_bin(cb1.bin_expr() && cb2.bin_expr()));
    return result;
  }

  /**
   * \brief Returns if this coverpoint is covered or not.
   * 
   * A coverpoint is covered only is all bins are covered.
   * 
   * \return true if the coverpoint is covered, false otherwise.
   */
  bool covered() {
    for (crv_bin& cb : bins_)
      if (!cb.covered()) return false;
    return true;
  }

 private:
  std::vector<crv_bin> bins_;
};

/**
 * \ingroup newAPI
 * \brief Group of coverpoints.
 * 
 * A crv_covergroup is the top-level coverage construct that has one or more coverpoints as member variables.
 */
class crv_covergroup : public crv_object {
 public:
  /**
  * \brief Empty constructor.
  */
  crv_covergroup() : points_(), vars_(), built_(false), eval_() {}

  /**
   * \brief Deleted copy constructor.
   */
  crv_covergroup(crv_covergroup const&) = delete;

  std::string obj_kind() const override { return "crv_covergroup"; }

  /**
   * \brief Samples the current state of the variables and hit bins if their expressions are evaluated to true.
   */
  void sample() {
    if (!built_) build();
    for (auto v : vars_) eval_.assign(v->id(), v->constant_expr());
    for (crv_coverpoint* cp : points_)
      for (crv_bin& cb : cp->bins())
        if (eval_.evaluate(cb.bin_expr()) && eval_.result<bool>()) cb.hit();
  }

  /**
   * \brief Prints the coverage report to std::cout.
   */
  void report() {
    if (!built_) build();
    for (crv_coverpoint* cp : points_) {
      std::cout << cp->obj_kind() << " " << cp->name() << std::endl;
      int c = 0;
      for (crv_bin& cb : cp->bins()) {
        std::cout << "  crv_bin " << c++ << ": " << cb.hit_count() << " " << cb.hit_minimum() << " "
                  << (cb.covered() ? "covered" : "uncovered") << std::endl;
      }
    }
  }

  /**
   * \brief Returns an expression_list of uncovered bins.
   * \return List of expressions of uncovered crv_bin
   */
  expression_list uncovered_as_list() {
    if (!built_) build();
    expression_list result;
    for (crv_coverpoint* cp : points_)
      for (crv_bin& cb : cp->bins())
        if (!cb.covered()) result.add_expr(cb.bin_expr());
    return result;
  }

  /**
   * Internal implementation details for coverage-directed generation.
   */
  expression_list bound_var_expr_list() {
    if (!built_) build();
    expression_list result;
    for (auto v : vars_) result.add_expr(v->bound_expr());
    return result;
  }

  /**
   * \brief Returns if this covergroup is fully covered or not.
   * 
   * A covergroup is covered if all contained coverpoints are covered.
   * 
   * \return true if the group is covered, false otherwise
   */
  bool covered() {
    if (!built_) build();
    for (crv_coverpoint* cp : points_)
      if (!cp->covered()) return false;
    return true;
  }

 private:
  void build() {
    built_ = true;
    for (crv_object* obj : children_) {
      if (obj->obj_kind() == "crv_variable") {
        crv_variable_base_* v = (crv_variable_base_*)obj;
        vars_.push_back(v);
      }
      if (obj->obj_kind() == "crv_coverpoint") {
        crv_coverpoint* cp = (crv_coverpoint*)obj;
        points_.push_back(cp);
      }
    }
  }

  std::vector<crv_coverpoint*> points_;   
  std::vector<crv_variable_base_*> vars_; 
  bool built_;                            
  Evaluator eval_;                        
};

}  // end namespace crave
