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
using crave::rand_vec;
using crave::_i;
using crave::foreach;
using crave::unique;
using crave::if_then;
using crave::if_then_else;

class item : public rand_obj {
 public:
  item(rand_obj* parent = 0) : rand_obj(parent), src_addr_vec(this), dest_addr_vec(this), data_vec(this), tmp(this) {
    constraint(5 <= tmp() && tmp() <= 10);

    constraint(src_addr_vec().size() == tmp());
    constraint(foreach (src_addr_vec(), src_addr_vec()[_i] < 0xFF));
    constraint(foreach (src_addr_vec(), src_addr_vec()[_i] % 4 == 0));
    constraint(unique(src_addr_vec()));

    constraint(dest_addr_vec().size() == tmp());
    constraint(foreach (dest_addr_vec(), if_then(_i == 0, dest_addr_vec()[_i] < 0xF)));
    constraint(foreach (dest_addr_vec(), dest_addr_vec()[_i] == dest_addr_vec()[_i - 1] + 8));
    constraint(unique(dest_addr_vec()));

    constraint(data_vec().size() == 2 * tmp());
    constraint(foreach (data_vec(), if_then_else(_i % 2 == 0, data_vec()[_i] <= 10,
                                                 data_vec()[_i] == data_vec()[_i - 1] * data_vec()[_i - 1])));
  }

  friend ostream& operator<<(ostream& os, item& it) {
    os << "src_addr_vec = ";
    for (unsigned i = 0; i < it.src_addr_vec.size(); i++) os << it.src_addr_vec[i] << " ";
    os << std::endl;
    os << "dest_addr_vec = ";
    for (unsigned i = 0; i < it.dest_addr_vec.size(); i++) os << it.dest_addr_vec[i] << " ";
    os << std::endl;
    os << "data_vec = ";
    for (unsigned i = 0; i < it.data_vec.size(); i++) os << it.data_vec[i] << " ";
    os << std::endl;
    return os;
  }

  rand_vec<unsigned> src_addr_vec;
  rand_vec<unsigned> dest_addr_vec;
  rand_vec<unsigned> data_vec;
  randv<unsigned> tmp;
};

int main(int argc, char* argv[]) {
  crave::init("crave.cfg");
  item it;

  for (int i = 0; i < 10; i++) {
    CHECK(it.next());
    std::cout << it << std::endl;
  }

  return 0;
}
