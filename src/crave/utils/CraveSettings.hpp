// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

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
  unsigned int get_specified_seed() const;
  void set_used_seed(unsigned int);

 private:
  std::string module_name_;

 private:
  std::string backend_;
  unsigned int specified_seed_;
  unsigned int used_seed_;

 private:
  std::string const BACKEND;
  std::string const SEED;
  std::string const LASTSEED;
};
}  // namespace crave
