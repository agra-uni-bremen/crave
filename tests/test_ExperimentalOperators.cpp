#include <boost/test/unit_test.hpp>
#include "../src/crave/experimental/Variable.hpp"

#include <set>
#include <iostream>

using namespace crave;

BOOST_FIXTURE_TEST_SUITE(Ex_Operators_t, Context_Fixture)

struct logical_not_s1 : crv_sequence_item {
    crv_variable<unsigned int> a{"a"};
    crv_constraint con{"not"};

    logical_not_s1(crv_object_name) {
        con = {!(a() != 0)};
    }
};

BOOST_AUTO_TEST_CASE(logical_not_t1) {
    logical_not_s1 item("Item");
    BOOST_REQUIRE(item.con.active());
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.a, 0);
}

struct logical_not_s2 : crv_sequence_item {
    crv_variable<unsigned int> a{"a"};
    crv_variable<unsigned int> b{"b"};
    crv_constraint con{"not"};

    logical_not_s2(crv_object_name) {
        con = {if_then_else(!(a() % 2 == 0), b() > 0 && b() <= 50, b() > 50 && b() <= 100)};
    }
};

BOOST_AUTO_TEST_CASE(logical_not_t2) {
    logical_not_s2 item("item");

    BOOST_REQUIRE(item.randomize());
    std::cout << "a =" << item.a << ", b = " << item.b << std::endl;
    if (item.a % 2 != 0) {
        BOOST_REQUIRE_GT(item.b, 0u);
        BOOST_REQUIRE_LE(item.b, 50u);
    } else {
        BOOST_REQUIRE_GT(item.b, 50u);
        BOOST_REQUIRE_LE(item.b, 100u);
    }
}

struct logical_and_s1 : crv_sequence_item {
    crv_variable<bool> a{"a"};
    crv_variable<bool> b{"b"};
    crv_variable<bool> c{"b"};
    crv_constraint con1{"and"};
    crv_constraint con2{"and"};

    logical_and_s1(crv_object_name) {
        con1 = {a() == true, b() == true};
        con2 = {c() == (a() && b())};
        con2.deactivate();
    }
};

BOOST_AUTO_TEST_CASE(logical_and_t1) {
    logical_and_s1 item("item");
    BOOST_REQUIRE(item.con1.active());
    BOOST_REQUIRE(!item.con2.active());
    BOOST_REQUIRE(item.randomize());
    item.con2.activate();
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.c, true);
    item.con1 = {item.a() == true, item.b() == false};
    item.con2.deactivate();
    BOOST_REQUIRE(item.randomize());
    item.con2.activate();
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.c, false);
}

struct logical_or_s1 : crv_sequence_item {
    crv_variable<bool> a{"a"};
    crv_variable<bool> b{"b"};
    crv_variable<bool> c{"b"};
    crv_constraint con1{"or"};
    crv_constraint con2{"or"};

    logical_or_s1(crv_object_name) {
        con1 = {a() == false, b() == false};
        con2 = {c() == (a() || b())};
        con2.deactivate();
    }
};

BOOST_AUTO_TEST_CASE(logical_or_t1) {
    logical_or_s1 item("item");

    BOOST_REQUIRE(item.randomize());
    item.con2.activate();

    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.c, false);

    item.con1 = {item.a() == true, item.b() == false};
    item.con2.deactivate();
    BOOST_REQUIRE(item.randomize());
    item.con2.activate();

    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.c, true);
}

struct logical_equal_s1 : crv_sequence_item {
    crv_variable<unsigned int> a{"a"};
    crv_variable<unsigned int> b{"b"};
    crv_constraint con1{"equal"};

    logical_equal_s1(crv_object_name) {
        con1 = {a() == 65535, a() == b()};
    }
};

BOOST_AUTO_TEST_CASE(equal_t1) {
    logical_equal_s1 item("equal");
    BOOST_REQUIRE(item.con1.active());
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.a, item.b);
}

struct not_equal_s1 : crv_sequence_item {
    crv_variable<unsigned int> a{"a"};
    crv_variable<unsigned int> b{"b"};
    crv_constraint con1{"not_equal"};

    not_equal_s1(crv_object_name) {
        con1 = {a() < 65535, a() != b()};
    }
};

BOOST_AUTO_TEST_CASE(not_equal_t1) {
    not_equal_s1 item("item");
    for (int i = 0; i < 300; ++i) {
        BOOST_REQUIRE(item.randomize());
        BOOST_CHECK_NE(item.a, item.b);
    }
}

struct neg_s1 : crv_sequence_item {
    crv_variable<unsigned int> a{"a"};
    crv_variable<unsigned int> b{"b"};
    crv_constraint con1{"neg"};

    neg_s1(crv_object_name) {
        con1 = {a() == -1337, b() == -a()};
    }
};

BOOST_AUTO_TEST_CASE(neg_t1) {
    neg_s1 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.a, -1337);
    BOOST_CHECK_EQUAL(item.b, 1337);
}

struct neg_s2 : crv_sequence_item {
    crv_variable<bool> a{"a"};
    crv_variable<int> b{"b"};
    crv_variable<int> c{"c"};
    crv_variable<int> d{"d"};
    crv_constraint con1{"neg"};

    neg_s2(crv_object_name) {
        con1 = {dist(a(), distribution<bool>::create(0.5)), b() == 1337 && c() == 42, if_then_else(a(), d() == -b(), d() == -c())};
    }
};

BOOST_AUTO_TEST_CASE(neg_t2) {
    neg_s2 item("item");
    for (int i = 0; i < 50; ++i) {
        BOOST_CHECK(item.randomize());
        if (item.a)
            BOOST_CHECK_EQUAL(item.d, -1337);
        else
            BOOST_CHECK_EQUAL(item.d, -42);
    }
}

struct comp_s1 : crv_sequence_item {
    crv_variable<unsigned int> a{"a"};
    crv_variable<unsigned int> b{"b"};
    crv_constraint con1{"neg"};

    comp_s1(crv_object_name) {
        con1 = {a() == 0, b() == ~a()};
    }
};

BOOST_AUTO_TEST_CASE(complement_t1) {
    comp_s1 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.a, 0);
    BOOST_CHECK_EQUAL(item.b, -1);
}

struct bitwise_and_s1 : crv_sequence_item {
    crv_variable<unsigned int> a{"a"};
    crv_variable<unsigned int> b{"b"};
    crv_variable<unsigned int> c{"c"};
    crv_constraint con1{"bitwise"};

    bitwise_and_s1(crv_object_name) {
        con1 = {a() == 42, b() == 1337, c() == (a() & b())};
    }
};

BOOST_AUTO_TEST_CASE(bitwise_and_t1) {
    bitwise_and_s1 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.c, 40);
}

struct bitwise_or_s1 : crv_sequence_item {
    crv_variable<unsigned int> a{"a"};
    crv_variable<unsigned int> b{"b"};
    crv_variable<unsigned int> c{"c"};
    crv_constraint con1{"bitwise"};

    bitwise_or_s1(crv_object_name) {
        con1 = {a() == 42, b() == 1337, c() == (a() | b())};
    }
};

BOOST_AUTO_TEST_CASE(bitwise_or_t1) {
    bitwise_or_s1 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.c, 1339);
}

struct xor_s1 : crv_sequence_item {
    crv_variable<bool> a{"a"};
    crv_variable<bool> b{"b"};
    crv_variable<unsigned int> c{"c"};
    crv_constraint con1{"xor"};
    crv_constraint con2{"xor"};

    xor_s1(crv_object_name) {
        con1 = {a() == false, b() == false, c() == (a() ^ b())};
        con2 = {a() == false, b() == true, c() == (a() ^ b())};
        con2.deactivate();
    }
};

BOOST_AUTO_TEST_CASE(xor_t1) {
    xor_s1 item("item");
    BOOST_REQUIRE(item.con1.active());
    BOOST_REQUIRE(!item.con2.active());
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.c != 0, false);
    item.con1.deactivate();
    item.con2.activate();
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.c != 0, true);
}

struct xor_s2 : crv_sequence_item {
    crv_variable<unsigned int> a{"a"};
    crv_variable<unsigned int> b{"b"};
    crv_variable<unsigned int> c{"c"};
    crv_constraint con1{"xor"};

    xor_s2(crv_object_name) {
        con1 = {a() == 65535, b() == 4080, c() == (a() ^ b())};
    }
};

BOOST_AUTO_TEST_CASE(xor_t2) {
    xor_s2 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.c, 61455);
}

struct element_inside_set_s1 : crv_sequence_item {
    crv_variable<unsigned int> a{"a"};
    crv_constraint con1{"inside"};
    std::set<unsigned> s;

    element_inside_set_s1(crv_object_name) {
        s.insert(1);
        s.insert(7);
        s.insert(9);
        con1 = {inside(a(), s)};
    }
};

BOOST_AUTO_TEST_CASE(element_inside_set_t1) {
    element_inside_set_s1 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(item.s.find(item.a) != item.s.end());

    unsigned first = item.a;
    item.con1&={item.a() != first};

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(item.s.find(item.a) != item.s.end());

    unsigned second = item.a;
    BOOST_REQUIRE_NE(first, second);
    
    item.con1&={item.a() != second};

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(item.s.find(item.a) != item.s.end());

    unsigned third = item.a;
    BOOST_REQUIRE_NE(third, second);
    BOOST_REQUIRE_NE(first, third);
    item.con1&={item.a() != third};

    BOOST_REQUIRE(!item.randomize());
}

struct element_inside_vec_s1 : crv_sequence_item {
    crv_variable<unsigned int> x{"x"};
    crv_constraint con1{"inside"};
    std::vector<unsigned> v;

    element_inside_vec_s1(crv_object_name) {
        v.push_back(1);
        v.push_back(7);
        v.push_back(9);
        con1 = {inside(x(), v)};
    }
};

BOOST_AUTO_TEST_CASE(element_inside_vec_t1) {
    element_inside_vec_s1 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(find(item.v.begin(), item.v.end(), item.x) != item.v.end());

    unsigned first = item.x;
    item.con1&={item.x() != first};

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(find(item.v.begin(), item.v.end(), item.x) != item.v.end());

    unsigned second = item.x;
    BOOST_REQUIRE_NE(first, second);
    item.con1&={item.x() != second};

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(find(item.v.begin(), item.v.end(), item.x) != item.v.end());

    unsigned third = item.x;
    BOOST_REQUIRE_NE(third, second);
    BOOST_REQUIRE_NE(first, third);
    item.con1&={item.x() != third};
    BOOST_REQUIRE(!item.randomize());
}

struct element_inside_array_s1 : crv_sequence_item {
    crv_variable<unsigned int> x{"x"};
    crv_constraint con1{"inside"};
    unsigned a[3];

    element_inside_array_s1(crv_object_name) {
        a[0] = 1;
        a[1] = 7;
        a[2] = 9;
        con1 = {inside(x(), a)};
    }
};

BOOST_AUTO_TEST_CASE(element_inside_array_t) {
    element_inside_array_s1 item("item");
    Generator gen;
    item.con1&={inside(item.x(), item.a)};

    BOOST_REQUIRE(item.randomize());

    unsigned first = item.x;
    item.con1&={item.x() != first};

    BOOST_REQUIRE(item.randomize());

    unsigned second = item.x;
    BOOST_REQUIRE_NE(first, second);
    item.con1&={item.x() != second};

    BOOST_REQUIRE(item.randomize());
    unsigned third = item.x;
    BOOST_REQUIRE_NE(third, second);
    BOOST_REQUIRE_NE(first, third);
    item.con1&={item.x() != third};

    BOOST_REQUIRE(!item.randomize());
}

struct element_inside_list_s1 : crv_sequence_item {
    crv_variable<unsigned int> x{"x"};
    crv_constraint con1{"inside"};
    std::list<unsigned> l;

    element_inside_list_s1(crv_object_name) {
        l.push_back(1);
        l.push_back(7);
        l.push_back(9);
        con1 = {inside(x(), l)};
    }
};

BOOST_AUTO_TEST_CASE(element_inside_list_t1) {
    element_inside_list_s1 item("item");

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(find(item.l.begin(), item.l.end(), item.x) != item.l.end());

    unsigned first = item.x;
    item.con1&={item.x() != first};

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(find(item.l.begin(), item.l.end(), item.x) != item.l.end());

    unsigned second = item.x;
    BOOST_REQUIRE_NE(first, second);
    item.con1&={item.x() != second};

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(find(item.l.begin(), item.l.end(), item.x) != item.l.end());

    unsigned third = item.x;
    BOOST_REQUIRE_NE(third, second);
    BOOST_REQUIRE_NE(first, third);
    item.con1&={item.x() != third};

    BOOST_REQUIRE(!item.randomize());
}

struct element_not_inside_s1 : crv_sequence_item {
    crv_variable<unsigned int> x{"x"};
    crv_constraint con1{"inside"};
    std::set<unsigned> s;

    element_not_inside_s1(crv_object_name) {
        con1 = {inside(x(), s)};
    }
};

BOOST_AUTO_TEST_CASE(element_not_inside_t1) {
    element_not_inside_s1 item("item");
    BOOST_REQUIRE(!item.randomize());
}

struct element_not_inside_s2 : crv_sequence_item {
    crv_variable<unsigned int> x{"x"};
    crv_constraint con1{"inside"};
    std::vector<unsigned> s;

    element_not_inside_s2(crv_object_name) {
        con1 = {inside(x(), s)};
    }
};

BOOST_AUTO_TEST_CASE(element_not_inside_t2) {
    element_not_inside_s2 item("item");
    BOOST_REQUIRE(!item.randomize());
}

struct element_not_inside_s3 : crv_sequence_item {
    crv_variable<unsigned int> x{"x"};
    crv_constraint con1{"inside"};
    std::list<unsigned> s;

    element_not_inside_s3(crv_object_name) {
        con1 = {inside(x(), s)};
    }
};

BOOST_AUTO_TEST_CASE(element_not_inside_t3) {
    element_not_inside_s3 item("item");
    BOOST_REQUIRE(!item.randomize());
}

struct if_then_else_s1 : crv_sequence_item {
    unsigned int a;
    crv_variable<unsigned int> b{"b"};
    crv_constraint con1{"if_then_else"};

    if_then_else_s1(crv_object_name) {
        con1 = {if_then_else(reference(a)<5, b()> 0 && b() <= 50, b() > 50 && b() <= 100)};
    }
};

BOOST_AUTO_TEST_CASE(if_then_else_t1) {
  if_then_else_s1 item("item");
  for (item.a = 0; item.a < 10; ++item.a) {
    BOOST_REQUIRE(item.randomize());
    std::cout << "a =" << item.a << ", b = " << item.b << std::endl;
    if (item.a < 5) {
      BOOST_REQUIRE_GT(item.b, 0);
      BOOST_REQUIRE_LE(item.b, 50);
    } else {
      BOOST_REQUIRE_GT(item.b, 50);
      BOOST_REQUIRE_LE(item.b, 100);
    }
  }
}

struct if_then_s1 : crv_sequence_item {
    unsigned int a;
    crv_variable<unsigned int> b{"b"};
    crv_constraint con1{"if_then_else"};

    if_then_s1(crv_object_name) {
        con1 = {if_then(reference(a)<5, b()> 0 && b() <= 100),if_then(reference(a) >= 5, b() > 100 && b() <= 1000)};
    }
};

BOOST_AUTO_TEST_CASE(if_then_t1) {
  if_then_s1 item("item");

  for (item.a = 0; item.a < 10; ++item.a) {
    BOOST_REQUIRE(item.randomize());
    std::cout << "a =" << item.a << ", b = " << item.b << std::endl;
    if (item.a < 5) {
      BOOST_REQUIRE_GT(item.b, 0);
      BOOST_REQUIRE_LE(item.b, 100);
    } else {
      BOOST_REQUIRE_GT(item.b, 100);
      BOOST_REQUIRE_LE(item.b, 1000);
    }
  }
}

struct bitslice_s1 : crv_sequence_item {
    crv_variable<short> x{"x"};
    crv_constraint con1{"bitslice"};

    bitslice_s1(crv_object_name) {
        con1 = {bitslice(10, 3, x()) == 0xFF};
    }
};

BOOST_AUTO_TEST_CASE(bitslice_t) {
  bitslice_s1 item("item");
  BOOST_REQUIRE(item.randomize());
  BOOST_REQUIRE_EQUAL((item.x >> 3) & 0xFF, 0xFF);

  BOOST_CHECK_THROW(item.con1&={bitslice(3, 10, item.x()) == 0xFF}, std::runtime_error);
  BOOST_CHECK_THROW(item.con1&={bitslice(16, 3, item.x()) == 0xFF}, std::runtime_error);
}

//TODO
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

//TODO
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

//TODO
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

struct mult_mod_s1 : crv_sequence_item {
    crv_variable<int> a{"a"};
    crv_variable<int> b{"b"};
    crv_constraint con1{"mult_mod"};

    mult_mod_s1(crv_object_name) {
        con1 = {-3 <= a() && a() <= 3,-3 <= b() && b() <= 3,a() * b() % 6 == 0};
    }
};

BOOST_AUTO_TEST_CASE(mult_mod_t1) {
  mult_mod_s1 item("item");

  int cnt = 0;
  for (int i = -3; i <= 3; i++)
    for (int j = -3; j <= 3; j++)
      if (i * j % 6 == 0) cnt++;

  int cnt1 = 0;
  
  while (item.randomize()) {
    cnt1++;
    BOOST_REQUIRE_EQUAL(item.a * item.b % 6, 0);
    item.con1&={item.a() != item.a || item.b() != item.b};
    std::cout << "result: a1=" << item.a << ", b1=" << item.b << "\n" << std::endl;
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
BOOST_AUTO_TEST_SUITE_END()
