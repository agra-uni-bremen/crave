#ifndef METASMTNODEVISITORIMPL_H
#define METASMTNODEVISITORIMPL_H

#include "../../crave/expression/metaSMTNodeVisitor.hpp"
#include "../../crave/AssignResult.hpp"

#include <boost/foreach.hpp>
#include <metaSMT/frontend/QF_BV.hpp>
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/Priority_Context.hpp>

#include <map>
#include <stack>
#include <utility>


namespace crave {
  namespace preds = metaSMT::logic;
  namespace qf_bv = metaSMT::logic::QF_BV;
  using metaSMT::evaluate;

template<typename SolverType>
class metaSMTVisitorImpl : public metaSMTVisitor {
public:
  metaSMTVisitorImpl() : metaSMTVisitor(), solver_(), exprStack_(), terminals_(), lazy_() { }

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
  virtual void visitRandomizeExpr( RandomizeExpr const & );
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
  virtual void visitAssignOpr( AssignOpr const & );
  virtual void visitVectorAccess( VectorAccess const & );
  virtual void visitIfThenElse( IfThenElse const & );


  virtual void makeAssertion( Node const & );
  virtual void makeAssumption( Node const & );
  virtual bool solve();
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
  std::map<Node const*, qf_bv::bitvector> terminals_;
  std::map<Node const*, result_type const *> lazy_;
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
void metaSMTVisitorImpl<SolverType>::visitTerminal( Terminal const &t )
{
  std::map<Node const*, qf_bv::bitvector>::iterator ite(terminals_.lower_bound(&t));

  qf_bv::bitvector bit_vec;
  if (ite != terminals_.end() && !(terminals_.key_comp()(&t, ite->first))) {
    // &t already exists
    bit_vec = ite->second;
  }
  else
  {
    bit_vec = qf_bv::new_bitvector( t.bitsize() );
    terminals_.insert( ite, std::make_pair(&t, bit_vec) );
  }

  result_type result = evaluate( solver_, bit_vec );
  exprStack_.push( std::make_pair( result, t.sign() ) );
}

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
  visitTerminal(ve);
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitConstant( Constant const &c )
{
  result_type result = c.sign()?
      evaluate( solver_, qf_bv::bvsint( c.value(), c.bitsize() ) ):
      evaluate( solver_, qf_bv::bvuint( c.value(), c.bitsize() ) );
  exprStack_.push( std::make_pair( result, c.sign() ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitVectorExpr( VectorExpr const &ve )
{
  visitTerminal(ve);
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitNotOpr( NotOpr const &o )
{
  visitUnaryExpr(o);

  stack_entry entry;
  pop( entry );

  result_type result = evaluate( solver_, qf_bv::bvnot( entry.first ) );
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

  result_type result = evaluate(solver_,  qf_bv::bit0);
  BOOST_FOREACH( Constant c, in.collection() ) {
    stack_entry constExpr;
    c.visit(*this);
    pop(constExpr);
    result = evaluate(solver_, qf_bv::bvor( result, qf_bv::bvcomp( entry.first, constExpr.first)) );
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
void metaSMTVisitorImpl<SolverType>::visitRandomizeExpr( RandomizeExpr const &e ) {

  lazy_.erase(e.child().get());
  exprStack_.push( std::make_pair( evaluate(solver_, preds::True), false ) );
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

  result_type result = evaluate( solver_, qf_bv::bvand( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitLogicalOrOpr( LogicalOrOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, qf_bv::bvor( fst.first, snd.first ) );
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

  result_type result = evaluate( solver_, qf_bv::bvcomp( fst.first, snd.first ) );
  exprStack_.push( std::make_pair( result, false ) );
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitNotEqualOpr( NotEqualOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, qf_bv::bvnot(qf_bv::bvcomp( fst.first, snd.first )) );
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
void metaSMTVisitorImpl<SolverType>::visitAssignOpr( AssignOpr const &o )
{
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = evaluate( solver_, preds::equal(snd.first, fst.first) );//qf_bv::bvcomp(fst.first, snd.first) );
  lazy_.insert(std::make_pair(&o, &result));
  exprStack_.push( std::make_pair( evaluate( solver_, preds::True ), false ) );
}

// TODO: create RandomizeExpression -> lazy_.erase(&o); evaluate(solver_, preds::True);

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitVectorAccess( VectorAccess const &o )
{

}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitIfThenElse( IfThenElse const &ite )
{
  stack_entry fst, snd, trd;
  evalTernExpr(ite, fst, snd, trd);

  result_type result = evaluate( solver_, preds::Ite( preds::equal(fst.first, qf_bv::bit1) , snd.first, trd.first ) );
  exprStack_.push( std::make_pair( result, fst.second || snd.second || trd.second ) );
}



template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::makeAssertion(Node const &expr)
{
  expr.visit(*this);

  stack_entry entry;
  pop(entry);

  metaSMT::assertion(solver_, preds::equal(entry.first, qf_bv::bit1));
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::makeAssumption(Node const &expr)
{
  expr.visit(*this);

  stack_entry entry;
  pop(entry);

  metaSMT::assumption(solver_, preds::equal(entry.first, qf_bv::bit1));
}

template<typename SolverType>
bool metaSMTVisitorImpl<SolverType>::solve()
{
  // pre_solve()
  for (typename std::map<Node const*, result_type const*>::const_iterator ite =
      lazy_.begin(); ite != lazy_.end(); ++ite) {
    metaSMT::assumption(solver_, *ite->second);
  }

  return metaSMT::solve(solver_);
}

template<typename SolverType>
bool metaSMTVisitorImpl<SolverType>::read(Node const& var, AssignResult& assign)
{
  // assert(type_of(Node) == Terminal)
  std::map<Node const *, qf_bv::bitvector>::const_iterator ite(terminals_.find(&var));
  if (ite == terminals_.end())
    return false;

  qf_bv::bitvector var_expr = ite->second;
  std::string res = metaSMT::read_value( solver_, var_expr );
  assign.set_value( res );

  return true;
}

} // namespace crave

#endif /* end of include guard: METASMTNODEVISITORIMPL_H */
