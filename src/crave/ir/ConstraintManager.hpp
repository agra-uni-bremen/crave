// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <map>
#include <string>

#include "UserConstraintType.hpp"
#include "UserVectorConstraint.hpp"

namespace crave {

int new_constraint_id();

struct ConstraintManager {
  friend struct ConstraintPartitioner;

  typedef std::map<std::string, ConstraintPtr> ConstraintMap;

  ConstraintManager();

  bool enableConstraint(std::string const& key);

  bool disableConstraint(std::string const& key);

  bool isConstraintEnabled(std::string const& key);

  bool isChanged() const;

  void resetChanged();

  ConstraintPtr makeConstraint(std::string const& name, int c_id, NodePtr n, bool const soft = false,
                               bool const cover = false);

  ConstraintPtr makeConstraint(std::string const& name, NodePtr e, bool const soft = false, bool const cover = false) {
    return makeConstraint(name, new_constraint_id(), e, soft, cover);
  }

  ConstraintPtr makeConstraint(NodePtr e, bool const soft = false, bool const cover = false) {
    int id = new_constraint_id();
    return makeConstraint("constraint_" + std::to_string(id), id, e, soft, cover);
  }

  std::ostream& printDotGraph(std::ostream& os);

  template <typename ostream>
  friend ostream& operator<<(ostream& os, const ConstraintManager& set);

 private:
  unsigned id_;
  ConstraintMap constr_map_;
  ConstraintList constraints_;
  bool changed_;
};

template <typename ostream>
ostream& operator<<(ostream& os, const ConstraintManager& set);
}  // namespace crave
