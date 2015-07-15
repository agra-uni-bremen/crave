// Copyright 2014 The CRAVE developers. All rights reserved.//

#include "../crave/ir/visitor/FixWidthVisitor.hpp"

#include <stdexcept>

namespace crave {

void FixWidthVisitor::pop(stack_entry& fst) {
  assert(exprStack_.size() >= 1);
  fst = exprStack_.top();
  exprStack_.pop();
}

void FixWidthVisitor::pop2(stack_entry& fst, stack_entry& snd) {
  assert(exprStack_.size() >= 2);
  fst = exprStack_.top();
  exprStack_.pop();
  snd = exprStack_.top();
  exprStack_.pop();
}

void FixWidthVisitor::pop3(stack_entry& fst, stack_entry& snd, stack_entry& trd) {
  assert(exprStack_.size() >= 3);
  fst = exprStack_.top();
  exprStack_.pop();
  snd = exprStack_.top();
  exprStack_.pop();
  trd = exprStack_.top();
  exprStack_.pop();
}

void FixWidthVisitor::evalBinExpr(BinaryExpression const& bin, stack_entry& fst, stack_entry& snd,
                                  bool fixWidth = true) {
  visitBinaryExpr(bin);
  pop2(snd, fst);
  if (!fixWidth) return;
  if (fst.second < snd.second) {
    unsigned int diff = snd.second - fst.second;
    fst.first = result_type(new ExtendExpression(fst.first.get(), diff));
    fst.second = snd.second;
  } else if (fst.second > snd.second) {
    unsigned int diff = fst.second - snd.second;
    snd.first = result_type(new ExtendExpression(snd.first.get(), diff));
    snd.second = fst.second;
  }
}

void FixWidthVisitor::evalTernExpr(TernaryExpression const& tern, stack_entry& fst, stack_entry& snd,
                                   stack_entry& trd) {
  visitTernaryExpr(tern);
  pop3(trd, snd, fst);
}

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

template <typename T>
void FixWidthVisitor::visitNumberResultBinExpr(const T& object) {
  stack_entry lhs, rhs;
  evalBinExpr(object, lhs, rhs);
  exprStack_.push(std::make_pair(new T(lhs.first, rhs.first), lhs.second));
}

template <typename T>
void FixWidthVisitor::visitNumberResultUnaryExpr(const T& object) {
  visitUnaryExpr(object);
  stack_entry e;
  pop(e);
  exprStack_.push(std::make_pair(new T(e.first), e.second));
}

template <typename T>
void FixWidthVisitor::visitBooleanResultBinExpr(const T& object) {
  stack_entry lhs, rhs;
  evalBinExpr(object, lhs, rhs);
  exprStack_.push(std::make_pair(new T(lhs.first, rhs.first), 1));
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

void FixWidthVisitor::visitNotOpr(const NotOpr& n) { visitNumberResultUnaryExpr(n); }

void FixWidthVisitor::visitNegOpr(const NegOpr& n) { visitNumberResultUnaryExpr(n); }

void FixWidthVisitor::visitComplementOpr(const ComplementOpr& c) { visitNumberResultUnaryExpr(c); }

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

void FixWidthVisitor::visitAndOpr(const AndOpr& a) { visitNumberResultBinExpr(a); }

void FixWidthVisitor::visitOrOpr(const OrOpr& o) { visitNumberResultBinExpr(o); }

void FixWidthVisitor::visitLogicalAndOpr(const LogicalAndOpr& la) { visitBooleanResultBinExpr(la); }

void FixWidthVisitor::visitLogicalOrOpr(const LogicalOrOpr& lo) { visitBooleanResultBinExpr(lo); }

void FixWidthVisitor::visitXorOpr(const XorOpr& x) { visitNumberResultBinExpr(x); }

void FixWidthVisitor::visitEqualOpr(const EqualOpr& eq) { visitBooleanResultBinExpr(eq); }

void FixWidthVisitor::visitNotEqualOpr(const NotEqualOpr& neq) { visitBooleanResultBinExpr(neq); }

void FixWidthVisitor::visitLessOpr(const LessOpr& l) { visitBooleanResultBinExpr(l); }

void FixWidthVisitor::visitLessEqualOpr(const LessEqualOpr& le) { visitBooleanResultBinExpr(le); }

void FixWidthVisitor::visitGreaterOpr(const GreaterOpr& g) { visitBooleanResultBinExpr(g); }

void FixWidthVisitor::visitGreaterEqualOpr(const GreaterEqualOpr& ge) { visitBooleanResultBinExpr(ge); }

void FixWidthVisitor::visitPlusOpr(const PlusOpr& p) { visitNumberResultBinExpr(p); }

void FixWidthVisitor::visitMinusOpr(const MinusOpr& m) { visitNumberResultBinExpr(m); }

void FixWidthVisitor::visitMultipliesOpr(const MultipliesOpr& m) { visitNumberResultBinExpr(m); }

void FixWidthVisitor::visitDevideOpr(const DevideOpr& d) { visitNumberResultBinExpr(d); }

void FixWidthVisitor::visitModuloOpr(const ModuloOpr& m) { visitNumberResultBinExpr(m); }

void FixWidthVisitor::visitShiftLeftOpr(const ShiftLeftOpr& shl) { visitNumberResultBinExpr(shl); }

void FixWidthVisitor::visitShiftRightOpr(const ShiftRightOpr& shr) { visitNumberResultBinExpr(shr); }

void FixWidthVisitor::visitVectorAccess(const VectorAccess& va) {
  stack_entry lhs, rhs;
  evalBinExpr(va, lhs, rhs, false);
  exprStack_.push(std::make_pair(new VectorAccess(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitForEach(const ForEach& fe) {
  stack_entry lhs, rhs;
  evalBinExpr(fe, lhs, rhs, false);
  exprStack_.push(std::make_pair(new ForEach(lhs.first, rhs.first), 1));
}

void FixWidthVisitor::visitUnique(const Unique& u) { visitNumberResultUnaryExpr(u); }

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
