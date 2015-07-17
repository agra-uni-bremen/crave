// Copyright 2014 The CRAVE developers. All rights reserved.//
#include "../crave/mt19937Manager.hpp"
#include <boost/foreach.hpp>

mt19937Manager::mt19937Manager(unsigned int seed) : seed_(seed), default_rng_(seed) {}

mt19937Manager::~mt19937Manager() {
  BOOST_FOREACH(random_map_t::value_type & entry, randomMap_) { delete entry.second; }
}

void mt19937Manager::set_global_seed(unsigned int s) {
  seed_ = s;
  default_rng_.seed(s);
}

#ifndef WITH_SYSTEMC

boost::mt19937* mt19937Manager::get() { return &default_rng_; }

#else

#include <sysc/kernel/sc_simcontext.h>
boost::mt19937* mt19937Manager::get() {
  sc_core::sc_process_b* process = sc_core::sc_get_current_process_b();
  if (!process) return &default_rng_;
  unsigned int processId = process->proc_id;
  if (randomMap_.count(processId)) {  // constains
    return randomMap_.at(processId);
  }
  boost::mt19937* rnd = new boost::mt19937(seed_ + 19937 * processId);
  randomMap_.insert(std::make_pair(processId, rnd));
  return rnd;
}

#endif

