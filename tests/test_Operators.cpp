#include <boost/test/unit_test.hpp>

#include <set>
#include <iostream>

// using namespace std;
using namespace crave;

BOOST_FIXTURE_TEST_SUITE(Operators_t, Context_Fixture)

BOOST_AUTO_TEST_CASE(logical_not_t1) {
  Variable<unsigned int> a;
  Generator gen;
  gen(!(a != 0));

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL(gen[a], 0);
}

BOOST_AUTO_TEST_CASE(logical_not_t2) {
  Variable<unsigned char> a;
  Variable<unsigned int> b;
  Generator gen;

  gen(if_then_else(!(a % 2 == 0), b > 0 && b <= 50, b > 50 && b <= 100));

  BOOST_REQUIRE(gen.next());
  std::cout << "a =" << gen[a] << ", b = " << gen[b] << std::endl;
  if (gen[a] % 2 != 0) {
    BOOST_REQUIRE_GT(gen[b], 0u);
    BOOST_REQUIRE_LE(gen[b], 50u);
  } else {
    BOOST_REQUIRE_GT(gen[b], 50u);
    BOOST_REQUIRE_LE(gen[b], 100u);
  }
}

BOOST_AUTO_TEST_CASE(logical_and_t1) {
  Variable<bool> a;
  Variable<bool> b;
  Variable<bool> c;
  Generator gen(a == true);
  gen(b == true);

  BOOST_REQUIRE(gen.next());
  gen(c == (a && b));

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL(gen[c], true);

  Generator gen2(a == true);
  gen2(b == false);

  BOOST_REQUIRE(gen2.next());
  gen2(c == (a && b));

  BOOST_REQUIRE(gen2.next());
  BOOST_CHECK_EQUAL(gen[c], false);
}

BOOST_AUTO_TEST_CASE(logical_or_t1) {
  Variable<bool> a;
  Variable<bool> b;
  Variable<bool> c;
  Generator gen(a == false);
  gen(b == false);

  BOOST_REQUIRE(gen.next());
  gen(c == (a || b));

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL(gen[c], false);

  Generator gen2(a == true);
  gen2(b == false);

  BOOST_REQUIRE(gen2.next());
  gen2(c == (a || b));

  BOOST_REQUIRE(gen2.next());
  BOOST_CHECK_EQUAL(gen[c], true);
}

BOOST_AUTO_TEST_CASE(equal_t1) {
  Variable<unsigned int> a;
  Variable<unsigned int> b;
  Generator gen(a == 65535);
  gen(b == a);

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL(gen[a], gen[b]);
}

BOOST_AUTO_TEST_CASE(not_equal_t1) {
  Variable<unsigned int> a;
  Variable<unsigned int> b;
  Generator gen(a < 65535);
  gen(b != a);

  for (int i = 0; i < 300; ++i) {
    BOOST_REQUIRE(gen.next());
    BOOST_CHECK_NE(gen[a], gen[b]);
  }
}

BOOST_AUTO_TEST_CASE(less) {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Variable<unsigned> a;

  Generator gen;
  gen(a < 256u);

  std::set<unsigned> generated;
  for (unsigned iterations = 0; gen.next(); ++iterations) {
    unsigned av = gen[a];
    generated.insert(av);
    gen(a != av);
    BOOST_REQUIRE_LT(iterations, 300);
  }

  BOOST_REQUIRE_EQUAL(generated.size(), 256);

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

BOOST_AUTO_TEST_CASE(less_equal) {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Variable<unsigned> a;

  Generator gen;
  gen(a <= 256u);

  std::set<unsigned> generated;
  for (unsigned iterations = 0; gen.next(); ++iterations) {
    unsigned av = gen[a];
    generated.insert(av);
    gen(a != av);
    BOOST_REQUIRE_LT(iterations, 300);
  }

  BOOST_REQUIRE_EQUAL(generated.size(), 257);

  VariableDefaultSolver::bypass_constraint_analysis = false;
}
/* TODO
BOOST_AUTO_TEST_CASE(greater) {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Variable<unsigned> a;

  Generator gen;
  gen(a > (std::numeric_limits<unsigned>::max - 256));

  std::set<unsigned> generated;
  for (unsigned iterations = 0; gen.next(); ++iterations) {
    unsigned av = gen[a];
    generated.insert(av);
    gen(a != av);
    BOOST_REQUIRE_LT(iterations, 300);
  }

  BOOST_REQUIRE_EQUAL(generated.size(), 256);

  VariableDefaultSolver::bypass_constraint_analysis = false;
}*/
/* TODO
BOOST_AUTO_TEST_CASE(greater_equal) {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Variable<unsigned> a;

  Generator gen;
  gen(a >= (std::numeric_limits<unsigned>::max - 256));

  std::set<unsigned> generated;
  for (unsigned iterations = 0; gen.next(); ++iterations) {
    unsigned av = gen[a];
    generated.insert(av);
    gen(a != av);
    BOOST_REQUIRE_LT(iterations, 300);
  }

  BOOST_REQUIRE_EQUAL(generated.size(), 257);

  VariableDefaultSolver::bypass_constraint_analysis = false;
}*/

BOOST_AUTO_TEST_CASE(neg_t1) {
  Variable<int> a;
  Variable<int> b;
  Generator gen(a == -1337);

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL(gen[a], -1337);
  gen(b == -a);

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL(gen[b], 1337);
}
/* TODO
BOOST_AUTO_TEST_CASE(neg_t2) {
  Variable<bool> a;
  Variable<int> b;
  Variable<int> c;
  Variable<int> d;
  Generator gen(dist(a, distribution<bool>::create(0.5)));
  gen(b == 1337 && c == 42);
  gen(if_then_else(a, d == -b, d == -c));

  for (int i = 0; i < 50; ++i) {
    BOOST_CHECK(gen.next());
    if (gen[a])
      BOOST_CHECK_EQUAL(gen[d], -1337);
    else
      BOOST_CHECK_EQUAL(gen[d], -42);
  }
}*/

BOOST_AUTO_TEST_CASE(complement_t1) {
  Variable<int> a;
  Variable<int> b;
  Generator gen(a == 0);

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL(gen[a], 0);
  gen(b == ~a);

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL(gen[b], -1);
}

BOOST_AUTO_TEST_CASE(bitwise_and_t1) {
  Variable<unsigned int> a;
  Variable<unsigned int> b;
  Variable<unsigned int> c;
  Generator gen(a == 42);
  gen(b == 1337);

  BOOST_REQUIRE(gen.next());
  gen(c == (a & b));

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL(gen[c], 40);
}

BOOST_AUTO_TEST_CASE(bitwise_or_t1) {
  Variable<unsigned int> a;
  Variable<unsigned int> b;
  Variable<unsigned int> c;
  Generator gen(a == 42);
  gen(b == 1337);

  BOOST_REQUIRE(gen.next());
  gen(c == (a | b));

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL(gen[c], 1339);
}

BOOST_AUTO_TEST_CASE(xor_t1) {
  Variable<bool> a;
  Variable<bool> b;
  Variable<unsigned int> c;
  Generator gen(a == false);
  gen(b == false);
  gen(c == (a ^ b));

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL(gen[c] != 0, false);

  Generator gen2(a == false);
  gen2(b == true);
  gen2(c == (a ^ b));

  BOOST_REQUIRE(gen2.next());
  BOOST_CHECK_EQUAL(gen[c] != 0, true);
}

BOOST_AUTO_TEST_CASE(xor_t2) {
  Variable<unsigned int> a;
  Variable<unsigned int> b;
  Variable<unsigned int> c;
  Generator gen(a == 65535);
  gen(b == 4080);

  BOOST_REQUIRE(gen.next());
  gen(c == (a ^ b));

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL(gen[c], 61455);
}

BOOST_AUTO_TEST_CASE(shiftleft) {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Variable<unsigned> a;
  Variable<unsigned> b;
  Variable<unsigned> c;

  Generator gen;
  gen(a < 256u)
  (b < (unsigned)(sizeof(unsigned) * 8u))(c == (a << b));

  int count = 0;
  while (gen.next() && ++count < 500) {
    unsigned av = gen[a];
    unsigned bv = gen[b];
    unsigned r = av << bv;

    BOOST_REQUIRE_EQUAL(r, gen[c]);

    gen(a != gen[a] || b != gen[b]);
  }

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

BOOST_AUTO_TEST_CASE(shiftright) {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Variable<unsigned> a;
  Variable<unsigned> b;
  Variable<unsigned> c;

  Generator gen;
  gen(a > 256u)
  (b < 8u)(c == (a >> b));

  int count = 0;
  while (gen.next() && ++count < 500) {
    unsigned av = gen[a];
    unsigned bv = gen[b];
    unsigned r = av >> bv;

    BOOST_REQUIRE_EQUAL(r, gen[c]);

    gen(a != gen[a] || b != gen[b]);
  }

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

BOOST_AUTO_TEST_CASE(plus_minus) {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Variable<unsigned int> a;
  Variable<unsigned int> b;
  Variable<unsigned int> q;
  Variable<unsigned int> r;

  Generator gen;
  gen(b != 0u)
  (b < a)(q == a + b)(r == a - b);

  unsigned int cnt = 0u;
  while (gen.next() && cnt < 300) {
    BOOST_REQUIRE_EQUAL(gen[a] + gen[b], gen[q]);
    BOOST_REQUIRE_EQUAL(gen[a] - gen[b], gen[r]);

    gen(a != gen[a] || b != gen[b]);
    std::cout << "#" << cnt++ << ": result: a=" << gen[a] << ", b=" << gen[b] << ", q=" << gen[q] << ", r=" << gen[r]
              << "\n" << std::endl;
  }

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

BOOST_AUTO_TEST_CASE(mult_mod) {
  Variable<int> a;
  Variable<int> b;

  Generator gen;
  gen(-3 <= a && a <= 3)
  (-3 <= b && b <= 3)(a * b % 6 == 0);

  int cnt = 0;
  for (int i = -3; i <= 3; i++)
    for (int j = -3; j <= 3; j++)
      if (i * j % 6 == 0) cnt++;

  int cnt1 = 0;
  while (gen.next()) {
    cnt1++;
    BOOST_REQUIRE_EQUAL(gen[a] * gen[b] % 6, 0);
    gen(a != a || b != b);
    std::cout << "result: a1=" << gen[a] << ", b1=" << gen[b] << "\n" << std::endl;
    BOOST_REQUIRE_LE(cnt1, cnt);
  }

  BOOST_REQUIRE_EQUAL(cnt, cnt1);
}

BOOST_AUTO_TEST_CASE(divide) {
  VariableDefaultSolver::bypass_constraint_analysis = true;

  Variable<unsigned char> a;
  Variable<unsigned char> b;
  Variable<unsigned char> q;
  Variable<unsigned char> r;

  Generator gen;
  gen(b != (unsigned char)0u)
  (a < (unsigned char)16u)(b < (unsigned char)16u)(q == a / b)(r == a % b);

  while (gen.next()) {
    BOOST_REQUIRE_EQUAL(gen[a] / gen[b], gen[q]);
    BOOST_REQUIRE_EQUAL(gen[a] % gen[b], gen[r]);

    gen(a != gen[a] || b != gen[b]);
    std::cout << "result: a=" << gen[a] << ", b=" << gen[b] << ", q=" << gen[q] << ", r=" << gen[r] << "\n"
              << std::endl;
  }

  VariableDefaultSolver::bypass_constraint_analysis = false;
}
/* TODO
BOOST_AUTO_TEST_CASE(element_inside_set) {
  std::set<unsigned> s;
  s.insert(1);
  s.insert(7);
  s.insert(9);

  Variable<unsigned> x;

  Generator gen;
  gen(inside(x, s));

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(s.find(gen[x]) != s.end());

  unsigned first = x;
  gen(x != first);

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(s.find(x) != s.end());

  unsigned second = x;
  BOOST_REQUIRE_NE(first, second);
  gen(x != second);

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(s.find(x) != s.end());

  unsigned third = x;
  BOOST_REQUIRE_NE(third, second);
  BOOST_REQUIRE_NE(first, third);
  gen(x != third);

  BOOST_REQUIRE(!gen.next());
}*/
/* TODO
BOOST_AUTO_TEST_CASE(element_inside_vec) {
  std::vector<unsigned> v;
  v.push_back(1);
  v.push_back(7);
  v.push_back(9);

  Variable<unsigned> x;

  Generator gen;
  gen(inside(gen[x], v));

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(find(v.begin(), v.end(), gen[x]) != v.end());

  unsigned first = gen[x];
  gen(gen[x] != first);

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(find(v.begin(), v.end(), gen[x]) != v.end());

  unsigned second = gen[x];
  BOOST_REQUIRE_NE(first, second);
  gen(x != second);

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(find(std::begin(v), std::end(v), gen[x]) != std::end(v));

  unsigned third = gen[x];
  BOOST_REQUIRE_NE(third, second);
  BOOST_REQUIRE_NE(first, third);
  gen(x != third);

  BOOST_REQUIRE(!gen.next());
}*/

/* TODO
BOOST_AUTO_TEST_CASE(element_inside_array) {
  unsigned a[3];
  a[0] = 1;
  a[1] = 7;
  a[2] = 9;

  Variable<unsigned> x;
  // unsigned y = a.begin();
  Generator gen;
  gen(inside(x, a));

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(std::find(std::begin(a), std::end(a), gen[x]) != std::end(a));

  unsigned first = gen[x];
  gen(x != first);

  BOOST_REQUIRE(gen.next());
  // BOOST_REQUIRE( find(a.begin(), a.end(), x) != a.end());

  unsigned second = gen[x];
  BOOST_REQUIRE_NE(first, second);
  gen(x != second);

  BOOST_REQUIRE(gen.next());
  // BOOST_REQUIRE( find(a.begin(), a.end(), x) != a.end());
  unsigned third = gen[x];
  BOOST_REQUIRE_NE(third, second);
  BOOST_REQUIRE_NE(first, third);
  gen(x != third);

  BOOST_REQUIRE(!gen.next());
}*/
/* TODO
BOOST_AUTO_TEST_CASE(element_inside_list) {
  std::list<unsigned> l;
  l.push_back(1);
  l.push_back(7);
  l.push_back(9);

  Variable<unsigned> x;

  Generator gen;
  gen(inside(gen[x], l));

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(find(l.begin(), l.end(), gen[x]) != l.end());

  unsigned first = gen[x];
  gen(x != first);

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(find(l.begin(), l.end(), gen[x]) != l.end());

  unsigned second = gen[x];
  BOOST_REQUIRE_NE(first, second);
  gen(x != second);

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(find(l.begin(), l.end(), gen[x]) != l.end());

  unsigned third = gen[x];
  BOOST_REQUIRE_NE(third, second);
  BOOST_REQUIRE_NE(first, third);
  gen(x != third);

  BOOST_REQUIRE(!gen.next());
}*/
/* TODO
BOOST_AUTO_TEST_CASE(element_not_inside) {
  {
    std::set<unsigned> s;

    Variable<unsigned> x;

    Generator gen;
    gen(inside(x, s));

    BOOST_REQUIRE(!gen.next());
  }
  {
    std::vector<unsigned> v;

    Variable<unsigned> x;

    Generator gen;
    gen(inside(x, v));

    BOOST_REQUIRE(!gen.next());
  }
  {
    std::vector<unsigned> l;

    Variable<unsigned> x;

    Generator gen;
    gen(inside(x, l));

    BOOST_REQUIRE(!gen.next());
  }
}*/

BOOST_AUTO_TEST_CASE(if_then_else_t1) {
  unsigned int a;
  Variable<unsigned int> b;
  Generator gen;

  gen(if_then_else(reference(a)<5, b> 0 && b <= 50, b > 50 && b <= 100));

  for (a = 0; a < 10; ++a) {
    BOOST_REQUIRE(gen.next());
    std::cout << "a =" << a << ", b = " << gen[b] << std::endl;
    if (a < 5) {
      BOOST_REQUIRE_GT(gen[b], 0);
      BOOST_REQUIRE_LE(gen[b], 50);
    } else {
      BOOST_REQUIRE_GT(gen[b], 50);
      BOOST_REQUIRE_LE(gen[b], 100);
    }
  }
}

BOOST_AUTO_TEST_CASE(if_then_t1) {
  unsigned int a;
  Variable<unsigned int> b;
  Generator gen;

  gen(if_then(reference(a)<5, b> 0 && b <= 100));
  gen(if_then(reference(a) >= 5, b > 100 && b <= 1000));

  for (a = 0; a < 10; ++a) {
    BOOST_REQUIRE(gen.next());
    std::cout << "a =" << a << ", b = " << gen[b] << std::endl;
    if (a < 5) {
      BOOST_REQUIRE_GT(gen[b], 0);
      BOOST_REQUIRE_LE(gen[b], 100);
    } else {
      BOOST_REQUIRE_GT(gen[b], 100);
      BOOST_REQUIRE_LE(gen[b], 1000);
    }
  }
}
/* TODO
BOOST_AUTO_TEST_CASE(bitslice_t) {
  Variable<short> x;

  Generator gen;
  gen(bitslice(10, 3, x) == 0xFF);
  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE_EQUAL((gen[x] >> 3) & 0xFF, 0xFF);

  BOOST_CHECK_THROW(gen(bitslice(3, 10, x) == 0xFF), std::runtime_error);
  BOOST_CHECK_THROW(gen(bitslice(16, 3, x) == 0xFF), std::runtime_error);
}*/

BOOST_AUTO_TEST_SUITE_END()  // Context
