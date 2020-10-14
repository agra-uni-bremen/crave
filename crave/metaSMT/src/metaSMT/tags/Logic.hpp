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
  struct nil { 
    typedef attr::ignore attribute;

    bool operator< (nil const &) const { return false; };
    template<typename STREAM> friend STREAM & 
      operator<< (STREAM & out, nil const & ) {  out << "nil"; return out; }
  };

  namespace logic {
    namespace tag {

    // variable tag
    struct var_tag { unsigned id;
      typedef attr::ignore attribute;

      template<typename STREAM>
      friend STREAM & operator<< (STREAM & out, var_tag const & self) {
        return (out << "var_tag[" << self.id  << "]");
      }

      bool operator< (var_tag const & other) const {
        return id < other.id;
      }
    };

#define PRINT(Tag, body) template<typename STREAM> \
  friend STREAM & operator<< (STREAM & out, Tag const & ) \
  {  out << body; return out; }
#define TAG( NAME, ATTR ) struct NAME##_tag { \
  typedef attr::ATTR attribute; \
  bool operator<(NAME##_tag const &) const {return false;} \
  PRINT(NAME##_tag, #NAME) \
};

    // constants
    TAG(true, constant)
    TAG(false, constant)
    TAG(bool, constant)

    // unary
    TAG(not, unary)

    // binary
    TAG(equal, binary) // chainable
    TAG(nequal, binary)
    TAG(distinct, binary) // pairwise
    TAG(implies, binary) // right_assoc

    TAG(and, left_assoc)
    TAG(nand, binary)
    TAG(or, left_assoc)
    TAG(nor, binary)
    TAG(xor, binary) // left_assoc
    TAG(xnor, binary)

    // ternary
    TAG(ite, ternary)

#undef PRINT
#undef TAG
    //
      // tag variant Predicate
      typedef boost::mpl::vector<
          false_tag
        , true_tag
        , not_tag
        , equal_tag
        , nequal_tag
        , distinct_tag
        , and_tag
        , nand_tag
        , or_tag
        , nor_tag
        , xor_tag
        , xnor_tag
        , implies_tag
        , ite_tag
        , var_tag
      >::type Predicate_Tags;

      typedef boost::make_variant_over< Predicate_Tags >::type Predicate_Tag;

    } // namespace tag
  } // namespace logic
} // namespace metaSMT

//  vim: ft=cpp:ts=2:sw=2:expandtab
