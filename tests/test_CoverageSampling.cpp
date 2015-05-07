#include <boost/test/unit_test.hpp>

#include <crave/experimental/Coverage.hpp>
#include <crave/experimental/Variable.hpp>

#include <vector>

using namespace crave;

BOOST_FIXTURE_TEST_SUITE(CoverageSampling, Context_Fixture)

struct my_covergroup : public crv_covergroup {
  crv_variable<unsigned> a{"a"};
  crv_variable<unsigned> b{"b"};

  crv_coverpoint cp1{"coverpoint1"};
  crv_coverpoint cp2{"coverpoint2"};

  my_covergroup(crv_object_name) {
    cp1.bins() = {crv_bin(a() == 0, 10), crv_bin(a() == 1, 10), crv_bin(a() == 2, 5)};
    cp2.bins() = {crv_bin(a() == 0, 10), crv_bin(a() == 1, 10), crv_bin(a() == 2, 5)};
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

  BOOST_CHECK(cg.covered());

  crv_object::root()->print_object_hierarchy();
}

BOOST_AUTO_TEST_SUITE_END()  // CoverageSampling

//  vim: ft=cpp:ts=2:sw=2:expandtab
