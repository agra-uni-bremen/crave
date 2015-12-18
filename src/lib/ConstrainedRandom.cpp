// Copyright 2014 The CRAVE developers. All rights reserved.//

#include <glog/logging.h>
#include <boost/filesystem.hpp>

#include <ctime>
#include <string>

#include "../crave/backend/FactoryMetaSMT.hpp"
#include "../crave/ConstrainedRandom.hpp"
#include "../crave/utils/Settings.hpp"
#include "../crave/ir/VariableContainer.hpp"
#include "../crave/RandomSeedManager.hpp"

namespace crave {

VariableContainer variables;

VariableContainer& variable_container() { return variables; }

int new_var_id() {
  static int var_ID = 0;
  return ++var_ID;
}

int new_constraint_id() {
  static int constraint_ID = 0;
  return ++constraint_ID;
}

unsigned int default_rand_vec_size() { return 5; }

unsigned int placeholder_bitsize() { return 32; }

RandomSeedManager rng(std::time(0));

struct random_bit_gen {
  random_bit_gen() : rnd(0, 1) {}

  bool operator()() { return rnd(*rng.get()); }

  boost::uniform_int<unsigned short> rnd;
};

boost::function0<bool> random_bit = random_bit_gen();

struct RNG {
  unsigned operator()(unsigned i) { return boost::uniform_int<>(0, i - 1)(*rng.get()); }
};

boost::function1<unsigned, unsigned> random_unsigned = RNG();

// if called with zero seed -> use default seed std::time(0)
void set_global_seed(unsigned int s) {
  if (s) rng.set_global_seed(s);
}

void set_solver_backend(std::string const& type) { FactoryMetaSMT::setSolverType(type); }

std::string config_file_name = "crave.cfg";
std::string const& get_config_file_name() { return config_file_name; }
void set_config_file_name(std::string const& str) { config_file_name = str; }

void init() { init(get_config_file_name()); }

void init(std::string const& cfg_file) {
  CraveSetting cSettings(cfg_file);
  cSettings.load();

  set_global_seed(cSettings.get_seed());
  set_solver_backend(cSettings.get_backend());
  set_config_file_name(cfg_file);

  // initalize glog
  LoggerSetting settings(cfg_file);

  settings.load();
  FLAGS_log_dir = settings.dirname();
  FLAGS_max_log_size = settings.filesize();
  FLAGS_minloglevel = settings.s_level();
  FLAGS_logtostderr = false;
  FLAGS_logbufsecs = 0;

  namespace fs = boost::filesystem;
  fs::path fs_log_dir(settings.dirname());
  if (!fs::exists(fs_log_dir)) fs::create_directory(fs_log_dir);

  static bool initialized = false;
  if (!initialized) {
    google::InitGoogleLogging(settings.filename().c_str());
    initialized = true;
  }

  cSettings.save();
  settings.save();
}

std::ostream& rand_obj::print_dot_graph(std::ostream& os, bool root = true) {
  if (root) os << "digraph AST {" << std::endl;
  for (uint i = 0; i < objChildren_.size(); i++) {
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
  if (rb->kind() == "rand_vec") request_rebuild();
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
  for (uint i = 0; i < baseChildren_.size(); i++) {
    if (baseChildren_[i]->kind() == "rand_vec") continue; // rand_vec to be generated later
    if (!baseChildren_[i]->next()) return false;
  }
  for (uint i = 0; i < objChildren_.size(); i++)
    if (!objChildren_[i]->gen_base_children()) return false;
  return true;
}

void rand_obj::gather_constraints(Generator* gen) {
  for (uint i = 0; i < objChildren_.size(); i++) {
    objChildren_[i]->gather_constraints(gen);
  }
  for (uint i = 0; i < baseChildren_.size(); i++) 
    if (baseChildren_[i]->kind() == "rand_vec") {
      __rand_vec_base* rvb = dynamic_cast<__rand_vec_base*>(baseChildren_[i]);
      gen->addVecId(rvb->id()); 
    }
  gen->merge(constraint);
}

}  // namespace crave

// vim: tabstop=2 shiftwidth=2 expandtab
