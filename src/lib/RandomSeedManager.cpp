// Copyright 2014 The CRAVE developers. All rights reserved.//
#include "../crave/RandomSeedManager.hpp"
#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>

RandomSeedManager::RandomSeedManager(unsigned int seed) : seed_(seed), default_rng_(seed) {}

RandomSeedManager::~RandomSeedManager() {
  BOOST_FOREACH(random_map_t::value_type & entry, randomMap_) { delete entry.second; }
}

void RandomSeedManager::set_global_seed(unsigned int s) {
  seed_ = s;
  default_rng_.seed(s);
}

#ifndef WITH_SYSTEMC

boost::mt19937* RandomSeedManager::get() { return &default_rng_; }

#else

#include <sysc/kernel/sc_simcontext.h>
boost::mt19937* RandomSeedManager::get() {
  sc_core::sc_process_b* process = sc_core::sc_get_current_process_b();
  if (!process) return &default_rng_;
  if (randomMap_.count(process->proc_id)) {  // constains
    return randomMap_.at(process->proc_id);
  }
  static boost::hash<std::string> string_hash;
  boost::mt19937* rnd = new boost::mt19937(seed_ + 19937 * string_hash(process->name()));
  randomMap_.insert(std::make_pair(process->proc_id, rnd));
  return rnd;
}

#endif

