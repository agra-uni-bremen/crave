#include "../../crave/expression/EvalVisitor.hpp"

namespace crave {

void EvalVisitor::visitNode(const Node&) { }

void EvalVisitor::visitTerminal(const Terminal& t)
{

}

void EvalVisitor::visitUnaryExpr(const UnaryExpression& u)
{
  u.child()->visit(*this);
}

void EvalVisitor::visitUnaryOpr(const UnaryOperator& )
{

}

void EvalVisitor::visitBinaryExpr(const BinaryExpression& b)
{
  b.lhs()->visit(*this);
  b.rhs()->visit(*this);
}

void EvalVisitor::visitBinaryOpr(const BinaryOperator& )
{

}

void EvalVisitor::visitTernaryExpr(const TernaryExpression& t)
{
  t.a()->visit(*this);
  t.b()->visit(*this);
  t.c()->visit(*this);
}


void EvalVisitor::visitPlaceholder(const Placeholder& )
{

}

void EvalVisitor::visitVariableExpr(const VariableExpr& v)
{
  eval_map::const_iterator ite(evalMap_.find(v.id()));
  if (ite != evalMap_.end())
    visitConstant(ite->second);
  else
    exprStack_.push(std::make_pair(Constant(), false));
}

void EvalVisitor::visitConstant(const Constant& c)
{
  exprStack_.push(std::make_pair(c, true));
}

void EvalVisitor::visitVectorExpr(const VectorExpr& v)
{
  eval_map::const_iterator ite(evalMap_.find(v.id()));
  if (ite != evalMap_.end())
    visitConstant(ite->second);
  else
    exprStack_.push(std::make_pair(Constant(), false));
}

void EvalVisitor::visitNotOpr(const NotOpr& n)
{
  visitUnaryExpr(n);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(Constant(!e.first.value()), e.second));
}

void EvalVisitor::visitNegOpr(const NegOpr& n)
{
  visitUnaryExpr(n);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(Constant(-e.first.value(), e.first.bitsize(), e.first.sign()),
                                 e.second));
}

void EvalVisitor::visitComplementOpr(const ComplementOpr& c)
{
  visitUnaryExpr(c);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(Constant(~e.first.value(), e.first.bitsize(), e.first.sign()),
                                 e.second));
}

void EvalVisitor::visitInside(const Inside& i)
{
  visitUnaryExpr(i);

  stack_entry e;
  pop(e);

  bool result = false;

  for (std::set<Constant>::iterator ite = i.collection().begin(); ite != i.collection().end(); ++ite)
    if (*ite == e.first.value())
      result = true;

  exprStack_.push(std::make_pair(Constant(result), e.second));
}

void EvalVisitor::visitExtendExpr(const ExtendExpression& e)
{
  visitUnaryExpr(e);

  stack_entry entry;
  pop(entry);

  Constant const& child = entry.first;
  Constant constant(child.value(), child.bitsize() + e.value(), child.sign());
  exprStack_.push(std::make_pair(constant, entry.second));
}

void EvalVisitor::visitAndOpr(const AndOpr& a)
{
  stack_entry lhs, rhs;
  evalBinExpr(a, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() & rhs.first.value(),
                                          lhs.first.bitsize(),
                                          lhs.first.sign() || rhs.first.sign()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitOrOpr(const OrOpr& o)
{
  stack_entry lhs, rhs;
  evalBinExpr(o, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() | rhs.first.value(),
                                          lhs.first.bitsize(),
                                          lhs.first.sign() || rhs.first.sign()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitLogicalAndOpr(const LogicalAndOpr& la)
{
  stack_entry lhs, rhs;
  evalBinExpr(la, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() && rhs.first.value()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitLogicalOrOpr(const LogicalOrOpr& lo)
{
  stack_entry lhs, rhs;
  evalBinExpr(lo, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() || rhs.first.value()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitXorOpr(const XorOpr& x)
{
  stack_entry lhs, rhs;
  evalBinExpr(x, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() ^ rhs.first.value(),
                                          lhs.first.bitsize(),
                                          lhs.first.sign() || rhs.first.sign()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitEqualOpr(const EqualOpr& eq)
{
  stack_entry lhs, rhs;
  evalBinExpr(eq, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() == rhs.first.value()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitNotEqualOpr(const NotEqualOpr& neq)
{
  stack_entry lhs, rhs;
  evalBinExpr(neq, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() != rhs.first.value()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitLessOpr(const LessOpr& l)
{
  stack_entry lhs, rhs;
  evalBinExpr(l, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() < rhs.first.value()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitLessEqualOpr(const LessEqualOpr& le)
{
  stack_entry lhs, rhs;
  evalBinExpr(le, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() <= rhs.first.value()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitGreaterOpr(const GreaterOpr& g)
{
  stack_entry lhs, rhs;
  evalBinExpr(g, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() > rhs.first.value()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitGreaterEqualOpr(const GreaterEqualOpr& ge)
{
  stack_entry lhs, rhs;
  evalBinExpr(ge, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() >= rhs.first.value()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitPlusOpr(const PlusOpr& p)
{
  stack_entry lhs, rhs;
  evalBinExpr(p, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() + rhs.first.value(),
                                          lhs.first.bitsize(),
                                          lhs.first.sign() || rhs.first.sign()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitMinusOpr(const MinusOpr& m)
{
  stack_entry lhs, rhs;
  evalBinExpr(m, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() - rhs.first.value(),
                                          lhs.first.bitsize(),
                                          lhs.first.sign() || rhs.first.sign()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitMultipliesOpr(const MultipliesOpr& m)
{
  stack_entry lhs, rhs;
  evalBinExpr(m, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() * rhs.first.value(),
                                          lhs.first.bitsize(),
                                          lhs.first.sign() || rhs.first.sign()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitDevideOpr(const DevideOpr& d)
{
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
      constant = Constant(static_cast<long>(lhs.first.value()) / static_cast<long>(rhs.first.value()),
                          lhs.first.bitsize(), true);
    else
      constant = Constant(lhs.first.value() / rhs.first.value(),
                          lhs.first.bitsize(), false);
  }
  exprStack_.push(std::make_pair(constant, result));
}

void EvalVisitor::visitModuloOpr(const ModuloOpr& m)
{
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
      constant = Constant(static_cast<long>(lhs.first.value()) % static_cast<long>(rhs.first.value()),
                          lhs.first.bitsize(), true);
    else
      constant = Constant(lhs.first.value() % rhs.first.value(),
                          lhs.first.bitsize(), false);
  }
  exprStack_.push(std::make_pair(constant, result));
}

void EvalVisitor::visitShiftLeftOpr(const ShiftLeftOpr& shl)
{
  stack_entry lhs, rhs;
  evalBinExpr(shl, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() << rhs.first.value(),
                                          lhs.first.bitsize(),
                                          lhs.first.sign() || rhs.first.sign()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitShiftRightOpr(const ShiftRightOpr& shr)
{
  stack_entry lhs, rhs;
  evalBinExpr(shr, lhs, rhs);

  exprStack_.push(std::make_pair(Constant(lhs.first.value() >> rhs.first.value(),
                                          lhs.first.bitsize(),
                                          lhs.first.sign() || rhs.first.sign()),
                                 lhs.second && rhs.second));
}

void EvalVisitor::visitVectorAccess(const VectorAccess& va)
{

}

void EvalVisitor::visitIfThenElse(const IfThenElse& ite)
{
  stack_entry a, b, c;
  evalTernExpr(ite, a, b, c);

  if (a.first.value())
    exprStack_.push(b);
  else
    exprStack_.push(c);
}

} // end namespace crave