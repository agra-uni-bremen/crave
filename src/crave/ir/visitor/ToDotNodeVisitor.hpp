// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <ostream>
#include <set>
#include <string>

#include "../Node.hpp"
#include "NodeVisitor.hpp"

namespace crave {

class ToDotVisitor : public NodeVisitor {
 public:
  explicit ToDotVisitor(std::ostream& os) : NodeVisitor(), out_(os), nodes_() {}

  void visitNode(Node const&);
  void visitTerminal(Terminal const&);
  void visitUnaryExpr(UnaryExpression const&);
  void visitUnaryOpr(UnaryOperator const&);
  void visitBinaryExpr(BinaryExpression const&);
  void visitBinaryOpr(BinaryOperator const&);
  void visitTernaryExpr(TernaryExpression const&);
  void visitPlaceholder(Placeholder const&);
  void visitVariableExpr(VariableExpr const&);
  void visitConstant(Constant const&);
  void visitVectorExpr(VectorExpr const&);
  void visitNotOpr(NotOpr const&);
  void visitNegOpr(NegOpr const&);
  void visitComplementOpr(ComplementOpr const&);
  void visitInside(Inside const&);
  void visitExtendExpr(ExtendExpression const&);
  void visitAndOpr(AndOpr const&);
  void visitOrOpr(OrOpr const&);
  void visitLogicalAndOpr(LogicalAndOpr const&);
  void visitLogicalOrOpr(LogicalOrOpr const&);
  void visitXorOpr(XorOpr const&);
  void visitEqualOpr(EqualOpr const&);
  void visitNotEqualOpr(NotEqualOpr const&);
  void visitLessOpr(LessOpr const&);
  void visitLessEqualOpr(LessEqualOpr const&);
  void visitGreaterOpr(GreaterOpr const&);
  void visitGreaterEqualOpr(GreaterEqualOpr const&);
  void visitPlusOpr(PlusOpr const&);
  void visitMinusOpr(MinusOpr const&);
  void visitMultipliesOpr(MultipliesOpr const&);
  void visitDevideOpr(DevideOpr const&);
  void visitModuloOpr(ModuloOpr const&);
  void visitShiftLeftOpr(ShiftLeftOpr const&);
  void visitShiftRightOpr(ShiftRightOpr const&);
  void visitVectorAccess(VectorAccess const&);
  void visitIfThenElse(IfThenElse const&);
  void visitForEach(ForEach const&);
  void visitUnique(Unique const&);
  void visitBitslice(Bitslice const&);

 private:
  bool putNode(Node const*);
  void binaryOperator(std::string name);

  template <typename T>
  void binaryOperator(T const &o ,std::string name)
{
    if (putNode(&o)) {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": "+name+"\"]" << std::endl;
  }
  visitBinaryExpr(o);
}
  
 private:
  std::ostream& out_;
  std::set<Node const*> nodes_;
};

}  // end namespace crave
