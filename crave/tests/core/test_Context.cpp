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

#include <boost/format.hpp>

#include <set>
#include <iostream>

// using namespace std;
using namespace crave;

using boost::format;

BOOST_FIXTURE_TEST_SUITE(Context_t, Context_Fixture)

BOOST_AUTO_TEST_CASE(multiple_solver_instances) {
  Generator gen1, gen2;
  Variable<int> r1, r2;
  gen1(r1 < 6);
  gen2(r2 < 6);
  BOOST_REQUIRE(gen1.next());
  BOOST_REQUIRE(gen2.next());
}

BOOST_AUTO_TEST_CASE(constants) {
  Variable<unsigned> x;
  Generator gen(x == 135421);
  gen();
  BOOST_REQUIRE_EQUAL(gen[x], 135421);
}

BOOST_AUTO_TEST_CASE(boolean) {
  Variable<bool> b;

  Generator gen;
  gen(b == b)();  // create a new assignment

  bool b1 = gen[b];
  // printf("result: b1=%d\n", b1);
  gen(b != b1)();
  bool b2 = gen[b];
  // printf("result: b2=%d\n", b2);

  BOOST_REQUIRE_THROW(gen(b != b2)(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(by_reference) {
  unsigned b = 0;
  Variable<unsigned> a;

  Generator gen(a == reference(b));

  while (gen.next()) {
    unsigned av = gen[a];
    BOOST_REQUIRE_EQUAL(av, b);
    ++b;
    if (b > 10) break;
  }

  BOOST_REQUIRE_GT(b, 10);
}

// temporaly fix a variable to a certain value using the assign operator
BOOST_AUTO_TEST_CASE(named_reference) {
  unsigned bv = 0;
  Variable<unsigned> a, c;
  ReadReference<unsigned> b(bv);

  Generator gen(a == b);
  gen(c != b);

  while (gen.next()) {
    unsigned av = gen[a];
    unsigned cv = gen[c];
    BOOST_REQUIRE_EQUAL(av, bv);
    BOOST_REQUIRE_NE(cv, bv);
    ++bv;
    if (bv > 10) break;
  }

  BOOST_REQUIRE_GT(bv, 10);
}

BOOST_AUTO_TEST_CASE(soft_constraint_t) {
  Generator gen;
  Variable<int> r;
  gen(r < 6);
  gen.soft(r == 2);

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE_EQUAL(gen[r], 2);

  gen.soft(r == 3);

  BOOST_REQUIRE(gen.next());
}

BOOST_AUTO_TEST_CASE(Variable_test) {
  Generator gen;
  Variable<int> a;
  Variable<int> b;
  gen(4 <= a && a <= 6)(9 <= a + b && a + b <= 11)(b % 2 == 0);
  int count = 0;
  while (gen.next()) {
    ++count;
    gen(a != gen[a] || b != gen[b]);
    BOOST_REQUIRE_LE(count, 10);
  }
  BOOST_REQUIRE_EQUAL(count, 4);
}

BOOST_AUTO_TEST_CASE(Variable_var_ref_mixed_test) {
  Generator gen;
  Variable<int> a;
  Variable<int> b;
  gen(4 <= a && a <= 6)(9 <= a + b && a + b <= 11)(b % 2 == 0);
  unsigned count = 0;
  while (gen.next()) {
    ++count;
    std::cout << "result: a = " << gen[a] << ", b = " << gen[b] << std::endl;
    gen(a != gen[a] || b != gen[b]);

    BOOST_REQUIRE_LE(count, 10);
  }
  BOOST_REQUIRE_EQUAL(count, 4);
}

BOOST_AUTO_TEST_CASE(alu) {
  Variable<unsigned> op;
  Variable<unsigned> a;
  Variable<unsigned> b;

  Generator gen;
  gen(a < 16)(b < 16)(op < 4)  // 4 opcodes
      (op != 0 || a + b < 16)  // no add overflow
      (op != 1 || a > b)       // no sub underflow
      (op != 2 || a * b < 16)  // no m overflow
      (op != 3 || b != 0)      // div valid
      ;
  gen();  // create a new assignment

  // printf("result: op=%d, a=%d, b=%d\n", gen[op], gen[a], gen[b]);
}

BOOST_AUTO_TEST_CASE(alu_enum) {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Variable<unsigned> op;
  Variable<unsigned> a;
  Variable<unsigned> b;

  Generator gen;
  gen(a < 8u)(b < 8u)(op < 4u)  // 4 opcodes
      (op != 0u || a + b < 8u)  // no add overflow
      (op != 1u || a > b)       // no sub underflow
      (op != 2u || a * b < 8u)  // no m overflow
      (op != 3u || b != 0u)     // div valid
      ;

  int count = 0;
  for (int a = 0; a < 8; a++)
    for (int b = 0; b < 8; b++) {
      if (a + b < 8) count++;  // op == 0
      if (a > b) count++;      // op == 1
      if (a * b < 8) count++;  // op == 2
      if (b != 0) count++;     // op == 3;
    }

  while (gen.next()) {
    --count;
    gen(op != gen[op] || a != gen[a] || b != gen[b]);

    BOOST_REQUIRE_GE(count, 0);
  }
  BOOST_REQUIRE_EQUAL(count, 0);

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

BOOST_AUTO_TEST_CASE(pythagoras) {
  Variable<unsigned long long> a;
  Variable<unsigned long long> b;
  Variable<unsigned long long> c;

  Generator gen;
  gen(a * a + b * b == c * c)();  // create a new assignment

  unsigned long long av = gen[a];
  unsigned long long bv = gen[b];
  unsigned long long cv = gen[c];

  BOOST_REQUIRE_EQUAL(av * av + bv * bv, cv * cv);
}

BOOST_AUTO_TEST_CASE(negative_var) {
  Variable<int> a;
  Variable<int> b;

  Generator gen;
  gen(a + b <= 120)(a > 120);

  gen();

  std::cout << format("result: a=%d, b=%d\n") % gen[a] % gen[b];

  BOOST_REQUIRE(gen[a] + gen[b] <= 120);
}

BOOST_AUTO_TEST_CASE(signed_less_zero) {
  Variable<int> a;

  Generator gen;
  gen(a < 0);

  gen();

  std::cout << format("result: a=%d\n") % gen[a];

  BOOST_REQUIRE(gen[a] < 0);
}

BOOST_AUTO_TEST_CASE(mixed_bv_width_1) {
  Variable<unsigned long> a;
  Variable<unsigned short> b;

  Generator gen;
  gen(a + b >= 120);

  BOOST_REQUIRE(gen.next());

  std::cout << format("result: a=%d, b=%d\n") % gen[a] % gen[b];

  BOOST_REQUIRE(gen[a] + gen[b] >= 120);
}

BOOST_AUTO_TEST_CASE(mixed_bv_width_2) {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Variable<signed char> a;
  Generator gen;
  gen(a < 10);

  std::set<signed char> generated;
  for (int iterations = 0; gen.next(); ++iterations) {
    generated.insert(gen[a]);
    gen(a != gen[a]);

    BOOST_REQUIRE_LT(iterations, 150);
  }

  BOOST_REQUIRE_EQUAL(generated.size(), 138);

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

BOOST_AUTO_TEST_CASE(mixed_bv_width_3) {
  Variable<short> a;
  Generator gen;
  gen(a < 10);
  gen(a > -10);

  std::set<short> generated;
  for (unsigned iterations = 0; gen.next(); ++iterations) {
    generated.insert(gen[a]);
    gen(a != gen[a]);

    BOOST_REQUIRE_LT(iterations, 20);
  }

  BOOST_REQUIRE_EQUAL(generated.size(), 19);
}

BOOST_AUTO_TEST_CASE(mixed_bv_width_4) {
  Variable<int> a;
  Generator gen;
  gen(a < (signed char)10);
  gen(a > (short)-10);

  std::set<int> generated;
  for (unsigned iterations = 0; gen.next(); ++iterations) {
    generated.insert(gen[a]);
    gen(a != gen[a]);

    BOOST_REQUIRE_LT(iterations, 20);
  }

  BOOST_REQUIRE_EQUAL(generated.size(), 19);
}

BOOST_AUTO_TEST_CASE(mixed_bv_width_5) {
  Variable<short> a;
  Variable<signed char> b;
  Generator gen;
  gen(-3 <= a && a <= 3);
  gen(-3 <= b && b <= 3);
  gen((-2 <= a + b) && (a + b <= 2));

  int cnt = 0;
  while (gen.next()) {
    cnt++;
    gen((a != gen[a]) || (b != gen[b]));

    BOOST_REQUIRE_LT(cnt, 300);
  }

  int cnt1 = 0;
  for (short i = -3; i <= 3; i++)
    for (signed char j = -3; j <= 3; j++)
      if ((-2 <= i + j) && (i + j <= 2)) cnt1++;

  BOOST_REQUIRE_EQUAL(cnt, cnt1);
}

BOOST_AUTO_TEST_CASE(mixed_bv_width_6) {
  Variable<short> a;
  Variable<signed char> b;

  Generator gen;
  gen(-3 <= a && a <= 3)(-3 <= b && b <= 3)(a * b % 6 == 0);

  int cnt = 0;
  for (int i = -3; i <= 3; i++)
    for (int j = -3; j <= 3; j++)
      if (i * j % 6 == 0) cnt++;

  int cnt1 = 0;
  while (gen.next()) {
    cnt1++;
    BOOST_REQUIRE_EQUAL(gen[a] * gen[b] % 6, 0);
    gen(a != gen[a] || b != gen[b]);
  }

  BOOST_REQUIRE_EQUAL(cnt, cnt1);
}

BOOST_AUTO_TEST_SUITE_END()  // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
