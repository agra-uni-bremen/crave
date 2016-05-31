// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "Object.hpp"
#include "Expression.hpp"
#include "Coverage.hpp"

namespace crave
{
struct Generator;

class crv_sequence_item : public crv_object {
 public:
  crv_sequence_item();

  crv_sequence_item(crv_sequence_item const & other);

  std::string obj_kind() const override final;

  bool randomize() override;

  template <typename... Exprs>
  bool randomize_with(Exprs... exprs) {
    expression_list list(exprs...);
    return randomize_with_expr_list(list);
  }

  void goal(crv_covergroup& group);

 protected:
  void request_rebuild();

  bool randomize_with_expr_list(expression_list const&);
  std::shared_ptr<Generator> gen_;
  std::shared_ptr<Generator> rand_with_gen_;
  bool built_;
  bool cloned_;
};
}