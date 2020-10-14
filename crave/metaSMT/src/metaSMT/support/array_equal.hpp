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
#include "../API/Evaluator.hpp"
#include "../frontend/QF_BV.hpp"
#include "../frontend/Array.hpp"

namespace metaSMT {
  namespace support {

    template < typename Lhs, typename Rhs >
    struct ArrayEqualBase
    {
      ArrayEqualBase( Lhs lhs, Rhs rhs, unsigned index_width )
        : lhs_(lhs)
        , rhs_(rhs)
        , index_width_(index_width)
      {

      }

      Lhs lhs_;
      Rhs rhs_;
      unsigned index_width_;
    };

    template < typename Lhs, typename Rhs >
    struct ArrayEqual : ArrayEqualBase<Lhs, Rhs>
    {
      ArrayEqual( Lhs lhs, Rhs rhs, unsigned index_width )
        : ArrayEqualBase<Lhs, Rhs>(lhs, rhs, index_width)
      {
      }
    };

    template < typename Lhs, typename Rhs >
    struct ArrayNequal : ArrayEqualBase<Lhs, Rhs>
    {
      ArrayNequal( Lhs lhs, Rhs rhs, unsigned index_width )
        : ArrayEqualBase<Lhs, Rhs>(lhs, rhs, index_width)
      {
      }
    };

    template < typename Lhs, typename Rhs >
    ArrayEqual<Lhs, Rhs> array_equal( Lhs lhs, Rhs rhs, unsigned index_width )
    {
      return ArrayEqual<Lhs, Rhs>( lhs, rhs, index_width);
    }

    template < typename Lhs, typename Rhs >
    ArrayNequal<Lhs, Rhs> array_nequal( Lhs lhs, Rhs rhs, unsigned index_width )
    {
      return ArrayNequal<Lhs, Rhs>( lhs, rhs, index_width );
    }

  } // support

  template < typename Lhs, typename Rhs >
  struct Evaluator< support::ArrayEqual<Lhs, Rhs> > : public boost::mpl::true_ {

    template < typename Context >
    static typename Context::result_type
    eval(Context &ctx, support::ArrayEqual<Lhs, Rhs> const &c)
    {
      using namespace logic;
      using namespace logic::QF_BV;
      using namespace logic::Array;

      const unsigned width = c.index_width_;
      uint64_t index_limit = static_cast<uint64_t>(1ul) << width;

      typename Context::result_type result = evaluate( ctx, True );

      for ( uint64_t index = 0; index < index_limit; ++index ) {
        result = evaluate( ctx, And( result,
	  equal(
	    select( c.lhs_, bvuint(index, width) ),
	    select( c.rhs_, bvuint(index, width) )
	  ))
        );
      }

      return result;
    }
  };

  template < typename Lhs, typename Rhs >
  struct Evaluator< support::ArrayNequal<Lhs, Rhs> > : public boost::mpl::true_ {

    template < typename Context >
    static typename Context::result_type
    eval(Context &ctx, support::ArrayNequal<Lhs, Rhs> const &c)
    {
      using namespace logic;
      using namespace logic::QF_BV;
      using namespace logic::Array;

      const unsigned width = c.index_width_;
      uint64_t index_limit = static_cast<uint64_t>(1ul) << width;

      typename Context::result_type result = evaluate( ctx, False );

      for ( uint64_t index = 0; index < index_limit; ++index ) {
        result = evaluate( ctx, Or( result,
	  nequal(
	    select( c.lhs_, bvuint(index, width) ),
	    select( c.rhs_, bvuint(index, width) )
	  ))
        );
      }

      return result;
    }
  };
} // metaSMT
