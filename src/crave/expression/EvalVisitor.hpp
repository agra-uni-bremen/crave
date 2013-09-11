#ifndef EVALVISITOR_HPP_
#define EVALVISITOR_HPP_

#include "../Context.hpp"
#include "../VariableContainer.hpp"
#include "Node.hpp"
#include "NodeVisitor.hpp"
#include "ReferenceExpression.hpp"

#include <map>
#include <stack>
#include <utility>

namespace crave {

class EvalVisitor : NodeVisitor {

  typedef NodePtr expression;
  typedef std::pair<Constant, bool> stack_entry;

public:
  typedef std::map<int, Constant> eval_map;

  EvalVisitor() : NodeVisitor(), exprStack_(), evalMap_(), result_() { }
  EvalVisitor(eval_map const& m) : NodeVisitor(), exprStack_(), evalMap_(m), result_() { }

private:
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

  void pop(stack_entry&);
  void pop2(stack_entry&, stack_entry&);
  void pop3(stack_entry&, stack_entry&, stack_entry&);
  void evalBinExpr( BinaryExpression const&, stack_entry&, stack_entry& );
  void evalTernExpr( TernaryExpression const&, stack_entry&, stack_entry&, stack_entry& );

public:
  void set_evaluation_map(eval_map const& m) {
    evalMap_ = m;
  }
  Constant get_result() const {
    return result_;
  }

  template<typename Expr>
  expression get_expression(Expr expr) {
    return LWGenerator()(expr);
  }

  template<typename Expr>
  bool evaluate(Expr expr, eval_map const& m) {
    set_evaluation_map(m);
    return evaluate(expr);
  }
  template<typename Expr>
  bool evaluate(Expr expr) {
    return evaluate(*get_expression(expr));
  }
  bool evaluate(Node const& expr, eval_map const& m) {
    set_evaluation_map(m);
    return evaluate(expr);
  }
  bool evaluate(Node const& expr) {

    expr.visit(*this);
    stack_entry entry;
    pop(entry);

    result_ = entry.first;
    return entry.second;
  }

private:
  std::stack<stack_entry> exprStack_;
  eval_map evalMap_;

  Constant result_;

  class LWGenerator {

    typedef std::pair<int, boost::shared_ptr<crave::ReferenceExpression> > ReadRefPair;
    typedef std::pair<int, boost::shared_ptr<crave::AssignResult> > WriteRefPair;

  public:
    LWGenerator()
      : vars_(), ctx_(vars_) { }

    template<typename Expr>
    expression operator()(Expr expr) {
      return boost::proto::eval(FixWidth()(expr), ctx_);
    }

  private:
    VariableContainer vars_;
    
    Context ctx_;
  };
};

inline void EvalVisitor::pop(stack_entry& fst)
{
  assert( exprStack_.size() >= 1 );
  fst = exprStack_.top();
  exprStack_.pop();
}
inline void EvalVisitor::pop2(stack_entry& fst, stack_entry& snd)
{
  assert( exprStack_.size() >= 2 );
  fst = exprStack_.top();
  exprStack_.pop();
  snd = exprStack_.top();
  exprStack_.pop();
}
inline void EvalVisitor::pop3(stack_entry& fst, stack_entry& snd, stack_entry& trd)
{
  assert( exprStack_.size() >= 3 );
  fst = exprStack_.top();
  exprStack_.pop();
  snd = exprStack_.top();
  exprStack_.pop();
  trd = exprStack_.top();
  exprStack_.pop();
}
inline void EvalVisitor::evalBinExpr(BinaryExpression const& bin, stack_entry& fst, stack_entry& snd)
{
  visitBinaryExpr(bin);
  pop2(snd, fst);
}
inline void EvalVisitor::evalTernExpr(TernaryExpression const& tern, stack_entry& fst,
                                      stack_entry& snd, stack_entry& trd )
{
  visitTernaryExpr(tern);
  pop3(trd, snd, fst);
}

} // end namespace crave
#endif
