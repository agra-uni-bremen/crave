#include "../../crave/expression/Node.hpp"
#include "../../crave/expression/NodeVisitor.hpp"

namespace crave {

bool metaSMTVisitor::visitNode(Node const &n)
{
  return false;
}

void metaSMTVisitor::visitTerminal(Terminal const &t)
{

}

void metaSMTVisitor::visitUnaryExpr(UnaryExpression const &ue)
{

}

void metaSMTVisitor::visitUnaryOpr(UnaryOperator const &uo)
{

}

void metaSMTVisitor::visitBinaryExpr(BinaryExpression const &be)
{

}

void metaSMTVisitor::visitBinaryOpr(BinaryOperator const &)
{

}

void metaSMTVisitor::visitTernaryExpr(TernaryExpression const &te)
{

}

void metaSMTVisitor::visitPlaceholder( Placeholder const &pl )
{

}

void metaSMTVisitor::visitVariableExpr(VariableExpr const &ve)
{

}

void metaSMTVisitor::visitConstant(Constant const &c)
{

}

void metaSMTVisitor::visitVectorExpr(VectorExpr const &ve)
{

}

void metaSMTVisitor::visitNotOpr(NotOpr const &o)
{

}

void metaSMTVisitor::visitNegOpr(NegOpr const &o)
{

}

void metaSMTVisitor::visitComplementOpr(ComplementOpr const &o)
{

}

void metaSMTVisitor::visitInside(Inside const &o)
{

}

void metaSMTVisitor::visitAndOpr(AndOpr const &o)
{

}

void metaSMTVisitor::visitOrOpr(OrOpr const &o)
{

}

void metaSMTVisitor::visitXorOpr(XorOpr const &o)
{

}

void metaSMTVisitor::visitEqualOpr(EqualOpr const &o)
{

}

void metaSMTVisitor::visitNotEqualOpr(NotEqualOpr const &o)
{

}

void metaSMTVisitor::visitLessOpr(LessOpr const &o)
{

}

void metaSMTVisitor::visitLessEqualOpr(LessEqualOpr const &o)
{
  /*
  if ( child_a.signed && child_b.signed )
    evaluate( solver, qf_bv::bvsle( expr1, expr2 ));
  else if ( !(child_a.signed || child_b.signed) )
    evaluate( solver, qf_bv::bvule( expr1, expr2 ));
  else if ( childe_a.signed )
    evaluate( solver, qf_bv::bvsle(
                      qf_bv::sign_extend( expr1 ),
                      qf_bv::zero_extend( expr2 )));
  else
    evaluate( solver, qf_bv::bvsle(
                      qf_bv::zero_extend( expr1 ),
                      qf_bv::sign_extend( expr2 )));
  */
}

void metaSMTVisitor::visitGreaterOpr(GreaterOpr const &o)
{

}

void metaSMTVisitor::visitGreaterEqualOpr(GreaterEqualOpr const &o)
{

}

void metaSMTVisitor::visitPlusOpr(PlusOpr const &o)
{

}

void metaSMTVisitor::visitMinusOpr(MinusOpr const &o)
{

}

void metaSMTVisitor::visitMultipliesOpr(MultipliesOpr const &o)
{

}

void metaSMTVisitor::visitDevideOpr(DevideOpr const &o)
{

}

void metaSMTVisitor::visitModuloOpr(ModuloOpr const &o)
{

}

void metaSMTVisitor::visitShiftLeftOpr(ShiftLeftOpr const &o)
{

}

void metaSMTVisitor::visitShiftRightOpr(ShiftRightOpr const &o)
{

}

void metaSMTVisitor::visitAssignOpr(AssignOpr const &o)
{

}

void metaSMTVisitor::visitVectorAccess(VectorAccess const &o)
{

}

void metaSMTVisitor::visitIfThenElse(IfThenElse const &ite) { }

} // end crave namespace
