#include <boost/foreach.hpp>

#include "../../crave/expression/ToDotNodeVisitor.hpp"

namespace crave {

bool ToDotVisitor::putNode(Node const *n)
{
  if (nodes_.find(n) != nodes_.end())
    return false;

  nodes_.insert(n);
  return true;
}

void ToDotVisitor::visitNode(Node const &n)
{
  out_ << "\t" << reinterpret_cast<long>(&n);
}

void ToDotVisitor::visitTerminal(Terminal const &t)
{
  out_ << " is " << (t.sign() ? "s" : "u") << t.bitsize();
}

void ToDotVisitor::visitUnaryExpr(UnaryExpression const &ue)
{
  ue.child()->visit(*this);
  out_ << "\t"
       << reinterpret_cast<long>(&ue) << " -> "
       << reinterpret_cast<long>(ue.child().get()) << std::endl;
}

void ToDotVisitor::visitUnaryOpr(UnaryOperator const &uo)
{
  out_ << "UnOp";
}

void ToDotVisitor::visitBinaryExpr(BinaryExpression const &be) {
  be.lhs()->visit(*this);
  out_ << "\t"
       << reinterpret_cast<long>(&be) << " -> "
       << reinterpret_cast<long>(be.lhs().get()) << std::endl;
  be.rhs()->visit(*this);
  out_ << "\t"
       << reinterpret_cast<long>(&be) << " -> "
       << reinterpret_cast<long>(be.rhs().get()) << std::endl;
}

void ToDotVisitor::visitBinaryOpr(BinaryOperator const &) {
  out_ << "BinOp";
}

void ToDotVisitor::visitTernaryExpr(TernaryExpression const &te) {
  te.a()->visit(*this);
  out_ << "\t"
       << reinterpret_cast<long>(&te) << " -> "
       << reinterpret_cast<long>(te.a().get()) << std::endl;
  te.b()->visit(*this);
  out_ << "\t"
       << reinterpret_cast<long>(&te) << " -> "
       << reinterpret_cast<long>(te.b().get()) << std::endl;
  te.c()->visit(*this);
  out_ << "\t"
       << reinterpret_cast<long>(&te) << " -> "
       << reinterpret_cast<long>(te.c().get()) << std::endl;
}

void ToDotVisitor::visitPlaceholder( Placeholder const &pl )
{
  if ( putNode(&pl) )
  {
    visitNode(pl);
    out_ << " [label=\"placeholder: " << pl.id() << "\"]" << std::endl;
  }
}

void ToDotVisitor::visitVariableExpr(VariableExpr const &ve)
{
  if ( putNode(&ve) )
  {
    visitNode(ve);
    out_ << " [label=\"variable: " << ve.id();
    visitTerminal(ve);
    out_ << "\"]" << std::endl;
  }
}

void ToDotVisitor::visitConstant(Constant const &c)
{
  if ( putNode(&c) )
  {
    visitNode(c);
    out_ << " [label=\"constant: ";
    if (c.sign())
      out_ << static_cast<long>(c.value());
    else
      out_ << c.value();
    visitTerminal(c);
    out_ << "\"]\n";
  }
}

void ToDotVisitor::visitVectorExpr(VectorExpr const &ve)
{
  if ( putNode(&ve) )
  {
    visitNode(ve);
    out_ << " [label=\"variable: " << ve.id();
    visitTerminal(ve);
    out_ << "\"]" << std::endl;
  }
}

void ToDotVisitor::visitNotOpr(NotOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitUnaryOpr(o);
    out_ << ": not (!)\"]" << std::endl;
  }
  visitUnaryExpr(o);
}

void ToDotVisitor::visitNegOpr(NegOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitUnaryOpr(o);
    out_ << ": neg (-)\"]" << std::endl;
  }
  visitUnaryExpr(o);
}

void ToDotVisitor::visitComplementOpr(ComplementOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitUnaryOpr(o);
    out_ << ": complement (~)\"]" << std::endl;
  }
  visitUnaryExpr(o);
}

void ToDotVisitor::visitInside(Inside const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"inside\n{ ";
    BOOST_FOREACH ( unsigned long u, o.collection() ) out_ << u << " ";
    out_ << "}\"]" << std::endl;
  }
  visitUnaryExpr(o);
}

void ToDotVisitor::visitExtendExpr( ExtendExpression const &e )
{
  if ( putNode(&e) )
  {
    visitNode(e);
    out_ << " [label=\"extend by " << e.value() << "\"]" << std::endl;
  }
  visitUnaryExpr(e);
}

void ToDotVisitor::visitRandomizeExpr(RandomizeExpr const &e)
{
  if ( putNode(&e) )
  {
    visitNode(e);
    out_ << " [label=\"randomize\"]" << std::endl;
  }
  visitUnaryExpr(e);
}

void ToDotVisitor::visitLogicalAndOpr(LogicalAndOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": and (&&)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitLogicalOrOpr(LogicalOrOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": or (||)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitAndOpr(AndOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": and (&&)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitOrOpr(OrOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": or (||)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitXorOpr(XorOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": xor (^)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitEqualOpr(EqualOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": equal (==)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitNotEqualOpr(NotEqualOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": not_equal (!=)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitLessOpr(LessOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": less (<)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitLessEqualOpr(LessEqualOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": less_equal (<=)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitGreaterOpr(GreaterOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": greater (>)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitGreaterEqualOpr(GreaterEqualOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": greater_equal (>=)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitPlusOpr(PlusOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": plus (+)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitMinusOpr(MinusOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": minus (-)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitMultipliesOpr(MultipliesOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": multiplies (*)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitDevideOpr(DevideOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": devide (/)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitModuloOpr(ModuloOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": modulo (%)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitShiftLeftOpr(ShiftLeftOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": shift_left (<<)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitShiftRightOpr(ShiftRightOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": shift_right (>>)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitAssignOpr(AssignOpr const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"";
    visitBinaryOpr(o);
    out_ << ": assign (=)\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitVectorAccess(VectorAccess const &o)
{
  if ( putNode(&o) )
  {
    visitNode(o);
    out_ << " [label=\"vector_access ([])\"]" << std::endl;
  }
  visitBinaryExpr(o);
}

void ToDotVisitor::visitIfThenElse(IfThenElse const &ite)
{
  if ( putNode(&ite) )
  {
    visitNode(ite);
    out_ << " [label=\"if_then_else (?:)\"]" << std::endl;
  }
  visitTernaryExpr(ite);
}

} // end crave namespace
