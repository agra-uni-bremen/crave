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

#include "Logic.hpp"
#include "QF_BV.hpp"
#include "Array.hpp"
#include "Cardinality.hpp"

#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/size.hpp>

namespace metaSMT {

  namespace _all_logic_tags {
    typedef boost::mpl::joint_view< 
        logic::tag::Predicate_Tags
      , logic::QF_BV::tag::QF_BV_Tags 
      >::type all_Tags1;

    typedef boost::mpl::joint_view< 
        all_Tags1
      , logic::Array::tag::Array_Tags 
      >::type all_Tags2;

    typedef boost::mpl::joint_view< 
        all_Tags2
      , logic::cardinality::tag::Cardinality_Tags 
      >::type all_Tags3;

    typedef boost::mpl::copy<
       all_Tags3
       , boost::mpl::back_inserter< boost::mpl::vector<> >
      >::type all_Tags;
    //BOOST_MPL_ASSERT_RELATION( boost::mpl::size<allTags>::value, ==, 1 );
  }

#if BOOST_VARIANT_VISITATION_UNROLLING_LIMIT < 58
#warning "BOOST_VARIANT_VISITATION_UNROLLING_LIMIT is too small!"
#warning "Visiting metaSMT::Tag (e.g., via boost::put(.)) may unexpectedly fail."
#warning "Try to include <metaSMT/support/default_visitation_unrolling_limit.hpp>."
#endif // BOOST_VARIANT_VISITATION_UNROLLING_LIMIT

  typedef boost::make_variant_over< _all_logic_tags::all_Tags >::type Tag;

} // namespace metaSMT

//  vim: ft=cpp:ts=2:sw=2:expandtab
