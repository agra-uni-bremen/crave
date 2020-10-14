/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/

#include "../crave/RandomSeedManager.hpp"
#include <functional>

RandomSeedManager::RandomSeedManager(unsigned int seed) : default_rng_(seed), seed_(seed) {}

RandomSeedManager::~RandomSeedManager() {
  for(random_map_t::value_type & entry : randomMap_) { delete entry.second; }
}

void RandomSeedManager::set_global_seed(unsigned int s) {
  seed_ = s;
  default_rng_.seed(s);
}

unsigned int RandomSeedManager::get_seed() {
  return seed_;
}

#ifndef WITH_SYSTEMC

std::mt19937* RandomSeedManager::get() { return &default_rng_; }

#else

#include <systemc>
std::mt19937* RandomSeedManager::get() {
	static std::hash<std::string> string_hash;
	auto process = sc_core::sc_get_current_process_handle();
	if (!process.valid())
		return &default_rng_;
	if (randomMap_.count(process.name())) {  // constains
		return randomMap_.at(process.name());
	}
	std::mt19937* rnd = new std::mt19937(
			seed_ + 19937 * string_hash(process.name()));
	randomMap_.insert(std::make_pair(process.name(), rnd));
	return rnd;
}

#endif
