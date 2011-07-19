#define BOOST_TEST_MODULE allsat
#include <boost/test/unit_test.hpp>

#include <crave/ConstrainedRandom.hpp>
#include <crave/AllSAT.hpp>

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
  Generator<AllSAT<> > gen;
};

class sif_seq_item : public rand_obj_of<AllSAT<> > {
public:
    sif_seq_item() : 
        tx_enable_parity(this),
        tx_odd_parity(this),
        dummy()
    {
      constraint( !tx_odd_parity() || tx_enable_parity());
    }

    friend ostream& operator<<(ostream& os, const sif_seq_item& ssi) {
        os << "CHECK: tx_odd_parity / enable  = " << ssi.tx_odd_parity
           << " " << ssi.tx_enable_parity << std::endl; 
        assert( !ssi.tx_odd_parity || ssi.tx_enable_parity ); 

        os << "EndConstraint" << std::endl; 
        return os; 
    }

    randv<bool> tx_enable_parity;
    randv<bool> tx_odd_parity;
    int dummy;
};

BOOST_FIXTURE_TEST_SUITE(allsat, Context_Fixture )

BOOST_AUTO_TEST_CASE ( gen10k )
{
  Variable<bool> r1, r2;
  gen( !r1 || r2);

  for (int i=0;i<10000; i++) {
    gen();
    BOOST_REQUIRE( !gen[r1] || gen[r2] );
  }
}

BOOST_AUTO_TEST_CASE ( randv10k )
{
  sif_seq_item it;

  for (int i=0;i<10000; i++) {
    BOOST_REQUIRE(it.next());
//    std::cout << it << std::endl;
    BOOST_REQUIRE( !it.tx_odd_parity ||  it.tx_enable_parity );
    //BOOST_REQUIRE( !it.rx_odd_parity ||  it.rx_enable_parity );
  }
}

BOOST_AUTO_TEST_CASE ( t1 )
{
  randv<uint> v(0);
  Generator< AllSAT<30> > gen;
  gen(v() <= 10);

  std::vector<uint> vec;
  for (uint i = 0; i <= 10; i++) {
    BOOST_REQUIRE(gen.next());
    for (uint j = 0; j < vec.size(); j++)
      BOOST_REQUIRE(vec[j] != v);      
    vec.push_back(v);
  }
}

BOOST_AUTO_TEST_CASE( constants )
{
  Variable<unsigned> x;
  Generator<AllSAT<10> > gen( x == 135421 );
  gen();
  BOOST_CHECK_EQUAL( gen[x], 135421 );
}

BOOST_AUTO_TEST_CASE( boolean )
{
  Variable<bool> b;

  Generator<AllSAT<10> > gen;  
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

  Generator<AllSAT<10> > gen;  
  gen(a < 256u);

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

  Generator<AllSAT<10> > gen;  
  gen(a <= 256u);

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

  Generator<AllSAT<10> > gen;  
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

  Generator<AllSAT<10> > gen;  
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

  Generator<AllSAT<10> > gen;
  gen
    ( b != (unsigned char)  0u )
    ( a  < (unsigned char) 16u )
    ( b  < (unsigned char) 16u )
    ( q == a/b )
    ( r == a%b )
  ;

  while( gen.next() ) {
    BOOST_CHECK_EQUAL( gen[a]/gen[b], gen[q] );
    BOOST_CHECK_EQUAL( gen[a]%gen[b], gen[r] );

    gen( a != gen[a] || b != gen[b] );
    //printf("result: a=%d, b=%d, q=%d, r=%d\n", gen[a], gen[b], gen[q], gen[r]);
  }
}

// temporaly fix a variable to a certain value using the assign operator
BOOST_AUTO_TEST_CASE ( fix_variable )
{
  Variable<unsigned> a;
  Variable<unsigned> b;

  Generator<AllSAT<10> > gen (a < b);
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

  Generator<AllSAT<15> > gen (a == reference(b) );

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

  Generator<AllSAT<15> > gen (a == b);
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
  Generator<AllSAT<10> > gen;
  Variable<int> r;
  gen( r<6 );
  soft(gen)( r == 2 );
  
  BOOST_REQUIRE( gen.next() );
  BOOST_REQUIRE_EQUAL(gen[r], 2);

  gen(soft)( r==3 );

  BOOST_REQUIRE( gen.next() );
}

BOOST_AUTO_TEST_CASE( alu )
{
  Variable<unsigned> op;
  Variable<unsigned> a;
  Variable<unsigned> b;

  Generator<AllSAT<10> > gen;  
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

  Generator<AllSAT<10> > gen;  
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
    //printf("result: op=%d, a=%d, b=%d\n", gen[op], gen[a], gen[b]);
  }
  BOOST_CHECK_EQUAL(count, 572);
}

BOOST_AUTO_TEST_CASE( pythagoras )
{
  Variable<unsigned long long> a;
  Variable<unsigned long long> b;
  Variable<unsigned long long> c;

  Generator<AllSAT<5> > gen;  
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

  Generator<AllSAT<10> > gen;
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

  Generator<AllSAT<10> > gen;
  gen
    ( a() < 0 )
  ;

  gen();

  std::cout << format("result: a=%d\n") % a;

  BOOST_CHECK(a < 0);
}


BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
