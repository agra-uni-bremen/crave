// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//
#pragma once
#include <map>
#include <random>

class RandomSeedManager {
 public:
  RandomSeedManager(unsigned int seed);
  virtual ~RandomSeedManager();
  void set_global_seed(unsigned int s);
  std::mt19937* get();
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
  typedef std::map<unsigned int, std::mt19937*> random_map_t;
  random_map_t randomMap_;
  std::mt19937 default_rng_;
  unsigned int seed_;
};
