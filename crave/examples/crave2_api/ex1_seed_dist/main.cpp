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

#include <crave/ConstrainedRandom.hpp>

#include <iostream>

using std::ostream;
using crave::rand_obj;
using crave::randv;
using crave::distribution;
using crave::dist;
using crave::range;
using crave::weighted_range;

class item : public rand_obj {
 public:
  item(rand_obj* parent = 0) : rand_obj(parent), src_addr(this), dest_addr(this) {
    constraint(dist(src_addr(), distribution<unsigned>::create(range<unsigned>(0, 9))(range<unsigned>(90, 99))));
    constraint(dist(dest_addr(), distribution<unsigned>::create(weighted_range<unsigned>(0, 9, 60))(
                                     weighted_range<unsigned>(10, 19, 30))(weighted_range<unsigned>(100, 109, 10))));
  }

  friend ostream& operator<<(ostream& os, item& it) {
    os << it.src_addr << " " << it.dest_addr;
    return os;
  }

  randv<unsigned> src_addr;
  randv<unsigned> dest_addr;
};

int main(int argc, char* argv[]) {
  crave::init("crave.cfg");
  crave::set_global_seed(1234567890);  // set the global seed for consistent results

  item it;

  for (int i = 0; i < 20; i++) {
    CHECK(it.next());
    std::cout << it << std::endl;
  }

  return 0;
}
