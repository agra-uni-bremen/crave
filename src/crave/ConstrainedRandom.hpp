#pragma once

#include "Generator.hpp"
#include "RandomBase.hpp"

#include <boost/bind.hpp>

#include <vector>

namespace crave {

  void set_global_seed(unsigned int s);
  void set_solver_backend(std::string const&);

  std::string const& get_config_file_name();
  void set_config_file_name(std::string const&);

  void init();
  void init(std::string const&);
  void init(std::string const&, std::string const&);

  class rand_obj : public rand_obj_base {
  public:
    rand_obj(rand_obj* parent = 0) { if (parent != 0) { parent->addChild(this); } }

    virtual bool next() { 
      for (uint i = 0; i < children.size(); i++)
        if (!children[i]->next()) return false;
      return constraint.next(); 
    }

    virtual void gatherValues(std::vector<long>& ch) {
      for (std::vector<rand_base*>::const_iterator i = children.begin();
           i != children.end(); ++i)
        (*i)->gatherValues(ch);
    }

    virtual void addChild(rand_base* rb) { children.push_back(rb); }

    bool enable_constraint(std::string name) { return constraint.enable_constraint(name); }
    bool disable_constraint(std::string name) { return constraint.disable_constraint(name); }
    bool is_constraint_enabled(std::string name) { return constraint.is_constraint_enabled(name); }

  public:
    Generator constraint;

  protected:
    std::vector<rand_base*> children;
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
