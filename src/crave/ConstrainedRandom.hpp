// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <vector>
#include <string>
#include <set>

#include <boost/preprocessor.hpp>                                                          \

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
  explicit rand_obj(rand_obj* parent = 0);

  virtual bool next();

  virtual bool next_cov();

  virtual void gather_values(std::vector<int64_t>* ch);

  virtual void add_base_child(rand_base* rb);

  void request_rebuild();

  void add_obj_child(rand_obj* ro);

  bool enable_constraint(std::string name);

  bool disable_constraint(std::string name);

  bool is_constraint_enabled(std::string name);

  std::ostream& print_dot_graph(std::ostream&, bool);
  void display_constraints();

 protected:
  bool gen_base_children();

  void gather_constraints(Generator* gen);

 public:
  Generator constraint;

 protected:
  std::vector<rand_base*> baseChildren_;
  std::vector<rand_obj*> objChildren_;
  rand_obj* parent_;
  bool rebuild_;
};

}  // end namespace crave

#define INSERT(s, DATA, ELEM) DATA.insert(ELEM);

#define CRAVE_ENUM(name, ...)                                                              \
  namespace crave {                                                                        \
  template <>                                                                              \
  struct randv<name> : randv<int> {                                                        \
    explicit randv(rand_obj* parent) : randv<int>(parent) {                                \
      if (parent == 0) {                                                                   \
        throw std::runtime_error("randv<enum> must be owned by an instance of rand_obj."); \
      }                                                                                    \
      std::set<int> s;                                                                     \
      BOOST_PP_SEQ_FOR_EACH(INSERT, s, __VA_ARGS__);                                       \
      parent->constraint(inside(var, s));                                                  \
    }                                                                                      \
  };                                                                                       \
  }  // namespace crave
