// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "CoverageBins.hpp"
#include "Expression.hpp"
#include "Object.hpp"
#include "Variable.hpp"
#include "../ir/UserExpression.hpp"
#include "../utils/Evaluator.hpp"

#include <string>
#include <vector>

namespace crave {

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
  crv_coverpoint(crv_object_name = "covpt") : simple_bins_(), transition_bins_() {}

  /**
   * \brief Deleted copy constructor.
   */
  crv_coverpoint(crv_coverpoint const&) = delete;

  std::string obj_kind() const override final { return "crv_coverpoint"; }

  /**
   * \brief Returns a vector of all simple bins of this coverpoint.
   * \return std::vector& of crv_bin
   */
  std::vector<crv_bin>& simple_bins() { return simple_bins_; }

  /**
   * \brief Adds new bins to the bin list.
   * \param elist List of bin expressions. Each expression will create a new bin with minimum hit count equal to 1.
   */
  void add_simple_bins(expression_list elist) {
    for (auto e : elist) simple_bins_.push_back(crv_bin(e));
  }

  /**
   * \brief Sets new bins for the expression list.
   * 
   * All currently existing bins are replaced with the given bin expressions.
   * 
   * \param list List of bin expressions.
   */
  void operator=(expression_list list) {
    simple_bins_.clear();
    add_simple_bins(list);
  }

  /**
   * \brief Adds a new transition bin to the bin list.
   * \param toAdd the transition bin to add.
   */
  void add_transition_bin(crv_transition_bin toAdd) { transition_bins_.push_back(toAdd); }

  /**
   * \brief Returns the last created transition bin of the coverpoint.
   * \return crv_transition_bin& of crv_bin
   */
  crv_transition_bin& last_transition_bin() { return transition_bins_.back(); }

  /**
   * \brief Returns the list of all bins of the coverpoint.
   * \return A vector of crv_abstract_bin pointers
   */
  std::vector<crv_abstract_bin*> all_bins() {
    std::vector<crv_abstract_bin*> res;
    for (crv_bin& cb : simple_bins_) res.push_back(&cb);
    for (crv_transition_bin& cb : transition_bins_) res.push_back(&cb);
    return res;
  }

  /**
   * \brief Creates a cross of two coverpoints cp1 and cp2.
   * 
   * Every simple bin of cp1 will be paired with every simple bin of cp2 to create a new bin.
   * 
   * \param cp1 First coverpoint of the cross
   * \param cp2 Second coverpoint of the cross
   * \return a std::vector of bins of the crossed product of cp1 bins and cp2 bins.
   */
  static std::vector<crv_bin> cross(crv_coverpoint& cp1, crv_coverpoint& cp2) {
    std::vector<crv_bin> result;
    for (crv_bin& cb1 : cp1.simple_bins())
      for (crv_bin& cb2 : cp2.simple_bins()) result.push_back(crv_bin(cb1.bin_expr() && cb2.bin_expr()));
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
    for (crv_abstract_bin* cb : all_bins())
      if (!cb->covered()) return false;
    return true;
  }

  //****************************
  // User visible start-functions
  //****************************

  template <typename Expr>
  std::shared_ptr<crv_transition_fsm_state> start_with(Expr expr) {
    std::shared_ptr<crv_transition_fsm_state> root = crv_transition_fsm_state::next(NULL, expr);
    add_transition_bin(root);
    return root;
  }

  // [* N:M]
  template <unsigned int N, unsigned int M = N, typename Expr>
  std::shared_ptr<crv_transition_fsm_state> start_with_consecutive(Expr expr) {
    std::shared_ptr<crv_transition_fsm_state> root = crv_transition_fsm_state::consecutive_repeat<N, M>(NULL, expr);
    add_transition_bin(root);
    return root;
  }

  // [-> N:M]
  template <unsigned int N, unsigned int M = N, typename Expr>
  std::shared_ptr<crv_transition_fsm_state> start_with_goto(Expr expr) {
    std::shared_ptr<crv_transition_fsm_state> root = crv_transition_fsm_state::goto_repeat<N, M>(NULL, expr);
    add_transition_bin(root);
    return root;
  }

  // [= N:M]
  template <unsigned int N, unsigned int M = N, typename Expr>
  std::shared_ptr<crv_transition_fsm_state> start_with_nonconsecutive(Expr expr) {
    std::shared_ptr<crv_transition_fsm_state> root = crv_transition_fsm_state::non_consecutive_repeat<N, M>(NULL, expr);
    add_transition_bin(root);
    return root;
  }

 private:
  std::vector<crv_bin> simple_bins_;
  std::vector<crv_transition_bin> transition_bins_;
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
      for (crv_abstract_bin* cb : cp->all_bins()) cb->calcHit(eval_);
  }

  /**
   * \brief Prints the coverage report to std::cout.
   */
  void report() {
    if (!built_) build();
    for (crv_coverpoint* cp : points_) {
      std::cout << cp->obj_kind() << " " << cp->name() << std::endl;
      int c = 0;
      for (crv_abstract_bin* cb : cp->all_bins()) {
        std::cout << "  crv_bin " << c++ << ": " << cb->hit_count() << " " << cb->hit_minimum() << " "
                  << (cb->covered() ? "covered" : "uncovered") << std::endl;
      }
    }
  }

  /**
   * \brief Returns an expression_list of uncovered simple bins.
   * \return List of expressions of uncovered crv_bin
   */
  expression_list uncovered_as_list() {
    if (!built_) build();
    expression_list result;
    for (crv_coverpoint* cp : points_)
      for (crv_bin& cb : cp->simple_bins())
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
