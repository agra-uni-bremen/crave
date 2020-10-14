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
  struct assertion_cmd { typedef void result_type; };
  
  /**
   * \brief Assertion API
   *
   *
   * \code
   *  DirectSolver_Context< Boolector > ctx;
   *
   *  assertion(ctx,  True);
   *  solve(ctx) == true;
   *
   *  assertion(ctx, equal(False, False))
   *  solve(ctx) == false;
   * \endcode
   * \ingroup API
   * \defgroup Assertion Assertion
   * @{
   */

  /**
   * \brief add an assumption to the current context
   *
   * \param ctx The metaSMT Context
   * \param e Any Boolean expression
   */
  template <typename Context_, typename Expr_>
  void assertion( Context_ & ctx, Expr_ const & e )
  {
    BOOST_MPL_ASSERT_MSG(
        ( features::supports< Context_, assertion_cmd>::value),
        context_does_not_support_assertion_api,
        ()
    );

    ctx.command(assertion_cmd(),  evaluate(ctx, e) );
  }
  /**@}*/
} /* metaSMT */
