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
#include <boost/mpl/bool.hpp>

namespace metaSMT {
  /**
   * \brief Evaluator API
   *
   * \ingroup API
   * \defgroup Evaluator Evaluator
   * @{
   */

  /**
   * \brief generic mechanism to extend the metaSMT primitives
   *
   * Evaluator provides a mechanism to enrich metaSMT's list of
   * primitives with arbitary types.  For each new type T a
   * specialization of metaSMT::Evaluator<T> has to be implemened
   * which provides a static member function eval(.) to convert the
   * new type into a result_type.
   *
   * See BoolEvaluator.hpp for an example.
   *
   * WARNING: Note that specializations of Evaluator must be placed
   *  into the namespace metaSMT.
   */
  template < typename Tag >
  struct Evaluator : public boost::mpl::false_
  {}; // evaluator
  /**@}*/
} // metaSMT
