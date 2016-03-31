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
  Item(crv_object_name) {}

  crv_vector<unsigned int> v;
  crv_constraint constraint = {30 <= v().size() && v().size() <= 50,
                               foreach (v(), v()[_i] == v()[_i - 1] + v()[_i - 2]),
                               foreach (v(), if_then(_i <= 1, v()[_i] == _i))};
};

BOOST_AUTO_TEST_CASE(fibo_test) {
  Item it{"item"};
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE_LE(30, it.v.size());
  BOOST_REQUIRE_LE(it.v.size(), 50);
  BOOST_REQUIRE_EQUAL(it.v[0], 0);
  BOOST_REQUIRE_EQUAL(it.v[1], 1);
  for (uint i = 2; i < it.v.size(); i++) BOOST_REQUIRE_EQUAL(it.v[i], it.v[i - 1] + it.v[i - 2]);
}

struct Item1 : public crv_sequence_item {
  Item1(crv_object_name) {}

  crv_vector<unsigned int> v;
};

BOOST_AUTO_TEST_CASE(free_vector_test) {
  Item1 it{"item"};
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(it.v.size() > 0);
  for (uint i = 0; i < it.v.size(); i++) std::cout << it.v[i] << " ";
  std::cout << std::endl;
}

struct Item2 : public crv_sequence_item {
  Item2(crv_object_name) {}

  crv_vector<unsigned int> v;
  crv_constraint constraint = {foreach (v(), 100 <= v()[_i] && v()[_i] <= 200)};
};

BOOST_AUTO_TEST_CASE(default_size_test) {
  Item2 it{"item"};
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(5 <= it.v.size() && it.v.size() <= 10);
  for (uint i = 0; i < it.v.size(); i++) {
    std::cout << it.v[i] << " ";
    BOOST_REQUIRE(100 <= it.v[i] && it.v[i] <= 200);
  }
  std::cout << std::endl;
}

struct Item3 : public crv_sequence_item {
  Item3(crv_object_name) {}

  crv_vector<int> v;
  crv_constraint constraint = {(v().size() == 100), (foreach (v(), v()[_i] < 100)), (foreach (v(), v()[_i] >= 0)),
                               (unique(v()))};
};

BOOST_AUTO_TEST_CASE(unique_test_1) {
  Item3 it{"item"};
  BOOST_REQUIRE(it.randomize());
  BOOST_REQUIRE(it.v.size() == 100);
  for (uint i = 0; i < it.v.size(); i++) {
    BOOST_REQUIRE(0 <= it.v[i] && it.v[i] < 100);
    for (uint j = 0; j < i; j++) BOOST_REQUIRE(it.v[i] != it.v[j]);
  }
}

struct s_unique_test_2 : crv_sequence_item {
  s_unique_test_2(crv_object_name) {}
  crv_vector<unsigned int> v;
  crv_constraint con = {v().size() == 7, foreach (v(), v()[_i] < 6)};
  crv_constraint con2 = {unique(v())};
};

BOOST_AUTO_TEST_CASE(unique_test_2) {
  s_unique_test_2 item("item");
  BOOST_REQUIRE(!item.randomize());
  item.con2.deactivate();
  BOOST_REQUIRE(item.randomize());
  BOOST_REQUIRE(item.v.size() == 7);
  for (uint i = 0; i < item.v.size(); i++) {
    std::cout << item.v[i] << " ";
    BOOST_REQUIRE(item.v[i] < 6);
  }
  std::cout << std::endl;
}

struct s_soft_unique_test : crv_sequence_item {
  s_soft_unique_test(crv_object_name) {}
  crv_vector<unsigned int> v;
  crv_constraint con = {v().size() == 7, foreach (v(), v()[_i] < 6)};
  crv_soft_constraint con2 = {unique(v())};
};

BOOST_AUTO_TEST_CASE(soft_unique_test) {
  s_soft_unique_test item("item");
  BOOST_REQUIRE(item.randomize());
}

struct Item4 : public crv_sequence_item {
  Item4(crv_object_name) {}

  crv_vector<unsigned int> v;
  crv_constraint constraint = {(v().size() == 10)};
  crv_constraint c1 = {(foreach (v(), v()[_i] <= 100))};
  crv_constraint c2 = {(foreach (v(), v()[_i] > 100))};
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
  Item5(crv_object_name) {}

  crv_vector<unsigned int> v;
  crv_constraint constraint = {(v().size() == 50), (foreach (v(), if_then(_i < 25, v()[_i] == _i))),
                               (foreach (v(), if_then(_i == 25, v()[_i] == 0))),
                               (foreach (v(), if_then(_i > 25, v()[_i] + _i == 200)))};
};

BOOST_AUTO_TEST_CASE(index_constraint_test) {
  Item5 it{"item"};
  BOOST_REQUIRE(it.randomize());
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

struct s_soft_vec_constraint : public crv_sequence_item {
  s_soft_vec_constraint(crv_object_name) {}
  crv_constraint con;
  crv_soft_constraint con2;
};

BOOST_AUTO_TEST_CASE(soft_vec_constraint) {
  crv_vector<unsigned int> v;
  s_soft_vec_constraint item1("item");
  s_soft_vec_constraint item2("item");

  item1.con &= {v().size() == 10, foreach (v(), v()[_i] >= v()[_i - 1])};
  item1.con2 &= {foreach (v(), v()[_i] < v()[_i - 1])};
  item2.con &= {v().size() == 4, foreach (v(), v()[_i] >= v()[_i - 1]), foreach (v(), v()[_i] <= 1000)};
  item2.con2 &= {foreach (v(), v()[_i] <= v()[_i - 1]), foreach (v(), if_then(_i == 0, v()[_i] % 13 == 3))};
  BOOST_REQUIRE(item1.randomize());
  BOOST_REQUIRE(item2.randomize());

  for (int j = 0; j < 10; j++) {
    BOOST_REQUIRE_EQUAL(v.size(), 4);
    std::cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << std::endl;
    BOOST_REQUIRE_EQUAL(v[0], v[1]);
    BOOST_REQUIRE_EQUAL(v[1], v[2]);
    BOOST_REQUIRE_EQUAL(v[2], v[3]);
    BOOST_REQUIRE_EQUAL(v[0] % 13, 3);
    if (!item2.randomize()) break;
  }
}

struct s_mixed_bv_width_1 : public crv_sequence_item {
  s_mixed_bv_width_1(crv_object_name) {}
  crv_vector<signed char> a;
  crv_constraint con = {a().size() == 28, foreach (a(), a()[_i] < (short)-100), unique(a())};
};

BOOST_AUTO_TEST_CASE(mixed_bv_width_1) {
  s_mixed_bv_width_1 item("item");
  BOOST_REQUIRE(item.randomize());
  for (uint i = 0; i < item.a.size(); i++) {
    BOOST_REQUIRE_LT(item.a[i], -100);
    std::cout << " " << (int)item.a[i];
  }
  std::cout << std::endl;
  BOOST_REQUIRE(check_unique(item.a));
}

struct s_mixed_bv_width_2 : public crv_sequence_item {
  s_mixed_bv_width_2(crv_object_name) {}
  crv_vector<short> a;
  crv_constraint con = {a().size() == 19, foreach (a(), a()[_i] < 10), foreach (a(), a()[_i] > -10), unique(a())};
};

BOOST_AUTO_TEST_CASE(mixed_bv_width_2) {
  s_mixed_bv_width_2 item("item");
  BOOST_REQUIRE(item.randomize());
  for (uint i = 0; i < item.a.size(); i++) BOOST_REQUIRE(-10 < item.a[i] && item.a[i] < 10);
  BOOST_REQUIRE(check_unique(item.a));
}

struct s_mixed_bv_width_3 : public crv_sequence_item {
  s_mixed_bv_width_3(crv_object_name) {}
  crv_vector<int> a;
  crv_constraint con = {a().size() == 19, foreach (a(), a()[_i] < (signed char)10), foreach (a(), a()[_i] > (short)-10),
                        unique(a())};
};

BOOST_AUTO_TEST_CASE(mixed_bv_width_3) {
  s_mixed_bv_width_3 item("item");
  BOOST_REQUIRE(item.randomize());
  for (uint i = 0; i < item.a.size(); i++) BOOST_REQUIRE(-10 < item.a[i] && item.a[i] < 10);
  BOOST_REQUIRE(check_unique(item.a));
}

struct s_mixed_bv_width_4 : public crv_sequence_item {
  s_mixed_bv_width_4(crv_object_name) {}
  crv_vector<short> a;
  crv_constraint con = {a().size() == 10, foreach (a(), -3 <= a()[_i] && a()[_i] <= 3), foreach (a(), a()[_i] != 0),
                        foreach (a(), a()[_i] * a()[_i - 1] % 6 == 0), foreach (a(), _i != 0 || a()[_i] % 2 == 0)};
};

BOOST_AUTO_TEST_CASE(mixed_bv_width_4) {
  s_mixed_bv_width_4 item("item");
  BOOST_REQUIRE(item.randomize());
  for (uint i = 0; i < item.a.size(); i++) {
    BOOST_REQUIRE(-3 <= item.a[i] && item.a[i] <= 3);
    BOOST_REQUIRE(item.a[i] != 0);
    if (i == 0)
      BOOST_REQUIRE(item.a[i] % 2 == 0);
    else
      BOOST_REQUIRE(item.a[i] * item.a[i - 1] % 6 == 0);
  }
}

struct s_bool_rand_vec : public crv_sequence_item {
  s_bool_rand_vec(crv_object_name) {}
  crv_vector<bool> a;
  crv_constraint con = {a().size() == 10, foreach (a(), a()[_i] != a()[_i - 1])};
};

BOOST_AUTO_TEST_CASE(bool_rand_vec) {
  s_bool_rand_vec item("item");
  BOOST_REQUIRE(item.randomize());
  BOOST_REQUIRE(item.a.size() == 10);
  for (uint i = 1; i < item.a.size(); i++) BOOST_REQUIRE(item.a[i] != item.a[i - 1]);
}

BOOST_AUTO_TEST_SUITE_END()  // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
