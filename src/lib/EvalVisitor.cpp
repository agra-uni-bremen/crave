// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#include "../crave/ir/visitor/EvalVisitor.hpp"

#include <stdexcept>
#include <set>

namespace crave {

void EvalVisitor::visitNode(const Node&) {}

void EvalVisitor::visitTerminal(const Terminal&) {}

void EvalVisitor::visitUnaryExpr(const UnaryExpression& u) { u.child()->visit(this); }

void EvalVisitor::visitUnaryOpr(const UnaryOperator&) {}

void EvalVisitor::visitBinaryExpr(const BinaryExpression& b) {
  b.lhs()->visit(this);
  b.rhs()->visit(this);
}

void EvalVisitor::visitBinaryOpr(const BinaryOperator&) {}

void EvalVisitor::visitTernaryExpr(const TernaryExpression& t) {
  t.a()->visit(this);
  t.b()->visit(this);
  t.c()->visit(this);
}

void EvalVisitor::visitPlaceholder(const Placeholder&) {}

void EvalVisitor::visitVariableExpr(const VariableExpr& v) {
  eval_map::const_iterator ite(evalMap_->find(v.id()));
  if (ite != evalMap_->end())
    visitConstant(ite->second);
  else
    exprStack_.push(std::make_pair(Constant(), false));
}

void EvalVisitor::visitConstant(const Constant& c) { exprStack_.push(std::make_pair(c, true)); }

void EvalVisitor::visitVectorExpr(const VectorExpr& v) {
  eval_map::const_iterator ite(evalMap_->find(v.id()));
  if (ite != evalMap_->end())
    visitConstant(ite->second);
  else
    exprStack_.push(std::make_pair(Constant(), false));
}

void EvalVisitor::visitNotOpr(const NotOpr& n) {
  visitUnaryExpr(n);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(Constant(!e.first.value()), e.second));
}

void EvalVisitor::visitNegOpr(const NegOpr& n) {
  visitUnaryExpr(n);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(Constant(0 - e.first.value(), e.first.bitsize(), e.first.sign()), e.second));
}

void EvalVisitor::visitComplementOpr(const ComplementOpr& c) {
  visitUnaryExpr(c);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(Constant(~e.first.value(), e.first.bitsize(), e.first.sign()), e.second));
}

void EvalVisitor::visitInside(const Inside& i) {
  visitUnaryExpr(i);

  stack_entry e;
  pop(e);

  bool result = false;

  for (std::set<Constant>::iterator ite = i.collection().begin(); ite != i.collection().end(); ++ite)
    if (*ite == e.first.value()) result = true;

  exprStack_.push(std::make_pair(Constant(result), e.second));
}

void EvalVisitor::visitExtendExpr(const ExtendExpression& e) {
  visitUnaryExpr(e);

  stack_entry entry;
  pop(entry);

  Constant const& child = entry.first;
  Constant constant(child.value(), child.bitsize() + e.value(), child.sign());
  exprStack_.push(std::make_pair(constant, entry.second));
}

void EvalVisitor::visitAndOpr(const AndOpr& a) {
  stack_entry lhs, rhs;
  evalBinExpr(a, lhs, rhs);

  exprStack_.push(std::make_pair(
      Constant(lhs.first.value() & rhs.first.value(), lhs.first.bitsize(), lhs.first.sign() || rhs.first.sign()),
      lhs.second && rhs.second));
}

void EvalVisitor::visitOrOpr(const OrOpr& o) {
  stack_entry lhs, rhs;
  evalBinExpr(o, lhs, rhs);

  exprStack_.push(std::make_pair(
      Constant(lhs.first.value() | rhs.first.value(), lhs.first.bitsize(), lhs.first.sign() || rhs.first.sign()),
      lhs.second && rhs.second));
}

void EvalVisitor::visitLogicalAndOpr(const LogicalAndOpr& la) {
  stack_entry lhs, rhs;
  evalBinExpr(la, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() && rhs.first.value()), lhs.second && rhs.second));
}

void EvalVisitor::visitLogicalOrOpr(const LogicalOrOpr& lo) {
  stack_entry lhs, rhs;
  evalBinExpr(lo, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() || rhs.first.value()), lhs.second && rhs.second));
}

void EvalVisitor::visitXorOpr(const XorOpr& x) {
  stack_entry lhs, rhs;
  evalBinExpr(x, lhs, rhs);

  exprStack_.push(std::make_pair(
      Constant(lhs.first.value() ^ rhs.first.value(), lhs.first.bitsize(), lhs.first.sign() || rhs.first.sign()),
      lhs.second && rhs.second));
}

void EvalVisitor::visitEqualOpr(const EqualOpr& eq) {
  // include tmp variable for inside operation
  VariableExpr* v1 = static_cast<VariableExpr*>(eq.lhs().get());
  VariableExpr* v2 = static_cast<VariableExpr*>(eq.rhs().get());

  if (v1->id() != 0 && v2->id() != 0) {
    // both operands are variables
    eval_map::iterator ite1 = evalMap_->find(v1->id());
    eval_map::iterator ite2 = evalMap_->find(v2->id());
    if (ite1 != evalMap_->end() && ite2 == evalMap_->end()) {
      evalMap_->insert(std::make_pair(v2->id(), ite1->second));
    }
    if (ite1 == evalMap_->end() && ite2 != evalMap_->end()) {
      evalMap_->insert(std::make_pair(v1->id(), ite2->second));
    }
  }

  // regular evaluation
  stack_entry lhs, rhs;
  evalBinExpr(eq, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() == rhs.first.value()), lhs.second && rhs.second));
}

void EvalVisitor::visitNotEqualOpr(const NotEqualOpr& neq) {
  stack_entry lhs, rhs;
  evalBinExpr(neq, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() != rhs.first.value()), lhs.second && rhs.second));
}

void EvalVisitor::visitLessOpr(const LessOpr& l) {
  stack_entry lhs, rhs;
  evalBinExpr(l, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() < rhs.first.value()), lhs.second && rhs.second));
}

void EvalVisitor::visitLessEqualOpr(const LessEqualOpr& le) {
  stack_entry lhs, rhs;
  evalBinExpr(le, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() <= rhs.first.value()), lhs.second && rhs.second));
}

void EvalVisitor::visitGreaterOpr(const GreaterOpr& g) {
  stack_entry lhs, rhs;
  evalBinExpr(g, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() > rhs.first.value()), lhs.second && rhs.second));
}

void EvalVisitor::visitGreaterEqualOpr(const GreaterEqualOpr& ge) {
  stack_entry lhs, rhs;
  evalBinExpr(ge, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() >= rhs.first.value()), lhs.second && rhs.second));
}

void EvalVisitor::visitPlusOpr(const PlusOpr& p) {
  stack_entry lhs, rhs;
  evalBinExpr(p, lhs, rhs);

  exprStack_.push(std::make_pair(
      Constant(lhs.first.value() + rhs.first.value(), lhs.first.bitsize(), lhs.first.sign() || rhs.first.sign()),
      lhs.second && rhs.second));
}

void EvalVisitor::visitMinusOpr(const MinusOpr& m) {
  stack_entry lhs, rhs;
  evalBinExpr(m, lhs, rhs);

  exprStack_.push(std::make_pair(
      Constant(lhs.first.value() - rhs.first.value(), lhs.first.bitsize(), lhs.first.sign() || rhs.first.sign()),
      lhs.second && rhs.second));
}

void EvalVisitor::visitMultipliesOpr(const MultipliesOpr& m) {
  stack_entry lhs, rhs;
  evalBinExpr(m, lhs, rhs);

  exprStack_.push(std::make_pair(
      Constant(lhs.first.value() * rhs.first.value(), lhs.first.bitsize(), lhs.first.sign() || rhs.first.sign()),
      lhs.second && rhs.second));
}

void EvalVisitor::visitDevideOpr(const DevideOpr& d) {
  stack_entry lhs, rhs;
  evalBinExpr(d, lhs, rhs);

  bool result;
  Constant constant;

  if (0 == rhs.first.value()) {
    result = false;
    constant = Constant();
  } else {
    result = lhs.second && rhs.second;

    if (lhs.first.sign() || rhs.first.sign())
      constant = Constant(static_cast<long long>(lhs.first.value()) / static_cast<long long>(rhs.first.value()),
                          lhs.first.bitsize(), true);
    else
      constant = Constant(lhs.first.value() / rhs.first.value(), lhs.first.bitsize(), false);
  }
  exprStack_.push(std::make_pair(constant, result));
}

void EvalVisitor::visitModuloOpr(const ModuloOpr& m) {
  stack_entry lhs, rhs;
  evalBinExpr(m, lhs, rhs);

  bool result;
  Constant constant;

  if (0 == rhs.first.value()) {
    result = false;
    constant = Constant();
  } else {
    result = lhs.second && rhs.second;

    if (lhs.first.sign() || rhs.first.sign())
      constant = Constant(static_cast<long long>(lhs.first.value()) % static_cast<long long>(rhs.first.value()),
                          lhs.first.bitsize(), true);
    else
      constant = Constant(lhs.first.value() % rhs.first.value(), lhs.first.bitsize(), false);
  }
  exprStack_.push(std::make_pair(constant, result));
}

void EvalVisitor::visitShiftLeftOpr(const ShiftLeftOpr& shl) {
  stack_entry lhs, rhs;
  evalBinExpr(shl, lhs, rhs);

  exprStack_.push(std::make_pair(
      Constant(lhs.first.value() << rhs.first.to_integer<int>(), lhs.first.bitsize(), lhs.first.sign() || rhs.first.sign()),
      lhs.second && rhs.second));
}

void EvalVisitor::visitShiftRightOpr(const ShiftRightOpr& shr) {
  stack_entry lhs, rhs;
  evalBinExpr(shr, lhs, rhs);

  exprStack_.push(std::make_pair(
      Constant(lhs.first.value() >> rhs.first.to_integer<int>(), lhs.first.bitsize(), lhs.first.sign() || rhs.first.sign()),
      lhs.second && rhs.second));
}

void EvalVisitor::visitVectorAccess(const VectorAccess&) {
  throw std::runtime_error("VectorAccess is not allowed in EvalVisitor.");
}
void EvalVisitor::visitForEach(const ForEach&) {
  throw std::runtime_error("ForEach is not allowed in EvalVisitor.");
}
void EvalVisitor::visitUnique(const Unique&) { throw std::runtime_error("Unique is not allowed in EvalVisitor."); }

void EvalVisitor::visitIfThenElse(const IfThenElse& ite) {
  stack_entry a, b, c;
  evalTernExpr(ite, a, b, c);

  if (a.first.value())
    exprStack_.push(b);
  else
    exprStack_.push(c);
}

void EvalVisitor::visitBitslice(const Bitslice& b) {
  visitUnaryExpr(b);

  stack_entry entry;
  pop(entry);

  Constant const& child = entry.first;
  unsigned long long v = 0;
  for (int i = b.l(); i <= b.r(); i++) v |= (1LL << i);
  Constant constant((v & child.value()) >> b.l(), b.r() - b.l() + 1, false);
  exprStack_.push(std::make_pair(constant, false));
}

}  // end namespace crave
