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

  bool operator==(const RandomSeedManager& rhs) const {
    bool equal = true;
    equal &= (this->randomMap_ == rhs.randomMap_);
    equal &= (this->default_rng_ == rhs.default_rng_);
    equal &= (this->seed_ == rhs.seed_);
    return equal;
  }

  RandomSeedManager& operator=(const RandomSeedManager& rhs) {
    if (&rhs != this) {
      this->default_rng_ = rhs.default_rng_;
      this->randomMap_ = rhs.randomMap_;
      this->seed_ = rhs.seed_;
    }
    return *this;
  }

 private:
  typedef std::map<unsigned int, boost::mt19937*> random_map_t;
  random_map_t randomMap_;
  boost::mt19937 default_rng_;
  unsigned int seed_;
};
