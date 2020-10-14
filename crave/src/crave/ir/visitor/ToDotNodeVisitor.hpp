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

  template <typename T>
  void binaryOperator(T const& o, std::string name) {
    if (putNode(&o)) {
      visitNode(o);
      out_ << " [label=\"";
      visitBinaryOpr(o);
      out_ << ": " + name + "\"]" << std::endl;
    }
    visitBinaryExpr(o);
  }
  template <typename T>
  void unaryOperator(T const& o, std::string name) {
    if (putNode(&o)) {
      visitNode(o);
      out_ << " [label=\"";
      visitUnaryOpr(o);
      out_ << ": " + name + "\"]" << std::endl;
    }
    visitUnaryExpr(o);
  }

 private:
  std::ostream& out_;
  std::set<Node const*> nodes_;
};

}  // end namespace crave
