#include <boost/test/unit_test.hpp>

#include <crave/utils/Evaluator.hpp>
#include <crave/ir/UserConstraint.hpp>
#include <crave/ir/UserExpression.hpp>

#include <set>
#include <iostream>

// using namespace std;
using namespace crave;

BOOST_FIXTURE_TEST_SUITE(Evaluations_t, Context_Fixture)

BOOST_AUTO_TEST_CASE(logical_not_t1) {
  crv_variable<unsigned int> a;
  Evaluator evaluator;

  evaluator.assign(a(), 0u);

  BOOST_REQUIRE(evaluator.evaluate(!(a() != 0)));
  BOOST_REQUIRE(evaluator.result<bool>());

  evaluator.assign(a(), 42u);

  BOOST_REQUIRE(evaluator.evaluate(!(a() == 0)));
  BOOST_REQUIRE(evaluator.result<bool>());
}

BOOST_AUTO_TEST_CASE(logical_not_t2) {
  crv_variable<unsigned char> a;
  crv_variable<unsigned int> b;
  Evaluator eval;
  expression expr = make_expression(if_then_else(!(a() % 2 == 0), b() > 0 && b() <= 50, b() > 50 && b() <= 100));

  eval.assign(a(), 1u);

  BOOST_REQUIRE(!eval.evaluate(expr));

  eval.assign(b(), 35u);

  BOOST_REQUIRE(eval.evaluate(expr));
  BOOST_REQUIRE(eval.result<bool>());

  eval.assign(a(), 2u);
  eval.assign(b(), 75u);

  BOOST_REQUIRE(eval.evaluate(expr));
  BOOST_REQUIRE(eval.result<bool>());

  eval.assign(a(), 1u);

  BOOST_REQUIRE(eval.evaluate(expr));
  BOOST_REQUIRE(!eval.result<bool>());
}

BOOST_AUTO_TEST_CASE(logical_and_t1) {
  crv_variable<bool> a;
  crv_variable<bool> b;
  crv_variable<bool> c;
  Evaluator eval;

  eval.assign(a(), true);
  eval.assign(b(), true);

  BOOST_REQUIRE(eval.evaluate(a() && b()));
  BOOST_REQUIRE(eval.result<bool>());

  eval.assign(c(), false);

  BOOST_REQUIRE(eval.evaluate(c() == (a() && b())));
  BOOST_REQUIRE(!eval.result<bool>());

  eval.assign(a(), false);
  eval.assign(b(), false);

  BOOST_REQUIRE(eval.evaluate(c() == (a() && b())));
  BOOST_REQUIRE(eval.result<bool>());
}

BOOST_AUTO_TEST_CASE(logical_or_t1) {
  crv_variable<bool> a;
  crv_variable<bool> b;
  crv_variable<bool> c;
  Evaluator eval;

  eval.assign(a(), false);
  eval.assign(b(), false);

  BOOST_REQUIRE(eval.evaluate(a() || b()));
  BOOST_REQUIRE(!eval.result<bool>());

  eval.assign(c(), false);

  BOOST_REQUIRE(eval.evaluate(c() == (a() || b())));
  BOOST_REQUIRE(eval.result<bool>());

  eval.assign(a(), true);
  eval.assign(c(), true);

  BOOST_REQUIRE(eval.evaluate(c() == (a() || b())));
  BOOST_REQUIRE(eval.result<bool>());
}

BOOST_AUTO_TEST_CASE(equal_t1) {
  crv_variable<unsigned int> a;
  crv_variable<unsigned int> b;
  Evaluator eval;

  eval.assign(a(), 65535);

  BOOST_REQUIRE(eval.evaluate(a()));
  BOOST_REQUIRE_EQUAL(eval.result<unsigned int>(), 65535);
  BOOST_REQUIRE(eval.evaluate(a() == 65535));
  BOOST_REQUIRE(eval.result<bool>());

  eval.assign(b(), 5);

  BOOST_REQUIRE(eval.evaluate(a() == b()));
  BOOST_REQUIRE(!eval.result<bool>());
}

BOOST_AUTO_TEST_CASE(not_equal_t1) {
  crv_variable<unsigned int> a;
  crv_variable<unsigned int> b;
  Evaluator eval;

  eval.assign(a(), 25u);

  for (unsigned int i = 0; i < 50u; ++i) {
    eval.assign(b(), i);
    BOOST_REQUIRE(eval.evaluate(a() != b()));

    if (i != 25u)
      BOOST_REQUIRE(eval.result<bool>());
    else
      BOOST_REQUIRE(!eval.result<bool>());
  }
}

BOOST_AUTO_TEST_CASE(less) {
  crv_variable<unsigned> a;
  crv_variable<unsigned> b;
  Evaluator eval;

  for (unsigned int i = 0u; i < 50u; ++i) {
    eval.assign(a(), i);
    BOOST_REQUIRE(eval.evaluate(a() < 50u));
    BOOST_REQUIRE(eval.result<bool>());

    eval.assign(b(), i);
    BOOST_REQUIRE(eval.evaluate(b() < 50u));
    BOOST_REQUIRE(eval.result<bool>());
  }
}

BOOST_AUTO_TEST_CASE(less_equal) {
  crv_variable<unsigned> a;
  crv_variable<unsigned> b;
  Evaluator eval;

  for (unsigned int i = 0u; i <= 50u; ++i) {
    eval.assign(a(), i);
    BOOST_REQUIRE(eval.evaluate(a() <= 50u));
    BOOST_REQUIRE(eval.result<bool>());

    eval.assign(b(), i);
    BOOST_REQUIRE(eval.evaluate(b() <= 50u));
    BOOST_REQUIRE(eval.result<bool>());
  }
}

BOOST_AUTO_TEST_CASE(greater) {
  crv_variable<unsigned> a;
  crv_variable<unsigned> b;
  Evaluator eval;

  for (int i = 50; i > 0; --i) {
    eval.assign(a(), i);
    BOOST_REQUIRE(eval.evaluate(a() > 0u));
    BOOST_REQUIRE(eval.result<bool>());

    eval.assign(b(), i);
    BOOST_REQUIRE(eval.evaluate(b() > 0u));
    BOOST_REQUIRE(eval.result<bool>());
  }
}

BOOST_AUTO_TEST_CASE(greater_equal) {
  crv_variable<unsigned> a;
  crv_variable<unsigned> b;
  Evaluator eval;

  for (int i = 50; i >= 0; --i) {
    eval.assign(a(), i);
    BOOST_REQUIRE(eval.evaluate(a() >= 0u));
    BOOST_REQUIRE(eval.result<bool>());

    eval.assign(b(), i);
    BOOST_REQUIRE(eval.evaluate(b() >= 0u));
    BOOST_REQUIRE(eval.result<bool>());
  }
}

BOOST_AUTO_TEST_CASE(neg_t1) {
  crv_variable<int> a;
  crv_variable<int> b;
  Evaluator eval;

  eval.assign(a(), 1337);
  BOOST_REQUIRE(eval.evaluate(-a() == 1337));
  BOOST_REQUIRE(!eval.result<bool>());

  eval.assign(b(), -1337);
  BOOST_REQUIRE(eval.evaluate(a() == -b()));
  BOOST_REQUIRE(eval.result<bool>());
}

BOOST_AUTO_TEST_CASE(neg_t2) {
  int a = 1337;
  crv_variable<int> b;
  Evaluator eval;

  eval.assign(b(), -a);
  BOOST_REQUIRE(eval.evaluate(b()));
  BOOST_REQUIRE_EQUAL(eval.result<int>(), -1337);
}

BOOST_AUTO_TEST_CASE(complement_t1) {
  crv_variable<int> a;
  crv_variable<int> b;
  Evaluator eval;

  eval.assign(a(), 0);
  eval.assign(b(), -1);
  BOOST_REQUIRE(eval.evaluate(~a() == b()));
  BOOST_REQUIRE(eval.result<bool>());
}

BOOST_AUTO_TEST_CASE(complement_t2) {
  int a = 42;
  crv_variable<int> b;
  Evaluator eval;

  eval.assign(b(), a);
  BOOST_REQUIRE(eval.evaluate(~b()));
  BOOST_REQUIRE_EQUAL(eval.result<int>(), -43);
}

BOOST_AUTO_TEST_CASE(bitwise_and_t1) {
  crv_variable<unsigned int> a;
  crv_variable<unsigned int> b;
  Evaluator eval;

  eval.assign(a(), 42);
  eval.assign(b(), 1337);

  BOOST_REQUIRE(eval.evaluate(a() & b()));
  BOOST_REQUIRE_EQUAL(eval.result<unsigned int>(), 40);
}

BOOST_AUTO_TEST_CASE(bitwise_or_t1) {
  crv_variable<unsigned int> a;
  crv_variable<unsigned int> b;
  Evaluator eval;

  eval.assign(a(), 42);
  eval.assign(b(), 1337);

  BOOST_REQUIRE(eval.evaluate(a() | b()));
  BOOST_REQUIRE_EQUAL(eval.result<unsigned int>(), 1339);
}

BOOST_AUTO_TEST_CASE(xor_t1) {
  crv_variable<bool> a;
  crv_variable<bool> b;
  Evaluator eval;

  eval.assign(a(), false);
  eval.assign(b(), false);

  BOOST_REQUIRE(eval.evaluate(a() ^ b()));
  BOOST_REQUIRE_EQUAL(eval.result<bool>(), false);

  eval.assign(b(), true);

  BOOST_REQUIRE(eval.evaluate(a() ^ b()));
  BOOST_REQUIRE_EQUAL(eval.result<bool>(), true);
}

BOOST_AUTO_TEST_CASE(xor_t2) {
  crv_variable<unsigned int> a;
  crv_variable<unsigned int> b;
  Evaluator eval;

  eval.assign(a(), 65535);
  eval.assign(b(), 4080);

  BOOST_REQUIRE(eval.evaluate(a() ^ b()));
  BOOST_REQUIRE_EQUAL(eval.result<unsigned int>(), 61455);
}

BOOST_AUTO_TEST_CASE(shiftleft) {
  crv_variable<unsigned> a;
  crv_variable<char> b;
  Evaluator eval;

  int count = 0;
  while (++count < 256) {
    eval.assign(a(), count);
    eval.assign(b(), count % (sizeof(unsigned) << 3u));

    BOOST_REQUIRE(eval.evaluate(a() << b()));
    BOOST_REQUIRE_EQUAL(eval.result<unsigned>(), count << (count % (sizeof(unsigned) << 3u)));
  }
}

BOOST_AUTO_TEST_CASE(shiftright) {
  crv_variable<unsigned> a;
  crv_variable<char> b;
  Evaluator eval;

  int count = 0;
  while (256 > ++count) {
    eval.assign(a(), count + 256);
    eval.assign(b(), count % 8);

    BOOST_REQUIRE(eval.evaluate(a() >> b()));
    BOOST_REQUIRE_EQUAL(eval.result<unsigned>(), (count + 256) >> (count % 8));
  }
}

BOOST_AUTO_TEST_CASE(plus_minus) {
  crv_variable<unsigned int> a;
  crv_variable<unsigned int> b;
  Evaluator eval;

  unsigned int cnt = 0;
  while (cnt++ < 300) {
    eval.assign(a(), cnt * cnt);
    eval.assign(b(), cnt + cnt);

    BOOST_REQUIRE(eval.evaluate(a() + b()));
    BOOST_REQUIRE_EQUAL(eval.result<unsigned>(), (cnt * cnt) + (cnt + cnt));

    BOOST_REQUIRE(eval.evaluate(a() - b()));
    BOOST_REQUIRE_EQUAL(eval.result<unsigned>(), (cnt * cnt) - (cnt + cnt));
  }
}

BOOST_AUTO_TEST_CASE(mult_mod) {
  crv_variable<int> a;
  crv_variable<int> b;
  Evaluator eval;

  for (int i = -3; i <= 3; i++) {
    for (int j = -3; j <= 3; j++) {
      eval.assign(a(), i);
      eval.assign(b(), j);

      BOOST_REQUIRE(eval.evaluate(a() * b() % 6));
      BOOST_REQUIRE_EQUAL(eval.result<int>(), i * j % 6);
    }
  }

  eval.assign(b(), 0);

  BOOST_REQUIRE(!eval.evaluate(a() % b()));
}

BOOST_AUTO_TEST_CASE(divide) {
  crv_variable<short> a;
  crv_variable<short> b;
  Evaluator eval;

  unsigned int cnt = 1;
  while (cnt++ < 256) {
    eval.assign(a(), cnt * cnt);
    eval.assign(b(), cnt + cnt);

    BOOST_REQUIRE(eval.evaluate(a() / b()));
    BOOST_REQUIRE_EQUAL(eval.result<short>(), (cnt * cnt) / (cnt + cnt));

    BOOST_REQUIRE(eval.evaluate(a() % b()));
    BOOST_REQUIRE_EQUAL(eval.result<short>(), (cnt * cnt) % (cnt + cnt));
  }

  eval.assign(b(), 0u);
  BOOST_REQUIRE(!eval.evaluate(a() / b()));
}

BOOST_AUTO_TEST_CASE(element_inside_set) {
  std::set<unsigned> s;
  s.insert(1);
  s.insert(7);
  s.insert(9);

  crv_variable<unsigned> x;
  Evaluator eval;

  eval.assign(x(), 1);

  BOOST_REQUIRE(eval.evaluate(inside(x(), s)));
  BOOST_REQUIRE(eval.result<bool>());

  eval.assign(x(), 5);

  BOOST_REQUIRE(eval.evaluate(inside(x(), s)));
  BOOST_REQUIRE(!eval.result<bool>());
}

BOOST_AUTO_TEST_CASE(element_inside_vec) {
  std::vector<unsigned> v;
  v.push_back(1);
  v.push_back(7);
  v.push_back(9);

  crv_variable<unsigned> x;
  Evaluator eval;

  eval.assign(x(), 7u);

  BOOST_REQUIRE(eval.evaluate(inside(x(), v)));
  BOOST_REQUIRE(eval.result<bool>());

  eval.assign(x(), 5u);

  BOOST_REQUIRE(eval.evaluate(inside(x(), v)));
  BOOST_REQUIRE(!eval.result<bool>());
}

BOOST_AUTO_TEST_CASE(element_inside_array) {
  unsigned a[3];
  a[0] = 1;
  a[1] = 7;
  a[2] = 9;

  crv_variable<unsigned> x;
  Evaluator eval;

  eval.assign(x(), 9);

  BOOST_REQUIRE(eval.evaluate(inside(x(), a)));
  BOOST_REQUIRE(eval.result<bool>());

  eval.assign(x(), 5u);

  BOOST_REQUIRE(eval.evaluate(inside(x(), a)));
  BOOST_REQUIRE(!eval.result<bool>());
}

BOOST_AUTO_TEST_CASE(element_inside_list) {
  std::list<unsigned> l;
  l.push_back(1);
  l.push_back(7);
  l.push_back(9);

  crv_variable<unsigned> x;
  Evaluator eval;

  eval.assign(x(), 7u);

  BOOST_REQUIRE(eval.evaluate(inside(x(), l)));
  BOOST_REQUIRE(eval.result<bool>());

  eval.assign(x(), 5u);

  BOOST_REQUIRE(eval.evaluate(inside(x(), l)));
  BOOST_REQUIRE(!eval.result<bool>());
}

BOOST_AUTO_TEST_CASE(element_not_inside) {
  Evaluator eval;
  {
    std::set<unsigned> s;
    crv_variable<unsigned> x;
    eval.assign(x(), 1u);

    BOOST_REQUIRE(eval.evaluate(inside(x(), s)));
    BOOST_REQUIRE(!eval.result<bool>());

    s.insert(1u);

    BOOST_REQUIRE(eval.evaluate(inside(x(), s)));
    BOOST_REQUIRE(eval.result<bool>());
  }
  {
    std::vector<unsigned> v;

    crv_variable<unsigned> x;
    eval.assign(x(), 1u);

    BOOST_REQUIRE(eval.evaluate(inside(x(), v)));
    BOOST_REQUIRE(!eval.result<bool>());

    v.push_back(1u);

    BOOST_REQUIRE(eval.evaluate(inside(x(), v)));
    BOOST_REQUIRE(eval.result<bool>());
  }
  {
    std::vector<unsigned> l;
    crv_variable<unsigned> x;
    eval.assign(x(), 1u);

    BOOST_REQUIRE(eval.evaluate(inside(x(), l)));
    BOOST_REQUIRE(!eval.result<bool>());

    l.push_back(1u);

    BOOST_REQUIRE(eval.evaluate(inside(x(), l)));
    BOOST_REQUIRE(eval.result<bool>());
  }
}

BOOST_AUTO_TEST_CASE(if_then_else_t1) {
  crv_variable<unsigned int> a;
  crv_variable<unsigned int> b;
  Evaluator eval;
  expression expr = make_expression(if_then_else(a()<5, b()> 0 && b() <= 50, b() > 50 && b() <= 100));

  for (int i = 0; i < 10; ++i) {
    eval.assign(a(), i);
    eval.assign(b(), 25);
    BOOST_REQUIRE(eval.evaluate(expr));

    if (i < 5) {
      BOOST_REQUIRE(eval.result<bool>());
    } else {
      BOOST_REQUIRE(!eval.result<bool>());
    }
    eval.assign(b(), 75);
    BOOST_REQUIRE(eval.evaluate(expr));

    if (i < 5) {
      BOOST_REQUIRE(!eval.result<bool>());
    } else {
      BOOST_REQUIRE(eval.result<bool>());
    }
  }
}

BOOST_AUTO_TEST_CASE(if_then_t1) {
  crv_variable<unsigned int> a;
  crv_variable<unsigned int> b;
  Evaluator eval;
  expression expr = make_expression(if_then(a()<5, b()> 0 && b() <= 100));

  for (int i = 0; i < 10; ++i) {
    eval.assign(a(), i);
    eval.assign(b(), 25u);
    BOOST_REQUIRE(eval.evaluate(expr));
    BOOST_REQUIRE(eval.result<bool>());

    eval.assign(b(), 705u);
    BOOST_REQUIRE(eval.evaluate(expr));

    if (i < 5) {
      BOOST_REQUIRE(!eval.result<bool>());
    } else {
      BOOST_REQUIRE(eval.result<bool>());
    }
  }
}

BOOST_AUTO_TEST_CASE(equal_t2) {
  crv_variable<unsigned int> a;
  crv_variable<unsigned int> b;
  Evaluator eval;

  eval.assign(a(), 1);
  eval.assign(b(), 2);

  BOOST_REQUIRE(eval.evaluate(a() == b()));
  BOOST_REQUIRE(!eval.result<bool>());
}

BOOST_AUTO_TEST_CASE(equal_t3) {
  crv_variable<unsigned int> a;
  crv_variable<unsigned int> b;
  crv_variable<unsigned int> c;
  Evaluator eval;

  eval.assign(a(), 1);
  eval.assign(b(), 2);
  eval.assign(c(), 3);

  BOOST_REQUIRE(eval.evaluate(a() + b() == c()));
  BOOST_REQUIRE(eval.result<bool>());
}

BOOST_AUTO_TEST_SUITE_END()  // Evaluations
