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

#pragma once

#include <boost/mpl/vector.hpp>
#include <boost/variant.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/logic/tribool_io.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <limits>
#include <vector>

#pragma warning (disable : 4800)
#pragma warning (disable : 4146)

namespace metaSMT {
  /**
   * return value wrapper
   *
   */
  class result_wrapper {

    // converter types
    struct as_vector_tribool
    {
      typedef std::vector<boost::logic::tribool > result_type;

      result_type operator() ( result_type const & v ) const {
        return v;
      }
      result_type operator() ( boost::logic::tribool const & t ) const {
        result_type ret (1, t);
        return ret;
      }

      result_type operator() (bool b ) const {
        result_type ret (1);
        ret[0]=b;
        return ret;
      }

      result_type operator() ( std::vector<bool> const & vb) const {
        result_type ret (vb.size());
        for (unsigned i = 0; i < vb.size(); ++i) {
          ret[i] = static_cast<bool>(vb[i]);
        }
        return ret;
      }

      result_type operator() ( std::string const & s ) const {
        unsigned size = s.size();
        result_type ret(size);
        for (unsigned i = 0; i < size; ++i) {
          switch(s[size-i-1]){
            case '0':
              ret[i] = false; break;
            case '1':
              ret[i] = true; break;
            default:
              ret[i] = boost::logic::indeterminate;
          }
        }
        return ret;
      }
    };

    struct as_tribool
    {
      typedef boost::logic::tribool result_type;

      result_type operator() ( result_type const & v ) const {
        return v;
      }
      result_type operator() ( std::vector<boost::logic::tribool> const & t ) const {
        boost::logic::tribool ret = false;
        for (unsigned i = 0; i < t.size(); ++i) {
          ret = ret||t[i];
        }
        return ret;
      }

      result_type operator() (bool b ) const {
        return b;
      }

      result_type operator() ( std::vector<bool> const & vb) const {
        for (unsigned i = 0; i < vb.size(); ++i) {
          if (vb[i]) return true;
        }
        return false;
      }

      result_type operator() ( std::string const & s ) const {
        // true if any bit is true;
        boost::logic::tribool ret = false;
        for (unsigned i = 0; i < s.size(); ++i) {
        switch(s[i]){
          case '0':
            break;
          case '1':
            return true;
          default:
            ret = boost::logic::indeterminate;
          }
        }
        return ret;
      }
    };


    struct as_vector_bool
    {
      typedef  std::vector<bool> result_type;

      result_type operator() ( result_type const & v ) const
      { return v; }

      result_type operator() ( boost::logic::tribool t) const {
        result_type ret(1);
        ret[0] = static_cast<bool>(t);
        return ret;
      }

      result_type operator() ( bool b ) const {
        result_type ret(1);
        ret[0] = b;
        return ret;
      }

      result_type operator() ( std::vector< boost::logic::tribool > vt ) const {
        result_type ret(vt.size());
        for (unsigned i = 0; i < vt.size(); ++i)
         ret[i] = static_cast<bool>(vt[i]);
        return ret;
      }

      result_type operator() ( std::string s) const {
        result_type ret(s.size());
        for (unsigned i = 0; i < s.size(); ++i)
         ret[i] = (s[s.size() - i -1] == '1');
        return ret;
      }
    };

    struct as_string
    {
      typedef std::string result_type;

      result_type operator() ( result_type const & v ) const {
        return v;
      }

      result_type operator() ( bool b ) const {
        return b ? "1" : "0";
      }

      result_type operator() ( boost::logic::tribool val ) const {
        if( boost::logic::indeterminate(val) ) {
          return "X";
        } else {
          return val ? "1" : "0";
        }
      }

      result_type operator() ( std::vector< boost::logic::tribool > val ) const {
        unsigned size = val.size();
        std::string s(size,'\0');
        for (unsigned i = 0; i < size; ++i) {
          if( boost::logic::indeterminate(val[i]) ) {
            s[size-i-1] = 'X';
          } else {
            s[size-i-1] = val[i] ? '1' : '0';
          }
        }
        return s;
      }

      result_type operator() ( std::vector< bool > val ) const {
        unsigned size = val.size();
        std::string s(size,'\0');
        for (unsigned i = 0; i < size; ++i) {
          s[size-i-1] = val[i] ? '1' : '0';
        }
        return s;
      }
    };

    struct check_if_X
    {
      typedef bool result_type;

      template<typename T>
      result_type operator() ( T const & ) const {
        return false;
      }

      result_type operator() ( boost::logic::tribool val ) const {
        return boost::logic::indeterminate(val);
      }

      result_type operator() ( std::vector< boost::logic::tribool > val ) const {
        unsigned size = val.size();
        for (unsigned i = 0; i < size; ++i) {
          if( boost::logic::indeterminate(val[i]) )
            return true;
        }
        return false;
      }

      result_type operator() ( std::string val ) const {
        unsigned size = val.size();
        for (unsigned i = 0; i < size; ++i) {
          if( val[i] == 'x' || val[i] == 'X' )
            return true;
        }
        return false;
      }
    };

    public:
      typedef boost::mpl::vector<
        bool
        , std::vector<boost::logic::tribool>
        , std::vector<bool>
        , std::string
        , boost::logic::tribool
        > result_types_list;
      typedef boost::make_variant_over<result_types_list>::type result_type;

    public:
      result_wrapper() : r (std::string("X")) { }
      result_wrapper( result_type r ) : r (r) { }
      result_wrapper( boost::logic::tribool t ) : r (t) { }
      result_wrapper( bool b ) : r (boost::logic::tribool(b)) { }
      result_wrapper( std::string const & s ) : r (boost::algorithm::to_upper_copy(s)) { }
      result_wrapper( const char* s ) : r(boost::algorithm::to_upper_copy(std::string(s))) { }
      result_wrapper( const char c )
      : r (c=='1' ? boost::logic::tribool(true)
        : (c=='0' ? boost::logic::tribool(false)
        : boost::logic::tribool(boost::logic::indeterminate))
        )
      { }

      result_wrapper( uint64_t value, unsigned width ) {
        std::string s(width, '0');
        for (unsigned i = 0; i < width; i++) {
          if (value & 1) s[width - i - 1] = '1';
          value >>= 1;
        }
        r = s;
      }

      template<typename T1, typename T2>
      result_wrapper( boost::dynamic_bitset<T1, T2> const & bs ) {
        std::string s;
        boost::to_string(bs, s);
        r = s;
      }

      operator std::vector<bool> () const {
        return boost::apply_visitor(as_vector_bool(), r);
      }

      operator std::vector<boost::logic::tribool> () const {
        return boost::apply_visitor(as_vector_tribool(), r);
      }

      operator std::string () const {
        return boost::apply_visitor(as_string(), r);
      }

      template<typename T1, typename T2>
      operator boost::dynamic_bitset<T1, T2> () const {
        std::vector<bool> val = *this;
        boost::dynamic_bitset<T1, T2> ret(val.size());
        for (unsigned i = 0; i < val.size(); ++i) {
          ret[i]=val[i];
        }
        return ret;
      }

      result_wrapper & throw_if_X() {
        if ( boost::apply_visitor( check_if_X(), r) ) {
          throw std::string("contains X");
        }
        return *this;
      }

      typedef boost::optional< boost::function0<bool> > Rng;

      result_wrapper & randX( Rng rng = Rng() ) {
        _rng=rng;
        return *this;
      }

      bool random_bit() const {
        return (*_rng)();
      }

      /**
       * For signed type with smaller bit-width than needed, the sign is kept and the rest is truncated.
       * The result of this operator might thus be different from the compiler's behavior as a signed
       * downcast is implementation-defined.
       */
      template< typename Integer>
      operator Integer () const {
        Integer ret = 0;
        std::string val = *this;
        if( boost::is_signed<Integer>::value && val[0] == '1' ) ret = static_cast<Integer>(-1);
        unsigned first = 0;
        if (std::numeric_limits<Integer>::digits < val.size()) // downcasting
          first = val.size() - std::numeric_limits<Integer>::digits;
        for (unsigned i = 0; i < val.size() && i < std::numeric_limits<Integer>::digits; ++i)
        {
          ret <<= static_cast<Integer>(1);
          switch ( val[first + i] ) {
            case '0':
              break;
            case '1':
              ret |= Integer(1);
              break;
            default:
              ret |= Integer( _rng && random_bit());
          }
        }
        return ret;
      }

      operator bool () const {
        boost::logic::tribool b = *this;
        if (b) return true;
        else if (!b) return false;
        else return _rng && random_bit();
      }

      operator boost::logic::tribool () const {
        return boost::apply_visitor(as_tribool(), r);
      }

      friend std::ostream &
      operator<< (std::ostream & out, result_wrapper const & rw) {
        std::string o = rw;
        out << o ;
        return out;
      }


    protected:
    result_type r;
    Rng _rng;
  };

} // namespace metaSMT

//  vim: ft=cpp:ts=2:sw=2:expandtab
