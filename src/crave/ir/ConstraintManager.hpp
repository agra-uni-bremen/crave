// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include "Context.hpp"

#include "visitor/FixWidthVisitor.hpp"
#include "visitor/GetSupportSetVisitor.hpp"
#include "visitor/ToDotNodeVisitor.hpp"

#include <glog/logging.h>

namespace crave {

int new_constraint_id();

struct ConstraintManager {
  friend struct ConstraintPartitioner;

  typedef std::map<std::string, ConstraintPtr> ConstraintMap;

  ConstraintManager() : changed_(false) {
    static unsigned ID = 0;
    id_ = ++ID;
  }

  template <typename ostream>
  friend ostream& operator<<(ostream& os, const ConstraintManager& set) {
    os << "Set " << set.id_ << " has " << set.constraints_.size() << " constraint(s) and has "
       << (set.changed_ ? "" : "not ") << "changed" << std::endl;

    BOOST_FOREACH(ConstraintPtr item, set.constraints_) { os << item << std::endl; }
    os << std::flush;
    return os;
  }

  bool enableConstraint(std::string const& key) {
    LOG(INFO) << "Enable constraint " << key << " in set " << id_ << ": ";
    ConstraintMap::iterator ite = constr_map_.find(key);
    if (ite != constr_map_.end()) {
      if (!ite->second->isEnabled()) {
        LOG(INFO) << "  ok";
        ite->second->enable();
        changed_ = true;
      } else
        LOG(INFO) << "  already enabled";
      return true;
    }
    LOG(INFO) << "not found";
    return false;
  }

  bool disableConstraint(std::string const& key) {
    LOG(INFO) << "Disable constraint " << key << " in set " << id_ << ": ";
    ConstraintMap::iterator ite = constr_map_.find(key);
    if (ite != constr_map_.end()) {
      if (ite->second->isEnabled()) {
        LOG(INFO) << "  ok";
        ite->second->disable();
        changed_ = true;
      } else
        LOG(INFO) << "  already enabled";
      return true;
    }
    LOG(INFO) << "  not found";
    return false;
  }

  bool isConstraintEnabled(std::string const& key) {
    ConstraintMap::iterator ite = constr_map_.find(key);
    return ite != constr_map_.end() && ite->second->isEnabled();
  }

  bool isChanged() const { return changed_; }

  void resetChanged() { changed_ = false; }

  template <typename Expr>
  ConstraintPtr makeConstraint(std::string const& name, int c_id, Expr e, Context& ctx, bool const soft = false,
                               bool const cover = false) {
    LOG(INFO) << "New " << (soft ? "soft " : "") << (cover ? "cover " : "") << "constraint " << name << " in set "
              << id_;

    if (constr_map_.find(name) != constr_map_.end()) throw std::runtime_error("Constraint already exists.");

    FixWidthVisitor fwv;
    NodePtr n(fwv.fixWidth(*boost::proto::eval(e, ctx)));

    GetSupportSetVisitor gssv;
    n->visit(gssv);

    ConstraintPtr c(boost::dynamic_pointer_cast<ForEach>(n) != 0
                        ? new UserVectorConstraint(c_id, n, name, gssv.getSupportVars(), false, soft, cover)
                        : (boost::dynamic_pointer_cast<Unique>(n) != 0
                               ? new UserVectorConstraint(c_id, n, name, gssv.getSupportVars(), true, soft, cover)
                               : new UserConstraint(c_id, n, name, gssv.getSupportVars(), soft, cover)));

    assert(!c->isSoft() || !c->isCover());              // soft cover constraint not defined/supported yet
    assert(!c->isVectorConstraint() || !c->isCover());  // cover vector constraint not defined/supported yet

    changed_ = true;
    constraints_.push_back(c);

    return constr_map_[name] = c;
  }

  template <typename Expr>
  ConstraintPtr makeConstraint(std::string const& name, Expr e, Context& ctx, bool const soft = false,
                               bool const cover = false) {
    return makeConstraint(name, new_constraint_id(), e, ctx, soft, cover);
  }

  template <typename Expr>
  ConstraintPtr makeConstraint(Expr e, Context& ctx, bool const soft = false, bool const cover = false) {
    int id = new_constraint_id();
    return makeConstraint("constraint_" + boost::lexical_cast<std::string>(id), id, e, ctx, soft, cover);
  }

  std::ostream& printDotGraph(std::ostream& os) {
    ToDotVisitor visitor(os);

    BOOST_FOREACH(ConstraintPtr c, constraints_) {
      long a = reinterpret_cast<long>(&*c);
      long b = reinterpret_cast<long>(&(*c->expr()));
      os << "\t" << a << " [label=\"" << c->name() << (c->isSoft() ? " soft" : "") << (c->isCover() ? " cover" : "")
         << (!c->isEnabled() ? " disabled" : "") << "\"]" << std::endl;
      os << "\t" << a << " -> " << b << std::endl;
      c->expr()->visit(visitor);
    }
    return os;
  }

 private:
  unsigned id_;
  ConstraintMap constr_map_;
  ConstraintList constraints_;
  bool changed_;
};
}
