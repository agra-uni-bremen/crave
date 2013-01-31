#pragma once

#include "NodeVisitor.hpp"

#include <boost/intrusive_ptr.hpp>
#include <boost/proto/eval.hpp>

#include <ostream>
#include <set>

namespace crave {

class Node;

typedef boost::intrusive_ptr<Node> NodePtr;

class Node {
protected:
  Node() : count_(0) { }
  virtual ~Node() { }
public:
  virtual void visit( NodeVisitor& v ) const { v.visitNode(*this); }

// reference counting
  friend inline void intrusive_ptr_add_ref(Node* n) { ++(n->count_); }
  friend inline void intrusive_ptr_release(Node* n) { if (--(n->count_) == 0) delete n; }
private:
  unsigned int count_;
};

class Placeholder : public Node {
public:
  Placeholder( unsigned int id ) : Node(), id_(id) { }

  void visit( NodeVisitor& v ) const { v.visitPlaceholder(*this); }

  unsigned int id() const { return id_; }
private:
  unsigned int id_;
};

class Terminal : public Node {
protected:
  Terminal( unsigned int bs, bool s ) : Node(), bitsize_(bs), sign_(s) { }

public:
  virtual void visit( NodeVisitor& v ) const { v.visitTerminal(*this); }

  unsigned int bitsize() const { return bitsize_; }
  bool sign() const { return sign_; }
private:
  unsigned int bitsize_;
  bool sign_;
};

class VariableExpr : public Terminal {
public:
  VariableExpr( unsigned int id, unsigned int bs, bool s ) : Terminal(bs, s), id_(id) { }

  void visit( NodeVisitor& v ) const { v.visitVariableExpr(*this); }

  unsigned int id() const { return id_; }
private:
  unsigned int id_;
};

class Constant : public Terminal {
public:
  Constant( unsigned long val, unsigned int bs, bool s ) : Terminal(bs, s), value_(val) { }

  void visit( NodeVisitor& v ) const { v.visitConstant(*this); }

  operator unsigned long() const { return value_; }

  unsigned long value() const { return value_; }
private:
  unsigned long value_;
};

class VectorExpr : public Terminal {
public:
  VectorExpr( unsigned int id, unsigned int bs, bool s ) : Terminal(bs, s), id_(id) { }

  void visit( NodeVisitor& v ) const { v.visitVectorExpr(*this); }

  unsigned int id() const { return id_; }
private:
  unsigned int id_;
};

class UnaryExpression : public Node {
protected:
  UnaryExpression(NodePtr c) : Node(), child_(c) { }

public:
  virtual void visit( NodeVisitor& v ) const { v.visitUnaryExpr(*this); }

  boost::intrusive_ptr<Node> child() const { return child_; }
private:
  boost::intrusive_ptr<Node> child_;
};

class UnaryOperator : public UnaryExpression {
protected:
  UnaryOperator( NodePtr c ) : UnaryExpression(c) { }

public:
  virtual void visit( NodeVisitor& v ) const { v.visitUnaryOpr(*this); }
};

class NotOpr : public UnaryOperator {
public:
  NotOpr( NodePtr c ) : UnaryOperator(c) { };

  void visit( NodeVisitor& v ) const { v.visitNotOpr(*this); }
};

class NegOpr : public UnaryOperator {
public:
  NegOpr( NodePtr c ) : UnaryOperator(c) { };

  void visit( NodeVisitor& v ) const { v.visitNegOpr(*this); }
};

class ComplementOpr : public UnaryOperator {
public:
  ComplementOpr( NodePtr c ) : UnaryOperator(c) { };

  void visit( NodeVisitor& v ) const { v.visitComplementOpr(*this); }
};

class Inside : public UnaryExpression {
public:
  Inside( NodePtr v, std::set<Constant> const &c ) : UnaryExpression(v), collection_(c) { }

  void visit( NodeVisitor& v ) const { v.visitInside(*this); }

  std::set<Constant> const & collection() const { return collection_; };
private:
  std::set<Constant> collection_;
};

class ExtendExpression : public UnaryExpression {
public:
  ExtendExpression( NodePtr v, unsigned int i ) : UnaryExpression(v), value_(i) { }

  void visit( NodeVisitor& v ) const { v.visitExtendExpr(*this); }

  unsigned int const & value() const { return value_; };
private:
  unsigned int value_;
};

class BinaryExpression : public Node {
protected:
  BinaryExpression(NodePtr lhs, NodePtr rhs) : lhs_(lhs), rhs_(rhs) { }

public:
  virtual void visit( NodeVisitor& v ) const { v.visitBinaryExpr(*this); }

  boost::intrusive_ptr<Node> lhs() const { return lhs_; }
  boost::intrusive_ptr<Node> rhs() const { return rhs_; }
private:
  boost::intrusive_ptr<Node> lhs_;
  boost::intrusive_ptr<Node> rhs_;
};

class BinaryOperator : public BinaryExpression {
protected:
  BinaryOperator( NodePtr lhs, NodePtr rhs ) : BinaryExpression( lhs, rhs ) { };
public:
  virtual void visit( NodeVisitor& v ) const { v.visitBinaryOpr(*this); }
};

class AndOpr : public BinaryOperator {
public:
  AndOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitAndOpr(*this); }
};

class OrOpr : public BinaryOperator {
public:
  OrOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitOrOpr(*this); }
};


class LogicalAndOpr : public BinaryOperator {
public:
  LogicalAndOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitLogicalAndOpr(*this); }
};

class LogicalOrOpr : public BinaryOperator {
public:
  LogicalOrOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitLogicalOrOpr(*this); }
};

class XorOpr : public BinaryOperator {
public:
  XorOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitXorOpr(*this); }
};

class EqualOpr : public BinaryOperator {
public:
  EqualOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitEqualOpr(*this); }
};

class NotEqualOpr : public BinaryOperator {
public:
  NotEqualOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitNotEqualOpr(*this); }
};

class LessOpr : public BinaryOperator {
public:
  LessOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitLessOpr(*this); }
};

class LessEqualOpr : public BinaryOperator {
public:
  LessEqualOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitLessEqualOpr(*this); }
};

class GreaterOpr : public BinaryOperator {
public:
  GreaterOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitGreaterOpr(*this); }
};

class GreaterEqualOpr : public BinaryOperator {
public:
  GreaterEqualOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitGreaterEqualOpr(*this); }
};

class PlusOpr : public BinaryOperator {
public:
  PlusOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitPlusOpr(*this); }
};

class MinusOpr : public BinaryOperator {
public:
  MinusOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitMinusOpr(*this); }
};

class MultipliesOpr : public BinaryOperator {
public:
  MultipliesOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitMultipliesOpr(*this); }
};

class DevideOpr : public BinaryOperator {
public:
  DevideOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitDevideOpr(*this); }
};

class ModuloOpr : public BinaryOperator {
public:
  ModuloOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitModuloOpr(*this); }
};

class ShiftLeftOpr : public BinaryOperator {
public:
  ShiftLeftOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitShiftLeftOpr(*this); }
};

class ShiftRightOpr : public BinaryOperator {
public:
  ShiftRightOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitShiftRightOpr(*this); }
};

class AssignOpr : public BinaryOperator {
public:
  AssignOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) const { v.visitAssignOpr(*this); }
};

class VectorAccess : public BinaryExpression {
public:
  void visit( NodeVisitor& v ) const { v.visitVectorAccess(*this); }
};

class TernaryExpression : public Node {
protected:
  TernaryExpression( NodePtr a, NodePtr b, NodePtr c ) : Node(), a_(a), b_(b), c_(c) { }
public:
  virtual void visit( NodeVisitor& v ) const { v.visitTernaryExpr(*this); }

  boost::intrusive_ptr<Node> a() const { return a_; }
  boost::intrusive_ptr<Node> b() const { return b_; }
  boost::intrusive_ptr<Node> c() const { return c_; }
private:
  boost::intrusive_ptr<Node> a_;
  boost::intrusive_ptr<Node> b_;
  boost::intrusive_ptr<Node> c_;
};

class IfThenElse : public TernaryExpression {
public:
  IfThenElse( NodePtr a, NodePtr b, NodePtr c ) : TernaryExpression(a,b,c) { }

  void visit( NodeVisitor& v ) const { v.visitIfThenElse(*this); }
};

} // end namespace crave
