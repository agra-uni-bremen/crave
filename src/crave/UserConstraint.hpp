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

struct ConstraintSet;

struct UserConstraint;
typedef boost::shared_ptr<UserConstraint> ConstraintPtr;
typedef std::vector<ConstraintPtr> ConstraintVector;

struct UserVectorConstraint;
typedef boost::shared_ptr<UserVectorConstraint> VectorConstraintPtr;

struct UserConstraint {

  friend struct ConstraintSet;

  typedef NodePtr expression;

protected:
  UserConstraint(unsigned const id, expression const expr, std::string const& name, std::set<int> & support_vars, bool const soft = false, bool const enabled = true)
  : id_(id), expr_(expr), name_(name), support_vars_(support_vars), soft_(soft), enabled_(enabled) { }

public:
  virtual ~UserConstraint() { }

  template<typename ostream>
  friend ostream& operator<<(ostream& os, UserConstraint constr) {
    os << constr.name_ << " is a " << (constr.soft_?"soft":"hard") << " constraint and " << (constr.enabled_?"enabled":"disabled");
    os << ", support vars =";
    BOOST_FOREACH (int item, constr.support_vars_) 
      os << " " << item;
    return os;
  }

  inline unsigned id() const {
    return id_;
  }

  inline expression const & get_expression() const {
    return expr_;
  }

  inline std::set<int> const & get_support_vars() const {
    return support_vars_;
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

  virtual inline bool is_vector_constraint() { return false; }

 protected:
  unsigned id_;
  expression expr_;
  std::string name_;
  std::set<int> support_vars_;
  bool soft_;
  bool enabled_;
};

struct UserVectorConstraint : UserConstraint {
friend struct ConstraintSet;

protected:
  UserVectorConstraint(unsigned const id, expression const expr, std::string const& name, std::set<int> & support_vars
  , bool unique, bool const soft = false, bool const enabled = true)
  : UserConstraint(id, expr, name, support_vars, soft, enabled), unique_(unique) { }

public:
  inline bool is_unique() { return unique_; }
  inline int get_vector_id() { return *support_vars_.begin(); }
  inline bool is_vector_constraint() { return true; }

protected:
  bool unique_;
};

struct ConstraintSet {
  typedef std::map<std::string, ConstraintPtr> ConstraintMap;
  typedef ConstraintVector::iterator iterator;
  typedef ConstraintVector::const_iterator const_iterator;
  typedef ConstraintVector::reference reference;
  typedef ConstraintVector::const_reference const_reference;
  typedef ConstraintVector::size_type size_type;
  typedef ConstraintVector::value_type value_type;

  ConstraintSet()
  : changed_(false), has_soft_(false) { }

  template<typename ostream>
  friend ostream& operator<<(ostream& os, ConstraintSet set) {
    os << "Set has " << set.constraints_.size() << " constraint(s) and has " << (set.changed_?"":"not ") << "changed" << std::endl; 
    BOOST_FOREACH (value_type item, set.constraints_) {
      os << item << std::endl;
    }
    os << std::flush;
    return os;
  }

  iterator begin() {
    return constraints_.begin();
  }
  const_iterator begin() const {
    return constraints_.begin();
  }
  iterator end() {
    return constraints_.end();
  }
  const_iterator end() const {
    return constraints_.end();
  }

  bool enable_constraint(std::string const& key) {
    ConstraintMap::iterator ite = cMap_.find(key);
    if (ite != cMap_.end()) {
      if (!ite->second->is_enabled()) {
        ite->second->enable();
        changed_ = true;
      }
      return true;
    }
    return false;
  }

  bool disable_constraint(std::string const& key) {
    ConstraintMap::iterator ite = cMap_.find(key);
    if (ite != cMap_.end()) {
      if (ite->second->is_enabled()) {
        ite->second->disable();
        changed_ = true;
      }
      return true;
    }
    return false;
  }

  bool is_constraint_enabled(std::string const& key) {
    ConstraintMap::iterator ite = cMap_.find(key);
    return ite != cMap_.end() && ite->second->is_enabled();
  }

  bool is_changed() const {
    return changed_;
  }

  void set_synced() {
    changed_ = false;
  }

  bool has_soft() const {
    return has_soft_;
  }

  template<typename Expr>
  ConstraintPtr makeConstraint(std::string const& name, Expr e, Context& ctx,
                                     bool const soft = false) {
    if (cMap_.find(name) != cMap_.end()) 
      throw std::runtime_error("Constraint already exists.");

    ctx.reset_support_vars();
    NodePtr n(boost::proto::eval(FixWidth()(e), ctx));

    ConstraintPtr c(
      boost::dynamic_pointer_cast<ForEach>(n) != 0
      ? new UserVectorConstraint(new_constraint_id(), n, name, ctx.support_vars(), false, soft)
      : (boost::dynamic_pointer_cast<Unique>(n) != 0
        ? new UserVectorConstraint(new_constraint_id(), n, name, ctx.support_vars(), true, soft)
        : new UserConstraint(new_constraint_id(), n, name, ctx.support_vars(), soft))
    );
    
    changed_ = true;
    has_soft_ |= c->is_soft();
    constraints_.push_back(c);

    return cMap_[name] = c;
  }

  template<typename Expr>
  ConstraintPtr makeConstraint(Expr e, Context& ctx, bool const soft = false) {
    return makeConstraint("constraint_" + boost::lexical_cast<std::string>(new_constraint_id()),
                          e, ctx, soft);
  }

private:
  ConstraintMap cMap_;
  ConstraintVector constraints_;
  bool changed_;
  bool has_soft_;
};

} // end namespace crave
