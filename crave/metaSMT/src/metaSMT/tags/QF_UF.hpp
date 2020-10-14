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
#ifndef HEADER_metaSMT_TAG_QF_UF_HPP
#define HEADER_metaSMT_TAG_QF_UF_HPP

#include "Logic.hpp"
#include "../types/Types.hpp"

#include <boost/mpl/vector.hpp>
#include <boost/variant.hpp>

namespace metaSMT {
  namespace logic {
    namespace QF_UF {
      namespace tag {

#define PRINT(Tag, body) template<typename STREAM> \
  friend STREAM & operator<< (STREAM & out, Tag const & self) \
  { return (out << body); }

        // uninterpreted function variable tag
        struct function_var_tag {
          typedef attr::ignore attribute;

          unsigned id;
          type::any_type result_type;
          std::vector<type::any_type> args;

          PRINT(function_var_tag, "function_var_tag[" << self.id << "]")
          bool operator< (function_var_tag const & other) const {
            return id < other.id;
          }
        };

#undef PRINT

        typedef boost::mpl::vector<
          nil
        , function_var_tag
        >::type QF_UF_Tags;

        typedef boost::make_variant_over<QF_UF_Tags>::type QF_UF_Tag;

      } // namespace metaSMT::logic::QF_UF::tag
    } // namespace metaSMT::logic::QF_UF
  } // namespace metaSMT::logic
} // namespace metaSMT
#endif // HEADER_metaSMT_TAG_ARRAY_HPP
//  vim: ft=cpp:ts=2:sw=2:expandtab
