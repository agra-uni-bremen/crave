// Copyright 2014 The CRAVE developers. All rights reserved.//

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
};

extern VariableContainer variables;

//static VariableContainer& variable_container();
static VariableContainer& variable_container() { return variables; }


}  // end namespace crave
