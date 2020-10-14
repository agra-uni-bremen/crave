/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
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
