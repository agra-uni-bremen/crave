#include "../../crave/expression/ReplaceVisitor.hpp"

#include <stdexcept>

namespace crave {

void ReplaceVisitor::evalUnaryExpr(UnaryExpression const& unary_expr, NodePtr& node) {
  visitUnaryExpr(unary_expr);
  assert(aux_stack_.size() >= 1);
  node = aux_stack_.top();
  aux_stack_.pop();
}

void ReplaceVisitor::evalBinExpr(BinaryExpression const& bin_expr, NodePtr &lhs, NodePtr&rhs) {
  visitBinaryExpr(bin_expr);
  assert(aux_stack_.size() >= 2);
  rhs = aux_stack_.top();
  aux_stack_.pop();
  lhs = aux_stack_.top();
  aux_stack_.pop();
}

void ReplaceVisitor::evalTernExpr(TernaryExpression const &tern_expr, NodePtr &a, NodePtr &b, NodePtr &c) {
  visitTernaryExpr(tern_expr);
  assert(aux_stack_.size() >= 3);
  c = aux_stack_.top();
  aux_stack_.pop();
  b = aux_stack_.top();
  aux_stack_.pop();
  a = aux_stack_.top();
  aux_stack_.pop();
}

void ReplaceVisitor::evalBinSubscript(int& lhs, int& rhs) {
  assert(subscript_stack_.size() >= 2);
  rhs = subscript_stack_.top();
  subscript_stack_.pop();
  lhs = subscript_stack_.top();
  subscript_stack_.pop();
}

void ReplaceVisitor::evalTernSubscript(int& a, int& b, int& c) {
  assert(subscript_stack_.size() >= 3);
  c = subscript_stack_.top();
  subscript_stack_.pop();
  b = subscript_stack_.top();
  subscript_stack_.pop();
  a = subscript_stack_.top();
  subscript_stack_.pop();
}

void ReplaceVisitor::visitNode( Node const& n ) { }
void ReplaceVisitor::visitTerminal( Terminal const& t ) { }
void ReplaceVisitor::visitUnaryExpr( UnaryExpression const& e ) {
  e.child()->visit(*this);
}
void ReplaceVisitor::visitUnaryOpr( UnaryOperator const& ) { }
void ReplaceVisitor::visitBinaryExpr( BinaryExpression const& e ) {
  e.lhs()->visit(*this);
  e.rhs()->visit(*this);
}
void ReplaceVisitor::visitBinaryOpr( BinaryOperator const& ) { }
void ReplaceVisitor::visitTernaryExpr( TernaryExpression const& t ) {
  t.a()->visit(*this);
  t.b()->visit(*this);
  t.c()->visit(*this);
}
void ReplaceVisitor::visitPlaceholder( Placeholder const& p ) {
  // assert( lsh is vector expression )
  std::map<int, NodePtr>::iterator ite(terminals_.lower_bound(p.id()));
  if (ite != terminals_.end() && !(terminals_.key_comp()(p.id(), ite->first))) {
    // v already exists
    aux_stack_.push(ite->second);
  } else {
    aux_stack_.push(new Constant(vec_idx_, 32, false));
    terminals_.insert(std::make_pair(p.id(), aux_stack_.top()));
  }
  updateResult();
  subscript_stack_.push(vec_idx_);
}
void ReplaceVisitor::visitVariableExpr( VariableExpr const& v ) {
  std::map<int, NodePtr>::iterator ite(terminals_.lower_bound(v.id()));
  if (ite != terminals_.end() && !(terminals_.key_comp()(v.id(), ite->first))) {
    // v already exists
    aux_stack_.push(ite->second);
  } else {
    aux_stack_.push(new VariableExpr(v));
    terminals_.insert(std::make_pair(v.id(), aux_stack_.top()));
  }
  updateResult();
  subscript_stack_.push(0);
}
void ReplaceVisitor::visitConstant( Constant const& c ) {
  aux_stack_.push(new Constant(c));
  updateResult();
  subscript_stack_.push(c.value());
}
void ReplaceVisitor::visitVectorExpr( VectorExpr const& v ) {
  std::map<int, NodePtr>::iterator ite(terminals_.lower_bound(v.id()));
  if (ite != terminals_.end() && !(terminals_.key_comp()(v.id(), ite->first))) {
    // v already exists
    aux_stack_.push(ite->second);
  } else {
    aux_stack_.push(new VectorExpr(v));
    terminals_.insert(std::make_pair(v.id(), aux_stack_.top()));
  }
  updateResult();
  subscript_stack_.push(0);
}
void ReplaceVisitor::visitNotOpr( NotOpr const& n ) {
  NodePtr child;
  evalUnaryExpr(n, child);
  aux_stack_.push(new NotOpr(child));
  updateResult();
  int idx = subscript_stack_.top();
  subscript_stack_.pop();
  subscript_stack_.push(!idx);
}
void ReplaceVisitor::visitNegOpr( NegOpr const& n ) {
  NodePtr child;
  evalUnaryExpr(n, child);
  aux_stack_.push(new NegOpr(child));
  updateResult();
  int idx = subscript_stack_.top();
  subscript_stack_.pop();
  subscript_stack_.push(-idx);
}
void ReplaceVisitor::visitComplementOpr( ComplementOpr const& c ) {
  NodePtr child;
  evalUnaryExpr(c, child);
  aux_stack_.push(new ComplementOpr(child));
  updateResult();
  int idx = subscript_stack_.top();
  subscript_stack_.pop();
  subscript_stack_.push(~idx);
}
void ReplaceVisitor::visitInside( Inside const& i ) {
  NodePtr child;
  evalUnaryExpr(i, child);
  aux_stack_.push(new Inside(child, i.collection()));
  updateResult();
}
void ReplaceVisitor::visitExtendExpr( ExtendExpression const& e ) {
  NodePtr child;
  evalUnaryExpr(e, child);
  aux_stack_.push(new ExtendExpression(child, e.value()));
  updateResult();
}
void ReplaceVisitor::visitAndOpr( AndOpr const& a ) {
  NodePtr lhs, rhs;
  evalBinExpr(a, lhs, rhs);
  aux_stack_.push(new AndOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val & r_val);
}
void ReplaceVisitor::visitOrOpr( OrOpr const& o ) {
  NodePtr lhs, rhs;
  evalBinExpr(o, lhs, rhs);
  aux_stack_.push(new OrOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val | r_val);
}
void ReplaceVisitor::visitLogicalAndOpr( LogicalAndOpr const& a ) {
  NodePtr lhs, rhs;
  evalBinExpr(a, lhs, rhs);
  aux_stack_.push(new LogicalAndOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val && r_val);
}
void ReplaceVisitor::visitLogicalOrOpr( LogicalOrOpr const& o ) {
  NodePtr lhs, rhs;
  evalBinExpr(o, lhs, rhs);
  aux_stack_.push(new LogicalOrOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val || r_val);
}
void ReplaceVisitor::visitXorOpr( XorOpr const& x ) {
  NodePtr lhs, rhs;
  evalBinExpr(x, lhs, rhs);
  aux_stack_.push(new XorOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val ^ r_val);
}
void ReplaceVisitor::visitEqualOpr( EqualOpr const& e ) {
  NodePtr lhs, rhs;
  evalBinExpr(e, lhs, rhs);
  aux_stack_.push(new EqualOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val == r_val);
}
void ReplaceVisitor::visitNotEqualOpr( NotEqualOpr const& n ) {
  NodePtr lhs, rhs;
  evalBinExpr(n, lhs, rhs);
  aux_stack_.push(new NotEqualOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val != r_val);
}
void ReplaceVisitor::visitLessOpr( LessOpr const& l ) {
  NodePtr lhs, rhs;
  evalBinExpr(l, lhs, rhs);
  aux_stack_.push(new LessOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val < r_val);
}
void ReplaceVisitor::visitLessEqualOpr( LessEqualOpr const& l ) {
  NodePtr lhs, rhs;
  evalBinExpr(l, lhs, rhs);
  aux_stack_.push(new LessEqualOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val <= r_val);
}
void ReplaceVisitor::visitGreaterOpr( GreaterOpr const& g ) {
  NodePtr lhs, rhs;
  evalBinExpr(g, lhs, rhs);
  aux_stack_.push(new GreaterOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val > r_val);
}
void ReplaceVisitor::visitGreaterEqualOpr( GreaterEqualOpr const& g ) {
  NodePtr lhs, rhs;
  evalBinExpr(g, lhs, rhs);
  aux_stack_.push(new GreaterEqualOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val >= r_val);
}
void ReplaceVisitor::visitPlusOpr( PlusOpr const& p ) {
  NodePtr lhs, rhs;
  evalBinExpr(p, lhs, rhs);
  aux_stack_.push(new PlusOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val + r_val);
}
void ReplaceVisitor::visitMinusOpr( MinusOpr const& m ) {
  NodePtr lhs, rhs;
  evalBinExpr(m, lhs, rhs);
  aux_stack_.push(new MinusOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val - r_val);
}
void ReplaceVisitor::visitMultipliesOpr( MultipliesOpr const& m ) {
  NodePtr lhs, rhs;
  evalBinExpr(m, lhs, rhs);
  aux_stack_.push(new MultipliesOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val * r_val);
}
void ReplaceVisitor::visitDevideOpr( DevideOpr const& d ) {
  NodePtr lhs, rhs;
  evalBinExpr(d, lhs, rhs);
  aux_stack_.push(new DevideOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val / r_val);
}
void ReplaceVisitor::visitModuloOpr( ModuloOpr const& m ) {
  NodePtr lhs, rhs;
  evalBinExpr(m, lhs, rhs);
  aux_stack_.push(new ModuloOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val % r_val);
}
void ReplaceVisitor::visitShiftLeftOpr( ShiftLeftOpr const& s ) {
  NodePtr lhs, rhs;
  evalBinExpr(s, lhs, rhs);
  aux_stack_.push(new ShiftLeftOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val << r_val);
}
void ReplaceVisitor::visitShiftRightOpr( ShiftRightOpr const& s ) {
  NodePtr lhs, rhs;
  evalBinExpr(s, lhs, rhs);
  aux_stack_.push(new ShiftRightOpr(lhs, rhs));
  updateResult();
  int l_val, r_val;
  evalBinSubscript(l_val, r_val);
  subscript_stack_.push(l_val >> r_val);
}
void ReplaceVisitor::visitVectorAccess( VectorAccess const& v ) {
  NodePtr lhs, rhs;
  evalBinExpr(v, lhs, rhs);
  int l_val, i;
  evalBinSubscript(l_val, i);

  if (0 <= i && i < (int) variables_.size()) {

    VectorExpr& vec = *static_cast<VectorExpr*>(lhs.get());
    VariableExpr& var = *static_cast<VariableExpr*>(variables_[i].get());
    variables_[i] = new VariableExpr(var.id(), vec.bitsize(), vec.sign());

    aux_stack_.push(variables_[i]);
  } else {
    aux_stack_.push(variables_[0]);
    okay_ = false;
  }
  updateResult();
  subscript_stack_.push(0);
}
void ReplaceVisitor::visitIfThenElse( IfThenElse const& i ) {
  NodePtr a, b, c;
  evalTernExpr(i, a, b, c);
  aux_stack_.push(new IfThenElse(a, b, c));
  updateResult();
  int a_val, b_val, c_val;
  evalTernSubscript(a_val, b_val, c_val);
  subscript_stack_.push(a_val ? b_val : c_val);
}

void ReplaceVisitor::visitForEach( ForEach const& fe ) {
  fe.rhs()->visit(*this);
}

void ReplaceVisitor::visitUnique( Unique const& u ) { throw std::runtime_error("Unique is not allowed in ReplaceVisitor."); }

void ReplaceVisitor::visitBitslice( Bitslice const& b ) {
  NodePtr child;
  evalUnaryExpr(b, child);
  aux_stack_.push(new Bitslice(child, b.r(), b.l(), b.expr_size()));
  updateResult();
  int idx = subscript_stack_.top();
  subscript_stack_.pop();
  subscript_stack_.push(!idx);
}

void ReplaceVisitor::updateResult() {
  result_ = aux_stack_.top();
}

void ReplaceVisitor::reset() {
  terminals_.clear();
  while (!aux_stack_.empty())
    aux_stack_.pop();
  while (!subscript_stack_.empty())
    subscript_stack_.pop();
  result_.reset();
  okay_ = true;
}

} // end namespace crave
