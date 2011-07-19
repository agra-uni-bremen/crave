#define BOOST_TEST_MODULE Constraint_Management_t
#include <boost/test/unit_test.hpp>

#include <crave/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

using boost::format;
using namespace crave;

#define ref(x) reference(x)

struct Context_Fixture {};

BOOST_FIXTURE_TEST_SUITE(Constraint_Management_t, Context_Fixture )

class Item : public rand_obj {
public:
  Item() : rand_obj(), a(this), b(this) {
    constraint("sum", a() + b() == 4);
    constraint("product", a() * b() == 4);
    constraint(a() < 10);
    constraint(b() < 10);
    constraint("x", a() != 2);
  }
  randv<uint> a;
  randv<uint> b;

  friend ostream& operator<<(ostream& os, const Item& it) { 
    os << it.a << " " << it.b; 
    return os; 
  }
};

BOOST_AUTO_TEST_CASE( t1 )
{
  Item it;

  BOOST_REQUIRE(!it.next());

  it.disable_constraint("sum");
  BOOST_REQUIRE(it.next());
  std::cout << it << std::endl;

  it.disable_constraint("product");
  BOOST_REQUIRE(it.next());
  std::cout << it << std::endl;

  it.enable_constraint("sum");
  BOOST_REQUIRE(it.next());
  std::cout << it << std::endl;

  it.enable_constraint("product");
  BOOST_REQUIRE(!it.next());

  it.disable_constraint("x");
  BOOST_REQUIRE(it.next());
  std::cout << it << std::endl;
  BOOST_REQUIRE(it.a == 2 && it.b == 2);
}

class Item1 : public rand_obj {
public:
  Item1() : rand_obj(), a(this) {
    constraint("abc", a() == 4);
    constraint("def", a() == 3);
    constraint("abc", a() == 3);
  }
  randv<uint> a;
};

BOOST_AUTO_TEST_CASE( t2 )
{
  BOOST_CHECK_THROW ( Item1 it, std::runtime_error );
}

class ItemPythagoras : public rand_obj {
public:
  ItemPythagoras() : rand_obj(), a(this), b(this), c(this) {
    constraint("pythagoras", a() * a() + b() * b() == c() * c());
    //constraint("div-zero", a() > 0 && b() > 0);
    constraint(a() > 0);
    constraint(b() > 0);
  }
  randv<unsigned char> a;
  randv<unsigned char> b;
  randv<unsigned char> c;

  friend ostream& operator<<(ostream& os, const ItemPythagoras& it) { 
    os << it.a << " " << it.b << " " << it.c; 
    return os; 
  }
};

BOOST_AUTO_TEST_CASE( Pythagoras )
{
  ItemPythagoras it;

  BOOST_REQUIRE(it.next());
  std::cout << it << std::endl;

  it.disable_constraint("pythagoras");
  BOOST_REQUIRE(it.next());

  it.enable_constraint("pythagoras");
  BOOST_REQUIRE(it.next());
  std::cout << it << std::endl;
}

class ItemPacketBaseConstraint : public rand_obj {
public:
	ItemPacketBaseConstraint() : rand_obj(), msg_length(this), src_addr(this), dest_addr(this), msg(this) {
	constraint(msg_length() < 80);
	constraint(msg_length() > 2);
	constraint(src_addr() != dest_addr());
	constraint(msg().size() == msg_length());
	

	constraint.foreach(msg, _i, msg()[_i] >= ' ' && msg()[_i] <= 'z');
	
	}
	randv<uint> src_addr;
	randv<uint> dest_addr;
	randv<unsigned short> msg_length;
	rand_vec<char> msg;
};


class ItemPacketHierConstraint : public ItemPacketBaseConstraint {
public:	
	ItemPacketHierConstraint() : ItemPacketBaseConstraint(), dest_min(this), dest_max(this) {
	constraint( (dest_addr() > dest_min() ) && (dest_addr() < dest_max()) );
	constraint( 
		  ( (src_addr() > (dest_addr() + 0x100000))  &&
		    (src_addr() < (dest_addr() + 0x200000)) ) ||
		  ( (src_addr() < (dest_addr() - 0x10000 )) ) &&
		    (src_addr() > (dest_addr() - 0xfffff ))  );
	}
	randv<uint>dest_min;
	randv<uint>dest_max;

	friend ostream& operator<<(ostream& os, ItemPacketHierConstraint& it) { 
    		os << it.src_addr << " " << it.dest_addr << " " << it.msg_length << " \n";
		for(unsigned i =0; i < it.msg.size(); ++i) {
			os << it.msg[i];
		}
    		return os; 
	}
};

BOOST_AUTO_TEST_CASE( PacketHierConstraint )
{
  ItemPacketHierConstraint it;
  for(unsigned i = 0; i < 11 ; ++i) {
    BOOST_REQUIRE(it.next());
    std::cout << it << std::endl;

  }
}


BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
