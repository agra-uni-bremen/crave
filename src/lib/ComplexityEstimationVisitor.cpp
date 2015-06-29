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
void ComplexityEstimationVisitor::visitSimpleTwoBinExpr(const T& object) {
  stack_entry lhs, rhs;
  evalBinExpr(object, lhs, rhs);

  exprStack_.push(std::make_pair(new T(lhs.first, rhs.first), lhs.second + rhs.second));
}

template <typename T>
void ComplexityEstimationVisitor::visitSimpleBinExpr(const T& object) {
  visitUnaryExpr(object);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new T(e.first), e.second));
}

void ComplexityEstimationVisitor::visitPlaceholder(const Placeholder& pl) {
  exprStack_.push(std::make_pair(new Placeholder(pl), placeholder_bitsize()));
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

void ComplexityEstimationVisitor::visitNotOpr(const NotOpr& n) { visitSimpleBinExpr<NotOpr>(n); }

void ComplexityEstimationVisitor::visitNegOpr(const NegOpr& n) { visitSimpleBinExpr<NegOpr>(n); }

void ComplexityEstimationVisitor::visitComplementOpr(const ComplementOpr& c) { visitSimpleBinExpr<ComplementOpr>(c); }

void ComplexityEstimationVisitor::visitInside(const Inside& i) {
  visitUnaryExpr(i);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new Inside(e.first, i.collection()), 1));
}

void ComplexityEstimationVisitor::visitExtendExpr(const ExtendExpression& e) {
  visitUnaryExpr(e);

  stack_entry entry;
  pop(entry);

  exprStack_.push(std::make_pair(new ExtendExpression(entry.first, e.value()), entry.second));
}

void ComplexityEstimationVisitor::visitAndOpr(const AndOpr& a) { visitSimpleTwoBinExpr<AndOpr>(a); }

void ComplexityEstimationVisitor::visitOrOpr(const OrOpr& o) { visitSimpleTwoBinExpr<OrOpr>(o); }

void ComplexityEstimationVisitor::visitLogicalAndOpr(const LogicalAndOpr& la) {
  visitSimpleTwoBinExpr<LogicalAndOpr>(la);
}

void ComplexityEstimationVisitor::visitLogicalOrOpr(const LogicalOrOpr& lo) { visitSimpleTwoBinExpr<LogicalOrOpr>(lo); }

void ComplexityEstimationVisitor::visitXorOpr(const XorOpr& x) { visitSimpleTwoBinExpr<XorOpr>(x); }

void ComplexityEstimationVisitor::visitEqualOpr(const EqualOpr& eq) { visitSimpleTwoBinExpr<EqualOpr>(eq); }

void ComplexityEstimationVisitor::visitNotEqualOpr(const NotEqualOpr& neq) { visitSimpleTwoBinExpr<NotEqualOpr>(neq); }

void ComplexityEstimationVisitor::visitLessOpr(const LessOpr& l) { visitSimpleTwoBinExpr<LessOpr>(l); }

void ComplexityEstimationVisitor::visitLessEqualOpr(const LessEqualOpr& le) { visitSimpleTwoBinExpr<LessEqualOpr>(le); }

void ComplexityEstimationVisitor::visitGreaterOpr(const GreaterOpr& g) { visitSimpleTwoBinExpr<GreaterOpr>(g); }

void ComplexityEstimationVisitor::visitGreaterEqualOpr(const GreaterEqualOpr& ge) {
  visitSimpleTwoBinExpr<GreaterEqualOpr>(ge);
}

void ComplexityEstimationVisitor::visitPlusOpr(const PlusOpr& p) { visitSimpleTwoBinExpr<PlusOpr>(p); }

void ComplexityEstimationVisitor::visitMinusOpr(const MinusOpr& m) { visitSimpleTwoBinExpr<MinusOpr>(m); }

void ComplexityEstimationVisitor::visitMultipliesOpr(const MultipliesOpr& m) {
  stack_entry lhs, rhs;
  evalBinExpr(m, lhs, rhs);
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
  exprStack_.push(std::make_pair(new MultipliesOpr(lhs.first, rhs.first), complexity));
}

void ComplexityEstimationVisitor::visitDevideOpr(const DevideOpr& d) {
  stack_entry lhs, rhs;
  evalBinExpr(d, lhs, rhs);
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
  exprStack_.push(std::make_pair(new DevideOpr(lhs.first, rhs.first), complexity));
}

void ComplexityEstimationVisitor::visitModuloOpr(const ModuloOpr& m) {
  stack_entry lhs, rhs;
  evalBinExpr(m, lhs, rhs);

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
  exprStack_.push(std::make_pair(new ModuloOpr(lhs.first, rhs.first), complexity));
}

void ComplexityEstimationVisitor::visitShiftLeftOpr(const ShiftLeftOpr& shl) {
  visitSimpleTwoBinExpr<ShiftLeftOpr>(shl);
}

void ComplexityEstimationVisitor::visitShiftRightOpr(const ShiftRightOpr& shr) {
  visitSimpleTwoBinExpr<ShiftRightOpr>(shr);
}

void ComplexityEstimationVisitor::visitVectorAccess(const VectorAccess& va) { visitSimpleTwoBinExpr<VectorAccess>(va); }

void ComplexityEstimationVisitor::visitForEach(const ForEach& fe) { visitSimpleTwoBinExpr<ForEach>(fe); }

void ComplexityEstimationVisitor::visitUnique(const Unique& u) { visitSimpleBinExpr<Unique>(u); }

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
