#include <boost/test/unit_test.hpp>

#include <crave/Generator.hpp>

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include <set>
#include <iostream>


using boost::format;
using namespace crave;
using boost::assign::list_of;

void print_vec (std::ostream & out, std::vector<std::string> const & v)
{
 out << "( ";
 BOOST_FOREACH(std::string u, v)  {
   out << u << ' ';
 }
 out << ')';
}


void print_vec_vec (std::ostream & out, std::vector<std::vector<std::string> > const & v)
{
 out << "( ";
 BOOST_FOREACH(std::vector<std::string> const & u, v)  {
   print_vec(out, u  );
   out << ' ';
 }
 out << ')';
}

bool cmp_vec(std::vector<std::string> const &a, std::vector<std::string> const &b)
{
  if(a.size() == b.size())
  {
    for(unsigned i = 0; i < a.size(); i++)
    {
      if(a[i].compare(b[i]) != 0)
      {
        return a[i].compare(b[i]) < 0;
      }
    }
  }
  return a.size() < b.size();
}

//sortieren
void sort_results( std::vector<std::vector<std::string> > &results )
{
  for(unsigned i = 0; i < results.size();i++)
  {
    sort(results[i].begin(), results[i].end());
  }
  sort(results.begin(), results.end(), cmp_vec);
}

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

  BOOST_REQUIRE(!it.next() );
  BOOST_REQUIRE( it.is_constraint_enabled("sum") );
  BOOST_REQUIRE( it.is_constraint_enabled("product") );
  BOOST_REQUIRE( it.is_constraint_enabled("x") );

  BOOST_REQUIRE( it.disable_constraint("sum") );
  BOOST_REQUIRE( it.next() );
  BOOST_REQUIRE( !it.is_constraint_enabled("sum") );
  BOOST_REQUIRE( it.is_constraint_enabled("product") );
  BOOST_REQUIRE( it.is_constraint_enabled("x") );
  std::cout << it << std::endl;

  BOOST_REQUIRE( it.disable_constraint("product") );
  BOOST_REQUIRE( it.next() );
  BOOST_REQUIRE( !it.is_constraint_enabled("sum") );
  BOOST_REQUIRE( !it.is_constraint_enabled("product") );
  BOOST_REQUIRE(  it.is_constraint_enabled("x") );
  std::cout << it << std::endl;

  BOOST_REQUIRE( it.enable_constraint("sum") );
  BOOST_REQUIRE( it.next() );
  BOOST_REQUIRE( it.is_constraint_enabled("sum") );
  BOOST_REQUIRE( !it.is_constraint_enabled("product") );
  BOOST_REQUIRE(  it.is_constraint_enabled("x") );
  std::cout << it << std::endl;

  BOOST_REQUIRE( it.enable_constraint("product") );
  BOOST_REQUIRE(!it.next() );
  BOOST_REQUIRE( it.is_constraint_enabled("sum") );
  BOOST_REQUIRE( it.is_constraint_enabled("product") );
  BOOST_REQUIRE( it.is_constraint_enabled("x") );

  BOOST_REQUIRE( it.disable_constraint("x"));
  BOOST_REQUIRE( it.next() );
  BOOST_REQUIRE( it.is_constraint_enabled("sum") );
  BOOST_REQUIRE( it.is_constraint_enabled("product") );
  BOOST_REQUIRE( !it.is_constraint_enabled("x") );

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
  ItemPacketBaseConstraint() : i_(), rand_obj(), msg_length(this), src_addr(this), dest_addr(this), msg(this) {
    constraint(msg_length() < 80);
    constraint(msg_length() > 2);
    constraint(src_addr() != dest_addr());
    constraint(msg().size() == msg_length());
    constraint.foreach(msg, i_, msg()[i_] >= ' ' && msg()[i_] <= 'z');
  }

  placeholder i_;
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
    for(unsigned i = 0; i < it.msg.size(); ++i) {
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
BOOST_AUTO_TEST_CASE(no_conflict)
{
  randv<unsigned short> a(0);
  randv<unsigned short> b(0);
  randv<unsigned short> c(0);
  Generator gen;
  gen
    ("a", a() != b() )
    ("b", b() != c() )
  ;

 BOOST_REQUIRE(gen.next());
 std::vector<std::vector<std::string> > result = gen.analyse_contradiction();
 BOOST_REQUIRE_EQUAL(result.size(), 0);
}

BOOST_AUTO_TEST_CASE(one_conflict1)
{
  randv<bool> a(0);
  randv<bool> b(0);
  randv<bool> c(0);
  Generator gen;
  gen
    ("a", a() != b() )
    ("b", b() != c() )
    ("c", a() != c() )
  ;

 BOOST_REQUIRE(!gen.next());
// gen.analyse_contradiction();
 std::vector<std::vector<std::string> > result = gen.analyse_contradiction();
 print_vec_vec(std::cout, result);
 std::cout << std::endl;
 //sort_results(result);

 BOOST_REQUIRE_EQUAL(result.size(), 1);
 std::vector<std::string> expected = list_of ("a")("b")("c");
 BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());

}

BOOST_AUTO_TEST_CASE(one_conflict2)
{
  randv<unsigned short> a(0);
  randv<unsigned short> b(0);
  Generator gen;
  gen
    ("a", a() == 3 )
    ("b", a() > 4  )
    ("c", b() == 0 )
 ;

 BOOST_REQUIRE(!gen.next());
 //gen.analyse_contradiction();
 std::vector<std::vector<std::string> > result = gen.analyse_contradiction();

 print_vec_vec(std::cout, result);
 std::cout << std::endl;
 //sort_results(result);

 BOOST_REQUIRE_EQUAL(result.size(), 1);
 std::vector<std::string> expected = list_of ("a")("b");
 BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());

}

BOOST_AUTO_TEST_CASE(two_conflicts1)
{
  randv<unsigned short> a(0);
  randv<unsigned short> b(0);
  randv<unsigned short> c(0);
  Generator gen;
  gen
    ("a", a() == 1 )
    ("b", a()  > 5 )
    ("c", b() == 0 )
    ("d", c() == b() && c() != 0 )
 ;

 BOOST_REQUIRE(!gen.next());
 //gen.analyse_contradiction();
 std::vector<std::vector<std::string> > result = gen.analyse_contradiction();
 print_vec_vec(std::cout, result);
 std::cout << std::endl;
 //sort_results(result);

 std::vector<std::string> expected;
 expected = list_of ("a")("b");
 BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());
  expected = list_of ("c")("d");
 BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());


 BOOST_REQUIRE_EQUAL(result.size(), 2);
 BOOST_REQUIRE_EQUAL(result[0].size(), 2);
 BOOST_REQUIRE_EQUAL(result[1].size(), 2);
}

BOOST_AUTO_TEST_CASE(two_conflicts2)
{
  randv<bool> a(0);
  randv<bool> b(0);
  randv<bool> c(0);
  randv<unsigned short> d(0);
  randv<unsigned short> e(0);

  Generator gen;
  gen
    ("a", a() != b() )
    ("b", b() != c() )
    ("c", a() != c() )
    ("d", d() == 0 )
    ("e", e() == d() && e() != 0 )
    ;

  BOOST_REQUIRE(!gen.next());
  //gen.analyse_contradiction();
  std::vector<std::vector<std::string> > result = gen.analyse_contradiction();

  sort_results(result);

  print_vec_vec(std::cout, result);
  std::cout << std::endl;

  std::vector<std::string> expected;
  expected = list_of ("d")("e");
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());
  expected = list_of ("a")("b")("c");
  BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());


  BOOST_REQUIRE_EQUAL(result.size(), 2);
  BOOST_REQUIRE_EQUAL(result[0].size(), 2);
  BOOST_REQUIRE_EQUAL(result[1].size(), 3);
}

BOOST_AUTO_TEST_CASE(two_conflicts3)
{
  randv<unsigned short> a(0);
  randv<unsigned short> b(0);
  randv<unsigned short> c(0);
  randv<unsigned short> d(0);
  Generator gen;
  gen
    ("c1", a() == 2 )
    ("c2", a()  > 5 )
    ("c3", b() == d()*2 )
    ("c4", c() % a() == 1 )
    ("c5", c() == b() )
 ;

 BOOST_REQUIRE(!gen.next());
 std::vector<std::vector<std::string> > result = gen.analyse_contradiction();

 print_vec_vec(std::cout, result);
 std::cout << std::endl;

 sort_results(result);

 BOOST_REQUIRE_EQUAL(result.size(), 2);
 BOOST_REQUIRE_EQUAL(result[0].size(), 2);
 BOOST_REQUIRE_EQUAL(result[1].size(), 4);

 std::vector<std::string> expected;
 expected = list_of ("c1")("c2");
 BOOST_REQUIRE_EQUAL_COLLECTIONS( result[0].begin(), result[0].end(), expected.begin(), expected.end());
  expected = list_of ("c1")("c3")("c4")("c5");
 BOOST_REQUIRE_EQUAL_COLLECTIONS( result[1].begin(), result[1].end(), expected.begin(), expected.end());


}

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
