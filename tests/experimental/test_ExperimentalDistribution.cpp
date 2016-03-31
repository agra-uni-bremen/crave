#include <boost/test/unit_test.hpp>

#include <crave/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

// using namespace std;
using namespace crave;

using boost::format;

BOOST_FIXTURE_TEST_SUITE(Distribution_t, Context_Fixture)

struct s_crv_variable_dist_t1 : public crv_sequence_item {
  s_crv_variable_dist_t1(crv_object_name) {}
  crv_variable<int> v;
  crv_constraint con = {
      dist(v(), make_distribution(range<int>(0, 5), range<int>(50, 65), range<int>(100, 125)))};
};

BOOST_AUTO_TEST_CASE(crv_variable_dist_t1) {
  s_crv_variable_dist_t1 item("item");
  std::map<int, int> s;
  int total = 100000;
  for (int i = 0; i < total; i++) {
    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE((0 <= item.v && item.v <= 5) || (50 <= item.v && item.v <= 65) || (100 <= item.v && item.v <= 125));
    ++s[item.v];
  }
  for (int i = 0; i <= 200; i++)
    if ((0 <= i && i <= 5) || (50 <= i && i <= 65) || (100 <= i && i <= 125))
      BOOST_REQUIRE(s.find(i) != s.end());
}

struct s_crv_variable_dist_t2 : public crv_sequence_item {
  s_crv_variable_dist_t2(crv_object_name) {}
  crv_variable<int> v;
  crv_constraint con;
};

BOOST_AUTO_TEST_CASE(crv_variable_dist_t2) {
  s_crv_variable_dist_t2 item("item");
  BOOST_REQUIRE_THROW(
      item.con = {dist(item.v(), make_distribution(range<int>(0, 10), range<int>(50, 75), range<int>(30, 51)))},
      std::runtime_error);
}

struct s_crv_variable_dist_t3 : public crv_sequence_item {
  s_crv_variable_dist_t3(crv_object_name) {}
  crv_variable<char> v;
  crv_constraint con = {dist(v(), make_distribution(weighted_range<char>(1, 5, 50), weighted_range<char>(10, 20, 20),
                        weighted_range<char>(-50, -50, 30)))};
};

BOOST_AUTO_TEST_CASE(crv_variable_dist_t3) {
  s_crv_variable_dist_t3 item("item");
  int cnt1 = 0, cnt2 = 0, cnt3 = 0;
  int total = 50000;
  for (int i = 0; i < total; i++) {
    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE((1 <= item.v && item.v <= 5) || (10 <= item.v && item.v <= 20) || (item.v == -50));
    if (1 <= item.v && item.v <= 5) cnt1++;
    if (10 <= item.v && item.v <= 20) cnt2++;
    if (item.v == -50) cnt3++;
  }
  BOOST_REQUIRE_LT(cnt2, cnt3);
  BOOST_REQUIRE_LT(cnt3, cnt1);
}

struct s_crv_variable_dist_t4 : public crv_sequence_item {
  s_crv_variable_dist_t4(crv_object_name) {}
  crv_variable<int> v;
  crv_constraint x = {dist(v(), make_distribution(range<int>(0, 10), range<int>(50, 75), range<int>(100, 200)))};
  crv_constraint y = {dist(v(), make_distribution(range<int>(5000, 6000)))};
};

BOOST_AUTO_TEST_CASE(crv_variable_dist_t4) {
  s_crv_variable_dist_t4 item("item");
  item.x.deactivate();
  int total = 10000;
  for (int i = 0; i < total; i++) {
    BOOST_REQUIRE(item.randomize());
    BOOST_REQUIRE(5000 <= item.v && item.v <= 6000);
  }
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

  BOOST_REQUIRE_LT(counter, 0);
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

  BOOST_REQUIRE_LT(counter, 280);
  BOOST_REQUIRE_GT(counter, -280);
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

  BOOST_REQUIRE_GT(counter, 0);
}

BOOST_AUTO_TEST_SUITE_END()  // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
