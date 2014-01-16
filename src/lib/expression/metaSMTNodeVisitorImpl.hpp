#ifndef METASMTNODEVISITORIMPL_H
#define METASMTNODEVISITORIMPL_H

#include "../../crave/expression/metaSMTNodeVisitor.hpp"
#include "../../crave/AssignResult.hpp"

#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <metaSMT/frontend/QF_BV.hpp>
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/Priority_Context.hpp>
#include <metaSMT/support/cardinality.hpp>
#include <metaSMT/support/contradiction_analysis.hpp>

#include <map>
#include <stack>
#include <utility>

namespace crave {
  namespace preds = metaSMT::logic;
  namespace qf_bv = metaSMT::logic::QF_BV;
  using metaSMT::evaluate;

  extern boost::mt19937 rng;
  struct RNG {
    unsigned operator()(unsigned i) { return boost::uniform_int<>(0, i - 1)(rng); }
  } rng_;


template<typename SolverType>
class metaSMTVisitorImpl : public metaSMTVisitor {
public:
  metaSMTVisitorImpl()
  : metaSMTVisitor(), solver_(), exprStack_(), terminals_(),
    softs_(), assumptions_(), suggestions_() { }

  virtual void visitNode( Node const & );
  virtual void visitTerminal( Terminal const & );
  virtual void visitUnaryExpr( UnaryExpression const & );
  virtual void visitUnaryOpr( UnaryOperator const & );
  virtual void visitBinaryExpr( BinaryExpression const & );
  virtual void visitBinaryOpr( BinaryOperator const & );
  virtual void visitTernaryExpr( TernaryExpression const & );
  virtual void visitPlaceholder( Placeholder const & );
  virtual void visitVariableExpr( VariableExpr const & );
  virtual void visitConstant( Constant const & );
  virtual void visitVectorExpr( VectorExpr const & );
  virtual void visitNotOpr( NotOpr const & );
  virtual void visitNegOpr( NegOpr const & );
  virtual void visitComplementOpr( ComplementOpr const & );
  virtual void visitInside( Inside const & );
  virtual void visitExtendExpr( ExtendExpression const & );
  virtual void visitAndOpr( AndOpr const & );
  virtual void visitOrOpr( OrOpr const & );
  virtual void visitLogicalAndOpr( LogicalAndOpr const & );
  virtual void visitLogicalOrOpr( LogicalOrOpr const & );
  virtual void visitXorOpr( XorOpr const & );
  virtual void visitEqualOpr( EqualOpr const & );
  virtual void visitNotEqualOpr( NotEqualOpr const & );
  virtual void visitLessOpr( LessOpr const & );
  virtual void visitLessEqualOpr( LessEqualOpr const & );
  virtual void visitGreaterOpr( GreaterOpr const & );
  virtual void visitGreaterEqualOpr( GreaterEqualOpr const & );
  virtual void visitPlusOpr( PlusOpr const & );
  virtual void visitMinusOpr( MinusOpr const & );
  virtual void visitMultipliesOpr( MultipliesOpr const & );
  virtual void visitDevideOpr( DevideOpr const & );
  virtual void visitModuloOpr( ModuloOpr const & );
  virtual void visitShiftLeftOpr( ShiftLeftOpr const & );
  virtual void visitShiftRightOpr( ShiftRightOpr const & );
  virtual void visitVectorAccess( VectorAccess const & );
  virtual void visitIfThenElse( IfThenElse const & );
  virtual void visitForEach( ForEach const & );
  virtual void visitUnique( Unique const & );
  virtual void visitBitslice( Bitslice const & );


  virtual void makeAssertion( Node const & );
  virtual void makeSoftAssertion( Node const & );
  virtual void makeSuggestion( Node const & );
  virtual void makeAssumption( Node const & );
  virtual std::vector<unsigned int> analyseSofts(bool exact);
  virtual std::vector<std::vector<unsigned int> > analyseContradiction(
                  std::map<unsigned int, NodePtr > const &);
  virtual bool solve(bool ignoreSofts);
  virtual bool read( Node const& var, AssignResult& );

private: // typedefs
  typedef typename SolverType::result_type result_type;
  typedef std::pair<result_type, bool> stack_entry;

private: // methods
  inline void pop( stack_entry & fst );
  inline void pop2( stack_entry & fst, stack_entry & snd );
  inline void pop3( stack_entry & fst, stack_entry & snd, stack_entry & trd );
  void evalBinExpr( BinaryExpression const& expr,  stack_entry & fst, stack_entry & snd );
  void evalTernExpr( TernaryExpression const& expr,  stack_entry & fst, stack_entry & snd, stack_entry & trd );

private: // data
  SolverType solver_;
  std::stack<stack_entry> exprStack_;
  std::map<int, qf_bv::bitvector> terminals_;
  std::vector<result_type> softs_;
  std::vector<result_type> assumptions_;
  std::vector<result_type> suggestions_;
};

template<typename SolverType>
inline void metaSMTVisitorImpl<SolverType>::pop( stack_entry & fst )
{
  assert( exprStack_.size() >= 1 );
  fst = exprStack_.top();
  exprStack_.pop();
}

template<typename SolverType>
inline void metaSMTVisitorImpl<SolverType>::pop2( stack_entry & fst, stack_entry & snd )
{
  assert( exprStack_.size() >= 2 );
  fst = exprStack_.top();
  exprStack_.pop();
  snd = exprStack_.top();
  exprStack_.pop();
}

template<typename SolverType>
inline void metaSMTVisitorImpl<SolverType>::pop3( stack_entry & fst, stack_entry & snd, stack_entry & trd )
{
  assert( exprStack_.size() >= 3 );
  fst = exprStack_.top();
  exprStack_.pop();
  snd = exprStack_.top();
  exprStack_.pop();
  trd = exprStack_.top();
  exprStack_.pop();
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::evalBinExpr( BinaryExpression const& expr, stack_entry & fst, stack_entry & snd )
{
  visitBinaryExpr(expr);
  pop2(snd, fst);
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::evalTernExpr( TernaryExpression const& expr, stack_entry & fst, stack_entry & snd, stack_entry & trd )
{
  visitTernaryExpr(expr);
  pop3(trd, snd, fst);
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitNode( Node const& ) { }

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitTerminal( Terminal const &t ) { }

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitUnaryExpr( UnaryExpression const &ue )
{
  ue.child()->visit(*this);
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitUnaryOpr( UnaryOperator const & ) { }

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitBinaryExpr( BinaryExpression const &be )
{
  be.lhs()->visit(*this);
  be.rhs()->visit(*this);
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitBinaryOpr( BinaryOperator const & ) { }

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitTernaryExpr( TernaryExpression const &te )
{
  te.a()->visit(*this);
  te.b()->visit(*this);
  te.c()->visit(*this);
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitPlaceholder( Placeholder const & ) { }

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitVariableExpr( VariableExpr const &ve )
{
  std::map<int, qf_bv::bitvector>::iterator ite(terminals_.lower_bound(ve.id()));

  qf_bv::bitvector bit_vec;
  if (ite != terminals_.end() && !(terminals_.key_comp()(ve.id(), ite->first))) {
    // &t already exists
    bit_vec = ite->second;
  }
  else
  {
    bit_vec = qf_bv::new_bitvector( ve.bitsize() );
    terminals_.insert( ite, std::make_pair(ve.id(), bit_vec) );
  }

  result_type result = evaluate( solver_, bit_vec );
  exprStack_.push( std::make_pair( result, ve.sign() ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitConstant( Constant const &c )
{
  result_type result = c.isBool()?
  	(c.value()? 
  	  evaluate( solver_,  preds::True):
  	  evaluate( solver_,  preds::False)
  	):
	(c.sign()?
      evaluate( solver_, qf_bv::bvsint( c.value(), c.bitsize() ) ):
      evaluate( solver_, qf_bv::bvuint( c.value(), c.bitsize() ) )
    );
  exprStack_.push( std::make_pair( result, c.sign() ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitNotOpr( NotOpr const &o )
{
  visitUnaryExpr(o);

  stack_entry entry;
  pop( entry );

  result_type result = evaluate( solver_, preds::Not( entry.first ) );
  exprStack_.push( std::make_pair( result, entry.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitNegOpr( NegOpr const &o )
{
  visitUnaryExpr(o);

  stack_entry entry;
  pop( entry );

  result_type result = evaluate( solver_, qf_bv::bvneg( entry.first ) );
  exprStack_.push( std::make_pair( result, entry.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitComplementOpr( ComplementOpr const &o )
{
  visitUnaryExpr(o);

  stack_entry entry;
  pop( entry );

  result_type result = evaluate( solver_, qf_bv::bvnot( entry.first ) );
  exprStack_.push( std::make_pair( result, entry.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitInside( Inside const &in )
{
  visitUnaryExpr(in);

  stack_entry entry;
  pop( entry );

  result_type result = evaluate(solver_, preds::False);
  BOOST_FOREACH( Constant c, in.collection() ) {
    stack_entry constExpr;
    c.visit(*this);
    pop(constExpr);
    result = evaluate(solver_, preds::Or( result, preds::equal( entry.first, constExpr.first)) );
  }

  exprStack_.push( std::make_pair( result, false ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitExtendExpr( ExtendExpression const &e ) {
  visitUnaryExpr(e);

  stack_entry entry;
  pop( entry );

  result_type result;
  if (entry.second)
    result = evaluate(solver_, qf_bv::sign_extend(e.value(), entry.first));
  else
    result = evaluate(solver_, qf_bv::zero_extend(e.value(), entry.first));

  exprStack_.push( std::make_pair( result, entry.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitAndOpr( AndOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, qf_bv::bvand( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitOrOpr( OrOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, qf_bv::bvor( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitLogicalAndOpr( LogicalAndOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, preds::And( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitLogicalOrOpr( LogicalOrOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, preds::Or( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitXorOpr( XorOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, qf_bv::bvxor( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitEqualOpr( EqualOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, preds::equal( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, false ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitNotEqualOpr( NotEqualOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, preds::nequal( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, false ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitLessOpr( LessOpr const &o )
{
  stack_entry rhs_p, lhs_p;
  evalBinExpr(o, lhs_p, rhs_p);

  result_type lhs = lhs_p.first;
  result_type rhs = rhs_p.first;
  bool lhs_signed = lhs_p.second;
  bool rhs_signed = rhs_p.second;

  result_type result;
  if ( lhs_signed && rhs_signed ) {
    result = evaluate( solver_, qf_bv::bvslt( lhs, rhs ));
  } else if ( !(lhs_signed || rhs_signed) ) {
    result = evaluate( solver_, qf_bv::bvult( lhs, rhs ));
  } else if ( lhs_signed ) {
    result = evaluate( solver_, qf_bv::bvslt(
                      qf_bv::sign_extend( 1, lhs ),
                      qf_bv::zero_extend( 1, rhs )));
  } else {
    result = evaluate( solver_, qf_bv::bvslt(
                      qf_bv::zero_extend( 1, lhs ),
                      qf_bv::sign_extend( 1, rhs )
    ));
  }
  exprStack_.push( std::make_pair( result, false ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitLessEqualOpr( LessEqualOpr const &o )
{
  stack_entry rhs_p, lhs_p;
  evalBinExpr(o, lhs_p, rhs_p);

  result_type lhs = lhs_p.first;
  result_type rhs = rhs_p.first;
  bool lhs_signed = lhs_p.second;
  bool rhs_signed = rhs_p.second;

  result_type result;
  if ( lhs_signed && rhs_signed ) {
    result = evaluate( solver_, qf_bv::bvsle( lhs, rhs ));
  } else if ( !(lhs_signed || rhs_signed) ) {
    result = evaluate( solver_, qf_bv::bvule( lhs, rhs ));
  } else if ( lhs_signed ) {
    result = evaluate( solver_, qf_bv::bvsle(
                      qf_bv::sign_extend( 1, lhs ),
                      qf_bv::zero_extend( 1, rhs )));
  } else {
    result = evaluate( solver_, qf_bv::bvsle(
                      qf_bv::zero_extend( 1, lhs ),
                      qf_bv::sign_extend( 1, rhs )
    ));
  }
  exprStack_.push( std::make_pair( result, false ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitGreaterOpr( GreaterOpr const &o )
{
  stack_entry rhs_p, lhs_p;
  evalBinExpr(o, lhs_p, rhs_p);

  result_type lhs = lhs_p.first;
  result_type rhs = rhs_p.first;
  bool lhs_signed = lhs_p.second;
  bool rhs_signed = rhs_p.second;

  result_type result;
  if ( lhs_signed && rhs_signed ) {
    result = evaluate( solver_, qf_bv::bvsgt( lhs, rhs ));
  } else if ( !(lhs_signed || rhs_signed) ) {
    result = evaluate( solver_, qf_bv::bvugt( lhs, rhs ));
  } else if ( lhs_signed ) {
    result = evaluate( solver_, qf_bv::bvsgt(
                      qf_bv::sign_extend( 1, lhs ),
                      qf_bv::zero_extend( 1, rhs )));
  } else {
    result = evaluate( solver_, qf_bv::bvsgt(
                      qf_bv::zero_extend( 1, lhs ),
                      qf_bv::sign_extend( 1, rhs )
    ));
  }
  exprStack_.push( std::make_pair( result, false ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitGreaterEqualOpr( GreaterEqualOpr const &o )
{
  stack_entry rhs_p, lhs_p;
  evalBinExpr(o, lhs_p, rhs_p);

  result_type lhs = lhs_p.first;
  result_type rhs = rhs_p.first;
  bool lhs_signed = lhs_p.second;
  bool rhs_signed = rhs_p.second;

  result_type result;
  if ( lhs_signed && rhs_signed ) {
    result = evaluate( solver_, qf_bv::bvsge( lhs, rhs ));
  } else if ( !(lhs_signed || rhs_signed) ) {
    result = evaluate( solver_, qf_bv::bvuge( lhs, rhs ));
  } else if ( lhs_signed ) {
    result = evaluate( solver_, qf_bv::bvsge(
                      qf_bv::sign_extend( 1, lhs ),
                      qf_bv::zero_extend( 1, rhs )));
  } else {
    result = evaluate( solver_, qf_bv::bvsge(
                      qf_bv::zero_extend( 1, lhs ),
                      qf_bv::sign_extend( 1, rhs )
    ));
  }
  exprStack_.push( std::make_pair( result, false ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitPlusOpr( PlusOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, qf_bv::bvadd( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitMinusOpr( MinusOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, qf_bv::bvsub( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitMultipliesOpr( MultipliesOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, qf_bv::bvmul( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitDevideOpr( DevideOpr const &o )
{
  stack_entry rhs_p, lhs_p;
  evalBinExpr(o, lhs_p, rhs_p);

  result_type lhs = lhs_p.first;
  result_type rhs = rhs_p.first;
  bool lhs_signed = lhs_p.second;
  bool rhs_signed = rhs_p.second;

  result_type result;
  if ( lhs_signed && rhs_signed ) {
    result = evaluate( solver_, qf_bv::bvsdiv( lhs, rhs ));
  } else if ( !(lhs_signed || rhs_signed) ) {
    result = evaluate( solver_, qf_bv::bvudiv( lhs, rhs ));
  } else if ( lhs_signed ) {
    result = evaluate( solver_, qf_bv::bvsdiv(
                      qf_bv::sign_extend( 1, lhs ),
                      qf_bv::zero_extend( 1, rhs )));
  } else {
    result = evaluate( solver_, qf_bv::bvsdiv(
                      qf_bv::zero_extend( 1, lhs ),
                      qf_bv::sign_extend( 1, rhs )
    ));
  }
  exprStack_.push( std::make_pair( result, lhs_signed || rhs_signed ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitModuloOpr( ModuloOpr const &o )
{
  stack_entry rhs_p, lhs_p;
  evalBinExpr(o, lhs_p, rhs_p);

  result_type lhs = lhs_p.first;
  result_type rhs = rhs_p.first;
  bool lhs_signed = lhs_p.second;

  result_type result;
  if ( lhs_signed )
    result = evaluate( solver_, qf_bv::bvsrem( lhs, rhs ));
  else
    result = evaluate( solver_, qf_bv::bvurem( lhs, rhs ));

  exprStack_.push( std::make_pair( result, lhs_signed || rhs_p.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitShiftLeftOpr( ShiftLeftOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, qf_bv::bvshl( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitShiftRightOpr( ShiftRightOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, qf_bv::bvshr( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitIfThenElse( IfThenElse const &ite )
{
  stack_entry fst, snd, trd;
  evalTernExpr(ite, fst, snd, trd);

  result_type result = evaluate( solver_, preds::Ite( preds::equal(fst.first, preds::True) , snd.first, trd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second || trd.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitBitslice( Bitslice const &b ) {
  visitUnaryExpr(b);

  stack_entry entry;
  pop( entry );

  int slice_size = b.r() - b.l() + 1;
  if (slice_size <= 0 || slice_size > b.expr_size()) {
    throw std::runtime_error("Invalid size of bit slice.");
  }      

  result_type result = 
  evaluate(solver_, 
            qf_bv::zero_extend( 
              b.expr_size() - slice_size, 
              qf_bv::extract(b.r(), b.l(), entry.first) 
            )
          );

  exprStack_.push( std::make_pair( result, false ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitVectorAccess( VectorAccess const &o ) { throw std::runtime_error("VectorAccess is not allowed in metaSMTNodeVisitor."); }
template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitVectorExpr( VectorExpr const &ve ) { throw std::runtime_error("VectorExpr is not allowed in metaSMTNodeVisitor."); }
template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitForEach( ForEach const &fe ) { throw std::runtime_error("ForEach is not allowed in metaSMTNodeVisitor."); }
template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitUnique( Unique const &u ) { throw std::runtime_error("Unique is not allowed in metaSMTNodeVisitor."); }

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::makeAssertion(Node const &expr)
{
  expr.visit(*this);

  stack_entry entry;
  pop(entry);

  metaSMT::assertion(solver_, preds::equal(entry.first, preds::True));
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::makeSoftAssertion( Node const &expr )
{
  expr.visit(*this);

  stack_entry entry;
  pop(entry);

  softs_.push_back(entry.first);
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::makeSuggestion( Node const &expr )
{
  expr.visit(*this);

  stack_entry entry;
  pop(entry);

  suggestions_.push_back(entry.first);
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::makeAssumption(Node const &expr)
{
  expr.visit(*this);

  stack_entry entry;
  pop(entry);

  assumptions_.push_back(evaluate(solver_, preds::equal(entry.first, preds::True)));
}

template<typename SolverType>
std::vector<unsigned int> metaSMTVisitorImpl<SolverType>::analyseSofts(bool exact)
{
  std::vector<unsigned int> result;
  // implements soft constraint semantics of the HVL e
  for (unsigned int i = 0; i < softs_.size(); i++) {
    metaSMT::assumption(solver_, preds::equal(softs_[i], preds::True));
    if (metaSMT::solve(solver_)) {
      // accept
      metaSMT::assertion(solver_, preds::equal(softs_[i], preds::True));
    }
    else {
      // reject
      result.push_back(i);
    }
  }
  softs_.clear();
  return result;
}

template<typename SolverType>
std::vector<std::vector<unsigned int> > metaSMTVisitorImpl<SolverType>::analyseContradiction(
                                  std::map<unsigned int, NodePtr > const &s)
{
  std::vector<std::vector<unsigned int> > results;
  std::map<unsigned int, result_type> result_map;

  typedef std::pair<unsigned int, NodePtr > NodePair;

  BOOST_FOREACH (NodePair entry, s) {
    entry.second->visit(*this);
    stack_entry st_entry;
    pop(st_entry);

    result_type var(evaluate(solver_, preds::new_variable()));
    metaSMT::assertion(solver_, preds::implies(var, st_entry.first));
    result_map.insert(std::make_pair(entry.first, var));
  }

  results = metaSMT::analyze_conflicts(solver_, result_map, metaSMT::evaluate(solver_, preds::True), results);

  return results;
}

template<typename SolverType>
bool metaSMTVisitorImpl<SolverType>::solve(bool ignoreSofts)
{
  bool result = false;

  std::random_shuffle(assumptions_.begin(), assumptions_.end(), rng_);
  std::random_shuffle(suggestions_.begin(), suggestions_.end(), rng_);

  for (int k = suggestions_.size(); k >= 0; --k) {

    for (typename std::vector<result_type>::const_iterator ite = assumptions_.begin();
         ite != assumptions_.end(); ++ite) {
      metaSMT::assumption(solver_, *ite);
    }

//    if (k > 0) 
//      metaSMT::assumption(solver_, metaSMT::cardinality_eq(solver_, suggestions_, k));
  
    for (int i = 0; i < k; i++)
      metaSMT::assumption(solver_, suggestions_[i]);

    if (!ignoreSofts) {
      for (typename std::vector<result_type>::const_iterator ite = softs_.begin(); ite != softs_.end(); ++ite) 
        metaSMT::assumption(solver_, preds::equal(*ite, preds::True));
    }  

    if (metaSMT::solve(solver_)) {
      result = true;
      break;
    }
  }
  
  assumptions_.clear();
  suggestions_.clear();
  
  return result;
}

template<typename SolverType>
bool metaSMTVisitorImpl<SolverType>::read(Node const& v, AssignResult& assign)
{
  // assert(static_cast<VariableExpr const *>(&var) != 0);
  VariableExpr const& var = *static_cast<VariableExpr const*>(&v);
  std::map<int, qf_bv::bitvector>::const_iterator ite(terminals_.find(var.id()));
  if (ite == terminals_.end())
    return false;

  qf_bv::bitvector var_expr = ite->second;
  std::string res = metaSMT::read_value( solver_, var_expr );
  assign.set_value( res );

  return true;
}

} // namespace crave

#endif /* end of include guard: METASMTNODEVISITORIMPL_H */
