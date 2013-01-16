#pragma once

#include "NodeVisitor.hpp"

#include <boost/intrusive_ptr.hpp>
#include <boost/proto/eval.hpp>

#include <ostream>
#include <set>

namespace crave {

class Node;

typedef boost::intrusive_ptr<Node> result_type; // TODO: rename to NodePtr

class Node {
protected:
  Node() : count_(0) { }
  virtual ~Node() { }
public:
  virtual void visit( NodeVisitor& v ) { v.visitNode(*this); }

// reference counting
  friend inline void intrusive_ptr_add_ref(Node* n) { ++(n->count_); }
  friend inline void intrusive_ptr_release(Node* n) { if (--(n->count_) == 0) delete n; }
private:
  unsigned int count_;
};

class Placeholder : public Node {
public:
  Placeholder( unsigned int id ) : Node(), id_(id) { }

  void visit( NodeVisitor& v ) { v.visitPlaceholder(*this); }

  unsigned int id() const { return id_; }
private:
  unsigned int id_;
};

class Terminal : public Node {
protected:
  Terminal( unsigned int bs, bool s ) : Node(), bitsize_(bs), sign_(s) { }

public:
  virtual void visit( NodeVisitor& v ) { v.visitTerminal(*this); }

  unsigned int bitsize() const { return bitsize_; }
  bool sign() const { return sign_; }
private:
  unsigned int bitsize_;
  bool sign_;
};

class VariableExpr : public Terminal {
public:
  VariableExpr( unsigned int id, unsigned int bs, bool s ) : Terminal(bs, s), id_(id) { }

  void visit( NodeVisitor& v ) { v.visitVariableExpr(*this); }

  unsigned int id() const { return id_; }
private:
  unsigned int id_;
};

class Constant : public Terminal {
public:
  Constant( unsigned long val, unsigned int bs, bool s ) : Terminal(bs, s), value_(val) { }

  void visit( NodeVisitor& v ) { v.visitConstant(*this); }

  operator unsigned long() const { return value_; }

  unsigned long value() const { return value_; }
private:
  unsigned long value_;
};

class VectorExpr : public Terminal {
public:
  VectorExpr( unsigned int id, unsigned int bs, bool s ) : Terminal(bs, s), id_(id) { }

  void visit( NodeVisitor& v ) { v.visitVectorExpr(*this); }

  unsigned int id() const { return id_; }
private:
  unsigned int id_;
};

class UnaryExpression : public Node {
protected:
  UnaryExpression(result_type c) : Node(), child_(c) { }

public:
  virtual void visit( NodeVisitor& v ) { v.visitUnaryExpr(*this); }

  boost::intrusive_ptr<Node> child() const { return child_; }
private:
  boost::intrusive_ptr<Node> child_;
};

class UnaryOperator : public UnaryExpression {
protected:
  UnaryOperator( result_type c ) : UnaryExpression(c) { }

public:
  virtual void visit( NodeVisitor& v ) { v.visitUnaryOpr(*this); }
};

class NotOpr : public UnaryOperator {
public:
  NotOpr( result_type c ) : UnaryOperator(c) { };

  void visit( NodeVisitor& v ) { v.visitNotOpr(*this); }
};

class NegOpr : public UnaryOperator {
public:
  NegOpr( result_type c ) : UnaryOperator(c) { };

  void visit( NodeVisitor& v ) { v.visitNegOpr(*this); }
};

class ComplementOpr : public UnaryOperator {
public:
  ComplementOpr( result_type c ) : UnaryOperator(c) { };

  void visit( NodeVisitor& v ) { v.visitComplementOpr(*this); }
};

class Inside : public UnaryExpression {
public:
  Inside( result_type c ) : UnaryExpression(c), collection_() { }

  void visit( NodeVisitor& v ) { v.visitInside(*this); }

  std::set<Constant> const & collection() const { return collection_; };
private:
  std::set<Constant> collection_;
};

class BinaryExpression : public Node {
protected:
  BinaryExpression(result_type lhs, result_type rhs) : lhs_(lhs), rhs_(rhs) { }

public:
  virtual void visit( NodeVisitor& v ) { v.visitBinaryExpr(*this); }

  boost::intrusive_ptr<Node> lhs() const { return lhs_; }
  boost::intrusive_ptr<Node> rhs() const { return rhs_; }
private:
  boost::intrusive_ptr<Node> lhs_;
  boost::intrusive_ptr<Node> rhs_;
};

class BinaryOperator : public BinaryExpression {
protected:
  BinaryOperator( result_type lhs, result_type rhs ) : BinaryExpression( lhs, rhs ) { };
public:
  virtual void visit( NodeVisitor& v ) { v.visitBinaryOpr(*this); }
};

class AndOpr : public BinaryOperator {
public:
  AndOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitAndOpr(*this); }
};

class OrOpr : public BinaryOperator {
public:
  OrOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitOrOpr(*this); }
};


class LogicalAndOpr : public BinaryOperator {
public:
  LogicalAndOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitLogicalAndOpr(*this); }
};

class LogicalOrOpr : public BinaryOperator {
public:
  LogicalOrOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitLogicalOrOpr(*this); }
};

class XorOpr : public BinaryOperator {
public:
  XorOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitXorOpr(*this); }
};

class EqualOpr : public BinaryOperator {
public:
  EqualOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitEqualOpr(*this); }
};

class NotEqualOpr : public BinaryOperator {
public:
  NotEqualOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitNotEqualOpr(*this); }
};

class LessOpr : public BinaryOperator {
public:
  LessOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitLessOpr(*this); }
};

class LessEqualOpr : public BinaryOperator {
public:
  LessEqualOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitLessEqualOpr(*this); }
};

class GreaterOpr : public BinaryOperator {
public:
  GreaterOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitGreaterOpr(*this); }
};

class GreaterEqualOpr : public BinaryOperator {
public:
  GreaterEqualOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitGreaterEqualOpr(*this); }
};

class PlusOpr : public BinaryOperator {
public:
  PlusOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitPlusOpr(*this); }
};

class MinusOpr : public BinaryOperator {
public:
  MinusOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitMinusOpr(*this); }
};

class MultipliesOpr : public BinaryOperator {
public:
  MultipliesOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitMultipliesOpr(*this); }
};

class DevideOpr : public BinaryOperator {
public:
  DevideOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitDevideOpr(*this); }
};

class ModuloOpr : public BinaryOperator {
public:
  ModuloOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitModuloOpr(*this); }
};

class ShiftLeftOpr : public BinaryOperator {
public:
  ShiftLeftOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitShiftLeftOpr(*this); }
};

class ShiftRightOpr : public BinaryOperator {
public:
  ShiftRightOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitShiftRightOpr(*this); }
};

class AssignOpr : public BinaryOperator {
public:
  AssignOpr( result_type lhs, result_type rhs ) : BinaryOperator(lhs, rhs) { }

  void visit( NodeVisitor& v ) { v.visitAssignOpr(*this); }
};

class VectorAccess : public BinaryExpression {
public:
  void visit( NodeVisitor& v ) { v.visitVectorAccess(*this); }
};

class TernaryExpression : public Node {
protected:
  TernaryExpression( result_type a, result_type b, result_type c ) : Node(), a_(a), b_(b), c_(c) { }
public:
  virtual void visit( NodeVisitor& v ) { v.visitTernaryExpr(*this); }

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
  IfThenElse( result_type a, result_type b, result_type c ) : TernaryExpression(a,b,c) { }

  void visit( NodeVisitor& v ) { v.visitIfThenElse(*this); }
};

} // end namespace crave
