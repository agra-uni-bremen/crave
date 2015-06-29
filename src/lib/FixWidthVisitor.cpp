// Copyright 2014 The CRAVE developers. All rights reserved.//

#include "../crave/ir/visitor/FixWidthVisitor.hpp"

#include <stdexcept>

namespace crave {

void FixWidthVisitor::visitNode(const Node&) {}

void FixWidthVisitor::visitTerminal(const Terminal&) {}

void FixWidthVisitor::visitUnaryExpr(const UnaryExpression& u) { u.child()->visit(this); }

void FixWidthVisitor::visitUnaryOpr(const UnaryOperator&) {}

void FixWidthVisitor::visitBinaryExpr(const BinaryExpression& b) {
  b.lhs()->visit(this);
  b.rhs()->visit(this);
}

void FixWidthVisitor::visitBinaryOpr(const BinaryOperator&) {}

void FixWidthVisitor::visitTernaryExpr(const TernaryExpression& t) {
  t.a()->visit(this);
  t.b()->visit(this);
  t.c()->visit(this);
}

void FixWidthVisitor::visitPlaceholder(const Placeholder& pl) {
  exprStack_.push(std::make_pair(new Placeholder(pl), placeholder_bitsize()));
}

void FixWidthVisitor::visitVariableExpr(const VariableExpr& v) {
  exprStack_.push(std::make_pair(new VariableExpr(v), v.bitsize()));
}

void FixWidthVisitor::visitConstant(const Constant& c) {
  exprStack_.push(std::make_pair(new Constant(c), c.bitsize()));
}

void FixWidthVisitor::visitVectorExpr(const VectorExpr& v) {
  exprStack_.push(std::make_pair(new VectorExpr(v), v.bitsize()));
}

void FixWidthVisitor::visitNotOpr(const NotOpr& n) {
  visitUnaryExpr(n);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new NotOpr(e.first), e.second));
}

void FixWidthVisitor::visitNegOpr(const NegOpr& n) {
  visitUnaryExpr(n);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new NegOpr(e.first), e.second));
}

void FixWidthVisitor::visitComplementOpr(const ComplementOpr& c) {
  visitUnaryExpr(c);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new ComplementOpr(e.first), e.second));
}

void FixWidthVisitor::visitInside(const Inside& i) {
  visitUnaryExpr(i);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new Inside(e.first, i.collection()), 1));
}

void FixWidthVisitor::visitExtendExpr(const ExtendExpression& e) {
  visitUnaryExpr(e);

  stack_entry entry;
  pop(entry);

  exprStack_.push(std::make_pair(new ExtendExpression(entry.first, e.value()), entry.second));
}

void FixWidthVisitor::visitAndOpr(const AndOpr& a) {
    visitSimpleTwoBinExpr<AndOpr> (a);
}

void FixWidthVisitor::visitOrOpr(const OrOpr& o) {
  visitSimpleTwoBinExpr<OrOpr>(o);
}

void FixWidthVisitor::visitLogicalAndOpr(const LogicalAndOpr& la) {
  visitSimpleTwoBin2Expr<LogicalAndOpr>(la);
}

void FixWidthVisitor::visitLogicalOrOpr(const LogicalOrOpr& lo) {
  visitSimpleTwoBin2Expr<LogicalOrOpr>(lo);
}

void FixWidthVisitor::visitXorOpr(const XorOpr& x) {
  visitSimpleTwoBinExpr<XorOpr> (x);
}

void FixWidthVisitor::visitEqualOpr(const EqualOpr& eq) {
  visitSimpleTwoBin2Expr<EqualOpr>(eq);
}

void FixWidthVisitor::visitNotEqualOpr(const NotEqualOpr& neq) {
  visitSimpleTwoBin2Expr<NotEqualOpr>(neq);
}

void FixWidthVisitor::visitLessOpr(const LessOpr& l) {
  visitSimpleTwoBin2Expr<LessOpr>(l);
}

void FixWidthVisitor::visitLessEqualOpr(const LessEqualOpr& le) {
  visitSimpleTwoBin2Expr<LessEqualOpr> (le);
}

void FixWidthVisitor::visitGreaterOpr(const GreaterOpr& g) {
  visitSimpleTwoBin2Expr<GreaterOpr> (g);
}

void FixWidthVisitor::visitGreaterEqualOpr(const GreaterEqualOpr& ge) {
  visitSimpleTwoBin2Expr<GreaterEqualOpr>(ge);
}

void FixWidthVisitor::visitPlusOpr(const PlusOpr& p) {
  visitSimpleTwoBinExpr<PlusOpr>(p);
}

void FixWidthVisitor::visitMinusOpr(const MinusOpr& m) {
  visitSimpleTwoBinExpr<MinusOpr>(m);
}

void FixWidthVisitor::visitMultipliesOpr(const MultipliesOpr& m) {
  visitSimpleTwoBinExpr<MultipliesOpr>(m);
}

void FixWidthVisitor::visitDevideOpr(const DevideOpr& d) {
  visitSimpleTwoBinExpr<DevideOpr>(d);
}

void FixWidthVisitor::visitModuloOpr(const ModuloOpr& m) {
  visitSimpleTwoBinExpr<ModuloOpr>(m);
}

void FixWidthVisitor::visitShiftLeftOpr(const ShiftLeftOpr& shl) {
  visitSimpleTwoBinExpr<ShiftLeftOpr>(shl);
}

void FixWidthVisitor::visitShiftRightOpr(const ShiftRightOpr& shr) {
  visitSimpleTwoBinExpr<ShiftRightOpr>(shr);
}

void FixWidthVisitor::visitVectorAccess(const VectorAccess& va) {
  visitSimpleTwoBinExpr<VectorAccess>(va);
}

void FixWidthVisitor::visitForEach(const ForEach& fe) {
  visitSimpleTwoBin2Expr<ForEach>(fe);
}

void FixWidthVisitor::visitUnique(const Unique& u) {
  visitUnaryExpr(u);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new Unique(e.first), e.second));
}

void FixWidthVisitor::visitIfThenElse(const IfThenElse& ite) {
  stack_entry a, b, c;
  evalTernExpr(ite, a, b, c);

  exprStack_.push(std::make_pair(new IfThenElse(a.first, b.first, c.first), a.second));
}

void FixWidthVisitor::visitBitslice(const Bitslice& b) {
  visitUnaryExpr(b);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new Bitslice(e.first, b.r(), b.l()), b.r() - b.l() + 1));
}

}  // end namespace crave
