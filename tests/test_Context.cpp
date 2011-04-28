#define BOOST_TEST_MODULE Context_t
#include <boost/test/unit_test.hpp>

#include <platzhalter/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

//using namespace std;
using namespace platzhalter;

using boost::format;

struct Context_Fixture {
  Context_Fixture ()
  : ctx(get_context())
  {
  }
  protected:
    Context ctx;
};

BOOST_FIXTURE_TEST_SUITE(Context_t, Context_Fixture )

BOOST_AUTO_TEST_CASE ( multiple_solver_instances )
{
  Generator<> gen1, gen2;
  Variable<int> r1, r2;
  gen1(r1 < 6);
  gen2(r2 < 6);
  gen1.next();
  gen2.next();
  
}

BOOST_AUTO_TEST_CASE( constants )
{
  Variable<unsigned> x;
  Generator<> gen( x == 135421 );
  gen();
  BOOST_CHECK_EQUAL( gen[x], 135421 );
}

BOOST_AUTO_TEST_CASE( boolean )
{
  Variable<bool> b;

  Generator<> gen;  
  gen(b == b)(); // create a new assignment

  bool b1 = gen[b];
  //printf("result: b1=%d\n", b1);
  gen(b != b1)();
  bool b2 = gen[b];
  //printf("result: b2=%d\n", b2);

  BOOST_CHECK_THROW ( gen( b != b2 )(), std::runtime_error );
}

BOOST_AUTO_TEST_CASE( less )
{
  Variable<unsigned> a;

  Generator<> gen;  
  gen(a < 256);

  std::set<unsigned> generated;
  while (gen.next()) {
    unsigned av = gen[a];
    generated.insert(av);
    gen( a != av );
  }

  BOOST_CHECK_EQUAL( generated.size(), 256);
}

BOOST_AUTO_TEST_CASE( less_equal )
{
  Variable<unsigned> a;

  Generator<> gen;  
  gen(a <= 256);

  std::set<unsigned> generated;
  while (gen.next()) {
    unsigned av = gen[a];
    generated.insert(av);
    gen( a != av );
  }

  BOOST_CHECK_EQUAL( generated.size(), 257);
}

BOOST_AUTO_TEST_CASE( greater )
{
  Variable<unsigned> a;

  Generator<> gen;  
  gen(a > (std::numeric_limits<unsigned>::max()-256) );

  std::set<unsigned> generated;
  while (gen.next()) {
    unsigned av = gen[a];
    generated.insert(av);
    gen( a != av );
  }

  BOOST_CHECK_EQUAL( generated.size(), 256);
}

BOOST_AUTO_TEST_CASE( greater_equal )
{
  Variable<unsigned> a;

  Generator<> gen;  
  gen(a >= (std::numeric_limits<unsigned>::max()-256) );

  std::set<unsigned> generated;
  while (gen.next()) {
    unsigned av = gen[a];
    generated.insert(av);
    gen( a != av );
  }

  BOOST_CHECK_EQUAL( generated.size(), 257);
}

BOOST_AUTO_TEST_CASE( divide ) 
{
  Variable<unsigned char> a;
  Variable<unsigned char> b;
  Variable<unsigned char> q;
  Variable<unsigned char> r;

  Generator<> gen;
  gen
    ( b != (unsigned char)  0 )
    ( a  < (unsigned char) 16 )
    ( b  < (unsigned char) 16 )
    ( q == a/b )
    ( r == a%b )
  ;

  while( gen.next() ) {
    gen( a != gen[a] || b != gen[b] );
    //printf("result: a=%d, b=%d, q=%d, r=%d\n", gen[a], gen[b], gen[q], gen[r]);
    BOOST_CHECK_EQUAL( gen[a]/gen[b], gen[q] );
    BOOST_CHECK_EQUAL( gen[a]%gen[b], gen[r] );
  }
}

BOOST_AUTO_TEST_CASE ( shiftleft )
{
  Variable<unsigned> a;
  Variable<unsigned> b;
  Variable<unsigned> c;

  Generator<> gen;
  gen
    ( a <  256 )
    ( b <  (unsigned) (sizeof(unsigned)*8) )
    ( c == (a << b) )
  ;

  while( gen.next() ) {
    unsigned av = gen[a];
    unsigned bv = gen[b];
    unsigned r  = av << bv;

    gen( a != gen[a] || b != gen[b] );
    //std::cout << format("result: a=%d, b=%d, r=%d,  c=%d\n") % gen[a]% gen[b] %r % gen[c];
    BOOST_REQUIRE_EQUAL( r, gen[c] );
  }
}

/**
 * temporaly fix a variable to a certain value using the assign operator
 **/
BOOST_AUTO_TEST_CASE ( fix_variable )
{
  Variable<unsigned> a;
  Variable<unsigned> b;

  Generator<> gen (a < b);
  gen ( b = 7 ) ;

  unsigned c = 0;
  while( gen.next() ) {
    ++c;
    unsigned av = gen[a];
    unsigned bv = gen[b];
    BOOST_REQUIRE_LT( av, 7 );
    BOOST_REQUIRE_EQUAL( bv, 7);
    gen( a != gen[a] );
    if (c > 10) break;
  }
  // found 7 solutions
  BOOST_REQUIRE_EQUAL( c, 7);

  //display_expr( b =  randomize );
  gen( b = randomize );

  gen();
}

BOOST_AUTO_TEST_CASE ( by_reference )
{
  unsigned b=0;
  Variable<unsigned> a;

  Generator<> gen (a == reference(b) );

  while( gen.next() ) {
    unsigned av = gen[a];
    BOOST_REQUIRE_EQUAL( av, b);
    ++b;
    if (b > 10) break;
  }
}

/**
 * temporaly fix a variable to a certain value using the assign operator
 **/
BOOST_AUTO_TEST_CASE ( named_reference )
{
  unsigned bv=0;
  Variable<unsigned> a, c;
  ReadReference<unsigned> b(bv);

  Generator<> gen (a == b);
  gen(c != b);

  while( gen.next() ) {
    unsigned av = gen[a];
    unsigned cv = gen[c];
    BOOST_REQUIRE_EQUAL( av, bv);
    BOOST_REQUIRE_NE   ( cv, bv);
    ++bv;
    if (bv > 10) break;
  }
}

BOOST_AUTO_TEST_CASE ( soft_constraint_t )
{
  Generator<> gen;
  Variable<int> r;
  gen( r<6 );
  soft(gen)( r == 2 );
  
  BOOST_REQUIRE( gen.next() );
  BOOST_REQUIRE_EQUAL(gen[r], 2);

  gen(soft)( r==3 );

  BOOST_REQUIRE( gen.next() );
}

BOOST_AUTO_TEST_CASE ( randv_test )
{
  Generator<> gen;
  randv<int> a(NULL);
  randv<int> b(NULL);
  std::cout << "init: a = " <<  a << ", b = " << b << std::endl;
  gen
    (4 <= a() && a() <= 6)
    (9 <= a() + b() && a() + b() <= 11)
    (b() % 2 == 0);
  unsigned count=0;
  while( gen.next() ) {
    ++count;
    std::cout << "result: a = " <<  a << ", b = " << b << std::endl;
    gen(a() != a || b() != b);
  }
  BOOST_CHECK_EQUAL(count, 4);
}

BOOST_AUTO_TEST_CASE ( randv_var_ref_mixed_test )
{
  Generator<> gen;
  randv<int> a(0);
  Variable<int> b;
  gen
    (4 <= a() && a() <= 6)
    (9 <= a() + b && a() + b <= 11)
    (b % 2 == 0);
  unsigned count=0;
  while( gen.next()) {
    ++count;
    std::cout << "result: a = " <<  a << ", b = " << gen[b] << std::endl;
    gen(a() != a || b != gen[b]);
  }
  BOOST_CHECK_EQUAL(count, 4);
}

BOOST_AUTO_TEST_CASE( alu )
{
  Variable<unsigned> op;
  Variable<unsigned> a;
  Variable<unsigned> b;

  Generator<> gen;  
  gen
    ( a < 16)
    ( b < 16)
    (op < 4 )  // 4 opcodes
    (op != 0 || a + b < 16 )  // no add overflow
    (op != 1 || a > b      )  // no sub underflow
    (op != 2 || a * b < 16 )  // no m overflow
    (op != 3 || b != 0     )  // div valid
  ;
  gen(); // create a new assignment

  //printf("result: op=%d, a=%d, b=%d\n", gen[op], gen[a], gen[b]);
}

BOOST_AUTO_TEST_CASE( alu_enum )
{
  Variable<unsigned> op;
  Variable<unsigned> a;
  Variable<unsigned> b;

  Generator<> gen;  
  gen
    ( a < 16)
    ( b < 16)
    (op < 4 )  // 4 opcodes
    (op != 0 || a + b < 16 )  // no add overflow
    (op != 1 || a > b      )  // no sub underflow
    (op != 2 || a * b < 16 )  // no m overflow
    (op != 3 || b != 0     )  // div valid
  ;

  unsigned count=0;
  while( gen.next() ) {
    ++count;
    gen( op != gen[op] || a != gen[a] || b != gen[b] );
    //printf("result: op=%d, a=%d, b=%d\n", gen[op], gen[a], gen[b]);
  }
  BOOST_CHECK_EQUAL(count, 572);
}

BOOST_AUTO_TEST_CASE( pythagoras )
{
  Variable<unsigned long long> a;
  Variable<unsigned long long> b;
  Variable<unsigned long long> c;

  Generator<> gen;  
  gen(a*a + b*b == c*c)(); // create a new assignment

  unsigned long long av = gen[a];
  unsigned long long bv = gen[b];
  unsigned long long cv = gen[c];

  BOOST_CHECK_EQUAL( av*av + bv*bv, cv*cv);
}

BOOST_AUTO_TEST_CASE ( signed_int_t )
{
  randv<int> a(NULL);
  randv<int> b(NULL);

  Generator<> gen;
  gen
    ( a() + b() >= 120 )
  ;

  gen.next();

  std::cout << format("result: a=%d, b=%d\n") % a % b;

  BOOST_CHECK(a + b >= 120);
}


BOOST_AUTO_TEST_CASE ( mixed_bv_width )
{
  randv<long> a(NULL);
  randv<short> b(NULL);

  Generator<> gen;
  gen
    ( a() + b() >= 120 )
  ;

  gen.next();

  std::cout << format("result: a=%d, b=%d\n") % a % b;

  BOOST_CHECK(a + b >= 120);
}


BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
