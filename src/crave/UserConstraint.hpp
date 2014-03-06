#pragma once

#include "Context.hpp"
#include "expression/Node.hpp"
#include "expression/FixWidthVisitor.hpp"
#include "expression/ToDotNodeVisitor.hpp"

#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/lexical_cast.hpp>

#include <glog/logging.h>

#include <ostream>
#include <string>
#include <vector>
#include <list>
#include <stdexcept>

namespace crave {

int new_constraint_id();

struct ConstraintManager;
struct ConstraintPartitioner;

struct UserConstraint;
typedef boost::shared_ptr<UserConstraint> ConstraintPtr;
typedef std::list<ConstraintPtr> ConstraintList;

struct UserVectorConstraint;
typedef boost::shared_ptr<UserVectorConstraint> VectorConstraintPtr;

/**
 *
 */
struct UserConstraint {
  friend struct ConstraintManager;
  friend struct ConstraintPartitioner;

  typedef NodePtr expression;

protected:
  UserConstraint(unsigned const id, expression const expr, std::string const& name, std::set<int> & support_vars, bool const soft = false, bool const enabled = true)
  : id_(id), expr_(expr), name_(name), support_vars_(support_vars), soft_(soft), enabled_(enabled) { }

public:
  virtual ~UserConstraint() { }

  template<typename ostream>
  friend ostream& operator<<(ostream& os, const UserConstraint& constr) {
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


/**
 *
 */
struct UserVectorConstraint : UserConstraint {
  friend struct ConstraintManager;
  friend struct ConstraintPartitioner;

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

/**
 *
 */
struct ConstraintPartition {
  friend struct ConstraintPartitioner;

  ConstraintPartition() { }

  typedef ConstraintList::iterator iterator;
  typedef ConstraintList::const_iterator const_iterator;
  typedef ConstraintList::reference reference;
  typedef ConstraintList::const_reference const_reference;
  typedef ConstraintList::size_type size_type;
  typedef ConstraintList::value_type value_type;

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

  void add(ConstraintPtr c) {
    iterator ite = constraints_.begin();
    while (ite != constraints_.end() && ((*ite)->id() > c->id())) ite++;
    constraints_.insert(ite, c);
  }

  inline bool contains_var(int id) { 
    return support_vars_.find(id) != support_vars_.end();
  }

  template<typename ostream>
  friend ostream& operator<<(ostream& os, const ConstraintPartition& cp) {
    os << "[ ";
    BOOST_FOREACH (ConstraintPtr c, cp) {
      os << c->get_name() << " ";
    }
    os << "]";
    os << std::flush;
    return os;
  }


private:
  ConstraintList constraints_;
  std::set<int> support_vars_;
};


/**
 *
 */
struct ConstraintManager {
  friend struct ConstraintPartitioner;

  typedef std::map<std::string, ConstraintPtr> ConstraintMap;

  ConstraintManager() : changed_(false) { 
    static unsigned ID = 0;
    id_ = ++ID;
  }

  template<typename ostream>
  friend ostream& operator<<(ostream& os, const ConstraintManager& set) {
    os << "Set " << set.id_ << " has " << set.constraints_.size() << " constraint(s) and has " << (set.changed_?"":"not ") << "changed" << std::endl;
    BOOST_FOREACH (ConstraintPtr item, set.constraints_) {
      os << item << std::endl;
    }
    os << std::flush;
    return os;
  }

  bool enable_constraint(std::string const& key) {
    LOG(INFO) << "Enable constraint " << key << " in set " << id_ << ": ";
    ConstraintMap::iterator ite = cMap_.find(key);
    if (ite != cMap_.end()) {
      if (!ite->second->is_enabled()) {
        LOG(INFO) << "  ok";
        ite->second->enable();
        changed_ = true;
      }
      else
        LOG(INFO) << "  already enabled";
      return true;
    }
    LOG(INFO) << "not found";
    return false;
  }

  bool disable_constraint(std::string const& key) {
    LOG(INFO) << "Disable constraint " << key << " in set " << id_ << ": ";
    ConstraintMap::iterator ite = cMap_.find(key);
    if (ite != cMap_.end()) {
      if (ite->second->is_enabled()) {
        LOG(INFO) << "  ok";
        ite->second->disable();
        changed_ = true;
      }
      else
        LOG(INFO) << "  already enabled";
      return true;
    }
    LOG(INFO) << "  not found";
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

  template<typename Expr>
  ConstraintPtr makeConstraint(std::string const& name, int c_id, Expr e, Context& ctx,
                               bool const soft = false) {

    LOG(INFO) << "New " << (soft?"soft ":"") << "constraint " << name << " in set " << id_;

    if (cMap_.find(name) != cMap_.end()) 
      throw std::runtime_error("Constraint already exists.");

    ctx.reset_support_vars();

    FixWidthVisitor vis;
    NodePtr n(vis.fix_width(*boost::proto::eval(e, ctx)));

    ConstraintPtr c(
      boost::dynamic_pointer_cast<ForEach>(n) != 0
      ? new UserVectorConstraint(c_id, n, name, ctx.support_vars(), false, soft)
      : (boost::dynamic_pointer_cast<Unique>(n) != 0
        ? new UserVectorConstraint(c_id, n, name, ctx.support_vars(), true, soft)
        : new UserConstraint(c_id, n, name, ctx.support_vars(), soft))
    );

    changed_ = true;
    constraints_.push_back(c);

    return cMap_[name] = c;
  }

  template<typename Expr>
  ConstraintPtr makeConstraint(std::string const& name, Expr e, Context& ctx,
                                     bool const soft = false) {
    return makeConstraint(name, new_constraint_id(), e, ctx, soft);
  }

  template<typename Expr>
  ConstraintPtr makeConstraint(Expr e, Context& ctx, bool const soft = false) {
    int id = new_constraint_id();
    return makeConstraint("constraint_" + boost::lexical_cast<std::string>(id), id,
                          e, ctx, soft);
  }

  std::ostream& print_dot_graph_(std::ostream& os)  {
    ToDotVisitor visitor(os);
    BOOST_FOREACH ( ConstraintPtr c , constraints_ ) {
      long a = reinterpret_cast<long>(&*c);
      long b = reinterpret_cast<long>(&(*c->get_expression()));
      os << "\t" << a << " [label=\"" << c->get_name() << (c->is_soft()?" soft":"") << (!c->is_enabled()?" disabled":"") << "\"]" << std::endl;
      os << "\t" << a << " -> " << b << std::endl; 
      c->get_expression()->visit(visitor);
    }
    return os;
  }

private:
  unsigned id_;
  ConstraintMap cMap_;
  ConstraintList constraints_;
  bool changed_;
};


/**
 *
 */
struct ConstraintPartitioner {

  void reset() { 
    constr_mngs_.clear();
    constraints_.clear();
    partitions_.clear();
    vec_constraints_.clear();
  }

  void mergeConstraints(ConstraintManager& mng) {
    LOG(INFO) << "Merge set " << mng.id_ << " with set(s)";
    BOOST_FOREACH (unsigned id, constr_mngs_)
      LOG(INFO) << " " << id;
    constr_mngs_.insert(mng.id_);
    BOOST_FOREACH (ConstraintPtr c, mng.constraints_) {
      if (c->is_enabled()) {
        if (c->is_vector_constraint())
          vec_constraints_.push_back(boost::static_pointer_cast<UserVectorConstraint>(c));
        else 
          constraints_.push_back(c);
      }
    }
  }

  void partition() {
    while (!constraints_.empty()) {
      ConstraintPartition cp;
      maximize_partition(cp, constraints_);
      partitions_.push_back(cp);
    }
    LOG(INFO) << "Partition results of set(s)";
    BOOST_FOREACH (unsigned id, constr_mngs_)
      LOG(INFO) << " " << id;
    LOG(INFO) << ": ";

    LOG(INFO) << "  " << vec_constraints_.size() << " vector constraint(s):";
    BOOST_FOREACH (VectorConstraintPtr c, vec_constraints_) {
      LOG(INFO) << "   " << c->get_name();
    }

    LOG(INFO) << "  " << partitions_.size() << " constraint partition(s):";
    uint cnt = 0;
    BOOST_FOREACH (ConstraintPartition& cp, partitions_) {
      LOG(INFO) << "    #" << ++cnt << ": " << cp;
    }
  }

  inline std::vector<ConstraintPartition>& get_partitions() { return partitions_; }
  inline std::vector<VectorConstraintPtr>& get_vector_constraints() {
    return vec_constraints_; 
  }

private:
  void maximize_partition(ConstraintPartition& cp, ConstraintList& lc) {
    ConstraintPtr c = lc.front();
    lc.pop_front();
    cp.support_vars_ = c->support_vars_;
    cp.add(c);
    while (true) {
      bool changed = false;
      ConstraintList::iterator ite = lc.begin();

      while (ite != lc.end()) {
        c = *ite;
        std::vector<int> v_intersection;
        std::set_intersection(cp.support_vars_.begin(), cp.support_vars_.end(),
                          c->support_vars_.begin(), c->support_vars_.end(),
                          std::back_inserter(v_intersection));
        if (!v_intersection.empty()) {
          changed = true;
          cp.add(c);
          cp.support_vars_.insert(c->support_vars_.begin(), c->support_vars_.end());
          ite = lc.erase(ite);
        }
        else {
          ++ite;
        }
      }
      if (!changed) break;
    }
  }

private:
  std::set<unsigned> constr_mngs_;
  ConstraintList constraints_;
  // results
  std::vector<ConstraintPartition> partitions_;
  std::vector<VectorConstraintPtr> vec_constraints_;
};


} // end namespace crave
