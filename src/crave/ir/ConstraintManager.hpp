// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <glog/logging.h>
#include <boost/make_shared.hpp>

#include <string>
#include <map>
#include "UserConstraintType.hpp"
#include "UserVectorConstraint.hpp"
#include "Context.hpp"
#include "visitor/FixWidthVisitor.hpp"
#include "visitor/GetSupportSetVisitor.hpp"
#include "visitor/ToDotNodeVisitor.hpp"

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

  template <typename Expr>
  ConstraintPtr makeConstraint(std::string const& name, int c_id, Expr e,
                               Context *ctx, bool const soft = false,
                               bool const cover = false) {
    LOG(INFO) << "New " << (soft ? "soft " : "") << (cover ? "cover " : "")
              << "constraint " << name << " in set " << id_;

    if (constr_map_.find(name) != constr_map_.end()) {
      throw std::runtime_error("Constraint already exists.");
    }

    FixWidthVisitor fwv;
    NodePtr n(fwv.fixWidth(*boost::proto::eval(e, *ctx)));

    return ConstraintManager::makeConstraint(name,c_id, n,ctx,soft,cover);
  }

  ConstraintPtr makeConstraint(std::string const& name, int c_id, NodePtr n,
                               Context *ctx, bool const soft = false,
                               bool const cover = false);

  template <typename Expr>
  ConstraintPtr makeConstraint(std::string const& name, Expr e, Context *ctx,
                               bool const soft = false,
                               bool const cover = false) {
    return makeConstraint(name, new_constraint_id(), e, ctx, soft, cover);
  }

  template <typename Expr>
  ConstraintPtr makeConstraint(Expr e, Context *ctx, bool const soft = false,
                               bool const cover = false) {
    int id = new_constraint_id();
    return makeConstraint("constraint_" + boost::lexical_cast<std::string>(id),
                          id, e, ctx, soft, cover);
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
