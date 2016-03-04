#include <boost/test/unit_test.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

// using namespace std;
using namespace crave;

using boost::format;

BOOST_FIXTURE_TEST_SUITE(Context_t, Context_Fixture)

struct s_multiple_solver_instances : public crv_sequence_item {
  s_multiple_solver_instances(crv_object_name) {}
  crv_variable<int> r;
  crv_constraint con = {r() < 6};
};

BOOST_AUTO_TEST_CASE(multiple_solver_instances) {
  s_multiple_solver_instances r1("r1");
  s_multiple_solver_instances r2("r2");
  BOOST_REQUIRE(r1.randomize());
  BOOST_REQUIRE(r2.randomize());
}

struct s_constants : public crv_sequence_item {
  s_constants(crv_object_name) {}
  crv_variable<unsigned> x;
  crv_constraint con = {x() == 135421};
};

BOOST_AUTO_TEST_CASE(constants) {
  s_constants item("item");
  BOOST_REQUIRE(item.randomize());
  BOOST_CHECK_EQUAL(item.x, 135421);
}

struct s_boolean : public crv_sequence_item {
  s_boolean(crv_object_name) {}
  crv_variable<bool> b;
  crv_constraint con = {b() == b()};
};

BOOST_AUTO_TEST_CASE(boolean) {
  s_boolean item("item");
  BOOST_REQUIRE(item.randomize());
  bool b1 = item.b;
  item.con &= {item.b() != b1};
  BOOST_REQUIRE(item.randomize());
  bool b2 = item.b;
  item.con &= {item.b() != b2};
  BOOST_REQUIRE(!item.randomize());
}

struct s_by_reference : public crv_sequence_item {
  s_by_reference(crv_object_name) {}
  crv_variable<unsigned> a;
  crv_constraint con;
};

BOOST_AUTO_TEST_CASE(by_reference) {
  s_by_reference item("item");
  unsigned b = 0;
  item.con &= {item.a == reference(b)};

  while (item.randomize()) {
    unsigned av = item.a;
    BOOST_REQUIRE_EQUAL(av, b);
    ++b;
    if (b > 10) break;
  }
}

struct s_named_reference : public crv_sequence_item {
  s_named_reference(crv_object_name) {}
  crv_variable<unsigned> a, c;
  crv_constraint con;
};

// temporaly fix a variable to a certain value using the assign operator
BOOST_AUTO_TEST_CASE(named_reference) {
  unsigned bv = 0;
  s_named_reference item("item");
  ReadReference<unsigned> b(bv);
  item.con &= {item.a() == b};
  item.con &= {item.c() != b};

  while (item.randomize()) {
    unsigned av = item.a;
    unsigned cv = item.c;
    BOOST_REQUIRE_EQUAL(av, bv);
    BOOST_REQUIRE_NE(cv, bv);
    ++bv;
    if (bv > 10) break;
  }
}

struct s_soft_constraint_t : crv_sequence_item {
  s_soft_constraint_t(crv_object_name) {}
  crv_variable<int> r;
  crv_constraint con = {r() < 6};
  crv_soft_constraint con2;
};

BOOST_AUTO_TEST_CASE(soft_constraint_t) {
  s_soft_constraint_t item("item");
  item.con2 &= {(item.r() == 2)};

  BOOST_REQUIRE(item.randomize());
  BOOST_REQUIRE_EQUAL(item.r, 2);
  item.con2 &= {(item.r() == 3)};

  BOOST_REQUIRE(item.randomize());
}

struct randv_test_s : public crv_sequence_item {
  randv_test_s(crv_object_name) {}
  crv_variable<unsigned int> a;
  crv_variable<unsigned int> b;
  crv_constraint con;
};

BOOST_AUTO_TEST_CASE(randv_test) {
  randv_test_s item("item");
  std::cout << "init: a = " << item.a << ", b = " << item.b << std::endl;
  item.con &=
      {4 <= item.a() && item.a() <= 6, 9 <= item.a() + item.b() && item.a() + item.b() <= 11, item.b() % 2 == 0};
  int count = 0;
  while (item.randomize()) {
    ++count;
    std::cout << "result: a = " << item.a << ", b = " << item.b << std::endl;
    item.con &= {(item.a() != item.a || item.b() != item.b)};
    BOOST_REQUIRE_LE(count, 10);
  }

  BOOST_REQUIRE_EQUAL(count, 4);
}

struct s_randv_var_ref_mixed_test : public crv_sequence_item {
  crv_variable<int> a;
  int b_value;
  WriteReference<int> b{&b_value};
  crv_constraint con = {4 <= a() && a() <= 6, 9 <= a() + b && a() + b <= 11, b % 2 == 0};
  s_randv_var_ref_mixed_test(crv_object_name) {}
};

BOOST_AUTO_TEST_CASE(randv_var_ref_mixed_test) {
  s_randv_var_ref_mixed_test item("item");
  unsigned count = 0;
  while (item.randomize()) {
    ++count;
    std::cout << "result: a = " << item.a << ", b = " << item.b_value;
    std::cout << std::endl;
    item.con &= {(item.a() != item.a || item.b != item.b_value)};
    BOOST_REQUIRE_LE(count, 10);
  }
  BOOST_REQUIRE_EQUAL(count, 4);
}

struct s_alu : public crv_sequence_item {
  s_alu(crv_object_name) {}
  crv_variable<unsigned> op;
  crv_variable<unsigned> a;
  crv_variable<unsigned> b;
  crv_constraint con = {
      a() < 16,                      b() < 16,
      op() < 4,                       // 4 opcodes
      (op() != 0 || a() + b() < 16),  // no add overflow
      (op() != 1 || a() > b()),       // no sub underflow
      (op() != 2 || a() * b() < 16),  // no m overflow
      (op() != 3 || b() != 0)         // div valid
  };
};
BOOST_AUTO_TEST_CASE(alu) {
  s_alu item("item");
  BOOST_REQUIRE(item.randomize());
  std::cout << "result: op=" << item.op << ", a=" << item.a << ", b=" << item.b << std::endl;
}

struct s_alu_enum : public crv_sequence_item {
  s_alu_enum(crv_object_name) {}
  crv_variable<unsigned> op, a, b;
  crv_constraint con = {(a() < 8u),                     (b() < 8u),
                        (op() < 4u),                     // 4 opcodes
                        (op() != 0u || a() + b() < 8u),  // no add overflow
                        (op() != 1u || a() > b()),       // no sub underflow
                        (op() != 2u || a() * b() < 8u),  // no m overflow
                        (op() != 3u || b() != 0u)};
};

BOOST_AUTO_TEST_CASE(alu_enum) {
  VariableDefaultSolver::bypass_constraint_analysis = true;
  s_alu_enum item("item");

  int count = 0;
  for (int a = 0; a < 8; a++)
    for (int b = 0; b < 8; b++) {
      if (a + b < 8) count++;  // op == 0
      if (a > b) count++;      // op == 1
      if (a * b < 8) count++;  // op == 2
      if (b != 0) count++;     // op == 3;
    }

  while (item.randomize()) {
    --count;
    item.con &= {item.op() != item.op || item.a() != item.a || item.b() != item.b};
    BOOST_REQUIRE_GE(count, 0);
  }
  BOOST_REQUIRE_EQUAL(count, 0);

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

struct s_pythagoras : public crv_sequence_item {
  s_pythagoras(crv_object_name) {}
  crv_variable<unsigned long long> a, b, c;
  crv_constraint con = {a() * a() + b() * b() == c() * c()};
};

BOOST_AUTO_TEST_CASE(pythagoras) {
  s_pythagoras item("pythagoras");
  BOOST_REQUIRE(item.randomize());
  unsigned long long av = item.a;
  unsigned long long bv = item.b;
  unsigned long long cv = item.c;

  BOOST_CHECK_EQUAL(av * av + bv * bv, cv * cv);
}

struct s_negative_var : public crv_sequence_item {
  s_negative_var(crv_object_name) {}
  crv_variable<int> a, b;
  crv_constraint con = {a() + b() <= 120, a() > 120};
};

BOOST_AUTO_TEST_CASE(negative_var) {
  s_negative_var item("item");
  BOOST_REQUIRE(item.randomize());

  std::cout << format("result: a=%d, b=%d\n") % item.a % item.b;

  BOOST_CHECK(item.a + item.b <= 120);
}

struct s_signed_less_zero : public crv_sequence_item {
  s_signed_less_zero(crv_object_name) {}
  crv_variable<int> a;
  crv_constraint con = {a() < 0};
};
BOOST_AUTO_TEST_CASE(signed_less_zero) {
  s_signed_less_zero item("item");
  BOOST_REQUIRE(item.randomize());

  std::cout << format("result: a=%d\n") % item.a;

  BOOST_CHECK(item.a < 0);
}

struct s_mixed_bv_width_1 : crv_sequence_item {
  s_mixed_bv_width_1(crv_object_name) {}
  crv_variable<unsigned long> a;
  crv_variable<unsigned short> b;
  crv_constraint con = {a() + b() >= 120};
};

BOOST_AUTO_TEST_CASE(mixed_bv_width_1) {
  s_mixed_bv_width_1 item("item");
  BOOST_REQUIRE(item.randomize());

  std::cout << format("result: a=%d, b=%d\n") % item.a % item.b;

  BOOST_CHECK(item.a + item.b >= 120);
}

struct s_mixed_bv_width_2 : crv_sequence_item {
  s_mixed_bv_width_2(crv_object_name) {}
  crv_variable<signed char> a;
  crv_constraint con = {a() < 10};
};

BOOST_AUTO_TEST_CASE(mixed_bv_width_2) {
  VariableDefaultSolver::bypass_constraint_analysis = true;
  s_mixed_bv_width_2 item("item");

  std::set<signed char> generated;
  for (int iterations = 0; item.randomize(); ++iterations) {
    generated.insert(item.a);
    item.con &= {item.a() != item.a};

    BOOST_REQUIRE_LT(iterations, 150);
  }

  BOOST_CHECK_EQUAL(generated.size(), 138);

  VariableDefaultSolver::bypass_constraint_analysis = false;
}

struct s_mixed_bv_width_3 : crv_sequence_item {
  s_mixed_bv_width_3(crv_object_name) {}
  crv_variable<short> a;
  crv_constraint con = {a()<10, a()> - 10};
};

BOOST_AUTO_TEST_CASE(mixed_bv_width_3) {
  s_mixed_bv_width_3 item("item");
  std::set<short> generated;
  for (unsigned iterations = 0; item.randomize(); ++iterations) {
    generated.insert(item.a);
    item.con &= {(item.a() != item.a)};

    BOOST_REQUIRE_LT(iterations, 20);
  }

  BOOST_CHECK_EQUAL(generated.size(), 19);
}

struct s_mixed_bv_width_4 : crv_sequence_item {
  s_mixed_bv_width_4(crv_object_name) {}
  crv_variable<int> a;
  crv_constraint con = {a()<(signed char)10, a()>(short)-10};
};

BOOST_AUTO_TEST_CASE(mixed_bv_width_4) {
  s_mixed_bv_width_4 item("item");
  std::set<int> generated;
  for (unsigned iterations = 0; item.randomize(); ++iterations) {
    generated.insert(item.a);
    item.con &= {item.a() != item.a};
    BOOST_REQUIRE_LT(iterations, 20);
  }

  BOOST_CHECK_EQUAL(generated.size(), 19);
}

struct s_mixed_bv_width_5 : crv_sequence_item {
  s_mixed_bv_width_5(crv_object_name) {}
  crv_variable<short> a;
  crv_variable<signed char> b;
  crv_constraint con = {-3 <= a() && a() <= 3, -3 <= b() && b() <= 3, (-2 <= a() + b()) && (a() + b() <= 2)};
};

BOOST_AUTO_TEST_CASE(mixed_bv_width_5) {
  s_mixed_bv_width_5 item("item");
  int cnt = 0;
  while (item.randomize()) {
    cnt++;
    item.con &= {(item.a() != item.a) || (item.b() != item.b)};
    BOOST_REQUIRE_LT(cnt, 300);
  }

  int cnt1 = 0;
  for (short i = -3; i <= 3; i++)
    for (signed char j = -3; j <= 3; j++)
      if ((-2 <= i + j) && (i + j <= 2)) cnt1++;

  BOOST_CHECK_EQUAL(cnt, cnt1);
}

struct s_mixed_bv_width_6 : public crv_sequence_item {
  s_mixed_bv_width_6(crv_object_name) {}
  crv_variable<short> a;
  crv_variable<signed char> b;
  crv_constraint con = {-3 <= a() && a() <= 3, -3 <= b() && b() <= 3, a() * b() % 6 == 0};
};

BOOST_AUTO_TEST_CASE(mixed_bv_width_6) {
  s_mixed_bv_width_6 item("item");
  int cnt = 0;
  for (int i = -3; i <= 3; i++)
    for (int j = -3; j <= 3; j++)
      if (i * j % 6 == 0) cnt++;

  int cnt1 = 0;
  while (item.randomize()) {
    cnt1++;
    BOOST_REQUIRE_EQUAL(item.a * item.b % 6, 0);
    item.con &= {item.a() != item.a || item.b() != item.b};
  }

  BOOST_REQUIRE_EQUAL(cnt, cnt1);
}

struct s_dist_of_boolean25 : crv_sequence_item {
  s_dist_of_boolean25(crv_object_name) {}
  crv_variable<bool> a;
  crv_constraint con = {dist(a(), distribution<bool>::create(0.25))};
};

BOOST_AUTO_TEST_CASE(dist_of_boolean25) {
  s_dist_of_boolean25 item("item");
  int counter = 0;
  for (unsigned i = 0; i < 1000; i++) {
    BOOST_REQUIRE(item.randomize());
    if (item.a) {
      ++counter;
    } else {
      --counter;
    }
  }

  BOOST_REQUIRE_LT(counter, -425);
  BOOST_REQUIRE_GT(counter, -575);
}

struct s_dist_of_boolean50 : crv_sequence_item {
  s_dist_of_boolean50(crv_object_name) {}
  crv_variable<bool> a;
  crv_constraint con = {dist(a(), distribution<bool>::create(0.5))};
};
BOOST_AUTO_TEST_CASE(dist_of_boolean50) {
  s_dist_of_boolean50 item("item");
  int counter = 0;
  for (unsigned i = 0; i < 1000; i++) {
    BOOST_REQUIRE(item.randomize());
    if (item.a) {
      ++counter;
    } else {
      --counter;
    }
  }

  BOOST_REQUIRE_LT(counter, 75);
  BOOST_REQUIRE_GT(counter, -75);
}

struct s_dist_of_boolean75 : public crv_sequence_item {
  s_dist_of_boolean75(crv_object_name) {}
  crv_variable<bool> a;
  crv_constraint con = {dist(a(), distribution<bool>::create(0.75))};
};
BOOST_AUTO_TEST_CASE(dist_of_boolean75) {
  s_dist_of_boolean75 item("item");
  int counter = 0;
  for (unsigned i = 0; i < 1000; i++) {
    BOOST_REQUIRE(item.randomize());
    if (item.a) {
      ++counter;
    } else {
      --counter;
    }
  }

  BOOST_REQUIRE_LT(counter, 575);
  BOOST_REQUIRE_GT(counter, 425);
}

BOOST_AUTO_TEST_SUITE_END()  // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
