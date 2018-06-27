// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#include "../../crave/experimental/SequenceItem.hpp"
#include "../../crave/backend/Generator.hpp"

namespace crave
{
  crv_sequence_item::crv_sequence_item() : gen_(), built_(false), cloned_(false) {}

  crv_sequence_item::crv_sequence_item(crv_sequence_item const & other) : crv_object(other), gen_(), built_(false), cloned_(true) {}

  std::string crv_sequence_item::obj_kind() const { return "crv_sequence_item"; }

  bool crv_sequence_item::randomize() {
    assert(!cloned_ && "cloned crv_sequence_item cannot be randomized");
    if (!built_) {
      gen_ = std::make_shared<Generator>();
      recursive_build(*gen_);
      built_ = true;
    }
    return gen_->nextCov();
  }
  
  void crv_sequence_item::goal(crv_covergroup& group) {
    for (auto e : group.bound_var_expr_list()) (*gen_)(e);
    for (auto e : group.uncovered_as_list()) gen_->cover(e);
  }
    
  void crv_sequence_item::request_rebuild() {
    built_ = false;
    gen_.reset();
    crv_object::request_rebuild();
  }

  bool crv_sequence_item::randomize_with_expr_list(const expression_list & list) {
    assert(!cloned_ && "cloned crv_sequence_item cannot be randomized");
    // TODO Generator caching
    rand_with_gen_ = std::make_shared<Generator>();
    recursive_build(*rand_with_gen_);
    for (auto e : list) (*rand_with_gen_)(e);
    return rand_with_gen_->next();
  }

  std::ostream& crv_sequence_item::print_dot_graph(std::ostream& os) {
    if (!built_)
      return (os << "Graph is not yet available" << std::endl);
    return gen_->printDotGraph(os);
  }

}
