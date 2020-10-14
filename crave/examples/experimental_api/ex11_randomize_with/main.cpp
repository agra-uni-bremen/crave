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

using namespace crave;

struct base_obj : public crv_sequence_item {
  crv_variable<unsigned> x{ "x" };
  crv_variable<unsigned> y{ "y" };

  crv_constraint c1{ "c1" };
  crv_constraint c2{ "c2" };

  base_obj(crv_object_name) {
    c1 = { x() <= 10 };
    c2 = { 10 < y() && y() <= 20 };
  }
};

struct derived_obj : public base_obj {
  crv_variable<unsigned> z{ "z" };

  derived_obj(crv_object_name name) : base_obj(name) {}
};

int main(int argc, char *argv[]) {
  crave::init("crave.cfg");
  derived_obj obj("obj");
  for (int i = 0; i < 10; i++) {
    CHECK(obj.randomize());
    std::cout << obj.x << " " << obj.y << " " << obj.z << std::endl;
  }
  std::cout << std::endl;
  for (int i = 0; i < 10; i++) {
    CHECK(obj.randomize_with(obj.x() % 5 == 0, obj.y() % 5 == 0, obj.x() + obj.y() == obj.z()));
    std::cout << obj.x << " " << obj.y << " " << obj.z << std::endl;
  }
}
