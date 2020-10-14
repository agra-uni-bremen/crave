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

#include "../tags/Array.hpp"
#include "../tags/QF_BV.hpp"
#include "../result_wrapper.hpp"

#ifdef __clang__
#define _BACKWARD_BACKWARD_WARNING_H
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvariadic-macros"
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#ifdef __GNUC__
# if __GNUC__ >= 4 and __GNUC_MINOR__ > 4
// with this definitions gcc 4.4 creates executables with random segfaults
#define _BACKWARD_BACKWARD_WARNING_H
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wvariadic-macros"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#endif

#include <cvc4/cvc4.h>

#ifdef __GNUC__
# if __GNUC__ >= 4 and __GNUC_MINOR__ > 4
// this makes gcc 4.4 corrupt executables and cause random segfaults
#pragma GCC pop_options
#undef _BACKWARD_BACKWARD_WARNING_H
#endif
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#undef _BACKWARD_BACKWARD_WARNING_H
#endif

#include <boost/mpl/map/map40.hpp>
#include <boost/any.hpp>
#include <boost/tuple/tuple.hpp>
#include <list>

namespace metaSMT {
  namespace solver {
    namespace predtags = ::metaSMT::logic::tag;
    namespace bvtags = ::metaSMT::logic::QF_BV::tag;
    namespace arraytags = ::metaSMT::logic::Array::tag;

    /**
     * @ingroup Backend
     * @class CVC4 CVC4.hpp metaSMT/backend/CVC4.hpp
     * @brief The CVC4 backend
     */
    class CVC4 {
    private:
      typedef boost::tuple<uint64_t, unsigned>  bvuint_tuple;
      typedef boost::tuple< int64_t, unsigned>  bvsint_tuple;

    public:
      typedef ::CVC4::Expr result_type;
      typedef std::list< ::CVC4::Expr > Exprs;

      CVC4()
        : exprManager_( )
        , engine_( &exprManager_ )
        , isPushed_( false )
      {
        engine_.setOption("incremental", true);
        engine_.setOption("produce-models", true);
        engine_.setLogic("QF_ABV");
      }

      ~CVC4() {
      }

	result_type operator() (arraytags::array_var_tag const &var,
                              boost::any const & ) {
		if (var.id == 0 ) {
		  throw std::runtime_error("uninitialized array used");
		}
		::CVC4::Type elementType = exprManager_.mkBitVectorType(var.elem_width);
		::CVC4::Type indexType = exprManager_.mkBitVectorType(var.index_width);
		::CVC4::Type arrayType = exprManager_.mkArrayType(indexType, elementType);
		return exprManager_.mkVar(arrayType);
	}

	result_type operator() (arraytags::select_tag const &
                              , result_type const &array
                              , result_type const &index) {
	return exprManager_.mkExpr(::CVC4::kind::SELECT, array, index);
}

	result_type operator() (arraytags::store_tag const &
                              , result_type const &array
                              , result_type const &index
                              , result_type const &value) {
	return exprManager_.mkExpr(::CVC4::kind::STORE, array, index, value);
	}

      void assertion( result_type e ) {
        assertions_.push_back( e );
      }

      void assumption( result_type e ) {
        assumptions_.push_back( e );
      }

      unsigned get_bv_width( result_type const &e ) {
        ::CVC4::Type type = e.getType();
        return type.isBitVector() ? ((::CVC4::BitVectorType) type).getSize() : 0;
      }

      bool solve() {
        removeOldAssumptions();
        pushAssertions();
        pushAssumptions();
        
        return engine_.checkSat().isSat();
      }

      result_wrapper read_value(result_type var) {
      
        ::CVC4::Expr value = engine_.getValue(var);
        ::CVC4::Type type = value.getType();

        if (type.isBoolean()) {
          return value.getConst<bool>();

        } else if (type.isBitVector()) {
          ::CVC4::BitVector bvValue = value.getConst< ::CVC4::BitVector >();
          return bvValue.toString();
        }
        return result_wrapper( false );
      }

      // predtags
      result_type operator()( predtags::var_tag const & , boost::any ) {
        return exprManager_.mkVar(exprManager_.booleanType());
      }

      result_type operator()( predtags::false_tag , boost::any ) {
        return exprManager_.mkConst(false);
      }

      result_type operator()( predtags::true_tag , boost::any ) {
        return exprManager_.mkConst(true);
      }

      result_type operator()( predtags::not_tag , result_type e ) {
        return exprManager_.mkExpr(::CVC4::kind::NOT, e);
      }

      result_type operator()( predtags::ite_tag , result_type a, result_type b, result_type c ) {
        return exprManager_.mkExpr(::CVC4::kind::ITE, a, b, c);
      }

      // bvtags
      result_type operator()( bvtags::var_tag const & var, boost::any ) {
        assert ( var.width != 0 );
        ::CVC4::Type bv_ty = exprManager_.mkBitVectorType(var.width);
        return exprManager_.mkVar(bv_ty);
      }

      result_type operator()( bvtags::bit0_tag , boost::any ) {
        return exprManager_.mkConst(::CVC4::BitVector(1u, 0u));
      }

      result_type operator()( bvtags::bit1_tag , boost::any ) {
        return exprManager_.mkConst(::CVC4::BitVector(1u, 1u));
      }

      result_type operator()( bvtags::bvuint_tag , boost::any arg ) {
        uint64_t value;
        unsigned width;
        boost::tie(value, width) = boost::any_cast<bvuint_tuple>(arg);
        return exprManager_.mkConst(::CVC4::BitVector(width, value));
      }

      result_type operator()( bvtags::bvsint_tag , boost::any arg ) {
        int64_t value;
        unsigned width;
        boost::tie(value, width) = boost::any_cast<bvsint_tuple>(arg);
        ::CVC4::BitVector bvValue (width, ::CVC4::Integer(value));
        return exprManager_.mkConst(bvValue);
      }

      result_type operator()( bvtags::bvbin_tag , boost::any arg ) {
        std::string val = boost::any_cast<std::string>(arg);
        return exprManager_.mkConst(::CVC4::BitVector(val));
      }

      result_type operator()( bvtags::bvhex_tag , boost::any arg ) {
        std::string hex = boost::any_cast<std::string>(arg);
        return exprManager_.mkConst(::CVC4::BitVector(hex, 16));
      }

      result_type operator()( bvtags::bvnot_tag , result_type e ) {
        return exprManager_.mkExpr(::CVC4::kind::BITVECTOR_NOT, e);
      }

      result_type operator()( bvtags::bvneg_tag , result_type e ) {
        return exprManager_.mkExpr(::CVC4::kind::BITVECTOR_NEG, e);
      }

      result_type operator()( bvtags::extract_tag const &
        , unsigned upper, unsigned lower
        , result_type e)
      {
        ::CVC4::BitVectorExtract bvOp (upper, lower);
        ::CVC4::Expr op = exprManager_.mkConst(bvOp);
        return exprManager_.mkExpr(op, e);
      }

      result_type operator()( bvtags::zero_extend_tag const &
        , unsigned width
        , result_type e)
      {
        ::CVC4::BitVectorZeroExtend bvOp (width);
        ::CVC4::Expr op = exprManager_.mkConst(bvOp);
        return exprManager_.mkExpr(op, e);
      }

      result_type operator()( bvtags::sign_extend_tag const &
        , unsigned width
        , result_type e)
      {
        ::CVC4::BitVectorSignExtend bvOp (width);
        ::CVC4::Expr op = exprManager_.mkConst(bvOp);
        return exprManager_.mkExpr(op, e);
      }

      result_type operator()( predtags::equal_tag const &
                             , result_type a
                             , result_type b) {
#ifndef CVC4_WITHOUT_KIND_IFF
        if (a.getType().isBoolean() && b.getType().isBoolean() ) {
          return exprManager_.mkExpr(::CVC4::kind::IFF, a, b);
        } else {
          return exprManager_.mkExpr(::CVC4::kind::EQUAL, a, b);
        }
#else
        return exprManager_.mkExpr(::CVC4::kind::EQUAL, a, b);
#endif
      }

      result_type operator()( predtags::nequal_tag const &
                             , result_type a
                             , result_type b) {
        return exprManager_.mkExpr(::CVC4::kind::DISTINCT, a, b);
      }

      result_type operator()( predtags::distinct_tag const &
                             , result_type a
                             , result_type b) {
        return exprManager_.mkExpr(::CVC4::kind::DISTINCT, a, b);
      }

      ////////////////////////
      // Fallback operators //
      ////////////////////////

      template <typename TagT>
      result_type operator() (TagT , boost::any ) {
        assert( false );
        return exprManager_.mkConst(false);
      }

      template< ::CVC4::kind::Kind_t KIND_ >
      struct Op2 {
        static result_type exec(::CVC4::ExprManager& em, ::CVC4::Expr x,
          ::CVC4::Expr y) 
        {
          return em.mkExpr(KIND_, x, y); 
        }
      };

      template< ::CVC4::kind::Kind_t KIND_ >
      struct NotOp2 {
        static result_type exec(::CVC4::ExprManager& em, ::CVC4::Expr x,
          ::CVC4::Expr y) 
        {
          return em.mkExpr(::CVC4::kind::NOT, em.mkExpr(KIND_, x, y)); 
        }
      };

      template <typename TagT>
      result_type operator() (TagT , result_type a, result_type b) {
        namespace mpl = boost::mpl;
        using namespace ::CVC4::kind;

        typedef mpl::map33<
          // binary Logic tags
          mpl::pair<predtags::and_tag,     Op2<AND> >
        , mpl::pair<predtags::nand_tag,    NotOp2<AND> >
        , mpl::pair<predtags::or_tag,      Op2<OR> >
        , mpl::pair<predtags::nor_tag,     NotOp2<OR> >
        , mpl::pair<predtags::xor_tag,     Op2<XOR> >
        , mpl::pair<predtags::xnor_tag,    NotOp2<XOR> >
        , mpl::pair<predtags::implies_tag, Op2<IMPLIES> >
        // binary QF_BV tags
        , mpl::pair<bvtags::bvand_tag,     Op2<BITVECTOR_AND> >
        , mpl::pair<bvtags::bvnand_tag,    Op2<BITVECTOR_NAND> >
        , mpl::pair<bvtags::bvor_tag,      Op2<BITVECTOR_OR> >
        , mpl::pair<bvtags::bvnor_tag,     Op2<BITVECTOR_NOR> >
        , mpl::pair<bvtags::bvxor_tag,     Op2<BITVECTOR_XOR> >
        , mpl::pair<bvtags::bvxnor_tag,    Op2<BITVECTOR_XNOR> >
        , mpl::pair<bvtags::bvadd_tag,     Op2<BITVECTOR_PLUS> >
        , mpl::pair<bvtags::bvsub_tag,     Op2<BITVECTOR_SUB> >
        , mpl::pair<bvtags::bvmul_tag,     Op2<BITVECTOR_MULT> >
        , mpl::pair<bvtags::bvudiv_tag,    Op2<BITVECTOR_UDIV> >
        , mpl::pair<bvtags::bvurem_tag,    Op2<BITVECTOR_UREM> >
        , mpl::pair<bvtags::bvsdiv_tag,    Op2<BITVECTOR_SDIV> >
        , mpl::pair<bvtags::bvsrem_tag,    Op2<BITVECTOR_SREM> >
        , mpl::pair<bvtags::bvslt_tag,     Op2<BITVECTOR_SLT> >
        , mpl::pair<bvtags::bvsle_tag,     Op2<BITVECTOR_SLE> >
        , mpl::pair<bvtags::bvsgt_tag,     Op2<BITVECTOR_SGT> >
        , mpl::pair<bvtags::bvsge_tag,     Op2<BITVECTOR_SGE> >
        , mpl::pair<bvtags::bvult_tag,     Op2<BITVECTOR_ULT> >
        , mpl::pair<bvtags::bvule_tag,     Op2<BITVECTOR_ULE> >
        , mpl::pair<bvtags::bvugt_tag,     Op2<BITVECTOR_UGT> >
        , mpl::pair<bvtags::bvuge_tag,     Op2<BITVECTOR_UGE> >
        , mpl::pair<bvtags::concat_tag,    Op2<BITVECTOR_CONCAT> >
        , mpl::pair<bvtags::bvcomp_tag,    Op2<BITVECTOR_COMP> >
        , mpl::pair<bvtags::bvshl_tag,     Op2<BITVECTOR_SHL> >
        , mpl::pair<bvtags::bvshr_tag,     Op2<BITVECTOR_LSHR> >
        , mpl::pair<bvtags::bvashr_tag,    Op2<BITVECTOR_ASHR> >
        > Opcode_Map;

        typedef
          typename mpl::has_key< Opcode_Map, TagT >::type
          has_key;

        if (has_key::value ) {
          typedef typename mpl::eval_if<
            has_key
          , mpl::at< Opcode_Map, TagT >
          , mpl::identity< Op2<UNDEFINED_KIND> >
          >::type opcode;
          return opcode::exec(exprManager_, a, b);
        }
        else {
          // std::cerr << "unknown operator: " << tag << std::endl;
          assert(false && "unknown operator");
        }
        return exprManager_.mkConst(false);
      }

      template <typename TagT>
      result_type operator() (TagT , result_type , result_type , result_type ) {
        assert( false );
        return exprManager_.mkConst(false);
      }

      // pseudo command
      void command ( CVC4 const & ) { }

    private:
      void removeOldAssumptions() {
        if (isPushed_) {
          engine_.pop();
          isPushed_ = false;
        }
      }

      void pushAssumptions() {
        engine_.push();
        isPushed_ = true;

        applyAssertions(assumptions_);
        assumptions_.clear();
      }

      void pushAssertions() {
        applyAssertions(assertions_);
        assertions_.clear();
      }

      void applyAssertions(Exprs const& expressions) {
        for ( Exprs::const_iterator it = expressions.begin(),
              ie = expressions.end(); it != ie; ++it ) {
          engine_.assertFormula(*it);
        }
      }

    private:
      ::CVC4::ExprManager exprManager_;
      ::CVC4::SmtEngine engine_;
      Exprs assumptions_;
      Exprs assertions_;
      bool isPushed_;
    }; // class CVC4

  } // solver
} // metaSMT
