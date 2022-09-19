// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#include "../crave/ConstrainedRandom.hpp"

#include <ctime>
#include <fstream>
#include <string>

#include "../crave/RandomSeedManager.hpp"
#include "../crave/backend/FactoryMetaSMT.hpp"
#include "../crave/ir/VariableContainer.hpp"
#include "../crave/utils/Settings.hpp"

namespace crave {

VariableContainer* variable_container() {
  static VariableContainer* variables = new VariableContainer();
  return variables;
}

int new_var_id() {
  static int var_ID = 0;
  return ++var_ID;
}

int new_constraint_id() {
  static int constraint_ID = 0;
  return ++constraint_ID;
}

unsigned int default_rand_vec_size() { return 5; }

unsigned int placeholder_bitsize() { return bitsize_traits<placeholder_tag>::value; }

placeholder _i;

RandomSeedManager rng(static_cast<unsigned>(std::time(0)));

std::function<bool(void)> random_bit = []() { return std::uniform_int_distribution<unsigned short>(0, 1)(*rng.get()); };

std::function<unsigned(unsigned)> random_unsigned = [](unsigned i) {
  return std::uniform_int_distribution<>(0, i - 1)(*rng.get());
};

// if called with zero seed -> use default seed std::time(0)
void set_global_seed(unsigned int s) {
  if (s) rng.set_global_seed(s);
}

void set_solver_backend(std::string const& type) { FactoryMetaSMT::setSolverType(type); }

std::ostream& rand_obj::print_dot_graph(std::ostream& os, bool root = true) {
  if (root) os << "digraph AST {" << std::endl;
  for (unsigned i = 0; i < objChildren_.size(); i++) {
    objChildren_[i]->print_dot_graph(os, false);
  }
  constraint.printDotGraph(os, false);
  if (root) os << "}" << std::endl;
  return os;
}

void rand_obj::display_constraints() {
  std::fstream fs;
  fs.open("temp.dot", std::fstream::out);
  print_dot_graph(fs);
  fs.close();
  system("dot -Txlib temp.dot");
}

bool rand_obj::next() {
  if (!gen_base_children()) return false;
  if (rebuild_) {
    constraint.reset();
    gather_constraints(&constraint);
    constraint.rebuild();
    rebuild_ = false;
  }
  return constraint.next();
}

rand_obj::rand_obj(rand_obj* parent) : parent_(parent), rebuild_(false) {
  if (parent_ != 0) parent_->add_obj_child(this);
}

bool rand_obj::next_cov() {
  if (!gen_base_children()) return false;
  if (rebuild_) {
    constraint.reset();
    gather_constraints(&constraint);
    constraint.rebuild();
    rebuild_ = false;
  }
  return constraint.nextCov();
}

void rand_obj::gather_values(std::vector<int64_t>* ch) {
  for (std::vector<rand_base*>::const_iterator i = baseChildren_.begin(); i != baseChildren_.end(); ++i)
    (*i)->gather_values(ch);
  for (std::vector<rand_obj*>::const_iterator i = objChildren_.begin(); i != objChildren_.end(); ++i) {
    (*i)->gather_values(ch);
  }
}

void rand_obj::add_base_child(rand_base* rb) {
  baseChildren_.push_back(rb);
  if (rb->obj_kind() == "rand_vec") request_rebuild();
}

void rand_obj::request_rebuild() {
  rebuild_ = true;
  if (parent_ != 0) parent_->request_rebuild();
}

void rand_obj::add_obj_child(rand_obj* ro) {
  objChildren_.push_back(ro);
  request_rebuild();
}

bool rand_obj::enable_constraint(std::string name) {
  bool res = constraint.enableConstraint(name);
  if (constraint.isChanged()) request_rebuild();
  return res;
}

bool rand_obj::disable_constraint(std::string name) {
  bool res = constraint.disableConstraint(name);
  if (constraint.isChanged()) request_rebuild();
  return res;
}

bool rand_obj::is_constraint_enabled(std::string name) { return constraint.isConstraintEnabled(name); }

bool rand_obj::gen_base_children() {
  for (unsigned i = 0; i < baseChildren_.size(); i++) {
    if (baseChildren_[i]->obj_kind() == "rand_vec") continue;  // rand_vec to be generated later
    if (!baseChildren_[i]->next()) return false;
  }
  for (unsigned i = 0; i < objChildren_.size(); i++)
    if (!objChildren_[i]->gen_base_children()) return false;
  return true;
}

void rand_obj::gather_constraints(Generator* gen) {
  for (unsigned i = 0; i < objChildren_.size(); i++) {
    objChildren_[i]->gather_constraints(gen);
  }
  for (unsigned i = 0; i < baseChildren_.size(); i++)
    if (baseChildren_[i]->obj_kind() == "rand_vec") {
      __rand_vec_base* rvb = dynamic_cast<__rand_vec_base*>(baseChildren_[i]);
      gen->addVecId(rvb->id());
    }
  gen->merge(constraint);
}

}  // namespace crave

// vim: tabstop=2 shiftwidth=2 expandtab
