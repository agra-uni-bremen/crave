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

#include "../crave/utils/CraveSettings.hpp"

namespace crave {

CraveSetting::CraveSetting(std::string const& filename)
    : Setting(filename), module_name_("crave"), backend_(), specified_seed_(), used_seed_(), BACKEND("backend"), SEED("seed"), LASTSEED("lastseed") {}

void CraveSetting::load_(const ptree& tree) {
  backend_ = tree.get(module_name_ + "." + BACKEND, "auto");
  specified_seed_ = tree.get(module_name_ + "." + SEED, 0);
}

void CraveSetting::save_(ptree* tree) const {
  tree->put(module_name_ + "." + BACKEND, backend_);
  tree->put(module_name_ + "." + SEED, specified_seed_);
  tree->put(module_name_ + "." + LASTSEED, used_seed_);
}

std::string const& CraveSetting::get_backend() const { return backend_; }

unsigned int CraveSetting::get_specified_seed() const { return specified_seed_; }

void CraveSetting::set_used_seed(unsigned int seed) { used_seed_ = seed; }

}
