// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//
#include "../crave/RandomSeedManager.hpp"

#include <functional>

RandomSeedManager::RandomSeedManager(unsigned int seed) : default_rng_(seed), seed_(seed) {}

RandomSeedManager::~RandomSeedManager() {
  for (random_map_t::value_type& entry : randomMap_) {
    delete entry.second;
  }
}

void RandomSeedManager::set_global_seed(unsigned int s) {
  seed_ = s;
  default_rng_.seed(s);
}

unsigned int RandomSeedManager::get_seed() { return seed_; }

#ifndef WITH_SYSTEMC

std::mt19937* RandomSeedManager::get() { return &default_rng_; }

#else

#include <sysc/kernel/sc_simcontext.h>
std::mt19937* RandomSeedManager::get() {
  sc_core::sc_process_b* process = sc_core::sc_get_current_process_b();
  if (!process) return &default_rng_;
  if (randomMap_.count(process->proc_id)) {  // constains
    return randomMap_.at(process->proc_id);
  }
  static std::hash<std::string> string_hash;
  std::mt19937* rnd = new std::mt19937(seed_ + 19937 * string_hash(process->name()));
  randomMap_.insert(std::make_pair(process->proc_id, rnd));
  return rnd;
}

#endif
