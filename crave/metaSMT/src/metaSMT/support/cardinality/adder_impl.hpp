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

#include "object.hpp"
#include <metaSMT/frontend/QF_BV.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/function.hpp>
#include <queue>

namespace metaSMT {
  namespace cardinality {
    namespace adder {
    namespace cardtags= logic::cardinality::tag;
      template <typename Context, typename Boolean>
      typename Context::result_type
      cardinality_any( Context &ctx, std::vector<Boolean> const &ps ) {
        typedef typename Context::result_type result_type;

	/*
        boost::function< result_type ( result_type const &, result_type const & ) >
          lazy_HA_sum = metaSMT::lazy( ctx, logic::Xor(arg1, arg2) );

        boost::function< result_type ( result_type const &, result_type const & ) >
          lazy_HA_carry = metaSMT::lazy( ctx, logic::And(arg1, arg2) );*/
	/*	
        boost::function< result_type ( result_type const &, result_type const &, result_type const & ) >
          lazy_FA_sum = metaSMT::lazy( ctx, logic::Xor(logic::Xor(arg1, arg2), arg3) );*/

        // boost::function< result_type ( result_type const &, result_type const &, result_type const & ) >
        //   lazy_FA_carry = metaSMT::lazy( ctx, Xor(And(arg1, arg2), And(Xor(arg1, arg2), arg3)) );

        // boost::function< result_type ( result_type const &, result_type const &, result_type const & ) >
        //   lazy_FA_carry = metaSMT::lazy( ctx, Or(And(arg1, arg2), And(Xor(arg1, arg2), arg3)) );

        // boost::function< result_type ( result_type const &, result_type const &, result_type const & ) >
        //   lazy_FA_carry = metaSMT::lazy( ctx, Ite(arg1, Ite(arg2, bit1, arg3), Ite(arg2, arg3, bit0)) );
	/*
        boost::function< result_type ( result_type const &, result_type const &, result_type const & ) >
          lazy_FA_carry = metaSMT::lazy( ctx, logic::Ite(arg1, logic::Or(arg2, arg3), logic::And(arg2, arg3)) );*/

        std::queue< result_type > line0, line1;
        std::queue< result_type > *wq0 = &line0;
        std::queue< result_type > *wq1 = &line1;
        std::vector< result_type > r;

        // initialize wq0
        for ( unsigned u = 0; u < ps.size(); ++u ) {
          wq0->push( evaluate(ctx, ps[u]) );
      }

        while ( !wq0->empty() ) {
          while ( wq0->size() >= 3 ) {
            result_type x = wq0->front(); wq0->pop();
            result_type y = wq0->front(); wq0->pop();
            result_type z = wq0->front(); wq0->pop();
            wq0->push( evaluate(ctx, logic::Xor(logic::Xor(x, y), z) ));
 	    
            wq1->push( evaluate(ctx, logic::Ite(x, logic::Or(y, z), logic::And(y, z)) ) );
          }

          if ( wq0->size() == 2 ) {
            result_type x = wq0->front(); wq0->pop();
            result_type y = wq0->front(); wq0->pop();
            wq0->push( evaluate(ctx, logic::Xor(x,y)) );
            wq1->push( evaluate(ctx, logic::And(x,y)) );
          }

          r.push_back( wq0->front() );
          wq0->pop();

          swap(wq0, wq1);
        }

        logic::QF_BV::bitvector n = logic::QF_BV::new_bitvector( r.size() );
        for ( unsigned u = 0; u < r.size(); ++u ) {
          assertion( ctx, logic::equal(logic::QF_BV::extract(u, u, n), logic::Ite(r[u], logic::QF_BV::bit1, logic::QF_BV::bit0)) );
        }
        
        return evaluate( ctx, n );

        // convert result to bitvector
        //result_type n = evaluate(ctx, logic::Ite(r[0], logic::QF_BV::bit1, logic::QF_BV::bit0));
        //for ( unsigned u = 1; u < r.size(); ++u ) {
        //  n = evaluate(ctx, concat(logic::Ite(r[u], logic::QF_BV::bit1, logic::QF_BV::bit0), n));
        //}
        //return n;
      }

      template < typename Context, typename Boolean, typename Tag >
      typename Context::result_type
      cardinality(Context & , cardinality::Cardinality<Tag, Boolean> const & ) {
        /** error: unknown tag **/
        BOOST_MPL_ASSERT_NOT( (boost::mpl::contains<cardtags::Cardinality_Tags, Tag>) );
      }

      template <typename Context, typename Boolean>
      typename Context::result_type
      cardinality(Context &ctx, cardinality::Cardinality<cardtags::eq_tag, Boolean> const &c) {
        std::vector<Boolean> const &ps = c.ps;
        unsigned const cardinality = c.cardinality;

        assert(ps.size() > 0 && "Equal cardinality constraint requires at least one input variable");
        unsigned const w = log2( ps.size() ) + 1;
        return evaluate(ctx, logic::equal(cardinality_any(ctx, ps), logic::QF_BV::bvuint(cardinality, w)));
      }

      template <typename Context, typename Boolean>
      typename Context::result_type
      cardinality(Context &ctx, cardinality::Cardinality<cardtags::le_tag, Boolean> const &c) {
        std::vector<Boolean> const &ps = c.ps;
        unsigned const cardinality = c.cardinality;

        assert(ps.size() > 0 && "Lower equal cardinality constraint requires at least one input variable");
        unsigned const w = log2( ps.size() ) + 1;
        return evaluate(ctx, bvule(cardinality_any(ctx, ps), logic::QF_BV::bvuint(cardinality, w)));
      }

      template <typename Context, typename Boolean>
      typename Context::result_type
      cardinality(Context &ctx, cardinality::Cardinality<cardtags::lt_tag, Boolean> const &c) {
        std::vector<Boolean> const &ps = c.ps;
        unsigned const cardinality = c.cardinality;

        assert(ps.size() > 0 && "Lower than cardinality constraint requires at least one input variable");
        unsigned const w = log2( ps.size() ) + 1;
        return evaluate(ctx, bvult(cardinality_any(ctx, ps), logic::QF_BV::bvuint(cardinality, w)));
      }

      template <typename Context, typename Boolean>
      typename Context::result_type
      cardinality(Context &ctx, cardinality::Cardinality<cardtags::gt_tag, Boolean> const &c) {
        std::vector<Boolean> const &ps = c.ps;
        unsigned const cardinality = c.cardinality;

        assert(ps.size() > 0 && "Greater than cardinality constraint requires at least one input variable");
        unsigned const w = log2( ps.size() ) + 1;
        return evaluate(ctx, bvugt(cardinality_any(ctx, ps), logic::QF_BV::bvuint(cardinality, w)));
      }

      template <typename Context, typename Boolean>
      typename Context::result_type
      cardinality(Context &ctx, cardinality::Cardinality<cardtags::ge_tag, Boolean> const &c) {
        std::vector<Boolean> const &ps = c.ps;
        unsigned const cardinality = c.cardinality;

        assert(ps.size() > 0 && "Greater equal cardinality constraint requires at least one input variable");
        unsigned const w = log2( ps.size() ) + 1;
        return evaluate(ctx, bvuge(cardinality_any(ctx, ps), logic::QF_BV::bvuint(cardinality, w)));
      }
    } // adder
  } // cardinality
} // metaSMT

