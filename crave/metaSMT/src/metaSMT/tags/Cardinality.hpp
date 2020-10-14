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

#include "attribute.hpp"
#include <boost/variant.hpp>
#include <boost/mpl/vector.hpp>

namespace metaSMT {
  namespace logic {
    namespace cardinality {
      namespace tag {

#define PRINT(Tag, body) template<typename STREAM> \
  friend STREAM & operator<< (STREAM & out, Tag const & ) \
  {  out << body; return out; }
#define TAG( NAME, ATTR ) struct NAME##_tag { \
  typedef attr::ATTR attribute; \
  bool operator<(NAME##_tag const &) const {return false;} \
  PRINT(NAME##_tag, #NAME) \
};

        TAG(lt, nary)
        TAG(le, nary)
        TAG(eq, nary)
        TAG(ge, nary)
        TAG(gt, nary)

#undef PRINT
#undef TAG

        // tag variant Cardinality_Tags
        typedef boost::mpl::vector<
          lt_tag
        , le_tag
        , eq_tag
        , ge_tag
        , gt_tag
        >::type Cardinality_Tags;

        typedef boost::make_variant_over< Cardinality_Tags >::type Cardinality_Tag;

      } // namespace cardinality
    } // namespace tag
  } // namespace logic
} // namespace metaSMT

//  vim: ft=cpp:ts=2:sw=2:expandtab
