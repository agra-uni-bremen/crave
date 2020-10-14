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

namespace metaSMT {
  namespace type {
    struct Array {
      Array() {}

      Array(unsigned elem_width,
            unsigned index_width)
        : elem_width(elem_width)
        , index_width(index_width)
      {}

      template <typename STREAM>
      friend STREAM & operator<<(STREAM & out, Array const &self) \
      { return (out << "Array [" << self.elem_width << "," << self.index_width << "]"); }

      unsigned elem_width;
      unsigned index_width;
    };

    // equality of Array types
    template <typename T>
    typename boost::enable_if< boost::is_same<type::Array, T>, bool>::type
    operator== (type::Array const &a , T const &b)                      \
    { return a.elem_width == b.elem_width && a.index_width == b.index_width; }

    template <typename T>
    typename boost::disable_if< boost::is_same<type::Array, T>, bool>::type
    operator== (type::Array const & , T const & )  \
    { return false; }

  } // type
} // metaSMT
