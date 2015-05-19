// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/foreach.hpp>
#include <set>
#include <string>

#include "Node.hpp"

namespace crave {

struct UserConstraint {
  friend struct ConstraintManager;
  friend struct ConstraintPartitioner;

  typedef NodePtr expression;

 protected:
  UserConstraint(unsigned const id,
                 expression const expr,
                 std::string const& name,
                 std::set<int>& support_vars,
                 bool const soft = false,
                 bool const cover = false,
                 bool const enabled = true)
      : id_(id),
        expr_(expr),
        name_(name),
        support_vars_(support_vars),
        soft_(soft),
        cover_(cover),
        enabled_(enabled) {}

 public:
  virtual ~UserConstraint() {}

  template <typename ostream>
  friend ostream& operator<<(ostream& os, const UserConstraint& constr) {
    os << constr.name_ << " is a "
       << (constr.soft_ ? "soft" : "hard")
       << " constraint and "
       << (constr.enabled_ ? "enabled" : "disabled");
    os << ", support vars =";
    BOOST_FOREACH(int item, constr.support_vars_) { os << " " << item; }
    return os;
  }

  unsigned id() const { return id_; }

  expression const& expr() const { return expr_; }

  std::string name() const { return name_; }

  bool isSoft() const { return soft_; }

  bool isCover() const { return cover_; }

  bool isEnabled() const { return enabled_; }

  void enable() { enabled_ = true; }

  void disable() { enabled_ = false; }

  virtual bool isVectorConstraint() { return false; }

 protected:
  unsigned id_;
  expression expr_;
  std::string name_;
  std::set<int> support_vars_;
  bool soft_;
  bool cover_;
  bool enabled_;
};
}  // namespace crave
