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
using crave::if_then_else;
using crave::if_then;
using crave::inside;
using crave::range;

enum car_type_enum { AUDI = 1, BMW = 2, MERCEDES = 3, VW = -1 };
CRAVE_ENUM(car_type_enum, (AUDI)(BMW)(MERCEDES)(VW));

enum color_enum { RED, GREEN, BLUE };
CRAVE_ENUM(color_enum, (RED)(GREEN)(BLUE));

class my_rand_obj : public rand_obj {
 public:
  randv<car_type_enum> car;
  randv<color_enum> color;
  randv<int> power;
  randv<int> price;

  my_rand_obj(rand_obj* parent = 0) : rand_obj(parent), car(this), color(this), power(this), price(this) {
    constraint(if_then(car() == AUDI, color() != GREEN));
    constraint(if_then(car() == BMW, color() != RED));
    constraint(if_then(car() == MERCEDES, color() != BLUE));
    constraint(80 <= power() && power() <= 400);
    constraint(if_then(car() == BMW, power() >= 200));
    int prices[] = { 20, 30, 40, 50, 60, 70, 80, 90, 100 };
    constraint(inside(price(), prices));
    constraint(if_then(car() == MERCEDES, price() >= 40));
    constraint(if_then(color() == RED, price() <= 40));
  }

  friend ostream& operator<<(ostream& os, my_rand_obj& obj) {
    switch (obj.color) {
      case RED:
        os << "RED";
        break;
      case GREEN:
        os << "GREEN";
        break;
      case BLUE:
        os << "BLUE";
        break;
      default:
        os << "UNKNOWN(" << obj.color << ")";
    }
    os << " ";
    switch (obj.car) {
      case AUDI:
        os << "AUDI";
        break;
      case BMW:
        os << "BMW";
        break;
      case MERCEDES:
        os << "MERCEDES";
        break;
      case VW:
        os << "VW";
        break;
      default:
        os << "UNKNOWN(" << obj.car << ")";
    }
    os << " ";
    os << obj.power;
    os << " ";
    os << obj.price;
    return os;
  }
};

int main(int argc, char* argv[]) {
  crave::init("crave.cfg");
  my_rand_obj obj;
  for (int i = 0; i < 50; i++) {
    CHECK(obj.next());
    std::cout << obj << std::endl;
  }
  return 0;
}
