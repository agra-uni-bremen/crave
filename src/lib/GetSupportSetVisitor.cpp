// Copyright 2014 The CRAVE developers. All rights reserved.//

#include "../crave/ir/visitor/GetSupportSetVisitor.hpp"

namespace crave {

void GetSupportSetVisitor::visitNode(const Node&) {}

void GetSupportSetVisitor::visitTerminal(const Terminal&) {}

void GetSupportSetVisitor::visitUnaryExpr(const UnaryExpression& u) {
  u.child()->visit(this);
}

void GetSupportSetVisitor::visitUnaryOpr(const UnaryOperator&) {}

void GetSupportSetVisitor::visitBinaryExpr(const BinaryExpression& b) {
  b.lhs()->visit(this);
  b.rhs()->visit(this);
}

void GetSupportSetVisitor::visitBinaryOpr(const BinaryOperator&) {}

void GetSupportSetVisitor::visitTernaryExpr(const TernaryExpression& t) {
  t.a()->visit(this);
  t.b()->visit(this);
  t.c()->visit(this);
}

void GetSupportSetVisitor::visitPlaceholder(const Placeholder& pl) {}

void GetSupportSetVisitor::visitVariableExpr(const VariableExpr& v) {
  support_vars.insert(v.id());
}

void GetSupportSetVisitor::visitConstant(const Constant& c) {}

void GetSupportSetVisitor::visitVectorExpr(const VectorExpr& v) {
  support_vars.insert(v.id());
}

void GetSupportSetVisitor::visitNotOpr(const NotOpr& n) { visitUnaryExpr(n); }

void GetSupportSetVisitor::visitNegOpr(const NegOpr& n) { visitUnaryExpr(n); }

void GetSupportSetVisitor::visitComplementOpr(const ComplementOpr& c) {
  visitUnaryExpr(c);
}

void GetSupportSetVisitor::visitInside(const Inside& i) { visitUnaryExpr(i); }

void GetSupportSetVisitor::visitExtendExpr(const ExtendExpression& e) {
  visitUnaryExpr(e);
}

void GetSupportSetVisitor::visitAndOpr(const AndOpr& a) { visitBinaryExpr(a); }

void GetSupportSetVisitor::visitOrOpr(const OrOpr& o) { visitBinaryExpr(o); }

void GetSupportSetVisitor::visitLogicalAndOpr(const LogicalAndOpr& la) {
  visitBinaryExpr(la);
}

void GetSupportSetVisitor::visitLogicalOrOpr(const LogicalOrOpr& lo) {
  visitBinaryExpr(lo);
}

void GetSupportSetVisitor::visitXorOpr(const XorOpr& x) { visitBinaryExpr(x); }

void GetSupportSetVisitor::visitEqualOpr(const EqualOpr& eq) {
  visitBinaryExpr(eq);
}

void GetSupportSetVisitor::visitNotEqualOpr(const NotEqualOpr& neq) {
  visitBinaryExpr(neq);
}

void GetSupportSetVisitor::visitLessOpr(const LessOpr& l) {
  visitBinaryExpr(l);
}

void GetSupportSetVisitor::visitLessEqualOpr(const LessEqualOpr& le) {
  visitBinaryExpr(le);
}

void GetSupportSetVisitor::visitGreaterOpr(const GreaterOpr& g) {
  visitBinaryExpr(g);
}

void GetSupportSetVisitor::visitGreaterEqualOpr(const GreaterEqualOpr& ge) {
  visitBinaryExpr(ge);
}

void GetSupportSetVisitor::visitPlusOpr(const PlusOpr& p) {
  visitBinaryExpr(p);
}

void GetSupportSetVisitor::visitMinusOpr(const MinusOpr& m) {
  visitBinaryExpr(m);
}

void GetSupportSetVisitor::visitMultipliesOpr(const MultipliesOpr& m) {
  visitBinaryExpr(m);
}

void GetSupportSetVisitor::visitDevideOpr(const DevideOpr& d) {
  visitBinaryExpr(d);
}

void GetSupportSetVisitor::visitModuloOpr(const ModuloOpr& m) {
  visitBinaryExpr(m);
}

void GetSupportSetVisitor::visitShiftLeftOpr(const ShiftLeftOpr& shl) {
  visitBinaryExpr(shl);
}

void GetSupportSetVisitor::visitShiftRightOpr(const ShiftRightOpr& shr) {
  visitBinaryExpr(shr);
}

void GetSupportSetVisitor::visitVectorAccess(const VectorAccess& va) {
  visitBinaryExpr(va);
}

void GetSupportSetVisitor::visitForEach(const ForEach& fe) {
  visitBinaryExpr(fe);
}

void GetSupportSetVisitor::visitUnique(const Unique& u) { visitUnaryExpr(u); }

void GetSupportSetVisitor::visitIfThenElse(const IfThenElse& ite) {
  visitTernaryExpr(ite);
}

void GetSupportSetVisitor::visitBitslice(const Bitslice& b) {
  visitUnaryExpr(b);
}

}  // end namespace crave
