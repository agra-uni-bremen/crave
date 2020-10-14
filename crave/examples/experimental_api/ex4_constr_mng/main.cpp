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

using crave::reference;
using crave::distribution;
using crave::dist;
using crave::bitslice;

class item : public crv_sequence_item {
 public:
  crv_variable<unsigned> src_addr;
  crv_variable<unsigned> dest_addr;
  crv_variable<short> data;
  crv_variable<int> x;
  crv_variable<short> y, z;

  crv_constraint c_addr_constraints{ dist(src_addr(), distribution<unsigned>::simple_range(0, 0xFE)),
                                     dest_addr() <= src_addr() };
  crv_constraint c_neg_data{ -16 < data(), data() < 0 };
  crv_constraint c_pos_data{ 16 > data(), data() > 0 };
  crv_constraint c_x_constraints{ bitslice(10, 3, x()) == 0xFF, x() > 123456 };
  crv_constraint c_y_constraints{(y() ^ 0x7FFF) == 0 };
  crv_constraint c_z_constraints{(z() >> 4) == 0x000F };

  item(crv_object_name) {}

  friend ostream& operator<<(ostream& os, item& it) {
    os << it.src_addr << " " << it.dest_addr << " " << it.data << " " << it.x << " " << it.y << " " << it.z;
    return os;
  }
};

int main(int argc, char* argv[]) {
  crave::init("crave.cfg");
  item it("single_packet");
  CHECK(!it.randomize());  // unsatisfiable constraints (data < 0) and (data > 0)

  std::cout << std::hex << std::showbase << std::internal;

  it.c_neg_data.deactivate();
  for (int i = 0; i < 10; i++) {
    CHECK(it.randomize());
    std::cout << it << std::endl;
  }

  std::cout << "***********************" << std::endl;

  it.c_neg_data.activate();
  it.c_pos_data.deactivate();
  for (int i = 0; i < 10; i++) {
    CHECK(it.randomize());
    std::cout << it << std::endl;
  }

  return 0;
}
