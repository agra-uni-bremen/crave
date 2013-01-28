#include "../../crave/expression/Node.hpp"
#include "../../crave/expression/NodeVisitor.hpp"
#include "metaSMTNodeVisitorImpl.hpp"

#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/backend/SWORD_Backend.hpp>

namespace crave {

class FactoryMetaSMT {
public:
  /**
    * creates a new metaSMTVisitor with SWORD as the backend.
    * Caller is responsible for deleting the visitor.
    **/
  static metaSMTVisitor* newVisitorSWORD() {
  	return new metaSMTVisitorImpl< metaSMT::DirectSolver_Context< metaSMT::solver::SWORD_Backend > > ();
  }
};

metaSMTVisitor* visitor = FactoryMetaSMT::newVisitorSWORD();


inline void metaSMTVisitor::visitNode( Node const &n ) {
  visitor->visitNode(n);
}

inline void metaSMTVisitor::visitTerminal( Terminal const &n ) {
  visitor->visitTerminal(n);
}

inline void metaSMTVisitor::visitUnaryExpr( UnaryExpression const &n ) {
  visitor->visitUnaryExpr(n);
}

inline void metaSMTVisitor::visitUnaryOpr( UnaryOperator const &n ) {
  visitor->visitUnaryOpr(n);
}

inline void metaSMTVisitor::visitBinaryExpr( BinaryExpression const &n ) {
  visitor->visitBinaryExpr(n);
}

inline void metaSMTVisitor::visitBinaryOpr( BinaryOperator const &n ) {
  visitor->visitBinaryOpr(n);
}

inline void metaSMTVisitor::visitTernaryExpr( TernaryExpression const &n ) {
  visitor->visitTernaryExpr(n);
}

inline void metaSMTVisitor::visitPlaceholder( Placeholder const &n ) {
  visitor->visitPlaceholder(n);
}

inline void metaSMTVisitor::visitVariableExpr( VariableExpr const &n ) {
  visitor->visitVariableExpr(n);
}

inline void metaSMTVisitor::visitConstant( Constant const &n ) {
  visitor->visitConstant(n);
}

inline void metaSMTVisitor::visitVectorExpr( VectorExpr const &n ) {
  visitor->visitVectorExpr(n);
}

inline void metaSMTVisitor::visitNotOpr( NotOpr const &n ) {
  visitor->visitNotOpr(n);
}

inline void metaSMTVisitor::visitNegOpr( NegOpr const &n ) {
  visitor->visitNegOpr(n);
}

inline void metaSMTVisitor::visitComplementOpr( ComplementOpr const &n ) {
  visitor->visitComplementOpr(n);
}

inline void metaSMTVisitor::visitInside( Inside const &n ) {
  visitor->visitInside(n);
}

inline void metaSMTVisitor::visitAndOpr( AndOpr const &n ) {
  visitor->visitAndOpr(n);
}

inline void metaSMTVisitor::visitOrOpr( OrOpr const &n ) {
  visitor->visitOrOpr(n);
}

inline void metaSMTVisitor::visitLogicalAndOpr( LogicalAndOpr const &n ) {
  visitor->visitLogicalAndOpr(n);
}

inline void metaSMTVisitor::visitLogicalOrOpr( LogicalOrOpr const &n ) {
  visitor->visitLogicalOrOpr(n);
}

inline void metaSMTVisitor::visitXorOpr( XorOpr const &n ) {
  visitor->visitXorOpr(n);
}

inline void metaSMTVisitor::visitEqualOpr( EqualOpr const &n ) {
  visitor->visitEqualOpr(n);
}

inline void metaSMTVisitor::visitNotEqualOpr( NotEqualOpr const &n ) {
  visitor->visitNotEqualOpr(n);
}

inline void metaSMTVisitor::visitLessOpr( LessOpr const &n ) {
  visitor->visitLessOpr(n);
}

inline void metaSMTVisitor::visitLessEqualOpr( LessEqualOpr const &n ) {
  visitor->visitLessEqualOpr(n);
}

inline void metaSMTVisitor::visitGreaterOpr( GreaterOpr const &n ) {
  visitor->visitGreaterOpr(n);
}

inline void metaSMTVisitor::visitGreaterEqualOpr( GreaterEqualOpr const &n ) {
  visitor->visitGreaterEqualOpr(n);
}

inline void metaSMTVisitor::visitPlusOpr( PlusOpr const &n ) {
  visitor->visitPlusOpr(n);
}

inline void metaSMTVisitor::visitMinusOpr( MinusOpr const &n ) {
  visitor->visitMinusOpr(n);
}

inline void metaSMTVisitor::visitMultipliesOpr( MultipliesOpr const &n ) {
  visitor->visitMultipliesOpr(n);
}

inline void metaSMTVisitor::visitDevideOpr( DevideOpr const &n ) {
  visitor->visitDevideOpr(n);
}

inline void metaSMTVisitor::visitModuloOpr( ModuloOpr const &n ) {
  visitor->visitModuloOpr(n);
}

inline void metaSMTVisitor::visitShiftLeftOpr( ShiftLeftOpr const &n ) {
  visitor->visitShiftLeftOpr(n);
}

inline void metaSMTVisitor::visitShiftRightOpr( ShiftRightOpr const &n ) {
  visitor->visitShiftRightOpr(n);
}

inline void metaSMTVisitor::visitAssignOpr( AssignOpr const &n ) {
  visitor->visitAssignOpr(n);
}

inline void metaSMTVisitor::visitVectorAccess( VectorAccess const &n ) {
  visitor->visitVectorAccess(n);
}

inline void metaSMTVisitor::visitIfThenElse( IfThenElse const &n ) {
  visitor->visitIfThenElse(n);
}

inline void metaSMTVisitor::makeAssertion( Node const &n ) {
  visitor->makeAssertion(n);
}

inline void metaSMTVisitor::makeAssumption( Node const &n ) {
  visitor->makeAssumption(n);
}

inline bool metaSMTVisitor::solve() {
  return visitor->solve();
}

} // end crave namespace
