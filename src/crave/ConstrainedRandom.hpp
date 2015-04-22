#pragma once

#include "backend/Generator.hpp"
#include "frontend/RandomBase.hpp"

#include <boost/bind.hpp>

#include <vector>

namespace crave {

  void set_global_seed(unsigned int s);
  void set_solver_backend(std::string const&);

  std::string const& get_config_file_name();
  void set_config_file_name(std::string const&);

  void init();
  void init(std::string const&);

  class rand_obj : public rand_obj_base {
  public:
    rand_obj(rand_obj* parent_ = 0) : parent(parent_), rebuild(false) { if (parent != 0) parent->add_obj_child(this); }

    virtual bool next() { 
      if (!gen_base_children()) return false;
      if (rebuild) {
        constraint.reset();
        gather_constraints(constraint);
        constraint.rebuild();
        rebuild = false;
      }
      return constraint.next();
    }

    virtual bool next_cov() { 
      if (!gen_base_children()) return false;
      if (rebuild) {
        constraint.reset();
        gather_constraints(constraint);
        constraint.rebuild();
        rebuild = false;
      }
      return constraint.nextCov();
    }

    virtual void gather_values(std::vector<long long>& ch) {
      for (std::vector<rand_base*>::const_iterator i = baseChildren.begin();
           i != baseChildren.end(); ++i)
        (*i)->gather_values(ch);
      for (std::vector<rand_obj*>::const_iterator i = objChildren.begin();
           i != objChildren.end(); ++i)
        (*i)->gather_values(ch);
    }

    virtual void add_base_child(rand_base* rb) { baseChildren.push_back(rb); }

    void request_rebuild() { rebuild = true; if (parent != 0) parent->request_rebuild(); }

    void add_obj_child(rand_obj* ro) { objChildren.push_back(ro); request_rebuild(); }

    bool enable_constraint(std::string name) { 
      bool res = constraint.enableConstraint(name); 
      if (constraint.isChanged()) request_rebuild();
      return res;
    }
    
    bool disable_constraint(std::string name) {
      bool res = constraint.disableConstraint(name);
      if (constraint.isChanged()) request_rebuild();
      return res;
    }
    
    bool is_constraint_enabled(std::string name) { return constraint.isConstraintEnabled(name); }

    std::ostream& print_dot_graph(std::ostream&, bool);
    void display_constraints();

  protected:
    bool gen_base_children() {
      for (uint i = 0; i < baseChildren.size(); i++)
        if (!baseChildren[i]->next()) return false;
      for (uint i = 0; i < objChildren.size(); i++)
        if (!objChildren[i]->gen_base_children()) return false;
      return true;  
    }

    void gather_constraints(Generator& gen) {
      for (uint i = 0; i < objChildren.size(); i++)
        objChildren[i]->gather_constraints(gen);
      gen.merge(constraint);
    }

  public:
    Generator constraint;

  protected:
    std::vector<rand_base*> baseChildren;
    std::vector<rand_obj*> objChildren;
    rand_obj* parent;
    bool rebuild;
  };

} // end namespace crave

#define INSERT(s, DATA, ELEM) DATA.insert(ELEM);

#define CRAVE_ENUM(name, ...) \
namespace crave { \
  template<> \
  struct randv<name> : randv<int> { \
    randv(rand_obj* parent) : randv<int>(parent) { \
      if (parent == 0) throw std::runtime_error("randv<enum> must be owned by an instance of rand_obj."); \
 \
      std::set<int> s; \
      BOOST_PP_SEQ_FOR_EACH(INSERT, s, __VA_ARGS__); \
      parent->constraint(inside(var, s)); \
    } \
  }; \
}
