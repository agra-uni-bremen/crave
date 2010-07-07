#define BOOST_TEST_MODULE Inheritence_t
#include <boost/test/unit_test.hpp>

#include <platzhalter/Constraint.hpp>
#include <platzhalter/Context.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

//using namespace std;
using boost::format;
using namespace platzhalter;

class Constraint_base : public Generator<> {
  public:
    Constraint_base() 
      : Generator<>()
      , constraint(*this)
      , soft_constraint( constraint )
    {}

  protected:
    Generator<> & constraint;
    Soft_Generator<> soft_constraint;
};


class Constraint1 : public Constraint_base {
  public:
    Variable<unsigned> x;
    
    Constraint1 () {
      constraint( x<10 );
    }
};

class Constraint2 : public Constraint1 {
  public:
    Constraint2 () {
      constraint( x>6 );
    }
};

struct Context_Fixture {};

BOOST_FIXTURE_TEST_SUITE(Inheritence_t, Context_Fixture )

BOOST_AUTO_TEST_CASE( t1 )
{
  Constraint2 c2;

  c2();
  unsigned r = c2[c2.x];
  BOOST_REQUIRE_LT( r, 10);
  BOOST_REQUIRE_GT( r,  6);

  c2( c2.x != r )();
  r = c2[c2.x];
  BOOST_REQUIRE_LT( r, 10);
  BOOST_REQUIRE_GT( r,  6);

  c2( c2.x != r )();
  r = c2[c2.x];
  BOOST_REQUIRE_LT( r, 10);
  BOOST_REQUIRE_GT( r,  6);

  c2( c2.x != r );
  BOOST_REQUIRE( ! c2.next() );
}


BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
