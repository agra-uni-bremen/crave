// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "Object.hpp"
#include "Variable.hpp"
#include "Expression.hpp"
#include "../ir/UserExpression.hpp"
#include "../utils/Evaluator.hpp"

#include <glog/logging.h>

#include <string>
#include <vector>

namespace crave {

class crv_covergroup;
class crv_coverpoint;
class crv_bin;

class crv_bin {
 public:
  template <typename Expr>
  crv_bin(Expr expr, unsigned min = 1)
      : bin_expr_(make_expression(expr)), hit_minimum_(min), hit_count_(0) {}

  expression bin_expr() { return bin_expr_; }
  unsigned hit_minimum() { return hit_minimum_; }
  void hit_minimum(unsigned min) { hit_minimum_ = min; }
  unsigned hit_count() { return hit_count_; }
  void hit() { hit_count_++; }
  bool covered() { return hit_count_ >= hit_minimum_; }

 private:
  expression bin_expr_;
  unsigned hit_minimum_;
  unsigned hit_count_;
};

class crv_coverpoint : public crv_object {
 public:
  crv_coverpoint(crv_object_name = "covpt") : bins_() {}

  crv_coverpoint(crv_coverpoint const &) = delete;

  std::string obj_kind() const override final { return "crv_coverpoint"; }

  std::vector<crv_bin>& bins() { return bins_; }

  void bins(expression_list elist) {
    for (auto e : elist) bins_.push_back(crv_bin(e));
  }

  void operator=(expression_list list) {
    bins_.clear();
    bins(list);
  }

  static std::vector<crv_bin> cross(crv_coverpoint& cp1, crv_coverpoint& cp2) {
    std::vector<crv_bin> result;
    for (crv_bin& cb1 : cp1.bins())
      for (crv_bin& cb2 : cp2.bins()) result.push_back(crv_bin(cb1.bin_expr() && cb2.bin_expr()));
    return result;
  }

  bool covered() {
    for (crv_bin& cb : bins_)
      if (!cb.covered()) return false;
    return true;
  }

 private:
  std::vector<crv_bin> bins_;
};

class crv_covergroup : public crv_object {
 public:
  crv_covergroup() : points_(), vars_(), built_(false), eval_() {}

  crv_covergroup(crv_covergroup const &) = delete;

  std::string obj_kind() const override { return "crv_covergroup"; }

  void sample() {
    if (!built_) build();
    for (auto v : vars_) eval_.assign(v->id(), v->constant_expr());
    for (crv_coverpoint* cp : points_)
      for (crv_bin& cb : cp->bins())
        if (eval_.evaluate(cb.bin_expr()) && eval_.result<bool>()) cb.hit();
  }

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

  expression_list uncovered_as_list() {
    if (!built_) build();
    expression_list result;
    for (crv_coverpoint* cp : points_)
      for (crv_bin& cb : cp->bins())
        if (!cb.covered()) result.add_expr(cb.bin_expr());
    return result;
  }

  expression_list bound_var_expr_list() {
    if (!built_) build();
    expression_list result;
    for (auto v : vars_) result.add_expr(v->bound_expr());
    return result;
  }

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
