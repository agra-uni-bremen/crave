#define BOOST_TEST_MODULE SystemC_Data
#include <boost/test/unit_test.hpp>

#include <crave/ConstrainedRandom.hpp>
#include <crave/frontend/SystemC.hpp>

#include <systemc.h>

#include <boost/format.hpp>
#include <iostream>
using boost::format;

int sc_main( int argc, char** argv) { return 1; }

using namespace crave;

struct SystemC_Data_Fixture {
  SystemC_Data_Fixture () {
    crave::set_solver_backend("Boolector");
  }
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
  const unsigned w = 2;

  Variable< sc_uint<w> > i,j,k;

  Generator gen( i != j  && i+j == k);

  unsigned count = 0;

  while(gen.next()) {
    ++count;
    sc_uint<w> iv = gen[i];
    sc_uint<w> jv = gen[j];
    sc_uint<w> kv = gen[k];

    sc_uint<w> rv = iv+jv;

    BOOST_REQUIRE_NE(iv, jv);
    BOOST_REQUIRE_EQUAL(rv, kv);

    gen( i != iv || j != jv || k != kv);    
  }

  BOOST_REQUIRE_EQUAL( count, 12);
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

BOOST_AUTO_TEST_CASE( randv_sc_dt_t )
{
  using namespace sc_dt;

  randv< sc_uint<4> > a, b, c;
  randv< sc_uint<8> > d;

  Generator gen;
  gen(a() == 15);
  gen(b() == 15);
  gen(c() == a() + b());
  gen(d() == a() + b());

  BOOST_REQUIRE(gen.next());
  BOOST_CHECK_EQUAL( ++a, 0 ); // wrap around
  BOOST_CHECK_EQUAL( b + 1, 16 );  
  BOOST_CHECK_EQUAL( c, 14 );
  BOOST_CHECK_EQUAL( d, 14 ); // deviation to sc_uint semantics (no overflow, thus d == 30)
}

BOOST_AUTO_TEST_CASE( randv_sc_dt_op_support_t ) 
{
  using namespace sc_dt;

  randv< sc_uint<4> > a;
  randv< sc_int<4> > b;
  randv< sc_bv<4> > c;

  a = 1;
  b = a;  

  1 + a; 1 - a; 1 * a; 1 / a; 1 % a;
  1 == a; 1 != a; 1 < a; 1 <= a; 1 > a; 1 >= a;
  1 & a; 1 | a; 1 ^ a; ~a;
  a << 1; a >> 1; a << b; a >> b;

  1 + b; 1 - b; 1 * b; 1 / b; 1 % b;
  1 == b; 1 != b; 1 < b; 1 <= b; 1 > b; 1 >= b;
  1 & b; 1 | b; 1 ^ b; ~b;
  b << 1; b >> 1; b << a; b >> a;

  b == a; b != a; b < a; b <= a; b > a; b >= a;  
  a + b; a - b; a * b; a / b; a % b;
    
  1 == c; 1 != c;
  c == 1; c != 1;
}

BOOST_AUTO_TEST_SUITE_END() // SystemC_Data

//  vim: ft=cpp:ts=2:sw=2:expandtab
