#include <boost/test/unit_test.hpp>

#include <crave/experimental/Coverage.hpp>
#include <crave/experimental/Variable.hpp>

#include <vector>

using namespace crave;

BOOST_FIXTURE_TEST_SUITE(CoverageSampling, Context_Fixture)

template<typename T>
void sample_from_list(crv_covergroup& cg, crv_variable<T>& v, std::vector<T>&& values) {
  for (T val : values) {
    v = val;
    cg.sample();
  }
}

struct my_covergroup : public crv_covergroup {
  crv_variable<unsigned> a;
  crv_variable<unsigned> b;

  crv_coverpoint cp1{"coverpoint1"};
  crv_coverpoint cp2{"coverpoint2"};

  my_covergroup(crv_object_name) {
    cp1.simple_bins() = {crv_bin(a() == 0, 10), crv_bin(a() == 1, 10), crv_bin(a() == 2, 5)};
    cp2.simple_bins() = {crv_bin(a() == 0, 10), crv_bin(a() == 1, 10), crv_bin(a() == 2, 5)};
  }
};

BOOST_AUTO_TEST_CASE(basic_test) {
  my_covergroup cg{"covergroup1"};
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 5; j++) {
      cg.a = i % 3;
      cg.b = j * 11 % 20;
      cg.sample();
    }

  BOOST_REQUIRE(cg.covered());
}

struct my_transitionCovergroup_next : public crv_covergroup {
  crv_variable<unsigned> a;

  crv_coverpoint cp1{"coverpoint1"};

  my_transitionCovergroup_next(crv_object_name) {
      //1
      cp1.start_with(a() == 1);
      cp1.last_transition_bin().hit_minimum(4);
      //1 => 2
      cp1.start_with(a() == 1)->next(a() == 2);
      cp1.last_transition_bin().hit_minimum(1);
      //0 => 1,2[*2] => 0
      cp1.start_with(a() == 0)->next<2>(a() == 1 || a() == 2)->next(a() == 0);
      cp1.last_transition_bin().hit_minimum(1);
      //0 [*2:3] => 1
      cp1.start_with<2,3>(a() == 0)->next(a() == 1);
      cp1.last_transition_bin().hit_minimum(2);
  }
};

BOOST_AUTO_TEST_CASE(transition_next_test) {
  my_transitionCovergroup_next cg{"covergroup2"};

  sample_from_list(cg, cg.a, {0, 1, 0, 1, 2, 0, 0, 1, 0, 0, 0, 1});

  BOOST_REQUIRE(cg.covered());
}

struct my_transitionCovergroup_consNext : public crv_covergroup {
  crv_variable<unsigned> a;

  crv_coverpoint cp1{"coverpoint1"};

  my_transitionCovergroup_consNext(crv_object_name) {
      //1
      cp1.start_with_consecutive(a() == 1);
      cp1.last_transition_bin().hit_minimum(4);
      //1 [->3]
      cp1.start_with_consecutive<3>(a() == 1);
      cp1.last_transition_bin().hit_minimum(2);
      //0 [->2] => 1
      cp1.start_with_consecutive<2>(a() == 0)->next(a() == 1);
      cp1.last_transition_bin().hit_minimum(3);
      //0 => 1[->2:4] => 0
      cp1.start_with(a() == 0)->consecutive_next<2,4>(a() == 1)->next(a() == 0);
      cp1.last_transition_bin().hit_minimum(1);
  }
};

BOOST_AUTO_TEST_CASE(transition_cons_next_test) {
  my_transitionCovergroup_consNext cg{"covergroup3"};

  sample_from_list(cg, cg.a, {0, 1, 0, 1, 2, 0, 0, 1, 0, 0, 0, 1});

  BOOST_REQUIRE(cg.covered());
}

struct my_transitionCovergroup_nconsNext : public crv_covergroup {
  crv_variable<unsigned> a;

  crv_coverpoint cp1{"coverpoint1"};

  my_transitionCovergroup_nconsNext(crv_object_name) {
      //1
      cp1.start_with_nonconsecutive(a() == 1);
      cp1.last_transition_bin().hit_minimum(4);
      //1 [= 3]
      cp1.start_with_nonconsecutive<3>(a() == 1);
      cp1.last_transition_bin().hit_minimum(2);
      //0 [= 2] => 2
      cp1.start_with_nonconsecutive<2>(a() == 0)->next(a() == 2);
      cp1.last_transition_bin().hit_minimum(1);
      //0 => 1[= 2:4] => 0
      cp1.start_with(a() == 0)->nonconsecutive_next<2,4>(a() == 1)->next(a() == 0);
      cp1.last_transition_bin().hit_minimum(2);
  }
};

BOOST_AUTO_TEST_CASE(transition_ncons_next_test) {
  my_transitionCovergroup_nconsNext cg{"covergroup4"};

  sample_from_list(cg, cg.a, {0, 1, 0, 1, 2, 0, 0, 1, 0, 0, 0, 1});

  BOOST_REQUIRE(cg.covered());
}

struct my_transitionCovergroup_overlap : public crv_covergroup {
  crv_variable<unsigned> a;

  crv_coverpoint cp1{"coverpoint1"};

  my_transitionCovergroup_overlap(crv_object_name) {
      //0 => 0
      cp1.start_with<2>(a() == 0);
      cp1.last_transition_bin().hit_minimum(6);
      //2 => 0[1:2]
      cp1.start_with(a() == 2)->next<1,2>(a()==0);
      cp1.last_transition_bin().hit_minimum(2);
      //0 => 1[->2]
      cp1.start_with(a() == 0)->consecutive_next<2>(a() == 1);
      cp1.last_transition_bin().hit_minimum(1);
      //0[*4] => 2 => 0[*2] => 1 => 0[*3] => 1
      cp1.start_with<4>(a() == 0)->next(a() == 2)->next<2>(a() == 0)->next(a()==1)->next<3>(a() == 0)->next(a()==1);
      cp1.last_transition_bin().hit_minimum(1);
  }
};

BOOST_AUTO_TEST_CASE(transition_overlap_test) {
  my_transitionCovergroup_overlap cg{"covergroup5"};

  sample_from_list(cg, cg.a, {0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 1});

  BOOST_REQUIRE(cg.covered());
}

BOOST_AUTO_TEST_SUITE_END()  // CoverageSampling

//  vim: ft=cpp:ts=2:sw=2:expandtab
