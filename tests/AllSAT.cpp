#define BOOST_TEST_MODULE allsat
#include <boost/test/unit_test.hpp>

#include <platzhalter/ConstrainedRandom.hpp>
#include <platzhalter/AllSAT.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

//using namespace std;
using namespace platzhalter;

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

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
