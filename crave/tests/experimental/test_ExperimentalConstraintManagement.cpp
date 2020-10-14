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

#include <boost/test/unit_test.hpp>

#include <crave/experimental/Variable.hpp>
#include <crave/experimental/Coverage.hpp>
#include <crave/experimental/Constraint.hpp>

#include <ostream>
using std::ostream;

using namespace crave;

BOOST_FIXTURE_TEST_SUITE(ConstraintManagement, Context_Fixture)

struct Item : public crv_sequence_item {
  crv_variable<unsigned int> a;
  crv_variable<unsigned int> b;

  crv_constraint sum{a() + b() == 4};
  crv_constraint product{a() * b() == 4};
  crv_constraint range{a() < 10, b() < 10};
  crv_constraint x{a() != 2};

  Item(crv_object_name) {}
  friend ostream& operator<<(ostream& os, const Item& it) {
    os << it.a << " " << it.b;
    return os;
  }
};

struct Item1 : public crv_sequence_item {
  Item item = {"Item"};

  crv_variable<unsigned int> c;
  crv_variable<unsigned int> d;

  crv_constraint eq = {c() == item.a() + 1, d() == item.b() + 1};

  Item1(crv_object_name) {}
  friend ostream& operator<<(ostream& os, const Item1& it) {
    os << it.c << " " << it.c << " -- " << it.item;
    return os;
  }
};

BOOST_AUTO_TEST_CASE(test1) {
  Item it("Item");

  BOOST_REQUIRE(!it.randomize());
  BOOST_REQUIRE(it.sum.active());
  BOOST_REQUIRE(it.product.active());
  BOOST_REQUIRE(it.x.active());

  it.sum.deactivate();
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(!it.sum.active());
  BOOST_REQUIRE(it.product.active());
  BOOST_REQUIRE(it.x.active());
  std::cout << it << std::endl;

  it.product.deactivate();
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(!it.sum.active());
  BOOST_REQUIRE(!it.product.active());
  BOOST_REQUIRE(it.x.active());
  std::cout << it << std::endl;

  it.sum.activate();
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(it.sum.active());
  BOOST_REQUIRE(!it.product.active());
  BOOST_REQUIRE(it.x.active());
  std::cout << it << std::endl;

  it.product.activate();
  BOOST_REQUIRE(!it.randomize());
  BOOST_REQUIRE(it.sum.active());
  BOOST_REQUIRE(it.product.active());
  BOOST_REQUIRE(it.x.active());

  it.x.deactivate();
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(it.sum.active());
  BOOST_REQUIRE(it.product.active());
  BOOST_REQUIRE(!it.x.active());

  std::cout << it << std::endl;
  BOOST_REQUIRE(it.a == 2 && it.b == 2);
}

BOOST_AUTO_TEST_CASE(test2) {
  Item1 it("Item1");

  BOOST_REQUIRE(!it.item.randomize());
  BOOST_REQUIRE(!it.randomize());

  it.item.x.deactivate();

  BOOST_REQUIRE(it.item.randomize());
  std::cout << it.item << std::endl;
  BOOST_REQUIRE(it.item.a == 2 && it.item.b == 2);

  BOOST_REQUIRE(it.randomize());
  std::cout << it << std::endl;
  BOOST_REQUIRE(it.c == 3 && it.d == 3);

  it.item.x.activate();

  BOOST_REQUIRE(!it.randomize());
  BOOST_REQUIRE(!it.item.randomize());
}

BOOST_AUTO_TEST_SUITE_END()  // ConstraintManagement
