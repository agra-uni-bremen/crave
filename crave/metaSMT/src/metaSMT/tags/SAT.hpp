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

#include <boost/variant.hpp>
#include <boost/mpl/vector.hpp>

namespace metaSMT {

  namespace SAT {
    namespace tag {

    // sat literal tag
    // 0 is invalid
    // n is variable n
    // -m is variable n negated
    struct lit_tag { 
      int id; 
      template<typename STREAM>
      friend STREAM & operator<< (STREAM & out, lit_tag const & self)
      {  out << "sat_lit[" << self.id  << "]"; return out; }
      bool operator< (lit_tag const & other) const { return id < other.id; }
      lit_tag operator- () const { lit_tag l = { -id }; return l; }
      int var() const {return id >= 0 ? id: -id; }
    };

    struct c_tag { 
      template<typename STREAM>
      friend STREAM & operator<< (STREAM & out, c_tag const & )
      {  out << "or"; return out; }
      bool operator< (c_tag const & ) const { return false; }
    };

    struct not_tag { 
      template<typename STREAM>
      friend STREAM & operator<< (STREAM & out, not_tag const & )
      {  out << "not"; return out; }
      bool operator< (not_tag const & ) const { return false; }
    };

      // tag variant SAT
      typedef boost::mpl::vector<
          lit_tag
        , c_tag
      >::type SAT_Tags;

      typedef boost::make_variant_over< SAT_Tags >::type SAT_Tag;

    } // namespace tag
  } // namespace SAT
} // namespace metaSMT

//  vim: ft=cpp:ts=2:sw=2:expandtab

