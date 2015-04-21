#pragma once

#include "Context.hpp"
#include "Node.hpp"
#include "visitor/FixWidthVisitor.hpp"
#include "visitor/GetSupportSetVisitor.hpp"
#include "visitor/ToDotNodeVisitor.hpp"

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

struct expr_tag : public Constraint< boost::proto::terminal<NodePtr>::type > {
  typedef Constraint< boost::proto::terminal<NodePtr>::type > base_type;
  expr_tag(NodePtr n) : base_type( boost::proto::make_expr<boost::proto::tag::terminal>(n)) { }
};

typedef typename boost::proto::result_of::make_expr<
  boost::proto::tag::terminal
  , Constraint_Domain
  , NodePtr
> ::type expression;

template<typename Expr>
expression make_expression(Expr e) {
  static Context ctx(crave::variables);
  return boost::proto::make_expr< boost::proto::tag::terminal, Constraint_Domain >( boost::proto::eval(e, ctx) );
}

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
  UserConstraint(unsigned const id, 
    expression const expr, 
    std::string const& name, 
    std::set<int> & support_vars, 
    bool const soft = false, 
    bool const cover = false, 
    bool const enabled = true)
  : id_(id), expr_(expr), name_(name), support_vars_(support_vars), soft_(soft), cover_(cover), enabled_(enabled) { }

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

  unsigned id() const { return id_; }

  expression const & expr() const { return expr_; }

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


/**
 *
 */
struct UserVectorConstraint : UserConstraint {
  friend struct ConstraintManager;
  friend struct ConstraintPartitioner;

protected:
  UserVectorConstraint(unsigned const id, 
    expression const expr, 
    std::string const& name, 
    std::set<int> & support_vars, 
    bool const unique, 
    bool const soft = false, 
    bool const cover = false, 
    bool const enabled = true)
  : UserConstraint(id, expr, name, support_vars, soft, cover, enabled)
  , unique_(unique) { }

public:
  bool isUnique() { return unique_; }
  int getVectorId() { return *support_vars_.begin(); }
  bool isVectorConstraint() { return true; }

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

  iterator begin() { return constraints_.begin(); }

  const_iterator begin() const { return constraints_.begin(); }

  iterator end() { return constraints_.end(); }

  const_iterator end() const { return constraints_.end(); }

  void add(ConstraintPtr c) {
    iterator ite = constraints_.begin();
    while (ite != constraints_.end() && ((*ite)->id() > c->id())) ite++;
    constraints_.insert(ite, c);
  }

  bool containsVar(int id) { return support_vars_.find(id) != support_vars_.end(); }

  template<typename ostream>
  friend ostream& operator<<(ostream& os, const ConstraintPartition& cp) {
    os << "[ ";
    BOOST_FOREACH (ConstraintPtr c, cp) {
      os << c->name() << " ";
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

  bool enableConstraint(std::string const& key) {
    LOG(INFO) << "Enable constraint " << key << " in set " << id_ << ": ";
    ConstraintMap::iterator ite = constr_map_.find(key);
    if (ite != constr_map_.end()) {
      if (!ite->second->isEnabled()) {
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

  bool disableConstraint(std::string const& key) {
    LOG(INFO) << "Disable constraint " << key << " in set " << id_ << ": ";
    ConstraintMap::iterator ite = constr_map_.find(key);
    if (ite != constr_map_.end()) {
      if (ite->second->isEnabled()) {
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

  bool isConstraintEnabled(std::string const& key) {
    ConstraintMap::iterator ite = constr_map_.find(key);
    return ite != constr_map_.end() && ite->second->isEnabled();
  }

  bool isChanged() const {
    return changed_;
  }

  void resetChanged() {
    changed_ = false;
  }

  template<typename Expr>
  ConstraintPtr makeConstraint(std::string const& name, int c_id, Expr e, Context& ctx,
                               bool const soft = false, bool const cover = false) {
    LOG(INFO) << "New " << (soft?"soft ":"") << (cover?"cover ":"") << "constraint " << name << " in set " << id_;

    if (constr_map_.find(name) != constr_map_.end()) 
      throw std::runtime_error("Constraint already exists.");

    FixWidthVisitor fwv;
    NodePtr n(fwv.fixWidth(*boost::proto::eval(e, ctx)));

    GetSupportSetVisitor gssv;
    n->visit(gssv);

    ConstraintPtr c(
      boost::dynamic_pointer_cast<ForEach>(n) != 0
      ? new UserVectorConstraint(c_id, n, name, gssv.getSupportVars(), false, soft, cover)
      : (boost::dynamic_pointer_cast<Unique>(n) != 0
        ? new UserVectorConstraint(c_id, n, name, gssv.getSupportVars(), true, soft, cover)
        : new UserConstraint(c_id, n, name, gssv.getSupportVars(), soft, cover))
    );

    assert(!c->isSoft() || !c->isCover()); // soft cover constraint not defined/supported yet
    assert(!c->isVectorConstraint() || !c->isCover()); // cover vector constraint not defined/supported yet

    changed_ = true;
    constraints_.push_back(c);

    return constr_map_[name] = c;
  }

  template<typename Expr>
  ConstraintPtr makeConstraint(std::string const& name, Expr e, Context& ctx,
                                     bool const soft = false, bool const cover = false) {
    return makeConstraint(name, new_constraint_id(), e, ctx, soft, cover);
  }

  template<typename Expr>
  ConstraintPtr makeConstraint(Expr e, Context& ctx, bool const soft = false, bool const cover = false) {
    int id = new_constraint_id();
    return makeConstraint("constraint_" + boost::lexical_cast<std::string>(id), id,
                          e, ctx, soft, cover);
  }

  std::ostream& printDotGraph(std::ostream& os)  {
    ToDotVisitor visitor(os);
    BOOST_FOREACH ( ConstraintPtr c , constraints_ ) {
      long a = reinterpret_cast<long>(&*c);
      long b = reinterpret_cast<long>(&(*c->expr()));
      os << "\t" << a << " [label=\"" << c->name() << (c->isSoft()?" soft":"") << (c->isCover()?" cover":"") << (!c->isEnabled()?" disabled":"") << "\"]" << std::endl;
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
      if (c->isEnabled()) {
        if (c->isVectorConstraint())
          vec_constraints_.push_back(boost::static_pointer_cast<UserVectorConstraint>(c));
        else 
          constraints_.push_back(c);
      }
    }
  }

  void partition() {
    while (!constraints_.empty()) {
      ConstraintPartition cp;
      maximizePartition(cp, constraints_);
      partitions_.push_back(cp);
    }
    LOG(INFO) << "Partition results of set(s)";
    BOOST_FOREACH (unsigned id, constr_mngs_)
      LOG(INFO) << " " << id;
    LOG(INFO) << ": ";

    LOG(INFO) << "  " << vec_constraints_.size() << " vector constraint(s):";
    BOOST_FOREACH (VectorConstraintPtr c, vec_constraints_) {
      LOG(INFO) << "   " << c->name();
    }

    LOG(INFO) << "  " << partitions_.size() << " constraint partition(s):";
    uint cnt = 0;
    BOOST_FOREACH (ConstraintPartition& cp, partitions_) {
      LOG(INFO) << "    #" << ++cnt << ": " << cp;
    }
  }

  std::vector<ConstraintPartition>& getPartitions() { return partitions_; }
  
  std::vector<VectorConstraintPtr>& getVectorConstraints() { return vec_constraints_; }

private:
  void maximizePartition(ConstraintPartition& cp, ConstraintList& lc) {
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
