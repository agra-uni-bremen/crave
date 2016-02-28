#include <boost/test/unit_test.hpp>
#include "../src/crave/experimental/Variable.hpp"

#include <set>
#include <iostream>

using namespace crave;

BOOST_FIXTURE_TEST_SUITE(Ex_Operators_t, Context_Fixture)

struct logical_not_s1 : crv_sequence_item {
    logical_not_s1(crv_object_name) {}

    crv_variable<unsigned> a;
    crv_constraint con { !(a() != 0) };
};

BOOST_AUTO_TEST_CASE(logical_not_t1) {
    logical_not_s1 item("Item");
    BOOST_REQUIRE(item.con.active());
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.a, 0);
}

struct logical_not_s2 : crv_sequence_item {
    logical_not_s2(crv_object_name) {}

    crv_variable<unsigned> a;
    crv_variable<unsigned> b;
    crv_constraint con { if_then_else(!(a() % 2 == 0), b() > 0 && b() <= 50, b() > 50 && b() <= 100) };
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
    crv_variable<bool> a;
    crv_variable<bool> b;
    crv_variable<bool> c;
    crv_constraint con1 {a() == true, b() == true};
    crv_constraint con2 {c() == (a() && b())};

    logical_and_s1(crv_object_name) { con2.deactivate(); }
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
    crv_variable<bool> a;
    crv_variable<bool> b;
    crv_variable<bool> c;
    crv_constraint con1 {a() == false, b() == false};
    crv_constraint con2 {c() == (a() || b())};

    logical_or_s1(crv_object_name) { con2.deactivate(); }
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
    crv_variable<unsigned> a;
    crv_variable<unsigned> b;
    crv_constraint con {a() == 65535, a() == b()};

    logical_equal_s1(crv_object_name) {}
};

BOOST_AUTO_TEST_CASE(equal_t1) {
    logical_equal_s1 item("equal");
    BOOST_REQUIRE(item.con.active());
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.a, item.b);
}

struct not_equal_s1 : crv_sequence_item {
    crv_variable<unsigned> a;
    crv_variable<unsigned> b;
    crv_constraint con {a() < 65535, a() != b()};

    not_equal_s1(crv_object_name) {}
};

BOOST_AUTO_TEST_CASE(not_equal_t1) {
    not_equal_s1 item("item");
    for (int i = 0; i < 300; ++i) {
        BOOST_REQUIRE(item.randomize());
        BOOST_CHECK_NE(item.a, item.b);
    }
}

struct neg_s1 : crv_sequence_item {
    crv_variable<unsigned> a;
    crv_variable<unsigned> b;
    crv_constraint con {a() == -1337, b() == -a()};

    neg_s1(crv_object_name) {}
};

BOOST_AUTO_TEST_CASE(neg_t1) {
    neg_s1 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.a, -1337);
    BOOST_CHECK_EQUAL(item.b, 1337);
}

struct neg_s2 : crv_sequence_item {
    crv_variable<bool> a;
    crv_variable<int> b;
    crv_variable<int> c;
    crv_variable<int> d;
    crv_constraint con1= {dist(a(), distribution<bool>::create(0.5)) && b() == 1337 && c() == 42 && if_then_else(a(), d() == -b(), d() == -c())};

    neg_s2(crv_object_name) { }
};

BOOST_AUTO_TEST_CASE(neg_t2) {
    neg_s2 item("item");
    for (int i = 0; i < 50; ++i) {
        BOOST_REQUIRE(item.randomize());
        if (item.a)
            BOOST_CHECK_EQUAL(item.d, -1337);
        else
            BOOST_CHECK_EQUAL(item.d, -42);
    }
}

struct comp_s1 : crv_sequence_item {
    crv_variable<unsigned> a;
    crv_variable<unsigned> b;
    crv_constraint con1= {a() == 0, b() == ~a()};

    comp_s1(crv_object_name) { }
};

BOOST_AUTO_TEST_CASE(complement_t1) {
    comp_s1 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.a, 0);
    BOOST_CHECK_EQUAL(item.b, -1);
}

struct bitwise_and_s1 : crv_sequence_item {
    crv_variable<unsigned> a;
    crv_variable<unsigned> b;
    crv_variable<unsigned> c;
    crv_constraint con1= {a() == 42, b() == 1337, c() == (a() & b())};

    bitwise_and_s1(crv_object_name) { }
};

BOOST_AUTO_TEST_CASE(bitwise_and_t1) {
    bitwise_and_s1 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.c, 40);
}

struct bitwise_or_s1 : crv_sequence_item {
    crv_variable<unsigned> a;
    crv_variable<unsigned> b;
    crv_variable<unsigned> c;
    crv_constraint con1= {a() == 42, b() == 1337, c() == (a() | b())};

    bitwise_or_s1(crv_object_name) {}
};

BOOST_AUTO_TEST_CASE(bitwise_or_t1) {
    bitwise_or_s1 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.c, 1339);
}

struct xor_s1 : crv_sequence_item {
    crv_variable<bool> a;
    crv_variable<bool> b;
    crv_variable<unsigned> c;
    crv_constraint con1={a() == false, b() == false, c() == (a() ^ b())};
    crv_constraint con2={a() == false, b() == true, c() == (a() ^ b())};

    xor_s1(crv_object_name) {
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
    crv_variable<unsigned> a;
    crv_variable<unsigned> b;
    crv_variable<unsigned> c;
    crv_constraint con1 = {a() == 65535, b() == 4080, c() == (a() ^ b())};

    xor_s2(crv_object_name) { }
};

BOOST_AUTO_TEST_CASE(xor_t2) {
    xor_s2 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_CHECK_EQUAL(item.c, 61455);
}

struct element_inside_set_s1 : crv_sequence_item {
    crv_variable<unsigned> a;
    std::set<unsigned> s { 1, 7, 9 };
    crv_constraint con1 { inside(a(), s) };

    element_inside_set_s1(crv_object_name) {}
};

BOOST_AUTO_TEST_CASE(element_inside_set_t1) {
    element_inside_set_s1 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(item.s.find(item.a) != item.s.end());

    unsigned first = item.a;
    item.con1 &= {item.a() != first};

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(item.s.find(item.a) != item.s.end());

    unsigned second = item.a;
    BOOST_REQUIRE_NE(first, second);
    
    item.con1 &= {item.a() != second};

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(item.s.find(item.a) != item.s.end());

    unsigned third = item.a;
    BOOST_REQUIRE_NE(third, second);
    BOOST_REQUIRE_NE(first, third);
    item.con1 &= {item.a() != third};

    BOOST_REQUIRE(!item.randomize());
}

struct element_inside_vec_s1 : crv_sequence_item {
    crv_variable<unsigned> x;
    std::vector<unsigned> v { 1, 7, 9 };
    crv_constraint con1= {inside(x(), v)};

    element_inside_vec_s1(crv_object_name) {}
};

BOOST_AUTO_TEST_CASE(element_inside_vec_t1) {
    element_inside_vec_s1 item("item");
    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(find(item.v.begin(), item.v.end(), item.x) != item.v.end());

    unsigned first = item.x;
    item.con1 &= {item.x() != first};

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(find(item.v.begin(), item.v.end(), item.x) != item.v.end());

    unsigned second = item.x;
    BOOST_REQUIRE_NE(first, second);
    item.con1 &= {item.x() != second};

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(find(item.v.begin(), item.v.end(), item.x) != item.v.end());

    unsigned third = item.x;
    BOOST_REQUIRE_NE(third, second);
    BOOST_REQUIRE_NE(first, third);
    item.con1 &= {item.x() != third};
    BOOST_REQUIRE(!item.randomize());
}

struct element_inside_array_s1 : crv_sequence_item {
    crv_variable<unsigned> x;
    unsigned a[3] { 1, 7, 9 };
    crv_constraint con1 { inside(x(), a) };

    element_inside_array_s1(crv_object_name) {}
};

BOOST_AUTO_TEST_CASE(element_inside_array_t) {
    element_inside_array_s1 item("item");
    BOOST_REQUIRE(item.randomize());

    unsigned first = item.x;
    item.con1 &= {item.x() != first};

    BOOST_REQUIRE(item.randomize());

    unsigned second = item.x;
    BOOST_REQUIRE_NE(first, second);
    item.con1 &= {item.x() != second};

    BOOST_REQUIRE(item.randomize());
    unsigned third = item.x;
    BOOST_REQUIRE_NE(third, second);
    BOOST_REQUIRE_NE(first, third);
    item.con1 &= {item.x() != third};

    BOOST_REQUIRE(!item.randomize());
}

struct element_inside_list_s1 : crv_sequence_item {
    crv_variable<unsigned> x;
    std::list<unsigned> l { 1, 7, 9 };
    crv_constraint con1 { inside(x(), l) };

    element_inside_list_s1(crv_object_name) {}
};

BOOST_AUTO_TEST_CASE(element_inside_list_t1) {
    element_inside_list_s1 item("item");

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(find(item.l.begin(), item.l.end(), item.x) != item.l.end());

    unsigned first = item.x;
    item.con1 &= {item.x() != first};

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(find(item.l.begin(), item.l.end(), item.x) != item.l.end());

    unsigned second = item.x;
    BOOST_REQUIRE_NE(first, second);
    item.con1 &= {item.x() != second};

    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(find(item.l.begin(), item.l.end(), item.x) != item.l.end());

    unsigned third = item.x;
    BOOST_REQUIRE_NE(third, second);
    BOOST_REQUIRE_NE(first, third);
    item.con1 &= {item.x() != third};

    BOOST_REQUIRE(!item.randomize());
}

struct element_not_inside_s1 : crv_sequence_item {
    crv_variable<unsigned> x;
    std::set<unsigned> s;
    crv_constraint con1 { inside(x(), s) };

    element_not_inside_s1(crv_object_name) { }
};

BOOST_AUTO_TEST_CASE(element_not_inside_t1) {
    element_not_inside_s1 item("item");
    BOOST_REQUIRE(!item.randomize());
}

struct element_not_inside_s2 : crv_sequence_item {
    crv_variable<unsigned> x;
    std::vector<unsigned> s;
    crv_constraint con1 {inside(x(), s)};

    element_not_inside_s2(crv_object_name) { }
};

BOOST_AUTO_TEST_CASE(element_not_inside_t2) {
    element_not_inside_s2 item("item");
    BOOST_REQUIRE(!item.randomize());
}

struct element_not_inside_s3 : crv_sequence_item {
    crv_variable<unsigned> x;
    std::list<unsigned> s;
    crv_constraint con1 { inside(x(), s) };

    element_not_inside_s3(crv_object_name) { }
};

BOOST_AUTO_TEST_CASE(element_not_inside_t3) {
    element_not_inside_s3 item("item");
    BOOST_REQUIRE(!item.randomize());
}

struct if_then_else_s1 : crv_sequence_item {
    unsigned a;
    crv_variable<unsigned> b;
    crv_constraint con1 { if_then_else(reference(a)<5, b()> 0 && b() <= 50, b() > 50 && b() <= 100) };

    if_then_else_s1(crv_object_name) { }
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
    unsigned a;
    crv_variable<unsigned> b;
    crv_constraint con1 { if_then(reference(a)<5, b()> 0 && b() <= 100),if_then(reference(a) >= 5, b() > 100 && b() <= 1000) };

    if_then_s1(crv_object_name) { }
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
    crv_variable<short> x;
    crv_constraint con1 { bitslice(10, 3, x()) == 0xFF };

    bitslice_s1(crv_object_name) { }
};

BOOST_AUTO_TEST_CASE(bitslice_t) {
  bitslice_s1 item("item");
  BOOST_REQUIRE(item.randomize());
  BOOST_REQUIRE_EQUAL((item.x >> 3) & 0xFF, 0xFF);

  BOOST_CHECK_THROW(item.con1&={bitslice(3, 10, item.x()) == 0xFF}, std::runtime_error);
  BOOST_CHECK_THROW(item.con1&={bitslice(16, 3, item.x()) == 0xFF}, std::runtime_error);
}

struct s_shiftleft : crv_sequence_item
{
    s_shiftleft(crv_object_name){}
    crv_variable<unsigned> a,b,c;
    crv_constraint con={a() < 256u,b() < ((unsigned)(sizeof(unsigned) * 8u)),c() == (a() << b())};
};
BOOST_AUTO_TEST_CASE(shiftleft) {
  VariableDefaultSolver::bypass_constraint_analysis = true;
  s_shiftleft item("item");
  int count = 0;
  while (item.randomize() && ++count < 480) {
    unsigned av = item.a;
    unsigned bv = item.b;
    unsigned r = av << bv;

    BOOST_REQUIRE_EQUAL(r, item.c);
    item.con&={item.a() != item.a || item.b() != item.b};
  }

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

struct s_shiftright : public crv_sequence_item
{
    s_shiftright(crv_object_name){}
    crv_variable<unsigned> a,b,c;
    crv_constraint con={a() > 256u,b() < 8u,c() == (a() >> b())};
};

BOOST_AUTO_TEST_CASE(shiftright) {
  VariableDefaultSolver::bypass_constraint_analysis = true;
  s_shiftright item("shiftright");
  int count = 0;
  while (item.randomize() && ++count < 500) {
    unsigned av = item.a;
    unsigned bv = item.b;
    unsigned r = av >> bv;

    BOOST_REQUIRE_EQUAL(r, item.c);
    item.con&={item.a != item.a(),item.b() != item.b};
  }

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

struct s_plus_minus : crv_sequence_item
{
    s_plus_minus(crv_object_name){}
    crv_variable<unsigned> a,b,q,r;
    crv_constraint con={b() != 0u,b()<a(),(q() == a() + b()),r() == a() - b()};
};
BOOST_AUTO_TEST_CASE(plus_minus) {
  VariableDefaultSolver::bypass_constraint_analysis = true;
  s_plus_minus item("item");

  unsigned cnt = 0u;
  while (item.randomize() && cnt < 300) {
    BOOST_REQUIRE_EQUAL(item.a + item.b, item.q);
    BOOST_REQUIRE_EQUAL(item.a - item.b, item.r);

    item.con&={(item.a() != item.a || item.b() != item.b)};
    std::cout << "#" << cnt++ << ": result: a=" << item.a << ", b=" << item.b << ", q=" << item.q << ", r=" << item.r
              << "\n" << std::endl;
  }

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

struct mult_mod_s1 : crv_sequence_item {
    crv_variable<int> a;
    crv_variable<int> b;
    crv_constraint con1 = {-3 <= a() && a() <= 3,-3 <= b() && b() <= 3,a() * b() % 6 == 0};

    mult_mod_s1(crv_object_name) { }
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

struct s_divide : public crv_sequence_item
{
    s_divide(crv_object_name){}
    crv_variable<unsigned char> a,b,q,r;
    crv_constraint con={b() != (unsigned char)0u,a() < (unsigned char)16u,b() < (unsigned char)16u,q() == a() / b(),r() == a() % b()};
};

BOOST_AUTO_TEST_CASE(divide) {
  s_divide item("item");
  VariableDefaultSolver::bypass_constraint_analysis = true;

  while (item.randomize()) {
    BOOST_REQUIRE_EQUAL(item.a / item.b, item.q);
    BOOST_REQUIRE_EQUAL(item.a % item.b, item.r);
    item.con&={item.a() != item.a,item.b() != item.b};
    std::cout << "result: a=" << item.a << ", b=" << item.b << ", q=" << item.q << ", r=" << item.r << "\n"
              << std::endl;
  }

  VariableDefaultSolver::bypass_constraint_analysis = false;
}
BOOST_AUTO_TEST_SUITE_END()
