// Copyright 2014 The CRAVE developers. All rights reserved.//
#include "../crave/RandomSeedManager.hpp"
#include <boost/foreach.hpp>

RandomSeedManager::RandomSeedManager(unsigned int seed) : seed_(seed), default_rng_(seed) {}

RandomSeedManager::~RandomSeedManager() {
  BOOST_FOREACH(random_map_t::value_type & entry, randomMap_) { delete entry.second; }
}

void RandomSeedManager::set_global_seed(unsigned int s) {
  seed_ = s;
  default_rng_.seed(s);
}

unsigned int RandomSeedManager::charToUIntSeed(const char* name)
{
    unsigned int value = 42; 
    unsigned int length = strlen(name);
    for(int i=0;i<length;i++)
    {
        value -= (i^name[i]);
    }
    return value;
}

#ifndef WITH_SYSTEMC

boost::mt19937* RandomSeedManager::get() { return &default_rng_; }

#else

#include <sysc/kernel/sc_simcontext.h>
boost::mt19937* RandomSeedManager::get() {
  sc_core::sc_process_b* process = sc_core::sc_get_current_process_b();
  if (!process) return &default_rng_;
  unsigned int processId = process->proc_id;
  if (randomMap_.count(processId)) {  // constains
    return randomMap_.at(processId);
  }
  unsigned int seed = charToUIntSeed(process->name());
  boost::mt19937* rnd = new boost::mt19937(seed_ + 19937 * seed);
  randomMap_.insert(std::make_pair(processId, rnd));
  return rnd;
}

#endif

