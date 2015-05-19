// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once
#include "Settings.hpp"

#include <fstream>
#include <set>
#include <string>

namespace crave {
class CraveSetting : public Setting {
 public:
  explicit CraveSetting(std::string const& filename)
      : Setting(filename),
        module_name_("crave"),
        backend_(),
        seed_(),
        BACKEND("backend"),
        SEED("seed") {}

 private:
  virtual void load_(const ptree& tree) {
    backend_ = tree.get(module_name_ + "." + BACKEND, "Boolector");
    seed_ = tree.get(module_name_ + "." + SEED, 42);
  }
  virtual void save_(ptree* tree) const {
    tree->put(module_name_ + "." + BACKEND, backend_);
    tree->put(module_name_ + "." + SEED, seed_);
  }

 public:
  std::string const& get_backend() const { return backend_; }
  unsigned int get_seed() const { return seed_; }

 private:
  std::string module_name_;

 private:
  std::string backend_;
  unsigned int seed_;

 private:
  std::string const BACKEND;
  std::string const SEED;
};
}  // namespace crave
