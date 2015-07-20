// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/foreach.hpp>
#include <set>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "Node.hpp"

namespace crave {

class UserConstraint {
  friend struct ConstraintManager;
  friend struct ConstraintPartitioner;
  friend struct ConstraintPartition;

 public:
  typedef NodePtr expression;

  UserConstraint(unsigned const id, expression const expr, std::string const& name, std::set<int> support_vars,
                 bool const soft = false, bool const cover = false, bool const enabled = true);

  virtual ~UserConstraint() {}

  unsigned id() const;

  expression const& expr() const;

  std::string name() const;

  bool isSoft() const;

  bool isCover() const;

  bool isEnabled() const;

  void enable();

  void disable();

  virtual bool isVectorConstraint();

  unsigned complexity() const;

  template <typename ostream>
  friend ostream& operator<<(ostream& os, const UserConstraint& constr);

 protected:
  unsigned id_;
  expression expr_;
  std::string name_;
  std::set<int> support_vars_;
  bool soft_;
  bool cover_;
  bool enabled_;
  unsigned complexity_;
};

template <typename ostream>
ostream& operator<<(ostream& os, const UserConstraint& constr);

typedef boost::shared_ptr<UserConstraint> ConstraintPtr;
typedef std::list<ConstraintPtr> ConstraintList;

}  // namespace crave
