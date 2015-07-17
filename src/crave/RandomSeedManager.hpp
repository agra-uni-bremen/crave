// Copyright 2014 The CRAVE developers. All rights reserved.//
#pragma once
#include <map>
#include <boost/random.hpp>

class RandomSeedManager {
 public:
  RandomSeedManager(unsigned int seed);
  virtual ~RandomSeedManager();
  void set_global_seed(unsigned int s);
  boost::mt19937* get();
  unsigned int charToUIntSeed(const char* name);

 private:
  typedef std::map<unsigned int, boost::mt19937*> random_map_t;
  random_map_t randomMap_;
  boost::mt19937 default_rng_;
  unsigned int seed_;
};
