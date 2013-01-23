#ifndef TODOTNODEVISITOR_HPP_
#define TODOTNODEVISITOR_HPP_

#include <set>

#include "Node.hpp"
#include "NodeVisitor.hpp"

namespace crave {

class ToDotVisitor : public NodeVisitor {
public:
  ToDotVisitor( std::ostream & os ) : NodeVisitor(), out_(os), nodes_() { }

  void visitNode( Node const & );
  void visitTerminal( Terminal const & );
  void visitUnaryExpr( UnaryExpression const & );
  void visitUnaryOpr( UnaryOperator const & );
  void visitBinaryExpr( BinaryExpression const & );
  void visitBinaryOpr( BinaryOperator const & );
  void visitTernaryExpr( TernaryExpression const & );
  void visitPlaceholder( Placeholder const & );
  void visitVariableExpr( VariableExpr const & );
  void visitConstant( Constant const & );
  void visitVectorExpr( VectorExpr const & );
  void visitNotOpr( NotOpr const & );
  void visitNegOpr( NegOpr const & );
  void visitComplementOpr( ComplementOpr const & );
  void visitInside( Inside const & );
  void visitAndOpr( AndOpr const & );
  void visitOrOpr( OrOpr const & );
  void visitLogicalAndOpr( LogicalAndOpr const & );
  void visitLogicalOrOpr( LogicalOrOpr const & );
  void visitXorOpr( XorOpr const & );
  void visitEqualOpr( EqualOpr const & );
  void visitNotEqualOpr( NotEqualOpr const & );
  void visitLessOpr( LessOpr const &o );
  void visitLessEqualOpr( LessEqualOpr const & );
  void visitGreaterOpr( GreaterOpr const & );
  void visitGreaterEqualOpr( GreaterEqualOpr const & );
  void visitPlusOpr( PlusOpr const & );
  void visitMinusOpr( MinusOpr const & );
  void visitMultipliesOpr( MultipliesOpr const & );
  void visitDevideOpr( DevideOpr const & );
  void visitModuloOpr( ModuloOpr const & );
  void visitShiftLeftOpr( ShiftLeftOpr const & );
  void visitShiftRightOpr( ShiftRightOpr const & );
  void visitAssignOpr( AssignOpr const & );
  void visitVectorAccess( VectorAccess const & );
  void visitIfThenElse( IfThenElse const & );

private:
  bool putNode(Node const *);

private:
  std::ostream& out_;
  std::set<Node const*> nodes_;
};

} //end namespace crave

#endif /* TODOTNODEVISITOR_HPP_ */
