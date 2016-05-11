// Copyright 2014 The CRAVE developers. All rights reserved.//

#include "../crave/ir/visitor/ComplexityEstimationVisitor.hpp"

#include <stdexcept>

namespace crave {

void ComplexityEstimationVisitor::pop(stack_entry& fst) {
  assert(exprStack_.size() >= 1);
  fst = exprStack_.top();
  exprStack_.pop();
}

void ComplexityEstimationVisitor::pop2(stack_entry& fst, stack_entry& snd) {
  assert(exprStack_.size() >= 2);
  fst = exprStack_.top();
  exprStack_.pop();
  snd = exprStack_.top();
  exprStack_.pop();
}

void ComplexityEstimationVisitor::pop3(stack_entry& fst, stack_entry& snd, stack_entry& trd) {
  assert(exprStack_.size() >= 3);
  fst = exprStack_.top();
  exprStack_.pop();
  snd = exprStack_.top();
  exprStack_.pop();
  trd = exprStack_.top();
  exprStack_.pop();
}

void ComplexityEstimationVisitor::evalBinExpr(BinaryExpression const& bin, stack_entry& fst, stack_entry& snd) {
  visitBinaryExpr(bin);
  pop2(snd, fst);
}

void ComplexityEstimationVisitor::evalTernExpr(TernaryExpression const& tern, stack_entry& fst, stack_entry& snd,
                                               stack_entry& trd) {
  visitTernaryExpr(tern);
  pop3(trd, snd, fst);
}

void ComplexityEstimationVisitor::visitNode(const Node&) {}

void ComplexityEstimationVisitor::visitTerminal(const Terminal&) {}

void ComplexityEstimationVisitor::visitUnaryExpr(const UnaryExpression& u) { u.child()->visit(this); }

void ComplexityEstimationVisitor::visitUnaryOpr(const UnaryOperator&) {}

void ComplexityEstimationVisitor::visitBinaryExpr(const BinaryExpression& b) {
  b.lhs()->visit(this);
  b.rhs()->visit(this);
}

void ComplexityEstimationVisitor::visitBinaryOpr(const BinaryOperator&) {}

void ComplexityEstimationVisitor::visitTernaryExpr(const TernaryExpression& t) {
  t.a()->visit(this);
  t.b()->visit(this);
  t.c()->visit(this);
}

template <typename T>
void ComplexityEstimationVisitor::visitSimpleUnaryExpr(const T& object) {
  visitUnaryExpr(object);
  stack_entry e;
  pop(e);
  exprStack_.push(std::make_pair(new T(e.first), e.second));
}

template <typename T>
void ComplexityEstimationVisitor::visitSimpleBinExpr(const T& object) {
  stack_entry lhs, rhs;
  evalBinExpr(object, lhs, rhs);
  exprStack_.push(std::make_pair(new T(lhs.first, rhs.first), lhs.second + rhs.second));
}

template <typename T>
void ComplexityEstimationVisitor::visitComplexBinExpr(const T& object) {
  stack_entry lhs, rhs;
  evalBinExpr(object, lhs, rhs);
  unsigned int complexity = 0;
  if (lhs.second > 0) {
    if (rhs.second > 0) {
      complexity = lhs.second * rhs.second;
    } else {
      complexity = lhs.second;
    }
  } else {
    complexity = rhs.second;
  }
  exprStack_.push(std::make_pair(new T(lhs.first, rhs.first), complexity));
}

void ComplexityEstimationVisitor::visitPlaceholder(const Placeholder&) {
  throw std::runtime_error("Placeholder is not allowed in ComplexityEstimationVisitor.");
}

void ComplexityEstimationVisitor::visitVariableExpr(const VariableExpr& v) {
  exprStack_.push(std::make_pair(new VariableExpr(v), v.bitsize()));
}

void ComplexityEstimationVisitor::visitConstant(const Constant& c) {
  exprStack_.push(std::make_pair(new Constant(c), 0));
}

void ComplexityEstimationVisitor::visitVectorExpr(const VectorExpr& v) {
  exprStack_.push(std::make_pair(new VectorExpr(v), v.bitsize()));
}

void ComplexityEstimationVisitor::visitNotOpr(const NotOpr& n) { visitSimpleUnaryExpr(n); }

void ComplexityEstimationVisitor::visitNegOpr(const NegOpr& n) { visitSimpleUnaryExpr(n); }

void ComplexityEstimationVisitor::visitComplementOpr(const ComplementOpr& c) { visitSimpleUnaryExpr(c); }

void ComplexityEstimationVisitor::visitInside(const Inside& i) {
  visitUnaryExpr(i);
  stack_entry e;
  pop(e);
  exprStack_.push(std::make_pair(new Inside(e.first, i.collection()), e.second));
}

void ComplexityEstimationVisitor::visitExtendExpr(const ExtendExpression& e) {
  visitUnaryExpr(e);

  stack_entry entry;
  pop(entry);

  exprStack_.push(std::make_pair(new ExtendExpression(entry.first, e.value()), entry.second));
}

void ComplexityEstimationVisitor::visitAndOpr(const AndOpr& a) { visitSimpleBinExpr(a); }

void ComplexityEstimationVisitor::visitOrOpr(const OrOpr& o) { visitSimpleBinExpr(o); }

void ComplexityEstimationVisitor::visitLogicalAndOpr(const LogicalAndOpr& la) { visitSimpleBinExpr(la); }

void ComplexityEstimationVisitor::visitLogicalOrOpr(const LogicalOrOpr& lo) { visitSimpleBinExpr(lo); }

void ComplexityEstimationVisitor::visitXorOpr(const XorOpr& x) { visitSimpleBinExpr(x); }

void ComplexityEstimationVisitor::visitEqualOpr(const EqualOpr& eq) { visitSimpleBinExpr(eq); }

void ComplexityEstimationVisitor::visitNotEqualOpr(const NotEqualOpr& neq) { visitSimpleBinExpr(neq); }

void ComplexityEstimationVisitor::visitLessOpr(const LessOpr& l) { visitSimpleBinExpr(l); }

void ComplexityEstimationVisitor::visitLessEqualOpr(const LessEqualOpr& le) { visitSimpleBinExpr(le); }

void ComplexityEstimationVisitor::visitGreaterOpr(const GreaterOpr& g) { visitSimpleBinExpr(g); }

void ComplexityEstimationVisitor::visitGreaterEqualOpr(const GreaterEqualOpr& ge) { visitSimpleBinExpr(ge); }

void ComplexityEstimationVisitor::visitPlusOpr(const PlusOpr& p) { visitSimpleBinExpr(p); }

void ComplexityEstimationVisitor::visitMinusOpr(const MinusOpr& m) { visitSimpleBinExpr(m); }

void ComplexityEstimationVisitor::visitMultipliesOpr(const MultipliesOpr& m) { visitComplexBinExpr(m); }

void ComplexityEstimationVisitor::visitDevideOpr(const DevideOpr& d) { visitComplexBinExpr(d); }

void ComplexityEstimationVisitor::visitModuloOpr(const ModuloOpr& m) { visitComplexBinExpr(m); }

void ComplexityEstimationVisitor::visitShiftLeftOpr(const ShiftLeftOpr& shl) { visitSimpleBinExpr(shl); }

void ComplexityEstimationVisitor::visitShiftRightOpr(const ShiftRightOpr& shr) { visitSimpleBinExpr(shr); }

void ComplexityEstimationVisitor::visitVectorAccess(const VectorAccess&) {
  throw std::runtime_error("VectorAccess is not allowed in ComplexityEstimationVisitor.");
}

void ComplexityEstimationVisitor::visitForEach(const ForEach&) {
  throw std::runtime_error("ForEach is not allowed in ComplexityEstimationVisitor.");
}

void ComplexityEstimationVisitor::visitUnique(const Unique&) {
  throw std::runtime_error("Unique is not allowed in ComplexityEstimationVisitor.");
}

void ComplexityEstimationVisitor::visitIfThenElse(const IfThenElse& ite) {
  stack_entry a, b, c;
  evalTernExpr(ite, a, b, c);
  exprStack_.push(std::make_pair(new IfThenElse(a.first, b.first, c.first), a.second + b.second + c.second));
}

void ComplexityEstimationVisitor::visitBitslice(const Bitslice& b) {
  visitUnaryExpr(b);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new Bitslice(e.first, b.r(), b.l()), b.r() - b.l() + 1));
}

}  // end namespace crave
