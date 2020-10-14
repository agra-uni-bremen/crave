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

#include "Evaluator.hpp"

namespace metaSMT {

namespace cardtags = metaSMT::logic::cardinality::tag;

  template<typename Context, typename Boolean>
  typename Context::result_type
  one_hot(Context &ctx, std::vector<Boolean> const &ps) {
    assert(ps.size() > 0 && "One hot encoding requires at least one input variable");

    if (ps.size() == 1) {
      return evaluate(ctx, logic::equal(ps[0], logic::True));
    }

    typename Context::result_type zero_rail = evaluate(ctx, ps[0]);
    typename Context::result_type one_rail = evaluate(ctx, Not(ps[0]));

    for (unsigned u = 1; u < ps.size() - 1; ++u) {
      zero_rail = evaluate(ctx, Ite(ps[u], one_rail, zero_rail));
      one_rail = evaluate(ctx, Ite(ps[u], logic::False, one_rail));
    }

    return evaluate(ctx, Ite(ps[ps.size()-1], one_rail, zero_rail));
  }

  template <typename Context, typename Boolean>
  typename Context::result_type
  cardinality_eq(Context &ctx, std::vector<Boolean> const &ps, unsigned cardinality) {
    return evaluate(ctx,
      cardinality::cardinality(cardtags::eq_tag(), ps, cardinality)
    );
  }

  template <typename Context, typename Boolean>
  typename Context::result_type
  cardinality_geq(Context &ctx, std::vector<Boolean> const &ps, unsigned cardinality) {
    return evaluate(ctx,
      cardinality::cardinality(cardtags::ge_tag(), ps, cardinality)
    );
  }

  template <typename Context, typename Boolean>
  typename Context::result_type
  cardinality_leq(Context &ctx, std::vector<Boolean> const &ps, unsigned cardinality) {
    return evaluate(ctx,
      cardinality::cardinality(cardtags::le_tag(), ps, cardinality)
    );
  }

  template <typename Context, typename Boolean>
  typename Context::result_type
  cardinality_gt(Context &ctx, std::vector<Boolean> const &ps, unsigned cardinality) {
    return evaluate(ctx,
      cardinality::cardinality(cardtags::gt_tag(), ps, cardinality)
    );
  }

  template <typename Context, typename Boolean>
  typename Context::result_type
  cardinality_lt(Context &ctx, std::vector<Boolean> const &ps, unsigned cardinality) {
    return evaluate(ctx,
      cardinality::cardinality(cardtags::lt_tag(), ps, cardinality)
    );
  }
} // metaSMT
