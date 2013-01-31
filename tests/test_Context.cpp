#define BOOST_TEST_MODULE Context_t
#include <boost/test/unit_test.hpp>

#include <crave/ConstrainedRandom.hpp>
#include <crave/ExpressionTraits.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

//using namespace std;
using namespace crave;

using boost::format;

struct Context_Fixture {
  Context_Fixture ()
  {
  }
  protected:
    Context ctx;
};

BOOST_FIXTURE_TEST_SUITE(Context_t, Context_Fixture )

BOOST_AUTO_TEST_CASE ( multiple_solver_instances )
{
  Generator gen1, gen2;
  Variable<int> r1, r2;
  gen1(r1 < 6);
  gen2(r2 < 6);
  gen1.next();
  gen2.next();

}

BOOST_AUTO_TEST_CASE( constants )
{
  Variable<unsigned> x;
  Generator gen( x == 135421 );
  gen();
  BOOST_CHECK_EQUAL( gen[x], 135421 );
}

BOOST_AUTO_TEST_CASE( boolean )
{
  Variable<bool> b;

  Generator gen;
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

  Generator gen;
  gen(a < 256u);

  std::set<unsigned> generated;
  for(unsigned iterations = 0; gen.next(); ++iterations) {
    unsigned av = gen[a];
    generated.insert(av);
    gen( a != av );
    BOOST_REQUIRE_LT( iterations, 300);
  }

  BOOST_REQUIRE_EQUAL( generated.size(), 256);
}

BOOST_AUTO_TEST_CASE( less_equal )
{
  Variable<unsigned> a;

  Generator gen;
  gen(a <= 256u);

  std::set<unsigned> generated;
  for(unsigned iterations = 0; gen.next(); ++iterations) {
    unsigned av = gen[a];
    generated.insert(av);
    gen( a != av );
    BOOST_REQUIRE_LE(iterations, 300);
  }

  BOOST_REQUIRE_EQUAL(generated.size(), 257);
}

BOOST_AUTO_TEST_CASE( greater )
{
  Variable<unsigned> a;

  Generator gen;
  gen(a > (std::numeric_limits<unsigned>::max()-256) );

  std::set<unsigned> generated;
  for(unsigned iterations = 0; gen.next(); ++iterations) {
    unsigned av = gen[a];
    generated.insert(av);
    gen( a != av );
    BOOST_REQUIRE_GT( iterations, 300);
  }

  BOOST_REQUIRE_EQUAL( generated.size(), 256);
}

BOOST_AUTO_TEST_CASE( greater_equal )
{
  Variable<unsigned> a;

  Generator gen;
  gen(a >= (std::numeric_limits<unsigned>::max()-256) );

  std::set<unsigned> generated;
  for(unsigned iterations = 0; gen.next(); ++iterations) {
    unsigned av = gen[a];
    generated.insert(av);
    gen( a != av );
    BOOST_REQUIRE_GE( iterations, 300);
  }

  BOOST_REQUIRE_EQUAL( generated.size(), 257);
}

BOOST_AUTO_TEST_CASE( divide )
{
  Variable<unsigned char> a;
  Variable<unsigned char> b;
  Variable<unsigned char> q;
  Variable<unsigned char> r;

  Generator gen;
  gen
    ( b != (unsigned char)  0u )
    ( a  < (unsigned char) 16u )
    ( b  < (unsigned char) 16u )
    ( q == a/b )
    ( r == a%b )
  ;

  while( gen.next() ) {
    BOOST_REQUIRE_EQUAL( gen[a]/gen[b], gen[q] );
    BOOST_REQUIRE_EQUAL( gen[a]%gen[b], gen[r] );

    gen( a != gen[a] || b != gen[b] );
    std::cout << "result: a=" << gen[a] << ", b=" << gen[b] << ", q=" << gen[q] <<", r=" << gen[r] << "\n" << std::endl;
  }
}

BOOST_AUTO_TEST_CASE( mult_mod )
{
  randv<int> a(0);
  randv<int> b(0);

  Generator gen;
  gen
    ( -3 <= a() && a() <= 3 )
    ( -3 <= b() && b() <= 3 )
    ( a() * b() % 6 == 0)
  ;

  int cnt = 0;
  for (int i = -3; i <= 3; i++)
    for (int j = -3; j <= 3; j++)
      if (i * j % 6 == 0) cnt++;

  int cnt1 = 0;
  while( gen.next() ) {
    cnt1++;
    BOOST_REQUIRE_EQUAL(a * b % 6, 0);
    gen( a() != a || b() != b );
    std::cout << "result: a1=" << a << ", b1=" << b << "\n" << std::endl;
  }

  BOOST_REQUIRE_EQUAL(cnt, cnt1);
}

BOOST_AUTO_TEST_CASE ( shiftleft )
{
  Variable<unsigned> a;
  Variable<unsigned> b;
  Variable<unsigned> c;

  Generator gen;
  gen
    ( a <  256u )
    ( b <  (unsigned) (sizeof(unsigned)*8u) )
    ( c == (a << b) )
  ;

  int count = 0;
  while( gen.next() && ++count < 1000) {
    unsigned av = gen[a];
    unsigned bv = gen[b];
    unsigned r  = av << bv;

    BOOST_REQUIRE_EQUAL( r, gen[c] );

    gen( a != gen[a] || b != gen[b] );
  }
}

// temporaly fix a variable to a certain value using the assign operator
BOOST_AUTO_TEST_CASE ( fix_variable )
{
  Variable<unsigned> a;
  Variable<unsigned> b;

  Generator gen (a < b);
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

  Generator gen (a == reference(b) );

  while( gen.next() ) {
    unsigned av = gen[a];
    BOOST_REQUIRE_EQUAL( av, b);
    ++b;
    if (b > 10) break;
  }
}

// temporaly fix a variable to a certain value using the assign operator
BOOST_AUTO_TEST_CASE ( named_reference )
{
  unsigned bv=0;
  Variable<unsigned> a, c;
  ReadReference<unsigned> b(bv);

  Generator gen (a == b);
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
  Generator gen;
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
  Generator gen;
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

    BOOST_REQUIRE_LE(count, 10);
  }

  BOOST_REQUIRE_EQUAL(count, 4);
}

BOOST_AUTO_TEST_CASE ( randv_var_ref_mixed_test )
{
  Generator gen;
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

    BOOST_REQUIRE_LE(count, 10);
  }
  BOOST_REQUIRE_EQUAL(count, 4);
}

BOOST_AUTO_TEST_CASE( alu )
{
  Variable<unsigned> op;
  Variable<unsigned> a;
  Variable<unsigned> b;

  Generator gen;
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

  Generator gen;
  gen
    ( a < 16u)
    ( b < 16u)
    (op < 4u )  // 4 opcodes
    (op != 0u || a + b < 16u )  // no add overflow
    (op != 1u || a > b      )  // no sub underflow
    (op != 2u || a * b < 16u )  // no m overflow
    (op != 3u || b != 0u     )  // div valid
  ;

  unsigned count=0;
  while( gen.next() ) {
    ++count;
    gen( op != gen[op] || a != gen[a] || b != gen[b] );

    BOOST_REQUIRE_LE(count, 600);
  }
  BOOST_REQUIRE_EQUAL(count, 572);
}

BOOST_AUTO_TEST_CASE( pythagoras )
{
  Variable<unsigned long long> a;
  Variable<unsigned long long> b;
  Variable<unsigned long long> c;

  Generator gen;
  gen(a*a + b*b == c*c)(); // create a new assignment

  unsigned long long av = gen[a];
  unsigned long long bv = gen[b];
  unsigned long long cv = gen[c];

  BOOST_CHECK_EQUAL( av*av + bv*bv, cv*cv);
}

BOOST_AUTO_TEST_CASE ( negative_var )
{
  randv<int> a(NULL);
  randv<int> b(NULL);

  Generator gen;
  gen
    ( a() + b() <= 120 )
    ( a() > 120  )
  ;

  gen();

  std::cout << format("result: a=%d, b=%d\n") % a % b;

  BOOST_CHECK(a + b <= 120);
}

BOOST_AUTO_TEST_CASE ( signed_less_zero )
{
  randv<int> a(NULL);

  Generator gen;
  gen
    ( a() < 0 )
  ;

  gen();

  std::cout << format("result: a=%d\n") % a;

  BOOST_CHECK(a < 0);
}

BOOST_AUTO_TEST_CASE( expressionSize ) {
  Variable<unsigned long>  a;
  Variable<unsigned short> b;
  Variable<short>          c;
  Variable<unsigned char>  d;
  ExpressionSize es;
  FixWidth fw;

  BOOST_CHECK_EQUAL( es(a).value, sizeof(long)*8);
  BOOST_CHECK_EQUAL( es(b).value, sizeof(short)*8);
  BOOST_CHECK_EQUAL( es (a+b).value, std::max( sizeof(short), sizeof(long))*8 );
  BOOST_CHECK_EQUAL( es (c+d).value, std::max( sizeof(short), sizeof(unsigned char))*8 );

  BOOST_AUTO ( fixed_expr1, fw(a+b));
  boost::proto::display_expr( fixed_expr1 );

  BOOST_CHECK_EQUAL(
      es ( fixed_expr1 ).value,
      std::max( sizeof(unsigned long), sizeof(unsigned short))*8
      );

  BOOST_AUTO ( fixed_expr2, fw(c+d));
  boost::proto::display_expr( fixed_expr2 );

  BOOST_CHECK_EQUAL(
      es ( fixed_expr2 ).value,
      std::max( sizeof(short), sizeof(unsigned char))*8
      );
}


BOOST_AUTO_TEST_CASE ( mixed_bv_width_1 )
{
  randv<unsigned long> a(NULL);
  randv<unsigned short> b(NULL);


  //boost::proto::display_expr( b()+a());
  //boost::proto::display_expr( FixWidth()( b()+a() ));

  //boost::proto::display_expr( a()+b());
  //boost::proto::display_expr( FixWidth()( a()+b() ));

  //boost::proto::display_expr( b()+a() >= 120);
  //boost::proto::display_expr( FixWidth()(b()+a() >= 120));

  Generator gen;
  gen
    (  a() + b() >= 120 )
  ;

  gen.next();

  std::cout << format("result: a=%d, b=%d\n") % a % b;

  BOOST_CHECK(a + b >= 120);
}

BOOST_AUTO_TEST_CASE ( mixed_bv_width_2 )
{
  randv<signed char> a(NULL);
  Generator gen;
  gen( a() < 10 );

  std::set<signed char> generated;
  for(unsigned iterations = 0; gen.next(); ++iterations) {
    generated.insert(a);
    gen( a() != a );

    BOOST_REQUIRE_LT( iterations, 150);
  }

  BOOST_CHECK_EQUAL( generated.size(), 138);
}

BOOST_AUTO_TEST_CASE ( mixed_bv_width_3 )
{
  randv<short> a(NULL);
  Generator gen;
  gen( a() <  10 );
  gen( a() > -10 );

  std::set<short> generated;
  for(unsigned iterations = 0; gen.next(); ++iterations) {
    generated.insert(a);
    gen( a() != a );

    BOOST_REQUIRE_LT( iterations, 20);
  }

  BOOST_CHECK_EQUAL( generated.size(), 19);
}

BOOST_AUTO_TEST_CASE ( mixed_bv_width_4 )
{
  randv<int> a(NULL);
  Generator gen;
  gen( a() < (signed char) 10 );
  gen( a() > (short) -10 );

  std::set<int> generated;
  for(unsigned iterations = 0; gen.next(); ++iterations) {
    generated.insert(a);
    gen( a() != a );

    BOOST_REQUIRE_LT( iterations, 20);
  }

  BOOST_CHECK_EQUAL( generated.size(), 19);
}

BOOST_AUTO_TEST_CASE ( mixed_bv_width_5 )
{
  randv<short> a(NULL);
  randv<signed char> b(NULL);
  Generator gen;
  gen( -3 <= a() && a() <= 3 );
  gen( -3 <= b() && b() <= 3 );
  gen( (-2 <= a() + b()) && (a() + b() <= 2) );

  int cnt = 0;
  while (gen.next()) {
    cnt++;
    gen( (a() != a) || (b() != b) );

    BOOST_REQUIRE_LT( cnt, 300);
  }

  int cnt1 = 0;
  for (short i = -3; i <= 3; i++)
    for (signed char j = -3; j <= 3; j++)
      if ((-2 <= i + j) && (i + j <= 2)) cnt1++;

  BOOST_CHECK_EQUAL( cnt, cnt1);
}

BOOST_AUTO_TEST_CASE( mixed_bv_width_6 )
{
  randv<short> a(0);
  randv<signed char> b(0);

  Generator gen;
  gen
    ( -3 <= a() && a() <= 3 )
    ( -3 <= b() && b() <= 3 )
    ( a() * b() % 6 == 0)
  ;

  int cnt = 0;
  for (int i = -3; i <= 3; i++)
    for (int j = -3; j <= 3; j++)
      if (i * j % 6 == 0) cnt++;

  int cnt1 = 0;
  while( gen.next() ) {
    cnt1++;
    BOOST_REQUIRE_EQUAL(a * b % 6, 0);
    gen( a() != a || b() != b );
  }

  BOOST_REQUIRE_EQUAL(cnt, cnt1);
}

BOOST_AUTO_TEST_CASE ( element_inside_set )
{
  std::set<unsigned> s;
  s.insert(1);
  s.insert(7);
  s.insert(9);

  randv<unsigned> x(0);

  Generator gen;
  gen ( inside(x(),s) );

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(s.find(x) != s.end());

  unsigned first = x;
  gen (x() != first);


  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(s.find(x) != s.end());

  unsigned second = x;
  BOOST_REQUIRE_NE(first, second);
  gen (x() != second);


  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(s.find(x) != s.end());

  unsigned third = x;
  BOOST_REQUIRE_NE(third, second);
  BOOST_REQUIRE_NE(first, third);
  gen (x() != third);


  BOOST_REQUIRE(!gen.next());

}

BOOST_AUTO_TEST_CASE ( element_inside_vec )
{
  std::vector<unsigned> v;
  v.push_back(1);
  v.push_back(7);
  v.push_back(9);

  randv<unsigned> x(0);
  unsigned tmp;

  Generator gen;
  gen ( inside(x(),v) );

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE( find(v.begin(), v.end(), x) != v.end());

  unsigned first = x;
  gen (x() != first);


  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(find(v.begin(), v.end(), x) != v.end());

  unsigned second = x;
  BOOST_REQUIRE_NE(first, second);
  gen (x() != second);


  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE( find(v.begin(), v.end(), x) != v.end());

  unsigned third = x;
  BOOST_REQUIRE_NE(third, second);
  BOOST_REQUIRE_NE(first, third);
  gen (x() != third);


  BOOST_REQUIRE(!gen.next());

}

BOOST_AUTO_TEST_CASE ( element_inside_array )
{
  unsigned a[3];
  a[0] = 1;
  a[1] = 7;
  a[2] = 9;


  randv<unsigned> x(0);
  //unsigned y = a.begin();
  Generator gen;
  gen ( inside(x(),a) );

  BOOST_REQUIRE(gen.next());
  //BOOST_REQUIRE( find(a.begin(), a.end(), x) != a.end());

  unsigned first = x;
  gen (x() != first);


  BOOST_REQUIRE(gen.next());
  //BOOST_REQUIRE( find(a.begin(), a.end(), x) != a.end());

  unsigned second = x;
  BOOST_REQUIRE_NE(first, second);
  gen (x() != second);


  BOOST_REQUIRE(gen.next());
  //BOOST_REQUIRE( find(a.begin(), a.end(), x) != a.end());
  unsigned third = x;
  BOOST_REQUIRE_NE(third, second);
  BOOST_REQUIRE_NE(first, third);
  gen (x() != third);


  BOOST_REQUIRE(!gen.next());

}


BOOST_AUTO_TEST_CASE ( element_inside_list )
{
  std::list<unsigned> l;
  l.push_back(1);
  l.push_back(7);
  l.push_back(9);

  randv<unsigned> x(0);

  Generator gen;
  gen ( inside(x(),l) );

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE( find(l.begin(), l.end(), x) != l.end());

  unsigned first = x;
  gen (x() != first);


  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE( find(l.begin(), l.end(), x) != l.end());

  unsigned second = x;
  BOOST_REQUIRE_NE(first, second);
  gen (x() != second);


  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE( find(l.begin(), l.end(), x) != l.end());

  unsigned third = x;
  BOOST_REQUIRE_NE(third, second);
  BOOST_REQUIRE_NE(first, third);
  gen (x() != third);


  BOOST_REQUIRE(!gen.next());

}

//BOOST_AUTO_TEST_CASE (boolean_variables)
//{
//  randv<bool> a(0);
//
//  Generator gen;
//  gen ( a() ); // a() is a bitvector of size 1, not a boolean
//
//  BOOST_REQUIRE(gen.next());
//  BOOST_REQUIRE(a);
//
//}

BOOST_AUTO_TEST_CASE (dist_of_boolean25)
{
  randv<bool> a(0);
  int counter = 0;

  Generator gen;
  gen ( dist(a(), 0.25) );
  for (unsigned i = 0; i < 1000; i++) {
    BOOST_REQUIRE( gen.next() );
    if (a) {
      ++counter;
    } else {
      --counter;
    }
  }

  BOOST_REQUIRE_LT( counter,-425 );
  BOOST_REQUIRE_GT( counter,-575 );
}

BOOST_AUTO_TEST_CASE ( dist_of_boolean50 )
{
  randv<bool> a(0);
  int counter = 0;

  Generator gen;
  gen ( dist(a(),0.5) );
  for (unsigned i = 0; i < 1000; i++) {
    BOOST_REQUIRE( gen.next() );
    if (a) {
      ++counter;
    } else {
      --counter;
    }
  }

  BOOST_REQUIRE_LT( counter, 75 );
  BOOST_REQUIRE_GT( counter,-75 );
}

BOOST_AUTO_TEST_CASE (dist_of_boolean75)
{
  randv<bool> a(0);
  int counter = 0;

  Generator gen;
  gen ( dist(a(),0.75) );
  for (unsigned i = 0; i < 1000; i++) {
    BOOST_REQUIRE( gen.next() );
    if (a) {
      ++counter;
    } else {
      --counter;
    }
  }

  BOOST_REQUIRE_LT( counter, 575 );
  BOOST_REQUIRE_GT( counter, 425 );
}

BOOST_AUTO_TEST_SUITE_END() // Context

bitsize_traits<long> _instanciate1;
bitsize_traits<short> _instanciate2;

//  vim: ft=cpp:ts=2:sw=2:expandtab
