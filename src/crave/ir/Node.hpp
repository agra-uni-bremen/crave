// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//
#pragma once

#include <stdint.h>

#include <memory>
#include <ostream>
#include <set>

#include "visitor/NodeVisitor.hpp"

namespace crave {

class Node;

typedef std::shared_ptr<Node> NodePtr;

class Node {
 protected:
  Node() : count_(0) {}
  virtual ~Node() {}
  Node(Node const&) : count_(0) {}

 public:
  virtual void visit(NodeVisitor* v) const { v->visitNode(*this); }
  std::ostream& printDot(std::ostream& out) const;

 private:
  unsigned int count_;
};

class Placeholder : public Node {
 public:
  explicit Placeholder(unsigned int id) : Node(), id_(id) {}
  Placeholder(Placeholder const& p) : Node(p), id_(p.id()) {}

  void visit(NodeVisitor* v) const { v->visitPlaceholder(*this); }

  unsigned int id() const { return id_; }

 private:
  unsigned int id_;
};

class Terminal : public Node {
 protected:
  Terminal(unsigned int bs, bool s) : Node(), bitsize_(bs), sign_(s) {}
  Terminal(Terminal const& t) : Node(t), bitsize_(t.bitsize()), sign_(t.sign()) {}

 public:
  virtual void visit(NodeVisitor* v) const { v->visitTerminal(*this); }

  unsigned int bitsize() const { return bitsize_; }
  bool sign() const { return sign_; }
  bool isBool() const { return bitsize() == 1 && sign(); }

 private:
  unsigned int bitsize_;
  bool sign_;
};

class VariableExpr : public Terminal {
 public:
  VariableExpr(unsigned int id, unsigned int bs, bool s) : Terminal(bs, s), id_(id) {}
  VariableExpr(VariableExpr const& v) : Terminal(v.bitsize(), v.sign()), id_(v.id()) {}

  void visit(NodeVisitor* v) const { v->visitVariableExpr(*this); }

  unsigned int id() const { return id_; }

 private:
  unsigned int id_;
};

class Constant : public Terminal {
 public:
  Constant() : Terminal(1, true), value_(false) {}
  Constant(uint64_t val, unsigned int bs, bool s) : Terminal(bs, s), value_(val) {}
  explicit Constant(bool b) : Terminal(1, true), value_(b) {}
  Constant(Constant const& c) : Terminal(c.bitsize(), c.sign()), value_(c.value()) {}

  void visit(NodeVisitor* v) const { v->visitConstant(*this); }

  operator uint64_t() const { return value_; }

  uint64_t value() const { return value_; }

 private:
  uint64_t value_;
};

class VectorExpr : public Terminal {
 public:
  VectorExpr(unsigned int id, unsigned int bs, bool s) : Terminal(bs, s), id_(id) {}
  VectorExpr(VectorExpr const& v) : Terminal(v.bitsize(), v.sign()), id_(v.id()) {}

  void visit(NodeVisitor* v) const { v->visitVectorExpr(*this); }

  unsigned int id() const { return id_; }

 private:
  unsigned int id_;
};

class UnaryExpression : public Node {
 protected:
  explicit UnaryExpression(NodePtr c) : Node(), child_(c) {}
  UnaryExpression(UnaryExpression const& u) : Node(), child_() { child_ = u.child(); }

 public:
  virtual void visit(NodeVisitor* v) const { v->visitUnaryExpr(*this); }

  NodePtr child() const { return child_; }

 private:
  NodePtr child_;
};

class UnaryOperator : public UnaryExpression {
 protected:
  explicit UnaryOperator(NodePtr c) : UnaryExpression(c) {}
  UnaryOperator(UnaryOperator const& u) : UnaryExpression(u) {}

 public:
  virtual void visit(NodeVisitor* v) const { v->visitUnaryOpr(*this); }
};

class NotOpr : public UnaryOperator {
 public:
  explicit NotOpr(NodePtr c) : UnaryOperator(c) {}
  NotOpr(NotOpr const& n) : UnaryOperator(n) {}

  void visit(NodeVisitor* v) const { v->visitNotOpr(*this); }
};

class NegOpr : public UnaryOperator {
 public:
  explicit NegOpr(NodePtr c) : UnaryOperator(c) {}
  NegOpr(NegOpr const& n) : UnaryOperator(n) {}

  void visit(NodeVisitor* v) const { v->visitNegOpr(*this); }
};

class ComplementOpr : public UnaryOperator {
 public:
  explicit ComplementOpr(NodePtr c) : UnaryOperator(c) {}
  ComplementOpr(ComplementOpr const& n) : UnaryOperator(n) {}

  void visit(NodeVisitor* v) const { v->visitComplementOpr(*this); }
};

class Inside : public UnaryExpression {
 public:
  Inside(NodePtr v, std::set<Constant> const& c) : UnaryExpression(v), collection_(c) {}
  Inside(Inside const& i) : UnaryExpression(i), collection_(i.collection()) {}

  void visit(NodeVisitor* v) const { v->visitInside(*this); }

  std::set<Constant> const& collection() const { return collection_; }

 private:
  std::set<Constant> collection_;
};

class ExtendExpression : public UnaryExpression {
 public:
  ExtendExpression(NodePtr v, unsigned int i) : UnaryExpression(v), value_(i) {}
  ExtendExpression(ExtendExpression const& e) : UnaryExpression(e), value_(e.value()) {}

  void visit(NodeVisitor* v) const { v->visitExtendExpr(*this); }

  unsigned int const& value() const { return value_; }

 private:
  unsigned int value_;
};

class BinaryExpression : public Node {
 protected:
  BinaryExpression(NodePtr lhs, NodePtr rhs) : lhs_(lhs), rhs_(rhs) {}
  BinaryExpression(BinaryExpression const& b) : Node(), lhs_(), rhs_() {
    lhs_ = b.lhs();
    rhs_ = b.rhs();
  }

 public:
  virtual void visit(NodeVisitor* v) const { v->visitBinaryExpr(*this); }

  NodePtr lhs() const { return lhs_; }
  NodePtr rhs() const { return rhs_; }

 private:
  NodePtr lhs_;
  NodePtr rhs_;
};

class BinaryOperator : public BinaryExpression {
 protected:
  BinaryOperator(NodePtr lhs, NodePtr rhs) : BinaryExpression(lhs, rhs) {}
  BinaryOperator(BinaryOperator const& b) : BinaryExpression(b) {}

 public:
  virtual void visit(NodeVisitor* v) const { v->visitBinaryOpr(*this); }
};

class AndOpr : public BinaryOperator {
 public:
  AndOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  AndOpr(AndOpr const& a) : BinaryOperator(a) {}

  void visit(NodeVisitor* v) const { v->visitAndOpr(*this); }
};

class OrOpr : public BinaryOperator {
 public:
  OrOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  OrOpr(OrOpr const& o) : BinaryOperator(o) {}

  void visit(NodeVisitor* v) const { v->visitOrOpr(*this); }
};

class LogicalAndOpr : public BinaryOperator {
 public:
  LogicalAndOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  LogicalAndOpr(LogicalAndOpr const& a) : BinaryOperator(a) {}

  void visit(NodeVisitor* v) const { v->visitLogicalAndOpr(*this); }
};

class LogicalOrOpr : public BinaryOperator {
 public:
  LogicalOrOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  LogicalOrOpr(LogicalOrOpr const& o) : BinaryOperator(o) {}

  void visit(NodeVisitor* v) const { v->visitLogicalOrOpr(*this); }
};

class XorOpr : public BinaryOperator {
 public:
  XorOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  XorOpr(XorOpr const& x) : BinaryOperator(x) {}

  void visit(NodeVisitor* v) const { v->visitXorOpr(*this); }
};

class EqualOpr : public BinaryOperator {
 public:
  EqualOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  EqualOpr(EqualOpr const& e) : BinaryOperator(e) {}

  void visit(NodeVisitor* v) const { v->visitEqualOpr(*this); }
};

class NotEqualOpr : public BinaryOperator {
 public:
  NotEqualOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  NotEqualOpr(NotEqualOpr const& n) : BinaryOperator(n) {}

  void visit(NodeVisitor* v) const { v->visitNotEqualOpr(*this); }
};

class LessOpr : public BinaryOperator {
 public:
  LessOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  LessOpr(LessOpr const& l) : BinaryOperator(l) {}

  void visit(NodeVisitor* v) const { v->visitLessOpr(*this); }
};

class LessEqualOpr : public BinaryOperator {
 public:
  LessEqualOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  LessEqualOpr(LessEqualOpr const& l) : BinaryOperator(l) {}

  void visit(NodeVisitor* v) const { v->visitLessEqualOpr(*this); }
};

class GreaterOpr : public BinaryOperator {
 public:
  GreaterOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  GreaterOpr(GreaterOpr const& g) : BinaryOperator(g) {}

  void visit(NodeVisitor* v) const { v->visitGreaterOpr(*this); }
};

class GreaterEqualOpr : public BinaryOperator {
 public:
  GreaterEqualOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  GreaterEqualOpr(GreaterEqualOpr const& g) : BinaryOperator(g) {}

  void visit(NodeVisitor* v) const { v->visitGreaterEqualOpr(*this); }
};

class PlusOpr : public BinaryOperator {
 public:
  PlusOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  PlusOpr(PlusOpr const& p) : BinaryOperator(p) {}

  void visit(NodeVisitor* v) const { v->visitPlusOpr(*this); }
};

class MinusOpr : public BinaryOperator {
 public:
  MinusOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  MinusOpr(MinusOpr const& m) : BinaryOperator(m) {}

  void visit(NodeVisitor* v) const { v->visitMinusOpr(*this); }
};

class MultipliesOpr : public BinaryOperator {
 public:
  MultipliesOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  MultipliesOpr(MultipliesOpr const& m) : BinaryOperator(m) {}

  void visit(NodeVisitor* v) const { v->visitMultipliesOpr(*this); }
};

class DivideOpr : public BinaryOperator {
 public:
  DivideOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  DivideOpr(DivideOpr const& d) : BinaryOperator(d) {}

  void visit(NodeVisitor* v) const { v->visitDivideOpr(*this); }
};

class ModuloOpr : public BinaryOperator {
 public:
  ModuloOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  ModuloOpr(ModuloOpr const& m) : BinaryOperator(m) {}

  void visit(NodeVisitor* v) const { v->visitModuloOpr(*this); }
};

class ShiftLeftOpr : public BinaryOperator {
 public:
  ShiftLeftOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  ShiftLeftOpr(ShiftLeftOpr const& s) : BinaryOperator(s) {}

  void visit(NodeVisitor* v) const { v->visitShiftLeftOpr(*this); }
};

class ShiftRightOpr : public BinaryOperator {
 public:
  ShiftRightOpr(NodePtr lhs, NodePtr rhs) : BinaryOperator(lhs, rhs) {}
  ShiftRightOpr(ShiftRightOpr const& s) : BinaryOperator(s) {}

  void visit(NodeVisitor* v) const { v->visitShiftRightOpr(*this); }
};

class VectorAccess : public BinaryExpression {
 public:
  VectorAccess(NodePtr lhs, NodePtr rhs) : BinaryExpression(lhs, rhs) {}
  VectorAccess(VectorAccess const& v) : BinaryExpression(v) {}

  void visit(NodeVisitor* v) const { v->visitVectorAccess(*this); }
};

class TernaryExpression : public Node {
 protected:
  explicit TernaryExpression(NodePtr a, NodePtr b, NodePtr c) : Node(), a_(a), b_(b), c_(c) {}
  TernaryExpression(TernaryExpression const& t) : Node(t), a_(), b_(), c_() {
    a_ = t.a();
    b_ = t.b();
    c_ = t.c();
  }

 public:
  virtual void visit(NodeVisitor* v) const { v->visitTernaryExpr(*this); }

  NodePtr a() const { return a_; }
  NodePtr b() const { return b_; }
  NodePtr c() const { return c_; }

 private:
  NodePtr a_;
  NodePtr b_;
  NodePtr c_;
};

class IfThenElse : public TernaryExpression {
 public:
  IfThenElse(NodePtr a, NodePtr b, NodePtr c) : TernaryExpression(a, b, c) {}
  IfThenElse(IfThenElse const& t) : TernaryExpression(t) {}

  void visit(NodeVisitor* v) const { v->visitIfThenElse(*this); }
};

class ForEach : public BinaryOperator {
 public:
  ForEach(NodePtr a, NodePtr b) : BinaryOperator(a, b) {}
  ForEach(ForEach const& fe) : BinaryOperator(fe) {}

  void visit(NodeVisitor* v) const { v->visitForEach(*this); }
};

class Unique : public UnaryOperator {
 public:
  explicit Unique(NodePtr a) : UnaryOperator(a) {}
  Unique(Unique const& u) : UnaryOperator(u) {}

  void visit(NodeVisitor* v) const { v->visitUnique(*this); }
};

class Bitslice : public UnaryExpression {
 public:
  Bitslice(NodePtr v, int r, int l) : UnaryExpression(v), r_(r), l_(l) {}

  Bitslice(Bitslice const& b) : UnaryExpression(b), r_(b.r_), l_(b.l_) {}

  void visit(NodeVisitor* v) const { v->visitBitslice(*this); }

  int r() const { return r_; }
  int l() const { return l_; }

 private:
  int r_, l_;
};

}  // end namespace crave
