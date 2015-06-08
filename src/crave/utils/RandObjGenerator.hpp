// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <algorithm>
#include <bitset>
#include <limits>
#include <vector>
#include <ctime>

#include "../frontend/RandomBase.hpp"

namespace crave {

class rand_obj_gen {
  typedef std::bitset<sizeof(int64_t) << 3> Bitset;
  typedef std::vector<Bitset> Solution;
  typedef std::vector<Solution> Solutions;
  typedef std::vector<std::clock_t> Times;

 public:
  rand_obj_gen(unsigned int, rand_obj_base*);

  bool generate();

  virtual bool next() { return obj_->next(); }
  void print_bench_values();

 private:
  unsigned int number_;
  rand_obj_base* obj_;
  Solutions solutions_;
  Times elapsed_gen_times_;
  bool values_generated_;
};
}  // end namespace crave
