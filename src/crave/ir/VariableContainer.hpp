// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include "../frontend/AssignResult.hpp"
#include "ReferenceExpression.hpp"

#include <boost/shared_ptr.hpp>

#include <map>
#include <utility>
#include <vector>

namespace crave {

struct VariableContainer {

  typedef std::pair<int, boost::shared_ptr<crave::ReferenceExpression> > ReadRefPair;
  typedef std::pair<int, boost::shared_ptr<crave::AssignResult> > WriteRefPair;

  std::map<int, NodePtr> variables;
  std::map<int, NodePtr> vector_variables;
  std::vector<ReadRefPair> read_references;
  std::vector<WriteRefPair> write_references;
  std::vector<ReadRefPair> dist_references;
};

static VariableContainer variables;

}  // end namespace crave
