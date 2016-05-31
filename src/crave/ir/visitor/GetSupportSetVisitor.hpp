// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <set>

#include "../Node.hpp"
#include "NodeVisitor.hpp"

namespace crave {

class GetSupportSetVisitor : public NodeVisitor {
 public:
  GetSupportSetVisitor() : NodeVisitor(), support_vars() {}

 private:
  virtual void visitNode(Node const&);
  virtual void visitTerminal(Terminal const&);
  virtual void visitUnaryExpr(UnaryExpression const&);
  virtual void visitUnaryOpr(UnaryOperator const&);
  virtual void visitBinaryExpr(BinaryExpression const&);
  virtual void visitBinaryOpr(BinaryOperator const&);
  virtual void visitTernaryExpr(TernaryExpression const&);
  virtual void visitPlaceholder(Placeholder const&);
  virtual void visitVariableExpr(VariableExpr const&);
  virtual void visitConstant(Constant const&);
  virtual void visitVectorExpr(VectorExpr const&);
  virtual void visitNotOpr(NotOpr const&);
  virtual void visitNegOpr(NegOpr const&);
  virtual void visitComplementOpr(ComplementOpr const&);
  virtual void visitInside(Inside const&);
  virtual void visitExtendExpr(ExtendExpression const&);
  virtual void visitAndOpr(AndOpr const&);
  virtual void visitOrOpr(OrOpr const&);
  virtual void visitLogicalAndOpr(LogicalAndOpr const&);
  virtual void visitLogicalOrOpr(LogicalOrOpr const&);
  virtual void visitXorOpr(XorOpr const&);
  virtual void visitEqualOpr(EqualOpr const&);
  virtual void visitNotEqualOpr(NotEqualOpr const&);
  virtual void visitLessOpr(LessOpr const&);
  virtual void visitLessEqualOpr(LessEqualOpr const&);
  virtual void visitGreaterOpr(GreaterOpr const&);
  virtual void visitGreaterEqualOpr(GreaterEqualOpr const&);
  virtual void visitPlusOpr(PlusOpr const&);
  virtual void visitMinusOpr(MinusOpr const&);
  virtual void visitMultipliesOpr(MultipliesOpr const&);
  virtual void visitDevideOpr(DevideOpr const&);
  virtual void visitModuloOpr(ModuloOpr const&);
  virtual void visitShiftLeftOpr(ShiftLeftOpr const&);
  virtual void visitShiftRightOpr(ShiftRightOpr const&);
  virtual void visitVectorAccess(VectorAccess const&);
  virtual void visitIfThenElse(IfThenElse const&);
  virtual void visitForEach(ForEach const&);
  virtual void visitUnique(Unique const&);
  virtual void visitBitslice(Bitslice const&);

 public:
  std::set<int>& getSupportVars() { return support_vars; }

 private:
  std::set<int> support_vars;
};

}  // end namespace crave
