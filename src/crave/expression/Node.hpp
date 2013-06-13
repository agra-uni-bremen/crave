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
  Node( Node const& n ) : count_(0) { }
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
  Placeholder( Placeholder const& p ) : Node(p), id_(p.id()) { }

  void visit( NodeVisitor& v ) const { v.visitPlaceholder(*this); }

  unsigned int id() const { return id_; }
private:
  unsigned int id_;
};

class Terminal : public Node {
protected:
  Terminal( unsigned int bs, bool s ) : Node(), bitsize_(bs), sign_(s) { }
  Terminal( Terminal const& t ) : Node(t), bitsize_(t.bitsize()), sign_(t.sign()) { }

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
  VariableExpr( VariableExpr const& v ) :Terminal(v.bitsize(), v.sign()), id_(v.id()) { }

  void visit( NodeVisitor& v ) const { v.visitVariableExpr(*this); }

  unsigned int id() const { return id_; }
private:
  unsigned int id_;
};

class Constant : public Terminal {
public:
  Constant( unsigned long val, unsigned int bs, bool s ) : Terminal(bs, s), value_(val) { }
  Constant( Constant const& c ) : Terminal(c.bitsize(), c.sign()), value_(c.value()) { }

  void visit( NodeVisitor& v ) const { v.visitConstant(*this); }

  operator unsigned long() const { return value_; }

  unsigned long value() const { return value_; }
private:
  unsigned long value_;
};

class VectorExpr : public Terminal {
public:
  VectorExpr( unsigned int id, unsigned int bs, bool s ) : Terminal(bs, s), id_(id) { }
  VectorExpr( VectorExpr const& v ) : Terminal(v.bitsize(), v.sign()), id_(v.id()) { }

  void visit( NodeVisitor& v ) const { v.visitVectorExpr(*this); }

  unsigned int id() const { return id_; }
private:
  unsigned int id_;
};

class UnaryExpression : public Node {
protected:
  UnaryExpression( NodePtr c ) : Node(), child_(c) { }
  UnaryExpression( UnaryExpression const& u ) : Node(), child_()
  {
    child_.operator=(u.child().get());
  }

public:
  virtual void visit( NodeVisitor& v ) const { v.visitUnaryExpr(*this); }

  boost::intrusive_ptr<Node> child() const { return child_; }
private:
  boost::intrusive_ptr<Node> child_;
};

class UnaryOperator : public UnaryExpression {
protected:
  UnaryOperator( NodePtr c ) : UnaryExpression(c) { }
  UnaryOperator( UnaryOperator const& u ) : UnaryExpression(u) { }

public:
  virtual void visit( NodeVisitor& v ) const { v.visitUnaryOpr(*this); }
};

class NotOpr : public UnaryOperator {
public:
  NotOpr( NodePtr c ) : UnaryOperator(c) { };
  NotOpr( NotOpr const &n ) : UnaryOperator(n) { }

  void visit( NodeVisitor& v ) const { v.visitNotOpr(*this); }
};

class NegOpr : public UnaryOperator {
public:
  NegOpr( NodePtr c ) : UnaryOperator(c) { };
  NegOpr( NegOpr const &n ) : UnaryOperator(n) { }

  void visit( NodeVisitor& v ) const { v.visitNegOpr(*this); }
};

class ComplementOpr : public UnaryOperator {
public:
  ComplementOpr( NodePtr c ) : UnaryOperator(c) { };
  ComplementOpr( ComplementOpr const &n ) : UnaryOperator(n) { }

  void visit( NodeVisitor& v ) const { v.visitComplementOpr(*this); }
};

class Inside : public UnaryExpression {
public:
  Inside( NodePtr v, std::set<Constant> const &c ) : UnaryExpression(v), collection_(c) { }
  Inside( Inside const& i ) : UnaryExpression(i), collection_(i.collection()) { }

  void visit( NodeVisitor& v ) const { v.visitInside(*this); }

  std::set<Constant> const & collection() const { return collection_; };
private:
  std::set<Constant> collection_;
};

class ExtendExpression : public UnaryExpression {
public:
  ExtendExpression( NodePtr v, unsigned int i ) : UnaryExpression(v), value_(i) { }
  ExtendExpression( ExtendExpression const& e ) : UnaryExpression(e), value_(e.value()) { }

  void visit( NodeVisitor& v ) const { v.visitExtendExpr(*this); }

  unsigned int const & value() const { return value_; };
private:
  unsigned int value_;
};

class BinaryExpression : public Node {
protected:
  BinaryExpression(NodePtr lhs, NodePtr rhs) : lhs_(lhs), rhs_(rhs) { }
  BinaryExpression( BinaryExpression const& b ) : Node(), lhs_(), rhs_()
  {
    lhs_.operator=(b.lhs().get());
    rhs_.operator=(b.rhs().get());
  }

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
  BinaryOperator( BinaryOperator const& b ) : BinaryExpression(b) { }
public:
  virtual void visit( NodeVisitor& v ) const { v.visitBinaryOpr(*this); }
};

class AndOpr : public BinaryOperator {
public:
  AndOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  AndOpr( AndOpr const& a ) : BinaryOperator(a) { }

  void visit( NodeVisitor& v ) const { v.visitAndOpr(*this); }
};

class OrOpr : public BinaryOperator {
public:
  OrOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  OrOpr( OrOpr const& o ) : BinaryOperator(o) { }

  void visit( NodeVisitor& v ) const { v.visitOrOpr(*this); }
};


class LogicalAndOpr : public BinaryOperator {
public:
  LogicalAndOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  LogicalAndOpr( LogicalAndOpr const& a ) : BinaryOperator(a) { }

  void visit( NodeVisitor& v ) const { v.visitLogicalAndOpr(*this); }
};

class LogicalOrOpr : public BinaryOperator {
public:
  LogicalOrOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  LogicalOrOpr( LogicalOrOpr const& o ) : BinaryOperator(o) { }

  void visit( NodeVisitor& v ) const { v.visitLogicalOrOpr(*this); }
};

class XorOpr : public BinaryOperator {
public:
  XorOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  XorOpr( XorOpr const& x ) : BinaryOperator(x) { }

  void visit( NodeVisitor& v ) const { v.visitXorOpr(*this); }
};

class EqualOpr : public BinaryOperator {
public:
  EqualOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  EqualOpr( EqualOpr const& e ) : BinaryOperator(e) { }

  void visit( NodeVisitor& v ) const { v.visitEqualOpr(*this); }
};

class NotEqualOpr : public BinaryOperator {
public:
  NotEqualOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  NotEqualOpr( NotEqualOpr const& n ) : BinaryOperator(n) { }

  void visit( NodeVisitor& v ) const { v.visitNotEqualOpr(*this); }
};

class LessOpr : public BinaryOperator {
public:
  LessOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  LessOpr( LessOpr const& l ) : BinaryOperator(l) { }

  void visit( NodeVisitor& v ) const { v.visitLessOpr(*this); }
};

class LessEqualOpr : public BinaryOperator {
public:
  LessEqualOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  LessEqualOpr( LessEqualOpr const& l ) : BinaryOperator(l) { }

  void visit( NodeVisitor& v ) const { v.visitLessEqualOpr(*this); }
};

class GreaterOpr : public BinaryOperator {
public:
  GreaterOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  GreaterOpr( GreaterOpr const& g ) : BinaryOperator(g) { }

  void visit( NodeVisitor& v ) const { v.visitGreaterOpr(*this); }
};

class GreaterEqualOpr : public BinaryOperator {
public:
  GreaterEqualOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  GreaterEqualOpr( GreaterEqualOpr const& g ) : BinaryOperator(g) { }

  void visit( NodeVisitor& v ) const { v.visitGreaterEqualOpr(*this); }
};

class PlusOpr : public BinaryOperator {
public:
  PlusOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  PlusOpr( PlusOpr const& p ) : BinaryOperator(p) { }

  void visit( NodeVisitor& v ) const { v.visitPlusOpr(*this); }
};

class MinusOpr : public BinaryOperator {
public:
  MinusOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  MinusOpr( MinusOpr const& m ) : BinaryOperator(m) { }

  void visit( NodeVisitor& v ) const { v.visitMinusOpr(*this); }
};

class MultipliesOpr : public BinaryOperator {
public:
  MultipliesOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  MultipliesOpr( MultipliesOpr const& m ) : BinaryOperator(m) { }

  void visit( NodeVisitor& v ) const { v.visitMultipliesOpr(*this); }
};

class DevideOpr : public BinaryOperator {
public:
  DevideOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  DevideOpr( DevideOpr const& d ) : BinaryOperator(d) { }

  void visit( NodeVisitor& v ) const { v.visitDevideOpr(*this); }
};

class ModuloOpr : public BinaryOperator {
public:
  ModuloOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  ModuloOpr( ModuloOpr const& m ) : BinaryOperator(m) { }

  void visit( NodeVisitor& v ) const { v.visitModuloOpr(*this); }
};

class ShiftLeftOpr : public BinaryOperator {
public:
  ShiftLeftOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  ShiftLeftOpr( ShiftLeftOpr const& s ) : BinaryOperator(s) { }

  void visit( NodeVisitor& v ) const { v.visitShiftLeftOpr(*this); }
};

class ShiftRightOpr : public BinaryOperator {
public:
  ShiftRightOpr( NodePtr lhs, NodePtr rhs ) : BinaryOperator(lhs, rhs) { }
  ShiftRightOpr( ShiftRightOpr const& s ) : BinaryOperator(s) { }

  void visit( NodeVisitor& v ) const { v.visitShiftRightOpr(*this); }
};

class VectorAccess : public BinaryExpression {
public:
  VectorAccess( NodePtr lhs, NodePtr rhs ) : BinaryExpression(lhs, rhs) {}
  VectorAccess( VectorAccess const& v ) : BinaryExpression(v) { }

  void visit( NodeVisitor& v ) const { v.visitVectorAccess(*this); }
};

class TernaryExpression : public Node {
protected:
  TernaryExpression( NodePtr a, NodePtr b, NodePtr c ) : Node(), a_(a), b_(b), c_(c) { }
  TernaryExpression( TernaryExpression const& t ) : Node(t), a_(), b_(), c_()
  {
    a_.operator=(t.a().get());
    b_.operator=(t.b().get());
    c_.operator=(t.c().get());
  }
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
  IfThenElse( IfThenElse const& t ) : TernaryExpression(t) { }

  void visit( NodeVisitor& v ) const { v.visitIfThenElse(*this); }
};

} // end namespace crave
