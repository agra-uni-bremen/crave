#pragma once

#include "Context.hpp"
#include "ConstrainedRandom.hpp"
#include "expression/Node.hpp"
#include "expression/ReplaceVisitor.hpp"

#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/lexical_cast.hpp>

#include <ostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace crave {

int new_constraint_id();

struct ConstraintManager;
struct UserConstraint;
typedef boost::shared_ptr<UserConstraint> ConstraintPtr;

struct UserConstraint {

  friend struct ConstraintManager;

  typedef NodePtr expression;

private:
  UserConstraint(unsigned const id, expression const expr, std::string const& name, bool const soft = false, bool const enabled = true)
  : expr_(expr), name_(name), soft_(soft), enabled_(enabled) { }

public:
  template<typename ostream>
  friend ostream& operator<<(ostream& os, UserConstraint constr) {
    os << constr.name_ << " is " << (constr.soft_?"soft":"hard") << " constraint and " << (constr.enabled_?"enabled":"disabled");
    return os;
  }

  inline unsigned id() const {
    return id_;
  }

  inline expression const & get_expression() const {
    return expr_;
  }

  inline std::string get_name() const {
    return name_;
  }

  inline bool is_soft() const {
    return soft_;
  }

  inline bool is_enabled() const {
    return enabled_;
  }
  inline void enable() {
    enabled_ = true;
  }
  inline void disable() {
    enabled_ = false;
  }

 protected:
  unsigned id_;
  expression expr_;
  std::string name_;
  bool soft_;
  bool enabled_;
};



struct ConstraintSet {

  typedef std::vector<ConstraintPtr> ConstraintsVector;
  typedef ConstraintsVector::iterator iterator;
  typedef ConstraintsVector::const_iterator const_iterator;
  typedef ConstraintsVector::reference reference;
  typedef ConstraintsVector::const_reference const_reference;
  typedef ConstraintsVector::size_type size_type;
  typedef ConstraintsVector::value_type value_type;

  typedef boost::intrusive_ptr<VariableExpr> VariablePtr;
  typedef std::vector<VariablePtr> VectorElements;

  ConstraintSet()
  : constraints_(), changed_(false), unique_(false), has_soft_(false), vec_elements_() { }

  template<typename ostream>
  friend ostream& operator<<(ostream& os, ConstraintSet set) {
    os << "Set is " << (set.changed_?"":"not ") << "changed and " << (set.unique_?"":"not ") << "unique\n"; 
    os << "Size is " << set.constraints_.size() << "\n";

    BOOST_FOREACH (value_type item, set.constraints_) {
      os << item << "\n";
    }
    os << std::flush;
    return os;
  }

  reference operator[](size_type n) {
    changed_ = true;
    return constraints_[n];
  }
  const_reference operator[](size_type n) const {
    return constraints_[n];
  }
  reference at(size_type n) {
    changed_ = true;
    return constraints_.at(n);
  }
  const_reference at(size_type n) const {
    return constraints_.at(n);
  }

  iterator begin() {
    changed_ = true;
    return constraints_.begin();
  }
  const_iterator begin() const {
    return constraints_.begin();
  }
  iterator end() {
    changed_ = true;
    return constraints_.end();
  }
  const_iterator end() const {
    return constraints_.end();
  }

  void push_back(value_type const& value) {
    changed_ = true;
    has_soft_ |= value->is_soft();
    constraints_.push_back(value);
  }
  void pop_back() {
    changed_ = true;
    constraints_.pop_back();
  }

  iterator insert(iterator position, value_type const& value) {
    changed_ = true;
    has_soft_ |= value->is_soft();
    return constraints_.insert(position, value);
  }
  template<typename InputIterator>
  void insert(iterator position, InputIterator first, InputIterator last) {
    changed_ = true;
    constraints_.insert(position, first, last);
  }

  iterator erase(iterator position) {
    changed_ = true;
    return constraints_.erase(position);
  }
  iterator erase(iterator first, iterator last) {
    changed_ = true;
    return constraints_.erase(first, last);
  }

  size_type size() const {
    return constraints_.size();
  }

  void clear() {
    changed_ = true;
    constraints_.clear();
  }

  bool enable_constraint(std::string const& key) {
    BOOST_FOREACH (value_type& c, constraints_) {
      if (0 == c->get_name().compare(key)) {
        c->enable();
        changed_ = true;
        return true;
      }
    }
    return false;
  }
  bool disable_constraint(std::string const& key) {
    BOOST_FOREACH (value_type& c, constraints_) {
      if (0 == c->get_name().compare(key)) {
        c->disable();
        changed_ = true;
        return true;
      }
    }
    return false;
  }
  bool is_constraint_enabled(std::string const& key) {
    BOOST_FOREACH (value_type const& c, constraints_)
      if (0 == c->get_name().compare(key))
        return c->is_enabled();

    return false;
  }

  bool is_changed() const {
    return changed_;
  }
  void set_synced() {
    changed_ = false;
  }

  bool is_unique() const {
    return unique_;
  }
  void set_unique(bool const val) {
    changed_ = true;
    unique_ = val;
  }

  bool has_soft() const {
    return has_soft_;
  }

  VectorElements& get_vec_vars() {
    return vec_elements_;
  }

private:
  ConstraintsVector constraints_;
  bool changed_;
  bool unique_;
  bool has_soft_;

  VectorElements vec_elements_;
};

struct ConstraintManager {

template<typename Expr>
ConstraintPtr makeConstraint(std::string const& name, Expr e, Context& ctx,
                                     bool const soft = false) {
  if (constraintMap.find(name) != constraintMap.end()) 
    throw std::runtime_error("Constraint already exists.");
  NodePtr n(boost::proto::eval(FixWidth()(e), ctx));
  ConstraintPtr c(new UserConstraint(new_constraint_id(), n, name, soft));
  return constraintMap[name] = c;
}

template<typename Expr>
ConstraintPtr makeConstraint(Expr e, Context& ctx, bool const soft = false) {
    return makeConstraint("constraint_" + boost::lexical_cast<std::string>(new_constraint_id()),
                          e, ctx, soft);
}

private:
  std::map<std::string, ConstraintPtr> constraintMap;
};

static ConstraintManager constraintManager;

} // end namespace crave
