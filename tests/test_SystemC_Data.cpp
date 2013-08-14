#define BOOST_TEST_MODULE SystemC_Data
#include <boost/test/unit_test.hpp>

#include <crave/ConstrainedRandom.hpp>
#include <crave/SystemC.hpp>

#include <systemc.h>

#include <boost/format.hpp>
#include <iostream>
using boost::format;

int sc_main( int argc, char** argv) { return 1; }

//using namespace std;
using namespace crave;

class SystemC_Data_Fixture {
  protected:
};

BOOST_FIXTURE_TEST_SUITE(SystemC_Data, SystemC_Data_Fixture )

BOOST_AUTO_TEST_CASE( sc_bv_t )
{
  using namespace sc_dt;
  const unsigned w = 4;

  Variable< sc_bv<w> > i,j;

  Generator gen( i != j );
  gen();

  sc_bv<w> iv = gen[i];
  sc_bv<w> jv = gen[j];

  BOOST_CHECK_NE(iv, jv);
}

BOOST_AUTO_TEST_CASE( sc_uint_t )
{
  using namespace sc_dt;
  const unsigned w = 4;

  Variable< sc_uint<w> > i,j,k;

  Generator gen( i != j  && i+j == k);

  unsigned count = 0;

  while(gen.next()) {
    ++count;
    sc_uint<w> iv = gen[i];
    sc_uint<w> jv = gen[j];
    sc_uint<w> kv = gen[k];

  //std::cout << format("iv: %d, jv: %d, kv: %d\n") %iv%jv%kv;
    sc_uint<w> rv = iv+jv;


    BOOST_REQUIRE_NE(iv, jv);
    BOOST_REQUIRE_EQUAL(rv, kv);

    gen( i != iv || j != jv || k != kv);
  }

  BOOST_REQUIRE_EQUAL( count, 240);
}

BOOST_AUTO_TEST_CASE( sc_int_t )
{
  using namespace sc_dt;
  const unsigned w = 4;

  Variable< sc_int<w> > i,j,k;

  Generator gen( i < j &&  i-j == k );
  gen();

  sc_int<w> iv = gen[i];
  sc_int<w> jv = gen[j];
  sc_int<w> kv = gen[k];

  sc_int<w> rv = iv-jv;

  //std::cout << format("iv: %d, jv: %d, kv: %d\n") %iv%jv%kv;

  BOOST_CHECK_LT(iv, jv);
  BOOST_CHECK_EQUAL( rv, kv );
}

BOOST_AUTO_TEST_SUITE_END() // SystemC_Data

//  vim: ft=cpp:ts=2:sw=2:expandtab
