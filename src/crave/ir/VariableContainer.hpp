// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "../frontend/AssignResult.hpp"
#include "ReferenceExpression.hpp"

namespace crave {

struct VariableContainer {
  typedef std::pair<int, std::shared_ptr<ReferenceExpression> > ReadRefPair;
  typedef std::pair<int, std::shared_ptr<AssignResult> > WriteRefPair;

  std::map<int, NodePtr> variables;
  std::map<int, NodePtr> vector_variables;
  std::vector<ReadRefPair> read_references;
  std::vector<WriteRefPair> write_references;
  std::vector<ReadRefPair> dist_references;
  std::map<int, int> dist_ref_to_var_map;
};

VariableContainer* variable_container();

}  // end namespace crave
