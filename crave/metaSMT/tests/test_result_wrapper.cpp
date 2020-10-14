/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2011-2016 University of Bremen, Germany.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/

#define BOOST_TEST_MODULE test_result_wrapper
#include <metaSMT/result_wrapper.hpp>

#include <boost/proto/debug.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/utility/enable_if.hpp>
#include <string>
#include <limits>
#include <bitset>

using namespace metaSMT;
using boost::dynamic_bitset;
using boost::logic::tribool;
using std::vector;
using std::string;

class Fixture {
  public:

  protected:
};

/**
 * pass in a three bit don't care ("XXX"), the function
 * checks the correct conversion to various types.
 **/
void check_conversion_XXX( result_wrapper const & rw)
{
  using boost::logic::indeterminate;
  tribool tri = rw;
  BOOST_REQUIRE( indeterminate(tri) );

  bool boolean = rw;
  BOOST_REQUIRE_EQUAL( boolean, false);

  std::string s = rw;
  BOOST_REQUIRE_EQUAL( s, "XXX" );

  int i = rw;
  BOOST_REQUIRE_EQUAL( i, 0);

  unsigned u = rw;
  BOOST_REQUIRE_EQUAL( u, 0u);

  unsigned long ul = rw;
  BOOST_REQUIRE_EQUAL( ul, 0ul);

  vector<bool> a, b(3, false);
  a = rw.operator std::vector<bool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(a.begin(), a.end(), b.begin(), b.end());

  vector<tribool> ta, tb(3, indeterminate);
  ta = rw.operator std::vector<boost::logic::tribool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(ta.begin(), ta.end(), tb.begin(), tb.end());

  unsigned char uc = rw;
  BOOST_REQUIRE_EQUAL( uc, 0u);

  signed char sc = rw;
  BOOST_REQUIRE_EQUAL( sc, 0);

  dynamic_bitset<> bs = rw;
  BOOST_REQUIRE_EQUAL(bs, dynamic_bitset<>(3, 0u));
}

void check_conversion_0_in_8bit( result_wrapper const & rw)
{
  tribool tri = rw;
  BOOST_REQUIRE( !indeterminate(tri) );
  BOOST_REQUIRE_EQUAL( static_cast<bool>(tri), false);

  bool boolean = rw;
  BOOST_REQUIRE_EQUAL( boolean, false);

  std::string s = rw;
  BOOST_REQUIRE_EQUAL( s, "00000000" );

  int i = rw;
  BOOST_REQUIRE_EQUAL( i, 0);

  unsigned u = rw;
  BOOST_REQUIRE_EQUAL( u, 0u);

  unsigned long ul = rw;
  BOOST_REQUIRE_EQUAL( ul, 0ul);

  vector<bool> a, b(8, false);
  a = rw.operator std::vector<bool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(a.begin(), a.end(), b.begin(), b.end());

  vector<tribool> tb = rw.operator std::vector<boost::logic::tribool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(tb.begin(), tb.end(), b.begin(), b.end());

  unsigned char uc = rw;
  BOOST_REQUIRE_EQUAL( uc, 0u);

  signed char sc = rw;
  BOOST_REQUIRE_EQUAL( sc, 0);


  dynamic_bitset<> bs = rw;
  BOOST_REQUIRE_EQUAL(bs, dynamic_bitset<>(8, 0u));
}

void check_conversion_1_in_8bit( result_wrapper const & rw)
{
  tribool tri = rw;
  BOOST_REQUIRE_EQUAL( static_cast<bool>(tri), true);

  bool boolean = rw;
  BOOST_REQUIRE_EQUAL( boolean, true);

  std::string s = rw;
  BOOST_REQUIRE_EQUAL( s, "00000001" );

  int i = rw;
  BOOST_REQUIRE_EQUAL( i, 1);

  unsigned u = rw;
  BOOST_REQUIRE_EQUAL( u, 1u);

  unsigned long ul = rw;
  BOOST_REQUIRE_EQUAL( ul, 1ul);

  vector<bool> a, b(8, false);
  b[0] = true;
  a = rw.operator std::vector<bool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(a.begin(), a.end(), b.begin(), b.end());

  vector<tribool> tb = rw.operator std::vector<boost::logic::tribool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(tb.begin(), tb.end(), b.begin(), b.end());

  unsigned char uc = rw;
  BOOST_REQUIRE_EQUAL( uc, 1);

  signed char sc = rw;
  BOOST_REQUIRE_EQUAL( sc, 1);


  dynamic_bitset<> bs = rw;
  BOOST_REQUIRE_EQUAL(bs, dynamic_bitset<>(8, 1u));
}

void check_conversion_128_in_8bit( result_wrapper const & rw)
{
  tribool tri = rw;
  BOOST_REQUIRE_EQUAL( static_cast<bool>(tri), true);

  bool boolean = rw;
  BOOST_REQUIRE_EQUAL( boolean, true);

  std::string s = rw;
  BOOST_REQUIRE_EQUAL( s, "10000000" );

  vector<bool> a, b(8, false);
  b[7] = true;
  a = rw.operator std::vector<bool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(a.begin(), a.end(), b.begin(), b.end());

  vector<tribool> tb = rw.operator std::vector<boost::logic::tribool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(tb.begin(), tb.end(), b.begin(), b.end());

  unsigned char uc = rw;
  BOOST_REQUIRE_EQUAL( uc, 128u);

  signed char sc = rw;
  BOOST_REQUIRE_EQUAL( sc, -128);

  int i = rw;
  BOOST_REQUIRE_EQUAL( i, -128);

  unsigned u = rw;
  BOOST_REQUIRE_EQUAL( u, 128u);

  unsigned long ul = rw.operator unsigned long();
  BOOST_REQUIRE_EQUAL( ul, 128ul);

  dynamic_bitset<> bs = rw;
  BOOST_REQUIRE_EQUAL(bs, dynamic_bitset<>(8, 128u));

}

// do not check downcasting for signed
template<typename T>
void check_conversion_integer_cast(result_wrapper const & rw, T value, typename boost::enable_if<boost::is_signed<T> >::type* = 0) {
  int16_t i16 = rw;
  if (std::numeric_limits<T>::digits <= 16) BOOST_REQUIRE_EQUAL( i16,  int16_t(value) );
  int32_t i32 = rw;
  if (std::numeric_limits<T>::digits <= 32) BOOST_REQUIRE_EQUAL( i32,  int32_t(value) );
  int64_t i64 = rw;
  if (std::numeric_limits<T>::digits <= 64) BOOST_REQUIRE_EQUAL( i64,  int64_t(value) );
}

template<typename T>
void check_conversion_integer_cast(result_wrapper const & rw, T value, typename boost::enable_if<boost::is_unsigned<T> >::type* = 0) {
  uint16_t u16 = rw;
  BOOST_REQUIRE_EQUAL( u16, uint16_t(value) );
  uint32_t u32 = rw;
  BOOST_REQUIRE_EQUAL( u32, uint32_t(value) );
  uint64_t u64 = rw;
  BOOST_REQUIRE_EQUAL( u64, uint64_t(value) );
}

template<unsigned width, typename Integer>
void check_conversion_from_integer(Integer value) {
  result_wrapper rw(value, width);

  tribool tri = rw;
  // only check if value is not truncated
  if (std::numeric_limits<Integer>::digits <= width) BOOST_REQUIRE_EQUAL( static_cast<bool>(tri), true );

  bool boolean = rw;
  // only check if value is not truncated
  if (std::numeric_limits<Integer>::digits <= width) BOOST_REQUIRE_EQUAL( boolean, true );

  std::string s = rw;
  BOOST_REQUIRE_EQUAL( s, std::bitset<width>(value).to_string() );

  vector<bool> a = rw.operator std::vector<bool>();
  vector<bool> b;
  for (unsigned i = 0; i < width; i++)
    b.push_back(s[width - i - 1] == '1');
  BOOST_REQUIRE_EQUAL_COLLECTIONS(a.begin(), a.end(), b.begin(), b.end());

  vector<tribool> tb = rw.operator std::vector<boost::logic::tribool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(tb.begin(), tb.end(), b.begin(), b.end());

//  XXX somehow dynamic_bitset truncates 64-bit values on 32-bit platforms...
//  dynamic_bitset<> bs = rw;
//  BOOST_REQUIRE_EQUAL(bs, dynamic_bitset<>(width, value));

  // only check if value is not truncated
  if (std::numeric_limits<Integer>::digits <= width) check_conversion_integer_cast(rw, value);
}

void check_conversion_13_in_8bit( result_wrapper const & rw)
{
  tribool tri = rw;
  BOOST_REQUIRE_EQUAL( static_cast<bool>(tri), true);

  bool boolean = rw;
  BOOST_REQUIRE_EQUAL( boolean, true);

  std::string s = rw;
  BOOST_REQUIRE_EQUAL( s, "00001101" );

  vector<bool> a, b(8, false);
  b[0] = true;
  b[2] = true;
  b[3] = true;
  a = rw.operator std::vector<bool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(a.begin(), a.end(), b.begin(), b.end());

  vector<tribool> tb = rw.operator std::vector<boost::logic::tribool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(tb.begin(), tb.end(), b.begin(), b.end());

  unsigned char uc = rw;
  BOOST_REQUIRE_EQUAL( uc, 13u);

  signed char sc = rw;
  BOOST_REQUIRE_EQUAL( sc, 13);

  int i = rw;
  BOOST_REQUIRE_EQUAL( i, 13);

  unsigned u = rw;
  BOOST_REQUIRE_EQUAL( u, 13u);

  unsigned long ul = rw;
  BOOST_REQUIRE_EQUAL( ul, 13ul);

  dynamic_bitset<> bs = rw;
  BOOST_REQUIRE_EQUAL(bs, dynamic_bitset<>(8, 13u));

}

void check_conversion_true( result_wrapper const & rw)
{
  using boost::logic::tribool;

  tribool tri = rw;
  BOOST_REQUIRE_EQUAL( static_cast<bool>(tri), true);

  bool boolean = rw;
  BOOST_REQUIRE_EQUAL( boolean, true);

  std::string s = rw;
  BOOST_REQUIRE_EQUAL( s, "1" );

  vector<bool> a, b(1, true);
  a = rw.operator std::vector<bool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(a.begin(), a.end(), b.begin(), b.end());

  vector<tribool> tb = rw.operator std::vector<boost::logic::tribool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(tb.begin(), tb.end(), b.begin(), b.end());

  int i = rw;
  BOOST_REQUIRE_EQUAL( i, -1);

  unsigned u = rw;
  BOOST_REQUIRE_EQUAL( u, 1u);

  unsigned long ul = rw;
  BOOST_REQUIRE_EQUAL( ul, 1l);

  unsigned char uc = rw;
  BOOST_REQUIRE_EQUAL( uc, 1u);

  signed char sc = rw;
  BOOST_REQUIRE_EQUAL( (int)sc, -1);

  dynamic_bitset<> bs = rw;
  BOOST_REQUIRE_EQUAL(bs, dynamic_bitset<>(1, 1u));
}

void check_conversion_false( result_wrapper const & rw)
{
  using boost::logic::tribool;

  tribool tri = rw;
  BOOST_REQUIRE( !indeterminate(tri) );
  BOOST_REQUIRE_EQUAL( static_cast<bool>(tri), false);

  bool boolean = rw;
  BOOST_REQUIRE_EQUAL( boolean, false);

  std::string s = rw;
  BOOST_REQUIRE_EQUAL( s, "0" );

  vector<bool> a, b(1, false);
  a = rw.operator std::vector<bool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(a.begin(), a.end(), b.begin(), b.end());

  vector<tribool> tb = rw.operator std::vector<boost::logic::tribool>();
  BOOST_REQUIRE_EQUAL_COLLECTIONS(tb.begin(), tb.end(), b.begin(), b.end());

  unsigned char uc = rw;
  BOOST_REQUIRE_EQUAL( uc, 0u);

  signed char sc = rw;
  BOOST_REQUIRE_EQUAL( sc, 0);

  int i = rw;
  BOOST_REQUIRE_EQUAL( i, 0);

  unsigned u = rw;
  BOOST_REQUIRE_EQUAL( u, 0u);

  unsigned long ul = rw;
  BOOST_REQUIRE_EQUAL( ul, 0ul);

  dynamic_bitset<> bs = rw;
  BOOST_REQUIRE_EQUAL(bs, dynamic_bitset<>(1, 0u));
}

BOOST_FIXTURE_TEST_SUITE(test_result_wrapper, Fixture )

BOOST_AUTO_TEST_CASE( from_string )
{
  const std::string val = "1101";
	result_wrapper rw(val);

  std::string s = rw;
  BOOST_REQUIRE_EQUAL(s, val);

  unsigned u = rw;
  BOOST_REQUIRE_EQUAL(u, 13);

  unsigned long ul = rw;
  BOOST_REQUIRE_EQUAL(ul, 13ul);

  signed int i = rw;
  BOOST_REQUIRE_EQUAL(i, -3);

  check_conversion_1_in_8bit( result_wrapper( std::string("00000001")) );
  check_conversion_128_in_8bit( result_wrapper( std::string("10000000")) );
  check_conversion_13_in_8bit( result_wrapper( std::string("00001101")) );
  check_conversion_0_in_8bit( result_wrapper( std::string("00000000")) );
  check_conversion_true( result_wrapper( std::string("1")) );
  check_conversion_false( result_wrapper( std::string("0")) );
  check_conversion_XXX( result_wrapper( std::string("XXX")) );
  check_conversion_XXX( result_wrapper( std::string("xxx")) );
  check_conversion_XXX( result_wrapper( std::string("XxX")) );
  check_conversion_XXX( result_wrapper( std::string("xXx")) );
}

BOOST_AUTO_TEST_CASE( from_bool )
{
  check_conversion_true ( result_wrapper( result_wrapper(true ) ) );
  check_conversion_false( result_wrapper( result_wrapper(false) ) );
}

BOOST_AUTO_TEST_CASE( tribool_from_string )
{
  boost::logic::tribool t;
  t = result_wrapper ("1");
  BOOST_REQUIRE_EQUAL(static_cast<bool>(t), true);

  t = result_wrapper ("0");
  BOOST_REQUIRE( !indeterminate(t) );
  BOOST_REQUIRE_EQUAL(static_cast<bool>(t), false);

	t = result_wrapper ("X");
  BOOST_REQUIRE(boost::logic::indeterminate(t));
	t = result_wrapper ("x");
  BOOST_REQUIRE(boost::logic::indeterminate(t));
}

BOOST_AUTO_TEST_CASE( tribool_from_char )
{
  boost::logic::tribool t;
  t = result_wrapper ('1');
  BOOST_REQUIRE_EQUAL(static_cast<bool>(t), true);

  t = result_wrapper ('0');
  BOOST_REQUIRE( !indeterminate(t) );
  BOOST_REQUIRE_EQUAL(static_cast<bool>(t), false);

	t = result_wrapper ('X');
  BOOST_REQUIRE(boost::logic::indeterminate(t));
	t = result_wrapper ('x');
  BOOST_REQUIRE(boost::logic::indeterminate(t));
}

BOOST_AUTO_TEST_CASE( minus_one_from_string4 )
{
  const std::string val = "1111";
	result_wrapper rw(val);

  int i = rw;
  BOOST_REQUIRE_EQUAL(i, -1);
}

BOOST_AUTO_TEST_CASE( minus_one_from_string8 )
{
  const std::string val = "11111111";
	result_wrapper rw(val);

  signed char i = rw;
  BOOST_REQUIRE_EQUAL(static_cast<int>(i), -1);
}

BOOST_AUTO_TEST_CASE( minus_one_from_string32 )
{
  const std::string val = "11111111111111111111111111111111";
	result_wrapper rw(val);

  int32_t i = rw;
  BOOST_REQUIRE_EQUAL(i, -1);
}

BOOST_AUTO_TEST_CASE( from_dynamic_bitset )
{
	boost::dynamic_bitset<> bs(8, 255);
	result_wrapper rw(bs);

  unsigned i = rw;
  BOOST_REQUIRE_EQUAL(i, 255);

  unsigned long il = rw;
  BOOST_REQUIRE_EQUAL(il, 255);

  check_conversion_1_in_8bit( result_wrapper(dynamic_bitset<>(8, 1)) );
  check_conversion_128_in_8bit( result_wrapper(dynamic_bitset<>(8, 128)) );
  check_conversion_13_in_8bit( result_wrapper( dynamic_bitset<>(8,13)) );
  check_conversion_0_in_8bit( result_wrapper( dynamic_bitset<>(8,0)) );
  check_conversion_true ( result_wrapper(dynamic_bitset<>(1, 1)) );
  check_conversion_false( result_wrapper(dynamic_bitset<>(1, 0)) );
}

BOOST_AUTO_TEST_CASE( minus_one_from_dynamic_bitset )
{
	boost::dynamic_bitset<> bs(8, -1);
	result_wrapper rw(bs);

  int32_t i = rw;
  BOOST_REQUIRE_EQUAL(i, -1);
}

BOOST_AUTO_TEST_CASE( negative_from_dynamic_bitset )
{
	boost::dynamic_bitset<> bs(8, -65);
	result_wrapper rw(bs);

  int32_t i = rw;
  BOOST_REQUIRE_EQUAL(i, -65);
}

BOOST_AUTO_TEST_CASE( from_vector_bool )
{
  // check for 1
  std::vector< bool > vec (8, false);
  vec[0] = true;

  check_conversion_1_in_8bit( result_wrapper(vec) );

  // check  for 8u/-8
  vec[0]=false;
  vec[7]=true;
  check_conversion_128_in_8bit( result_wrapper(vec) );

  // check for 13 in 8bit
  vec[0]=true;
  vec[2]=true;
  vec[3]=true;
  vec[7]=false;
  check_conversion_13_in_8bit( result_wrapper(vec) );

  check_conversion_0_in_8bit( result_wrapper(vector<bool>(8, false)) );

  check_conversion_true ( result_wrapper(vector<bool>(1, true)) );
  check_conversion_false( result_wrapper(vector<bool>(1, false)) );
}

BOOST_AUTO_TEST_CASE( from_vector_tribool )
{
  using boost::logic::indeterminate;
  using boost::logic::tribool;
  // check for 1
  std::vector< tribool > vec (8, false);
  vec[0] = true;

  check_conversion_1_in_8bit( result_wrapper(vec) );

  // check  for 8u/-8
  vec[0]=false;
  vec[7]=true;
  check_conversion_128_in_8bit( result_wrapper(vec) );

  // check for 13 in 8bit
  vec[0]=true;
  vec[2]=true;
  vec[3]=true;
  vec[7]=false;
  check_conversion_13_in_8bit( result_wrapper(vec) );
  check_conversion_true ( result_wrapper(vector<tribool>(1, true)) );
  check_conversion_false( result_wrapper(vector<tribool>(1, false)) );

  vec.resize(3);
  vec[0]=indeterminate;
  vec[1]=indeterminate;
  vec[2]=indeterminate;
  check_conversion_XXX( result_wrapper(vec) );
}

BOOST_AUTO_TEST_CASE( from_integral_value_and_width )
{
  using boost::logic::indeterminate;

  // check for 1
  check_conversion_1_in_8bit( result_wrapper(1, 8) );

  // check  for 8u/-8
  check_conversion_128_in_8bit( result_wrapper(128, 8) );

  // check  for 8u/-8
  check_conversion_13_in_8bit( result_wrapper(13, 8) );

  check_conversion_0_in_8bit( result_wrapper(0, 8) );

  check_conversion_true ( result_wrapper(1, 1) );
  check_conversion_false( result_wrapper(0, 1) );

  check_conversion_from_integer<64>(std::numeric_limits<uint64_t>::max());
  check_conversion_from_integer<64>(std::numeric_limits<uint32_t>::max());
  check_conversion_from_integer<32>(std::numeric_limits<uint64_t>::max());
  check_conversion_from_integer<64>(std::numeric_limits< int64_t>::min());
  check_conversion_from_integer<64>(std::numeric_limits< int32_t>::min());
  check_conversion_from_integer<32>(std::numeric_limits< int64_t>::min());
}

BOOST_AUTO_TEST_SUITE_END() // result_wrapper

//  vim: ft=cpp:ts=2:sw=2:expandtab
