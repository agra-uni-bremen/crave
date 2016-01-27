#include <boost/test/unit_test.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

using boost::format;
using namespace crave;

template <typename T>
bool check_unique(crv_vector<T>& v) {
  for (uint i = 0; i < v.size(); i++)
    for (uint j = 0; j < i; j++)
      if (v[i] == v[j]) return false;
  return true;
}

BOOST_FIXTURE_TEST_SUITE(Vector_Constraint_t, Context_Fixture)

struct Item : public crv_sequence_item {
  Item(crv_object_name){
    constraint={30 <= v().size() && v().size() <= 50,
    foreach(v(), v()[_i] == v()[_i - 1] + v()[_i - 2]),
    foreach(v(), if_then(_i <= 1, v()[_i] == _i))};
  }

  crv_vector<unsigned int> v;
  crv_constraint constraint{"constraint"};
};

BOOST_AUTO_TEST_CASE(fibo_test) {
  Item it{"item"};
  it.randomize();
  BOOST_REQUIRE_LE(30, it.v.size());
  BOOST_REQUIRE_LE(it.v.size(), 50);
  BOOST_REQUIRE_EQUAL(it.v[0], 0);
  BOOST_REQUIRE_EQUAL(it.v[1], 1);
  for (uint i = 2; i < it.v.size(); i++) BOOST_REQUIRE_EQUAL(it.v[i], it.v[i - 1] + it.v[i - 2]);
}

struct Item1 : public crv_sequence_item {
  Item1(crv_object_name) {}

  crv_vector<unsigned int> u;
  crv_vector<unsigned int> v;
};

BOOST_AUTO_TEST_CASE(free_vector_test) {
  Item1 it{"item"};
  it.randomize();
  BOOST_REQUIRE(it.u.size() == 0);
  BOOST_REQUIRE(it.v.size() > 0);
  for (uint i = 0; i < it.v.size(); i++) std::cout << it.v[i] << " ";
  std::cout << std::endl;
}

struct Item2 : public crv_sequence_item {
  Item2(crv_object_name){ constraint={foreach(v(), 100 <= v()[_i] && v()[_i] <= 200)}; }

  crv_vector<unsigned int> v;
   crv_constraint constraint{"constraint"};
};

BOOST_AUTO_TEST_CASE(default_size_test) {
  Item2 it{"item"};
  it.randomize();
  BOOST_REQUIRE(5 <= it.v.size() && it.v.size() <= 10);
  for (uint i = 0; i < it.v.size(); i++) {
    std::cout << it.v[i] << " ";
    BOOST_REQUIRE(100 <= it.v[i] && it.v[i] <= 200);
  }
  std::cout << std::endl;
}

struct Item3 : public crv_sequence_item {
  Item3(crv_object_name) {
    constraint={(v().size() == 100),
    (foreach(v(), v()[_i] < 100)),
    (foreach(v(), v()[_i] >= 0)),
    (unique(v()))};
  }

  crv_vector<int> v;
   crv_constraint constraint{"constraint"};
};

BOOST_AUTO_TEST_CASE(unique_test_1) {
  Item3 it{"item"};
  it.randomize();
  BOOST_REQUIRE(it.v.size() == 100);
  for (uint i = 0; i < it.v.size(); i++) {
    BOOST_REQUIRE(0 <= it.v[i] && it.v[i] < 100);
    for (uint j = 0; j < i; j++) BOOST_REQUIRE(it.v[i] != it.v[j]);
  }
}

BOOST_AUTO_TEST_CASE(unique_test_2) {
  crv_vector<unsigned int> v;
  Generator gen;
  gen(v().size() == 7);
  gen(foreach(v(), v()[_i] < 6));

  gen("unique", unique(v()));
  BOOST_REQUIRE(!gen.next());

  gen.disableConstraint("unique");
  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(v.size() == 7);
  for (uint i = 0; i < v.size(); i++) {
    std::cout << v[i] << " ";
    BOOST_REQUIRE(v[i] < 6);
  }
  std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(soft_unique_test) {
  crv_vector<unsigned int> v(NULL);
  Generator gen;
  gen(v().size() == 7);
  gen(foreach(v(), v()[_i] < 6));
  gen.soft(unique(v()));
  BOOST_REQUIRE(gen.next());
}

struct Item4 : public crv_sequence_item {
  Item4(crv_object_name) {
    constraint={(v().size() == 10)};
    c1={(foreach(v(), v()[_i] <= 100))};
    c2={(foreach(v(), v()[_i] > 100))};
  }

  crv_vector<unsigned int> v;
  crv_constraint constraint{"constraint"};
  crv_constraint c1{"c1"};
  crv_constraint c2{"c2"};
};

BOOST_AUTO_TEST_CASE(constraint_management_test) {
  Item4 it{"item"};

  BOOST_REQUIRE(!it.randomize());
  it.c1.deactivate();
  BOOST_REQUIRE(it.randomize());
  for (uint i = 0; i < it.v.size(); i++) BOOST_REQUIRE_GT(it.v[i], 100);

  it.c1.activate();
  it.c2.deactivate();
  BOOST_REQUIRE(it.randomize());
  for (uint i = 0; i < it.v.size(); i++) BOOST_REQUIRE(it.v[i] <= 100);

  it.c2.activate();
  BOOST_REQUIRE(!it.randomize());
}

struct Item5 : public crv_sequence_item {
  Item5(crv_object_name) {
    constraint={(v().size() == 50),
    (foreach(v(), if_then(_i < 25, v()[_i] == _i))),
    (foreach(v(), if_then(_i == 25, v()[_i] == 0))),
    (foreach(v(), if_then(_i > 25, v()[_i] + _i == 200)))};
  }

  crv_vector<unsigned int> v;
   crv_constraint constraint{"constraint"};
};

BOOST_AUTO_TEST_CASE(index_constraint_test) {
  Item5 it{"item"};
  //it.constraint.printDotGraph(std::cout); compile failure
  it.randomize();
  BOOST_REQUIRE(it.v.size() == 50);
  for (uint i = 0; i < it.v.size(); i++) {
    if (i < 25)
      BOOST_REQUIRE(it.v[i] == i);
    else if (i > 25)
      BOOST_REQUIRE(it.v[i] + i == 200);
    else
      BOOST_REQUIRE(it.v[i] == 0);
  }
}

BOOST_AUTO_TEST_CASE(soft_vec_constraint) {
  crv_vector<unsigned int> v;

  Generator gen;
  gen(v().size() == 10);
  gen(foreach(v(), v()[_i] >= v()[_i - 1]));
  gen.soft(foreach(v(), v()[_i] < v()[_i - 1]));
  BOOST_REQUIRE(gen.next());

  Generator gen1;
  gen1(v().size() == 4);
  gen1(foreach(v(), v()[_i] >= v()[_i - 1]));
  gen1(foreach(v(), v()[_i] <= 1000));
  gen1.soft(foreach(v(), v()[_i] <= v()[_i - 1]));
  gen1.soft(foreach(v(), if_then(_i == 0, v()[_i] % 13 == 3)));
  BOOST_REQUIRE(gen1.next());
  for (int j = 0; j < 10; j++) {
    BOOST_REQUIRE_EQUAL(v.size(), 4);
    std::cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << std::endl;
    BOOST_CHECK_EQUAL(v[0], v[1]);
    BOOST_CHECK_EQUAL(v[1], v[2]);
    BOOST_CHECK_EQUAL(v[2], v[3]);
    BOOST_CHECK_EQUAL(v[0] % 13, 3);
    if (!gen1.next()) break;
  }
}

BOOST_AUTO_TEST_CASE(mixed_bv_width_1) {
  crv_vector<signed char> a;
  Generator gen;
  gen(a().size() == 28);
  gen(foreach(a(), a()[_i] < (short)-100));
  gen(unique(a()));

  BOOST_REQUIRE(gen.next());
  for (uint i = 0; i < a.size(); i++) {
    BOOST_REQUIRE_LT(a[i], -100);
    std::cout << " " << (int)a[i];
  }
  std::cout << std::endl;
  BOOST_REQUIRE(check_unique(a));
}

BOOST_AUTO_TEST_CASE(mixed_bv_width_2) {
  crv_vector<short> a;
  Generator gen;
  gen(a().size() == 19);
  gen(foreach(a(), a()[_i] < 10));
  gen(foreach(a(), a()[_i] > -10));
  gen(unique(a()));

  BOOST_REQUIRE(gen.next());
  for (uint i = 0; i < a.size(); i++) BOOST_REQUIRE(-10 < a[i] && a[i] < 10);
  BOOST_REQUIRE(check_unique(a));
}

BOOST_AUTO_TEST_CASE(mixed_bv_width_3) {
  crv_vector<int> a;
  Generator gen;
  gen(a().size() == 19);
  gen(foreach(a(), a()[_i] < (signed char)10));
  gen(foreach(a(), a()[_i] > (short)-10));
  gen(unique(a()));

  BOOST_REQUIRE(gen.next());
  for (uint i = 0; i < a.size(); i++) BOOST_REQUIRE(-10 < a[i] && a[i] < 10);
  BOOST_REQUIRE(check_unique(a));
}

BOOST_AUTO_TEST_CASE(mixed_bv_width_4) {
  crv_vector<short> a;
  Generator gen;
  gen(a().size() == 10);
  gen(foreach(a(), -3 <= a()[_i] && a()[_i] <= 3));
  gen(foreach(a(), a()[_i] != 0));
  gen(foreach(a(), a()[_i] * a()[_i - 1] % 6 == 0));
  gen(foreach(a(), _i != 0 || a()[_i] % 2 == 0));

  BOOST_REQUIRE(gen.next());
  for (uint i = 0; i < a.size(); i++) {
    BOOST_REQUIRE(-3 <= a[i] && a[i] <= 3);
    BOOST_REQUIRE(a[i] != 0);
    if (i == 0)
      BOOST_REQUIRE(a[i] % 2 == 0);
    else
      BOOST_REQUIRE(a[i] * a[i - 1] % 6 == 0);
  }
}

BOOST_AUTO_TEST_CASE(bool_rand_vec) {
  crv_vector<bool> a;
  Generator gen;
  gen(a().size() == 10);
  gen(foreach(a(), a()[_i] != a()[_i - 1]));

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(a.size() == 10);
  for (uint i = 1; i < a.size(); i++) BOOST_REQUIRE(a[i] != a[i - 1]);
}

BOOST_AUTO_TEST_SUITE_END()  // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
