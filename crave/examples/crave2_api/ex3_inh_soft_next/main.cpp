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
using crave::reference;

class item : public rand_obj {
 public:
  item(rand_obj* parent = 0) : rand_obj(parent), src_addr(this), dest_addr(this) {
    constraint(src_addr() <= 20);
    constraint.soft(src_addr() % 4 == 0);
    constraint(dest_addr() <= 100);
  }

  friend ostream& operator<<(ostream& os, item& it) {
    os << it.src_addr << " " << it.dest_addr;
    return os;
  }

  randv<unsigned> src_addr;
  randv<unsigned> dest_addr;
};

class item_ext : public item {
 public:
  item_ext(rand_obj* parent = 0) : item(parent), last_dest_addr(0) {
    // item_ext inherits all constraints of item
    constraint(src_addr() % 4 == 3);  // this constraint makes the soft constraint in item useless.
    constraint(dest_addr() > reference(last_dest_addr));
  }

  bool next() {
    // custom next() saves the generated value of dest_addr
    if (item::next()) {
      last_dest_addr = dest_addr;
      return true;
    }
    return false;
  }

  unsigned last_dest_addr;
};

int main(int argc, char* argv[]) {
  crave::init("crave.cfg");
  item it;
  for (int i = 0; i < 10; i++) {
    CHECK(it.next());
    std::cout << it << std::endl;
  }
  std::cout << "***********************" << std::endl;
  item_ext it_ext;
  while (it_ext.next()) {
    std::cout << it_ext << std::endl;
  }
  return 0;
}
