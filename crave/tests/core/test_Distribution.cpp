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

#include <crave/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

// using namespace std;
using namespace crave;

using boost::format;

BOOST_FIXTURE_TEST_SUITE(Distribution_t, Context_Fixture)

BOOST_AUTO_TEST_CASE(variable_dist_t1) {
  Variable<int> v;
  Generator gen;
  gen(dist(v, distribution<int>::create(range<int>(0, 5))(range<int>(50, 65))(range<int>(100, 125))));

  std::map<int, int> s;
  int total = 10000;
  for (int i = 0; i < total; i++) {
    BOOST_REQUIRE(gen.next());
    BOOST_REQUIRE((0 <= gen[v] && gen[v] <= 5) || (50 <= gen[v] && gen[v] <= 65) || (100 <= gen[v] && gen[v] <= 125));
    ++s[gen[v]];
  }
  for (int i = 0; i <= 200; i++)
    if ((0 <= i && i <= 5) || (50 <= i && i <= 65) || (100 <= i && i <= 125))
      BOOST_REQUIRE(s.find(i) != s.end());
}

BOOST_AUTO_TEST_CASE(variable_dist_t2) {
  Variable<int> v;
  Generator gen;
  BOOST_REQUIRE_THROW(gen(dist(v, distribution<int>::create(range<int>(0, 10))(range<int>(50, 75))(range<int>(30, 51)))),
                    std::runtime_error);
}

BOOST_AUTO_TEST_CASE(variable_dist_t3) {
  Variable<char> v;
  Generator gen;
  gen(dist(v, distribution<char>::create(weighted_range<char>(1, 5, 50))(weighted_range<char>(10, 20, 20))(
                  weighted_range<char>(-50, -50, 30))));
  int cnt1 = 0, cnt2 = 0, cnt3 = 0;
  int total = 50000;
  for (int i = 0; i < total; i++) {
    BOOST_REQUIRE(gen.next());
    BOOST_REQUIRE((1 <= gen[v] && gen[v] <= 5) || (10 <= gen[v] && gen[v] <= 20) || (gen[v] == -50));
    if (1 <= gen[v] && gen[v] <= 5) cnt1++;
    if (10 <= gen[v] && gen[v] <= 20) cnt2++;
    if (gen[v] == -50) cnt3++;
  }
  BOOST_REQUIRE_LT(cnt2, cnt3);
  BOOST_REQUIRE_LT(cnt3, cnt1);
}

BOOST_AUTO_TEST_CASE(variable_dist_t4) {
  Variable<int> v;
  Generator gen;
  gen("x", dist(v, distribution<int>::create(range<int>(0, 10))(range<int>(50, 75))(range<int>(100, 200))));
  gen("y", dist(v, distribution<int>::simple_range(5000, 6000)));
  gen.disableConstraint("x");
  int total = 10000;
  for (int i = 0; i < total; i++) {
    BOOST_REQUIRE(gen.next());
    BOOST_REQUIRE(5000 <= gen[v] && gen[v] <= 6000);
  }
}

BOOST_AUTO_TEST_CASE(dist_of_boolean25) {
  Variable<bool> a;
  int counter = 0;

  Generator gen;
  gen(dist(a, distribution<bool>::create(0.25)));
  for (unsigned i = 0; i < 1000; i++) {
    BOOST_REQUIRE(gen.next());
    if (gen[a]) {
      ++counter;
    } else {
      --counter;
    }
  }

  BOOST_REQUIRE_LT(counter, 0);
}

BOOST_AUTO_TEST_CASE(dist_of_boolean50) {
  Variable<bool> a;
  int counter = 0;

  Generator gen;
  gen(dist(a, distribution<bool>::create(0.5)));
  for (unsigned i = 0; i < 1000; i++) {
    BOOST_REQUIRE(gen.next());
    if (gen[a]) {
      ++counter;
    } else {
      --counter;
    }
  }

  BOOST_REQUIRE_LT(counter, 280);
  BOOST_REQUIRE_GT(counter, -280);
}

BOOST_AUTO_TEST_CASE(dist_of_boolean75) {
  Variable<bool> a;
  int counter = 0;

  Generator gen;
  gen(dist(a, distribution<bool>::create(0.75)));
  for (unsigned i = 0; i < 1000; i++) {
    BOOST_REQUIRE(gen.next());
    if (gen[a]) {
      ++counter;
    } else {
      --counter;
    }
  }

  BOOST_REQUIRE_GT(counter, 0);
}

BOOST_AUTO_TEST_SUITE_END()  // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
