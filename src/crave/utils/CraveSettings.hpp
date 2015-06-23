// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <fstream>
#include <set>
#include <string>
#include "Settings.hpp"

namespace crave {
class CraveSetting : public Setting {
 public:
  explicit CraveSetting(std::string const& filename);

 private:
  virtual void load_(const ptree& tree);

  virtual void save_(ptree* tree) const;

 public:
  std::string const& get_backend() const;
  unsigned int get_seed() const;

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
