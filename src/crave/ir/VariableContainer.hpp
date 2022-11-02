// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "../frontend/AssignResultToRef.hpp"
#include "ReferenceExpressionImpl.hpp"

namespace crave {
extern int new_var_id();

struct VariableContainer {
  typedef std::pair<int, std::shared_ptr<ReferenceExpression> > ReadRefPair;
  typedef std::pair<int, std::shared_ptr<AssignResult> > WriteRefPair;

  template <typename T>
  NodePtr new_write_var(T* var, unsigned width, bool sign) {
    auto it = ptr_to_id.find(var);
    if (it != ptr_to_id.end()) return variables[it->second];
    int id = new_var_id();
    ptr_to_id[var] = id;
    write_references.push_back(
        std::make_pair(id, std::shared_ptr<AssignResult>(new AssignResultToRef<T>(var, width, sign))));
    return variables[id] = std::make_shared<VariableExpr>(id, width, sign);
  }

  template <typename T>
  NodePtr new_write_var(Variable<T>* var, unsigned width, bool sign) {
    NodePtr& node = variables[var->id()];
    if (node) return node;
    return node = std::make_shared<VariableExpr>(var->id(), width, sign);
  }

  template <typename T>
  NodePtr new_read_var(const T* var, unsigned width, bool sign) {
    auto it = ptr_to_id.find(var);
    if (it != ptr_to_id.end()) return variables[it->second];
    int id = new_var_id();
    ptr_to_id[var] = id;
    NodePtr node = variables[id] = std::make_shared<VariableExpr>(id, width, sign);
    std::shared_ptr<ReferenceExpression> ref_expr(new ReferenceExpressionImpl<T>(*var, node));
    read_references.push_back(std::make_pair(id, ref_expr));
    return node;
  }

  template <typename T>
  NodePtr new_read_var(Variable<T>* var, unsigned width, bool sign) {
    NodePtr& node = variables[var->id()];
    if (node) return node;
    return node = std::make_shared<VariableExpr>(var->id(), width, sign);
  }

  std::map<int, NodePtr> variables;
  std::map<int, NodePtr> vector_variables;
  std::vector<ReadRefPair> read_references;
  std::vector<WriteRefPair> write_references;
  std::vector<ReadRefPair> dist_references;
  std::map<int, int> dist_ref_to_var_map;
  std::map<const void*, int> ptr_to_id;
};

VariableContainer* variable_container();

}  // end namespace crave
