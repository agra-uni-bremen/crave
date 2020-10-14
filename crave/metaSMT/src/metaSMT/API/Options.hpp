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

#include "../Features.hpp"

namespace metaSMT {
  struct setup_option_map_cmd { typedef void result_type; };
  struct get_option_cmd { typedef std::string result_type; };
  struct set_option_cmd { typedef void result_type; };

  template <typename Context_ >
  std::string get_option( Context_ &ctx, std::string const &key ) {
    BOOST_MPL_ASSERT_MSG(
      ( features::supports<Context_, get_option_cmd>::value )
    , context_does_not_support_get_option_api
    , ()
    );
    return ctx.command(get_option_cmd(), key );
  }

  template <typename Context_ >
  std::string get_option( Context_ &ctx, std::string const &key, std::string const &default_value ) {
    BOOST_MPL_ASSERT_MSG(
      ( features::supports<Context_, get_option_cmd>::value )
    , context_does_not_support_get_option_api
    , ()
    );
    return ctx.command(get_option_cmd(), key, default_value );
  }

  template <typename Context_ >
  void set_option( Context_ &ctx, std::string const &key, std::string const &value ) {
    BOOST_MPL_ASSERT_MSG(
      ( features::supports<Context_, set_option_cmd>::value )
    , context_does_not_support_set_option_api
    , ()
    );
    ctx.command(set_option_cmd(), key, value );
  }
} // metaSMT
