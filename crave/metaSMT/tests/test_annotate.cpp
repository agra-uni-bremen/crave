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

#include <metaSMT/frontend/QF_BV.hpp>
#include <boost/test/unit_test.hpp>

// lazy headers

using namespace metaSMT;
using namespace logic;
using namespace logic::QF_BV;
namespace proto = boost::proto;

BOOST_FIXTURE_TEST_SUITE(annotate_t, Solver_Fixture )

struct Lookup {
  std::map<unsigned, std::string> map_;

  std::string operator()(unsigned id) {
    return map_[id];
  }

  void insert(predicate p, std::string const &name) {
    map_.insert(std::make_pair(boost::proto::value(p).id, name));
  }
};

BOOST_AUTO_TEST_SUITE_END() //lazy_t

//  vim: ft=cpp:ts=2:sw=2:expandtab

