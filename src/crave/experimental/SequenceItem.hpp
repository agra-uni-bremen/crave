// Copyright 2014 The CRAVE developers. All rights reserved.

#pragma once

#include "../backend/Generator.hpp"
#include "Object.hpp"
#include "Expression.hpp"
#include "Coverage.hpp"

namespace crave
{
 class crv_sequence_item : public crv_object {
 public:
  crv_sequence_item() : gen_(), built_(false), cloned_(false) {}

  crv_sequence_item(crv_sequence_item const & other) : crv_object(other), gen_(), built_(false), cloned_(true) {}

  std::string obj_kind() const override final { return "crv_sequence_item"; }

  bool randomize() override {
    assert(!cloned_ && "cloned crv_sequence_item cannot be randomized");
    if (!built_) {
      gen_ = std::make_shared<Generator>();
      recursive_build(*gen_);
      built_ = true;
    }
    return gen_->nextCov();
  }

  template <typename... Exprs>
  bool randomize_with(Exprs... exprs) {
    assert(!cloned_ && "cloned crv_sequence_item cannot be randomized");
    // TODO Generator caching
    rand_with_gen_ = std::make_shared<Generator>();
    recursive_build(*rand_with_gen_);
    expression_list list(exprs...);
    for (auto e : list) (*rand_with_gen_)(e);
    return rand_with_gen_->next();
  }

  void goal(crv_covergroup& group) {
    for (auto e : group.bound_var_expr_list()) (*gen_)(e);
    for (auto e : group.uncovered_as_list()) gen_->cover(e);
  }

 protected:
  void request_rebuild() override {
    built_ = false;
    gen_.reset();
    crv_object::request_rebuild();
  }

  std::shared_ptr<Generator> gen_;
  std::shared_ptr<Generator> rand_with_gen_;
  bool built_;
  bool cloned_;
};
}