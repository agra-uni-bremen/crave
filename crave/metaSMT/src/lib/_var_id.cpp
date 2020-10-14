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

#include "../metaSMT/impl/_var_id.hpp"

#if defined(_WIN32) || (__cplusplus > 199711L) // essentially require C++11 on Windows

#include <atomic>
#define var_id_type std::atomic_uint

#else

#include <boost/version.hpp>
#if BOOST_VERSION >= 105500
#include <boost/atomic.hpp>
#define var_id_type boost::atomic_uint
#else
#define var_id_type unsigned;
#pragma message("Creating a new variable in metaSMT is not an atomic operation!")
#endif

#endif

unsigned metaSMT::impl::new_var_id()
{
  static var_id_type _id ( 0u );
  ++_id;
  return _id;
}
