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

#include "../result_wrapper.hpp"
#include "../tags/Logic.hpp"
#include "../tags/QF_BV.hpp"
#include "../tags/QF_UF.hpp"
#include "../tags/Array.hpp"

#include <list>

#include <boost/any.hpp>
#include <boost/tuple/tuple.hpp>

#include <yices.h>

namespace metaSMT {
namespace solver {

namespace predtags = ::metaSMT::logic::tag;
namespace bvtags = ::metaSMT::logic::QF_BV::tag;
namespace uftags = ::metaSMT::logic::QF_UF::tag;
namespace arraytags = ::metaSMT::logic::Array::tag;

namespace detail {

struct yices_type_visitor : boost::static_visitor<type_t> {
  yices_type_visitor(context_t &ctx) : ctx(ctx) {}

  type_t operator()(type::Boolean const &) const { return yices_bool_type(); }

  type_t operator()(type::BitVector const &arg) const { return yices_bv_type(arg.width); }

  type_t operator()(type::Array const &arg) const {
    type_t index_type = yices_bv_type(arg.index_width);
    type_t var_type = yices_bv_type(arg.elem_width);
    type_t function_type = yices_function_type(1, &index_type, var_type);
    return yices_new_uninterpreted_term(function_type);
  }

  context_t &ctx;
};  // yices_type_visitor
}

template <class T>
struct ObjectCounter {
  static int count;
};

template <class T>
int ObjectCounter<T>::count = 0;

template <bool RealIncreamentalMode = false>
class Yices2Impl {
 private:
  typedef boost::tuple<uint64_t, unsigned>  bvuint_tuple;
  typedef boost::tuple< int64_t, unsigned>  bvsint_tuple;

 public:
  typedef term_t result_type;
  typedef std::list<term_t> Exprs;

 private:
  Exprs assumptions_;
  Exprs assertions_;
  bool isPushed_;
  context_t *ctx;

  std::string yices_error_message_with_prefix(std::string prefix) { return prefix + yices_error_string(); }

  term_t throw_error(term_t value) {
    if (value == NULL_TERM) {
      throw std::runtime_error(yices_error_message_with_prefix("throw error: "));
    }
    return value;
  }

 public:
  Yices2Impl() : isPushed_(false) {
    if (ObjectCounter<Yices2Impl>::count == 0) {
      yices_init();
    }
    ctx_config_t *config = yices_new_config();
    yices_default_config_for_logic(config, "QF_AUFBV");
    if (RealIncreamentalMode)
      yices_set_config(config, "mode", "push-pop");
    else
      yices_set_config(config, "mode", "one-shot");
    ctx = yices_new_context(config);
    yices_free_config(config);
    ObjectCounter<Yices2Impl>::count++;
  }

  ~Yices2Impl() {
    yices_free_context(ctx);
    ObjectCounter<Yices2Impl>::count--;
    if (ObjectCounter<Yices2Impl>::count == 0) {
      yices_exit();
    }
  }

  void assertion(result_type e) { assertions_.push_back(e); }

  void assumption(result_type e) { assumptions_.push_back(e); }

  bool solve() {
    if (RealIncreamentalMode) removeOldAssumptions();
    else yices_reset_context(ctx);
    pushAssertions();
    pushAssumptions();
    smt_status_t status = yices_check_context(ctx, NULL);
    return (status == STATUS_SAT);
  }

  result_wrapper read_value(result_type var) {
    model_t *model = yices_get_model(ctx, true);
    if (!model) {
      throw std::runtime_error(std::string("read_value: Cannot get model from Yices2"));
    }
    if (yices_term_is_bitvector(var)) {
      int32_t bits = yices_term_bitsize(var);
      int32_t *array = new int32_t[bits];
      std::vector<bool> booleans;
      if (yices_get_bv_value(model, var, array) != -1) {
        for (int32_t i = 0; i < bits; i++) {
          booleans.push_back(array[i]);
        }
        delete[] array;
        return result_wrapper(booleans);
      } else {
        throw std::runtime_error(std::string("read_value: Failed to read bitvector from Yices2"));
      }
    } else if (yices_term_is_bool(var)) {
      int32_t value = 0;
      if (yices_get_bool_value(model, var, &value) == 0) {
        return result_wrapper((bool)value);
      } else {
        throw std::runtime_error(std::string("read_value: Failed to read bool from Yices2"));
      }
    } else if (yices_term_is_tuple(var)) {
      throw std::runtime_error(std::string("read_value: Reading tuple from Yices2 not yet supported"));
    }

    throw std::runtime_error(yices_error_message_with_prefix("read_value: "));
    return result_wrapper(false);
  }

  // predtags

  result_type operator()(predtags::var_tag const &, boost::any) {
    type_t bool_type = yices_bool_type();
    return yices_new_uninterpreted_term(bool_type);
  }

  result_type operator()(predtags::false_tag, boost::any) { return yices_false(); }

  result_type operator()(predtags::true_tag, boost::any) { return yices_true(); }

  result_type operator()(predtags::not_tag, result_type e) { return yices_not(e); }

  result_type operator()(predtags::equal_tag const &, result_type a, result_type b) {
    if (yices_term_is_bool(a) && yices_term_is_bool(b)) {
      return yices_iff(a, b);
    } else {
      return yices_eq(a, b);
    }
  }

  result_type operator()(predtags::nequal_tag const &, result_type a, result_type b) { return yices_neq(a, b); }

  result_type operator()(predtags::distinct_tag const &, result_type a, result_type b) { return yices_neq(a, b); }

  result_type operator()(predtags::and_tag const &, result_type a, result_type b) {
    return throw_error(yices_and2(a, b));
  }

  result_type operator()(predtags::nand_tag const &, result_type a, result_type b) {
    return throw_error(yices_not(yices_and2(a, b)));
  }

  result_type operator()(predtags::or_tag const &, result_type a, result_type b) {
    return throw_error(yices_or2(a, b));
  }

  result_type operator()(predtags::nor_tag const &, result_type a, result_type b) {
    return throw_error(yices_not(yices_or2(a, b)));
  }

  result_type operator()(predtags::xor_tag const &, result_type a, result_type b) {
    return throw_error(yices_xor2(a, b));
  }

  result_type operator()(predtags::xnor_tag const &, result_type a, result_type b) {
    return throw_error(yices_not(yices_xor2(a, b)));
  }

  result_type operator()(predtags::implies_tag const &, result_type a, result_type b) { return yices_implies(a, b); }

  result_type operator()(predtags::ite_tag, result_type a, result_type b, result_type c) { return yices_ite(a, b, c); }
  // BVTAG

  result_type operator()(bvtags::bit0_tag, boost::any) { return yices_bvconst_zero(1); }

  result_type operator()(bvtags::bit1_tag, boost::any) { return yices_bvconst_one(1); }

  result_type operator()(bvtags::bvnot_tag, result_type e) { return yices_bvnot(e); }

  result_type operator()(bvtags::bvneg_tag, result_type e) { return yices_bvneg(e); }

  result_type operator()(bvtags::bvand_tag, result_type a, result_type b) { return yices_bvand2(a, b); }

  result_type operator()(bvtags::bvnand_tag, result_type a, result_type b) { return yices_bvnand(a, b); }

  result_type operator()(bvtags::bvor_tag, result_type a, result_type b) { return yices_bvor2(a, b); }

  result_type operator()(bvtags::bvnor_tag, result_type a, result_type b) { return yices_bvnor(a, b); }

  result_type operator()(bvtags::bvxor_tag, result_type a, result_type b) { return yices_bvxor2(a, b); }

  result_type operator()(bvtags::bvxnor_tag, result_type a, result_type b) { return yices_bvxnor(a, b); }

  result_type operator()(bvtags::bvcomp_tag, result_type a, result_type b) { return yices_redcomp(a, b); }

  result_type operator()(bvtags::bvadd_tag, result_type a, result_type b) { return yices_bvadd(a, b); }

  result_type operator()(bvtags::bvmul_tag, result_type a, result_type b) { return yices_bvmul(a, b); }

  result_type operator()(bvtags::bvsub_tag, result_type a, result_type b) { return yices_bvsub(a, b); }

  result_type operator()(bvtags::bvsrem_tag, result_type a, result_type b) { return yices_bvsrem(a, b); }

  result_type operator()(bvtags::bvsdiv_tag, result_type a, result_type b) { return yices_bvsdiv(a, b); }

  result_type operator()(bvtags::bvurem_tag, result_type a, result_type b) { return yices_bvrem(a, b); }

  result_type operator()(bvtags::bvudiv_tag, result_type a, result_type b) { return yices_bvdiv(a, b); }

  result_type operator()(bvtags::bvuint_tag, boost::any arg) {
    uint64_t value;
    unsigned width;
    boost::tie(value, width) = boost::any_cast<bvuint_tuple>(arg);
    return yices_bvconst_uint64(width, value);
  }

  result_type operator()(bvtags::bvsint_tag, boost::any arg) {
    int64_t value;
    unsigned width;
    boost::tie(value, width) = boost::any_cast<bvsint_tuple>(arg);
    return yices_bvconst_int64(width, value);
  }

  result_type operator()(bvtags::bvbin_tag, boost::any arg) {
    std::string val = boost::any_cast<std::string>(arg);
    return throw_error(yices_parse_bvbin(val.c_str()));
  }

  result_type operator()(bvtags::bvhex_tag, boost::any arg) {
    std::string hex = boost::any_cast<std::string>(arg);
    return throw_error(yices_parse_bvhex(hex.c_str()));
  }

  result_type operator()(bvtags::bvslt_tag, result_type a, result_type b) { return yices_bvslt_atom(a, b); }

  result_type operator()(bvtags::bvsgt_tag, result_type a, result_type b) { return yices_bvsgt_atom(a, b); }

  result_type operator()(bvtags::bvsle_tag, result_type a, result_type b) { return yices_bvsle_atom(a, b); }

  result_type operator()(bvtags::bvsge_tag, result_type a, result_type b) { return yices_bvsge_atom(a, b); }

  result_type operator()(bvtags::bvult_tag, result_type a, result_type b) { return yices_bvlt_atom(a, b); }

  result_type operator()(bvtags::bvugt_tag, result_type a, result_type b) { return yices_bvgt_atom(a, b); }

  result_type operator()(bvtags::bvule_tag, result_type a, result_type b) { return yices_bvle_atom(a, b); }

  result_type operator()(bvtags::bvuge_tag, result_type a, result_type b) { return yices_bvge_atom(a, b); }

  result_type operator()(bvtags::concat_tag, result_type a, result_type b) {
    return throw_error(yices_bvconcat2(a, b));
  }

  result_type operator()(bvtags::extract_tag const &, unsigned upper, unsigned lower, result_type e) {
    return throw_error(yices_bvextract(e, lower, upper));
  }

  result_type operator()(bvtags::zero_extend_tag const &, unsigned width, result_type e) {
    return throw_error(yices_zero_extend(e, width));
  }

  result_type operator()(bvtags::sign_extend_tag const &, unsigned width, result_type e) {
    return throw_error(yices_sign_extend(e, width));
  }

  result_type operator()(bvtags::bvshl_tag, result_type a, result_type b) { return yices_bvshl(a, b); }

  result_type operator()(bvtags::bvshr_tag, result_type a, result_type b) { return yices_bvlshr(a, b); }

  result_type operator()(bvtags::bvashr_tag, result_type a, result_type b) { return yices_bvashr(a, b); }

  result_type operator()(bvtags::var_tag const &var, boost::any) {
    type_t bv_type = yices_bv_type(var.width);
    return yices_new_uninterpreted_term(bv_type);
  }

  result_type operator()(arraytags::array_var_tag const &var, boost::any const &) {
    if (var.id == 0) {
      throw std::runtime_error("uninitialized array used");
    }
    type_t index_type = yices_bv_type(var.index_width);
    type_t var_type = yices_bv_type(var.elem_width);
    type_t function_type = yices_function_type(1, &index_type, var_type);
    return yices_new_uninterpreted_term(function_type);
  }

  result_type operator()(arraytags::select_tag const &, result_type const &array, result_type const &index) {
    return throw_error(yices_application1(array, index));
  }

  result_type operator()(arraytags::store_tag const &, result_type const &array, result_type const &index,
                         result_type const &value) {
    return throw_error(yices_update1(array, index, value));
  }

  result_type operator()(uftags::function_var_tag const &var, boost::any) {
    unsigned const num_args = var.args.size();
    type_t *yices_type = new type_t[num_args];

    type_t result_sort = boost::apply_visitor(detail::yices_type_visitor(*ctx), var.result_type);

    for (unsigned u = 0; u < num_args; ++u) {
      yices_type[u] = boost::apply_visitor(detail::yices_type_visitor(*ctx), var.args[u]);
    }
    type_t function_type = yices_function_type(num_args, yices_type, result_sort);
    return yices_new_uninterpreted_term(function_type);
  }

  result_type operator()(proto::tag::function, result_type const &func_decl) {
    term_t *arg_array = 0;
    return throw_error(yices_application(func_decl, 0, arg_array));
  }

  result_type operator()(proto::tag::function, result_type func_decl, result_type arg) {
    term_t arg_array[] = {arg};
    return throw_error(yices_application(func_decl, 1, arg_array));
  }

  result_type operator()(proto::tag::function, result_type func_decl, result_type arg1, result_type arg2) {
    term_t arg_array[] = {arg1, arg2};
    return throw_error(yices_application(func_decl, 2, arg_array));
  }

  result_type operator()(proto::tag::function, result_type func_decl, result_type arg1, result_type arg2,
                         result_type arg3) {
    term_t arg_array[] = {arg1, arg2, arg3};
    return throw_error(yices_application(func_decl, 3, arg_array));
  }

  unsigned get_bv_width(result_type const &e) { return yices_term_bitsize(e); }

  ////////////////////////
  // Fallback operators //
  ////////////////////////

  template <result_type (*FN)(result_type, result_type)>
  struct Yices_F2 {
    static result_type exec(result_type x, result_type y) { return (*FN)(x, y); }
  };

  template <typename TagT>
  result_type operator()(TagT, boost::any) {
    assert(false && "unknown operator");
    return yices_false();
  }

  /*
  template <typename TagT>
  result_type operator() (TagT , result_type a, result_type b) {}

  template <typename TagT>
  result_type operator() (TagT , result_type , result_type , result_type ) {}*/

  void command(Yices2Impl const &) {}

 private:
  void removeOldAssumptions() {
    if (isPushed_) {
      if (yices_pop(ctx)) {
        throw std::runtime_error(yices_error_message_with_prefix("removeOldAssumptions error: "));
      }
      isPushed_ = false;
    }
  }

  void pushAssumptions() {
    if (assumptions_.empty()) return;
    if (RealIncreamentalMode) {
      if (yices_push(ctx)) {
        throw std::runtime_error(yices_error_message_with_prefix("pushAssumptions error: "));
      }
      isPushed_ = true;
    }
    applyAssertions(assumptions_);
    assumptions_.clear();
  }

  void pushAssertions() {
    applyAssertions(assertions_);
    if (RealIncreamentalMode) assertions_.clear();
  }

  void applyAssertions(Exprs const &expressions) {
    for (Exprs::const_iterator it = expressions.begin(), ie = expressions.end(); it != ie; ++it) {
      if (yices_assert_formula(ctx, *it)) {
        throw std::runtime_error(yices_error_message_with_prefix("applyAssertions error: "));
      }
    }
  }

};  // class Yices2Impl

// redefine to Yices2Impl<false> if incremental solving does not work as expected
typedef Yices2Impl<true> Yices2;

}  // solver
}  // metaSMT
