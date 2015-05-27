// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/bind.hpp>

#include <vector>
#include <string>
#include <set>

#include "backend/Generator.hpp"
#include "frontend/RandomBase.hpp"
#include "ir/UserExpression.hpp"

namespace crave {

void set_global_seed(unsigned int s);
void set_solver_backend(std::string const&);

std::string const& get_config_file_name();
void set_config_file_name(std::string const&);

void init();
void init(std::string const&);

class rand_obj : public rand_obj_base {
 public:
  explicit rand_obj(rand_obj* parent = 0) : parent_(parent), rebuild_(false) {
    if (parent_ != 0) parent_->add_obj_child(this);
  }

  virtual bool next() {
    if (!gen_base_children()) return false;
    if (rebuild_) {
      constraint_.reset();
      gather_constraints(constraint_);
      constraint_.rebuild();
      rebuild_ = false;
    }
    return constraint_.next();
  }

  virtual bool next_cov() {
    if (!gen_base_children()) return false;
    if (rebuild_) {
      constraint_.reset();
      gather_constraints(constraint_);
      constraint_.rebuild();
      rebuild_ = false;
    }
    return constraint_.nextCov();
  }

  virtual void gather_values(std::vector<int64_t> *ch) {
    for (std::vector<rand_base*>::const_iterator i = baseChildren_.begin();
         i != baseChildren_.end(); ++i)
      (*i)->gather_values(ch);
    for (std::vector<rand_obj*>::const_iterator i = objChildren_.begin();
         i != objChildren_.end(); ++i) {
      (*i)->gather_values(ch);
    }
  }

  virtual void add_base_child(rand_base* rb) { baseChildren_.push_back(rb); }

  void request_rebuild() {
    rebuild_ = true;
    if (parent_ != 0) parent_->request_rebuild();
  }

  void add_obj_child(rand_obj* ro) {
    objChildren_.push_back(ro);
    request_rebuild();
  }

  bool enable_constraint(std::string name) {
    bool res = constraint_.enableConstraint(name);
    if (constraint_.isChanged()) request_rebuild();
    return res;
  }

  bool disable_constraint(std::string name) {
    bool res = constraint_.disableConstraint(name);
    if (constraint_.isChanged()) request_rebuild();
    return res;
  }

  bool is_constraint_enabled(std::string name) {
    return constraint_.isConstraintEnabled(name);
  }

  std::ostream& print_dot_graph(std::ostream&, bool);
  void display_constraints();

 protected:
  bool gen_base_children() {
    for (uint i = 0; i < baseChildren_.size(); i++)
      if (!baseChildren_[i]->next()) return false;
    for (uint i = 0; i < objChildren_.size(); i++)
      if (!objChildren_[i]->gen_base_children()) return false;
    return true;
  }

  void gather_constraints(Generator& gen) {
    for (uint i = 0; i < objChildren_.size(); i++) {
      objChildren_[i]->gather_constraints(gen);
    }
    gen.merge(constraint_);
  }

 public:
  Generator constraint_;

 protected:
  std::vector<rand_base*> baseChildren_;
  std::vector<rand_obj*> objChildren_;
  rand_obj* parent_;
  bool rebuild_;
};

}  // end namespace crave

#define INSERT(s, DATA, ELEM) DATA.insert(ELEM);

#define CRAVE_ENUM(name, ...)                                         \
  namespace crave {                                                   \
  template <>                                                         \
  struct randv<name> : randv<int> {                                   \
    explicit randv(rand_obj* parent) : randv<int>(parent) {           \
      if (parent == 0) {                                              \
        throw std::runtime_error(                                     \
            "randv<enum> must be owned by an instance of rand_obj."); \
      }                                                               \
      std::set<int> s;                                                \
      BOOST_PP_SEQ_FOR_EACH(INSERT, s, __VA_ARGS__);                  \
      parent->constraint_(inside(var, s));                             \
    }                                                                 \
  };                                                                  \
}  // namespace crave
