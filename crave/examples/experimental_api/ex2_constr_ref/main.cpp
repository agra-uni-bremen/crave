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
#include <crave/experimental/Experimental.hpp>

#include <iostream>

using std::ostream;

using crave::crv_sequence_item;
using crave::crv_constraint;
using crave::crv_variable;
using crave::crv_object_name;

using crave::rand_obj;
using crave::randv;
using crave::distribution;
using crave::dist;
using crave::range;
using crave::reference;
using crave::weighted_range;

class item : public crv_sequence_item {
 public:
  item(crv_object_name) : src_addr(0) {
    c_dest_aligned = { dest_addr() % 4 == 0 };
    c_dest_bigger_src = { dest_addr() >= reference(src_addr) + 3 };
  }

  friend ostream& operator<<(ostream& os, item& it) {
    os << it.src_addr << " " << it.dest_addr;
    return os;
  }

  unsigned src_addr;
  crv_variable<unsigned> dest_addr;

  crv_constraint c_dest_aligned;
  crv_constraint c_dest_bigger_src;
};

int main(int argc, char* argv[]) {
  crave::init("crave.cfg");
  item it("single_packet");
  for (unsigned i = 0; i < 20; i++) {
    it.src_addr = i;
    CHECK(it.randomize());
    std::cout << it << std::endl;
  }
  return 0;
}
