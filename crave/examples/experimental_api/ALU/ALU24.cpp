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

#include <crave/experimental/SystemC.hpp>
#include <crave/ConstrainedRandom.hpp>
#include <systemc.h>
#include <boost/timer.hpp>
#include <crave/experimental/Experimental.hpp>

using crave::crv_sequence_item;
using crave::crv_constraint;
using crave::crv_variable;
using crave::crv_object_name;

using sc_dt::sc_bv;
using sc_dt::sc_uint;

struct ALU24 : public crv_sequence_item {
  crv_variable<sc_bv<2> > op;
  crv_variable<sc_uint<24> > a, b;

  crv_constraint c_add{ "add" };
  crv_constraint c_sub{ "sub" };
  crv_constraint c_mul{ "mul" };
  crv_constraint c_div{ "div" };

  ALU24(crv_object_name) {
    c_add = {(op() != 0x0) || (16777215 >= a() + b()) };
    c_sub = {(op() != 0x1) || ((16777215 >= a() - b()) && (b() <= a())) };
    c_mul = {(op() != 0x2) || (16777215 >= a() * b()) };
    c_div = {(op() != 0x3) || (b() != 0) };
  }

  friend std::ostream& operator<<(std::ostream& o, ALU24 const& alu) {
    o << alu.op << ' ' << alu.a << ' ' << alu.b;
    return o;
  }
};

int sc_main(int argc, char** argv) {
  crave::init("crave.cfg");
  boost::timer timer;
  ALU24 c("ALU");
  CHECK(c.randomize());
  std::cout << "first: " << timer.elapsed() << "\n";
  for (int i = 0; i < 1000; ++i) {
    CHECK(c.randomize());
  }
  std::cout << "complete: " << timer.elapsed() << "\n";
  return 0;
}
