// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <boost/shared_ptr.hpp>

#include <map>
#include <utility>
#include <vector>

#include "ReferenceExpression.hpp"
#include "../frontend/AssignResult.hpp"

namespace crave {

struct VariableContainer {
  typedef std::pair<int, boost::shared_ptr<ReferenceExpression> > ReadRefPair;
  typedef std::pair<int, boost::shared_ptr<AssignResult> > WriteRefPair;

  std::map<int, NodePtr> variables;
  std::map<int, NodePtr> vector_variables;
  std::vector<ReadRefPair> read_references;
  std::vector<WriteRefPair> write_references;
  std::vector<ReadRefPair> dist_references;
  std::map<int, int> dist_ref_to_var_map;
};

VariableContainer* variable_container();

}  // end namespace crave
