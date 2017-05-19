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

struct my_transitionCovergroup_consec : public crv_covergroup {
  crv_variable<unsigned> a;

  crv_coverpoint cp1{"coverpoint1"};

  my_transitionCovergroup_consec(crv_object_name) {
      //1
      cp1.start_with(a() == 1);
      cp1.last_transition_bin().hit_minimum(4);
      //1 => 2
      cp1.start_with(a() == 1)->next(a() == 2);
      cp1.last_transition_bin().hit_minimum(1);
      //0 => 1,2[*2] => 0
      cp1.start_with(a() == 0)->consecutive_next<2>(a() == 1 || a() == 2)->next(a() == 0);
      cp1.last_transition_bin().hit_minimum(1);
      //0 [*2:3] => 1
      cp1.start_with_consecutive<2,3>(a() == 0)->next(a() == 1);
      cp1.last_transition_bin().hit_minimum(2);
  }
};

BOOST_AUTO_TEST_CASE(transition_consec_test) {
  my_transitionCovergroup_consec cg{"covergroup2"};

  sample_from_list(cg, cg.a, {0, 1, 0, 1, 2, 0, 0, 1, 0, 0, 0, 1});

  BOOST_REQUIRE(cg.covered());
}

struct my_transitionCovergroup_goto : public crv_covergroup {
  crv_variable<unsigned> a;

  crv_coverpoint cp1{"coverpoint1"};

  my_transitionCovergroup_goto(crv_object_name) {
      //1
      cp1.start_with(a() == 1);
      cp1.last_transition_bin().hit_minimum(4);
      //1 [->3]
      cp1.start_with_goto<3>(a() == 1);
      cp1.last_transition_bin().hit_minimum(2);
      //0 [->2] => 1
      cp1.start_with_goto<2>(a() == 0)->next(a() == 1);
      cp1.last_transition_bin().hit_minimum(3);
      //0 => 1[->2:4] => 0
      cp1.start_with(a() == 0)->goto_next<2,4>(a() == 1)->next(a() == 0);
      cp1.last_transition_bin().hit_minimum(1);
  }
};

BOOST_AUTO_TEST_CASE(transition_goto_test) {
  my_transitionCovergroup_goto cg{"covergroup3"};

  sample_from_list(cg, cg.a, {0, 1, 0, 1, 2, 0, 0, 1, 0, 0, 0, 1});

  BOOST_REQUIRE(cg.covered());
}

struct my_transitionCovergroup_ncons : public crv_covergroup {
  crv_variable<unsigned> a;

  crv_coverpoint cp1{"coverpoint1"};

  my_transitionCovergroup_ncons(crv_object_name) {
      //1
      cp1.start_with(a() == 1);
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

BOOST_AUTO_TEST_CASE(transition_ncons_test) {
  my_transitionCovergroup_ncons cg{"covergroup4"};

  sample_from_list(cg, cg.a, {0, 1, 0, 1, 2, 0, 0, 1, 0, 0, 0, 1});

  BOOST_REQUIRE(cg.covered());
}

struct my_transitionCovergroup_overlap : public crv_covergroup {
  crv_variable<unsigned> a;

  crv_coverpoint cp1{"coverpoint1"};

  my_transitionCovergroup_overlap(crv_object_name) {
      //0 => 0
      cp1.start_with_consecutive<2>(a() == 0);
      cp1.last_transition_bin().hit_minimum(6);
      //2 => 0[1:2]
      cp1.start_with(a() == 2)->consecutive_next<1,2>(a()==0);
      cp1.last_transition_bin().hit_minimum(2);
      //0 => 1[->2]
      cp1.start_with(a() == 0)->goto_next<2>(a() == 1);
      cp1.last_transition_bin().hit_minimum(1);
      //0[*4] => 2 => 0[*2] => 1 => 0[*3] => 1
      cp1.start_with_consecutive<4>(a() == 0)->next(a() == 2)->consecutive_next<2>(a() == 0)->next(a()==1)->consecutive_next<3>(a() == 0)->next(a()==1);
      cp1.last_transition_bin().hit_minimum(1);
  }
};

BOOST_AUTO_TEST_CASE(transition_overlap_test) {
  my_transitionCovergroup_overlap cg{"covergroup5"};

  sample_from_list(cg, cg.a, {0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 1});

  BOOST_REQUIRE(cg.covered());
}

struct ieee1800_2012_p530_covergroup : public crv_covergroup {
  crv_variable<unsigned> a;

  crv_coverpoint cp1{"coverpoint1"};

  ieee1800_2012_p530_covergroup(crv_object_name) {
      // b2 = 2 [-> 3:5]
      // 1, 4, 3, [[2, 3, 3, 2, 2], 3, 2], 3, 1, 5, 5, 5, 5, 5, 5 (2 Hits)
      cp1.start_with_goto<3, 5>(a() == 2);
      cp1.last_transition_bin().hit_minimum(2);
      // b3 = 3 [-> 3:5]
      // 1, 4, [[[3, 2, 3, 3], 2, 2, 3], 2, 3], 1, 5, 5, 5, 5, 5, 5 (3 Hits)
      cp1.start_with_goto<3, 5>(a() == 3);
      cp1.last_transition_bin().hit_minimum(3);
      // b5 = 5 [* 3]
      // 1, 4, 3, 2, 3, 3, 2, 2, 3, 2, 3, 1, [5, (5, {5], [5), 5}, 5] (4 Hits)
      cp1.start_with_consecutive<3>(a() == 5);
      cp1.last_transition_bin().hit_minimum(4);
      // b6 = 1 => 3 [-> 4:6] => 1
      // [1, 4, 3, 2, 3, 3, 2, 2, 3, 2, 3, 1], 5, 5, 5, 5, 5, 5 (1 Hit)
      cp1.start_with(a() == 1)->goto_next<4, 6>(a() == 3)->next(a() == 1);
      cp1.last_transition_bin().hit_minimum(1);
      // b7 = 1 => 2 [= 3:6] => 5
      // [1, 4, 3, 2, 3, 3, 2, 2, 3, 2, 3, 1, 5], 5, 5, 5, 5, 5 (1 Hit)
      cp1.start_with(a() == 1)->nonconsecutive_next<3, 6>(a() == 2)->next(a() == 5);
      cp1.last_transition_bin().hit_minimum(1);
  }
};

BOOST_AUTO_TEST_CASE(ieee1800_2012_p530_covergroup_test) {
  ieee1800_2012_p530_covergroup cg{"covergroup5"};

  sample_from_list(cg, cg.a, {1, 4, 3, 2, 3, 3, 2, 2, 3, 2, 3, 1, 5, 5, 5, 5, 5, 5});

  cg.report();

  BOOST_REQUIRE(cg.covered());
}


BOOST_AUTO_TEST_SUITE_END()  // CoverageSampling

//  vim: ft=cpp:ts=2:sw=2:expandtab
