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

#include <systemc.h>

#include <crave/ConstrainedRandom.hpp>
#include <crave/experimental/Experimental.hpp>
#include <crave/experimental/SystemC.hpp>

using namespace crave;

struct sysc_cont : public crv_sequence_item {
  crv_variable<sc_int<5>> x{ "x" };
  crv_variable<sc_uint<6>> y{ "y" };
  crv_variable<sc_bv<7>> z{ "z" };

  sc_uint<5> t = 13;

  crv_constraint constr{ "constr" };

  sysc_cont(crv_object_name) {
    constr = { dist(x(), make_distribution(range<int>(5, 8))), y() > 0, y() % reference(t) == 0, y() != y(prev),
               (z() & 0xF) == 0xE };
  }
};

int main(int argc, char *argv[]) {
  crave::init("crave.cfg");
  sysc_cont sc("sc");
  for (int i = 0; i < 40; i++) {
    CHECK(sc.randomize());
    std::cout << sc.x << " " << sc.y << " " << sc.z << std::endl;
    if (i == 20) {
      std::cout << std::endl;
      sc.t = 17;
    }
  }
  return 0;
}
