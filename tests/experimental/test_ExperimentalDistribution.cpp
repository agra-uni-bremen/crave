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
      dist(v(), distribution<int>::create(range<int>(0, 5))(range<int>(50, 65))(range<int>(100, 125)))};
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
  int min = s[0], max = s[0];
  for (int i = 1; i <= 200; i++)
    if (s.find(i) != s.end()) {
      if (s[i] < min) min = s[i];
      if (s[i] > max) max = s[i];
    }
  double avg = total / (6. + 16. + 26.);
  // allow 10% deviation
  BOOST_REQUIRE_LT(100. * (avg - min) / avg, 10);
  BOOST_REQUIRE_LT(100. * (max - avg) / avg, 10);
}

struct s_crv_variable_dist_t2 : public crv_sequence_item {
  s_crv_variable_dist_t2(crv_object_name) {}
  crv_variable<int> v;
  crv_constraint con;
};

BOOST_AUTO_TEST_CASE(crv_variable_dist_t2) {
  s_crv_variable_dist_t2 item("item");
  BOOST_CHECK_THROW(
      item.con = {dist(item.v(), distribution<int>::create(range<int>(0, 10))(range<int>(50, 75))(range<int>(30, 51)))},
      std::runtime_error);
}

struct s_crv_variable_dist_t3 : public crv_sequence_item {
  s_crv_variable_dist_t3(crv_object_name) {}
  crv_variable<char> v;
  crv_constraint con = {dist(v(), distribution<char>::create(weighted_range<char>(1, 5, 50))(
                                      weighted_range<char>(10, 20, 20))(weighted_range<char>(-50, -50, 30)))};
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
  double q = 50.0 / cnt1;
  BOOST_REQUIRE_LT(abs(cnt2 * q - 20), 0.2);
  BOOST_REQUIRE_LT(abs(cnt3 * q - 30), 0.2);
}

struct s_crv_variable_dist_t4 : public crv_sequence_item {
  s_crv_variable_dist_t4(crv_object_name) {}
  crv_variable<int> v;
  crv_constraint x = {
      dist(v(), distribution<int>::create(range<int>(0, 10))(range<int>(50, 75))(range<int>(100, 200)))};
  crv_constraint y = {dist(v(), distribution<int>::simple_range(5000, 6000))};
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

BOOST_AUTO_TEST_SUITE_END()  // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
