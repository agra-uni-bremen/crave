// Copyright 2014 The CRAVE developers. All rights reserved.//

#include "../crave/ir/visitor/ComplexityEstimationVisitor.hpp"

#include <stdexcept>

namespace crave {

    void ComplexityEstimationVisitor::visitNode(const Node&) {
    }

    void ComplexityEstimationVisitor::visitTerminal(const Terminal&) {
    }

    void ComplexityEstimationVisitor::visitUnaryExpr(const UnaryExpression& u) {
        u.child()->visit(this);
    }

    void ComplexityEstimationVisitor::visitUnaryOpr(const UnaryOperator&) {
    }

    void ComplexityEstimationVisitor::visitBinaryExpr(const BinaryExpression& b) {
        b.lhs()->visit(this);
        b.rhs()->visit(this);
    }

    void ComplexityEstimationVisitor::visitBinaryOpr(const BinaryOperator&) {
    }

    void ComplexityEstimationVisitor::visitTernaryExpr(const TernaryExpression& t) {
        t.a()->visit(this);
        t.b()->visit(this);
        t.c()->visit(this);
    }

    void ComplexityEstimationVisitor::visitPlaceholder(const Placeholder& pl) {
        exprStack_.push(std::make_pair(new Placeholder(pl), placeholder_bitsize()));
    }

    void ComplexityEstimationVisitor::visitVariableExpr(const VariableExpr& v) {
        exprStack_.push(std::make_pair(new VariableExpr(v), v.bitsize()));
    }

    void ComplexityEstimationVisitor::visitConstant(const Constant& c) {
        exprStack_.push(std::make_pair(new Constant(c), c.bitsize()));
    }

    void ComplexityEstimationVisitor::visitVectorExpr(const VectorExpr& v) {
        exprStack_.push(std::make_pair(new VectorExpr(v), v.bitsize()));
    }

    void ComplexityEstimationVisitor::visitNotOpr(const NotOpr& n) {
        visitUnaryExpr(n);

        stack_entry e;
        pop(e);

        exprStack_.push(std::make_pair(new NotOpr(e.first), e.second));
    }

    void ComplexityEstimationVisitor::visitNegOpr(const NegOpr& n) {
        visitUnaryExpr(n);

        stack_entry e;
        pop(e);

        exprStack_.push(std::make_pair(new NegOpr(e.first), e.second));
    }

    void ComplexityEstimationVisitor::visitComplementOpr(const ComplementOpr& c) {
        visitUnaryExpr(c);

        stack_entry e;
        pop(e);

        exprStack_.push(std::make_pair(new ComplementOpr(e.first), e.second));
    }

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

    void ComplexityEstimationVisitor::visitAndOpr(const AndOpr& a) {
        stack_entry lhs, rhs;
        evalBinExpr(a, lhs, rhs);

        exprStack_.push(std::make_pair(new AndOpr(lhs.first, rhs.first), lhs.second));
    }

    void ComplexityEstimationVisitor::visitOrOpr(const OrOpr& o) {
        stack_entry lhs, rhs;
        evalBinExpr(o, lhs, rhs);

        exprStack_.push(std::make_pair(new OrOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitLogicalAndOpr(const LogicalAndOpr& la) {
        stack_entry lhs, rhs;
        evalBinExpr(la, lhs, rhs);

        exprStack_.push(std::make_pair(new LogicalAndOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitLogicalOrOpr(const LogicalOrOpr& lo) {
        stack_entry lhs, rhs;
        evalBinExpr(lo, lhs, rhs);

        exprStack_.push(std::make_pair(new LogicalOrOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitXorOpr(const XorOpr& x) {
        stack_entry lhs, rhs;
        evalBinExpr(x, lhs, rhs);

        exprStack_.push(std::make_pair(new XorOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitEqualOpr(const EqualOpr& eq) {
        stack_entry lhs, rhs;
        evalBinExpr(eq, lhs, rhs);

        exprStack_.push(std::make_pair(new EqualOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitNotEqualOpr(const NotEqualOpr& neq) {
        stack_entry lhs, rhs;
        evalBinExpr(neq, lhs, rhs);

        exprStack_.push(std::make_pair(new NotEqualOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitLessOpr(const LessOpr& l) {
        stack_entry lhs, rhs;
        evalBinExpr(l, lhs, rhs);

        exprStack_.push(std::make_pair(new LessOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitLessEqualOpr(const LessEqualOpr& le) {
        stack_entry lhs, rhs;
        evalBinExpr(le, lhs, rhs);

        exprStack_.push(std::make_pair(new LessEqualOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitGreaterOpr(const GreaterOpr& g) {
        stack_entry lhs, rhs;
        evalBinExpr(g, lhs, rhs);

        exprStack_.push(std::make_pair(new GreaterOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitGreaterEqualOpr(const GreaterEqualOpr& ge) {
        stack_entry lhs, rhs;
        evalBinExpr(ge, lhs, rhs);

        exprStack_.push(std::make_pair(new GreaterEqualOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitPlusOpr(const PlusOpr& p) {
        stack_entry lhs, rhs;
        evalBinExpr(p, lhs, rhs);

        exprStack_.push(std::make_pair(new PlusOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitMinusOpr(const MinusOpr& m) {
        stack_entry lhs, rhs;
        evalBinExpr(m, lhs, rhs);

        exprStack_.push(std::make_pair(new MinusOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

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
  if(lhs.second > 0)
  {
      if(rhs.second > 0)
      {
          complexity = lhs.second * rhs.second;
      }
      else
      {
          complexity = lhs.second;
      }
  }
  else
  {
      complexity = rhs.second;
  } 
        exprStack_.push(std::make_pair(new ModuloOpr(lhs.first, rhs.first), complexity));
    }

    void ComplexityEstimationVisitor::visitShiftLeftOpr(const ShiftLeftOpr& shl) {
        stack_entry lhs, rhs;
        evalBinExpr(shl, lhs, rhs);

        exprStack_.push(std::make_pair(new ShiftLeftOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitShiftRightOpr(const ShiftRightOpr& shr) {
        stack_entry lhs, rhs;
        evalBinExpr(shr, lhs, rhs);

        exprStack_.push(std::make_pair(new ShiftRightOpr(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitVectorAccess(const VectorAccess& va) {
        stack_entry lhs, rhs;
        evalBinExpr(va, lhs, rhs);
        exprStack_.push(std::make_pair(new VectorAccess(lhs.first, rhs.first), rhs.second+lhs.second));
    }

    void ComplexityEstimationVisitor::visitForEach(const ForEach& fe) {
        stack_entry lhs, rhs;
        evalBinExpr(fe, lhs, rhs);

        exprStack_.push(std::make_pair(new ForEach(lhs.first, rhs.first), lhs.second+rhs.second));
    }

    void ComplexityEstimationVisitor::visitUnique(const Unique& u) {
        visitUnaryExpr(u);

        stack_entry e;
        pop(e);

        exprStack_.push(std::make_pair(new Unique(e.first), e.second));
    }

    void ComplexityEstimationVisitor::visitIfThenElse(const IfThenElse& ite) {
        stack_entry a, b, c;
        evalTernExpr(ite, a, b, c);
        exprStack_.push(std::make_pair(new IfThenElse(a.first, b.first, c.first), a.second+b.second+c.second));
    }

    void ComplexityEstimationVisitor::visitBitslice(const Bitslice& b) {
        visitUnaryExpr(b);

        stack_entry e;
        pop(e);

        exprStack_.push(std::make_pair(new Bitslice(e.first, b.r(), b.l()), b.r() - b.l() + 1));
    }

} // end namespace crave
