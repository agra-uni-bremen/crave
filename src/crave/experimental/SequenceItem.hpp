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
  crv_sequence_item();

  crv_sequence_item(crv_sequence_item const & other);

  std::string obj_kind() const override final;

  bool randomize() override;

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

  void goal(crv_covergroup& group);

 protected:
  void request_rebuild();

  std::shared_ptr<Generator> gen_;
  std::shared_ptr<Generator> rand_with_gen_;
  bool built_;
  bool cloned_;
};
}