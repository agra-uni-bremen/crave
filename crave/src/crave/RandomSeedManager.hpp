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

#pragma once
#include <cstddef>
#include <map>
#include <random>

class RandomSeedManager {
 public:
  RandomSeedManager(unsigned int seed);
  virtual ~RandomSeedManager();
  void set_global_seed(unsigned int s);
  std::mt19937* get();
  unsigned int get_seed();
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
  typedef std::map<std::string, std::mt19937*> random_map_t;
  random_map_t randomMap_;
  std::mt19937 default_rng_;
  unsigned int seed_;
};
