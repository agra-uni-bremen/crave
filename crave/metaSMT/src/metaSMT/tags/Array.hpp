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
#ifndef HEADER_metaSMT_TAG_ARRAY_HPP
#define HEADER_metaSMT_TAG_ARRAY_HPP

#include "Logic.hpp"

#include <boost/mpl/vector.hpp>
#include <boost/variant.hpp>

namespace metaSMT {
  namespace logic {
    namespace Array {
      namespace tag {

#define PRINT(Tag, body) template<typename STREAM> \
  friend STREAM & operator<< (STREAM & out, Tag const & ) \
  { return (out << body); }
#define TAG( NAME, ATTR ) struct NAME##_tag { \
  typedef attr::ATTR attribute; \
  bool operator<(NAME##_tag const &) const {return false;} \
  PRINT(NAME##_tag, #NAME) \
};

        // array variable tag
        struct array_var_tag {
          typedef attr::ignore attribute;

          unsigned id;
          unsigned elem_width;
          unsigned index_width;

          template<typename STREAM>
          friend STREAM & operator<< (STREAM & out, array_var_tag const & self) {
            return (out << "array_var_tag[" << self.id  << ',' << self.elem_width << ',' << self.index_width << "]");
          }

          bool operator< (array_var_tag const & other) const {
            return id < other.id;
          }
        };

        TAG(select, ignore)
        TAG(store, ignore)

#undef PRINT
#undef TAG

        typedef boost::mpl::vector<
          nil
        , select_tag
        , store_tag
        , array_var_tag
        >::type Array_Tags;

        typedef boost::make_variant_over<Array_Tags>::type Array_Tag;

      } // namespace metaSMT::logic::Array::tag
    } // namespace metaSMT::logic::Array
  } // namespace metaSMT::logic
} // namespace metaSMT
#endif // HEADER_metaSMT_TAG_ARRAY_HPP
//  vim: ft=cpp:ts=2:sw=2:expandtab
