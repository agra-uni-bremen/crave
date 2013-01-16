#ifndef METASMTNODEVISITORIMPL_H
#define METASMTNODEVISITORIMPL_H

#include "../../crave/expression/metaSMTNodeVisitor.hpp"

#include <metaSMT/frontend/QF_BV.hpp>
#include <metaSMT/DirectSolver_Context.hpp>

#include <stack>


namespace crave {
  namespace qf_bv = metaSMT::logic::QF_BV;
  using metaSMT::evaluate;

template<typename SolverType>
class metaSMTVisitorImpl : public metaSMTVisitor {
public:
  metaSMTVisitorImpl() : metaSMTVisitor(), solver_(), exprStack_() { }

  virtual bool visitNode( Node const & );
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
  virtual void visitAndOpr( AndOpr const & );
  virtual void visitOrOpr( OrOpr const & );
  virtual void visitLogicalAndOpr( LogicalAndOpr const & );
  virtual void visitLogicalOrOpr( LogicalOrOpr const & );
  virtual void visitXorOpr( XorOpr const & );
  virtual void visitEqualOpr( EqualOpr const & );
  virtual void visitNotEqualOpr( NotEqualOpr const & );
  virtual void visitLessOpr( LessOpr const &o );
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


  virtual void makeAssertion( Node const &);
  virtual void makeAssumption( Node const&);
  virtual bool solve();

private: // typedefs
  typedef typename SolverType::result_type result_type;

private: // methods

  void pop2( result_type & fst, result_type & snd );
  void evalChilds2( BinaryExpression const& expr,  result_type & fst, result_type & snd );

private: // data
  SolverType solver_;
  std::stack<result_type> exprStack_;
};

template<typename SolverType>
bool metaSMTVisitorImpl<SolverType>::visitNode( Node const&) {
  
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::pop2( result_type & fst, result_type & snd ) {
  assert( exprStack_.size() >= 2);
  fst = exprStack_.top();
  exprStack_.pop();
  snd = exprStack_.top();
  exprStack_.pop();
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::evalChilds2( BinaryExpression const& expr, result_type & fst, result_type & snd ) {
  expr.lhs()->visit(*this);
  expr.rhs()->visit(*this);
  pop2(snd, fst);
}

template<typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitLessEqualOpr(LessEqualOpr const &o)
{
  result_type rhs, lhs, result; 
  evalChilds2(o, rhs, lhs);
 
  bool lhs_signed = false; // TODO: get real signs
  bool rhs_signed = false; // TODO: get real signs

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
  exprStack_.push(result);
}



} // namespace crave

#endif /* end of include guard: METASMTNODEVISITORIMPL_H */
