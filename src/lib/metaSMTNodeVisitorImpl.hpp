// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once
#include <algorithm>
#include <functional>
#include <map>
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/support/cardinality.hpp>
#include <metaSMT/support/contradiction_analysis.hpp>
#include <metaSMT/tags/QF_BV.hpp>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "../crave/frontend/AssignResult.hpp"
#include "../crave/frontend/RandomBase.hpp"
#include "../crave/ir/visitor/metaSMTNodeVisitor.hpp"

namespace crave {
namespace preds = metaSMT::logic::tag;
namespace qf_bv = metaSMT::logic::QF_BV::tag;
using metaSMT::evaluate;

extern std::function<unsigned(unsigned)> random_unsigned;

template <typename SolverType>
class metaSMTVisitorImpl : public metaSMTVisitor {
 public:
  metaSMTVisitorImpl()
      : metaSMTVisitor(), solver_(), exprStack_(), terminals_(), softs_(), assumptions_(), suggestions_() {}

  virtual void visitNode(Node const &);
  virtual void visitTerminal(Terminal const &);
  virtual void visitUnaryExpr(UnaryExpression const &);
  virtual void visitUnaryOpr(UnaryOperator const &);
  virtual void visitBinaryExpr(BinaryExpression const &);
  virtual void visitBinaryOpr(BinaryOperator const &);
  virtual void visitTernaryExpr(TernaryExpression const &);
  virtual void visitPlaceholder(Placeholder const &);
  virtual void visitVariableExpr(VariableExpr const &);
  virtual void visitConstant(Constant const &);
  virtual void visitVectorExpr(VectorExpr const &);
  virtual void visitNotOpr(NotOpr const &);
  virtual void visitNegOpr(NegOpr const &);
  virtual void visitComplementOpr(ComplementOpr const &);
  virtual void visitInside(Inside const &);
  virtual void visitExtendExpr(ExtendExpression const &);
  virtual void visitAndOpr(AndOpr const &);
  virtual void visitOrOpr(OrOpr const &);
  virtual void visitLogicalAndOpr(LogicalAndOpr const &);
  virtual void visitLogicalOrOpr(LogicalOrOpr const &);
  virtual void visitXorOpr(XorOpr const &);
  virtual void visitEqualOpr(EqualOpr const &);
  virtual void visitNotEqualOpr(NotEqualOpr const &);
  virtual void visitLessOpr(LessOpr const &);
  virtual void visitLessEqualOpr(LessEqualOpr const &);
  virtual void visitGreaterOpr(GreaterOpr const &);
  virtual void visitGreaterEqualOpr(GreaterEqualOpr const &);
  virtual void visitPlusOpr(PlusOpr const &);
  virtual void visitMinusOpr(MinusOpr const &);
  virtual void visitMultipliesOpr(MultipliesOpr const &);
  virtual void visitDivideOpr(DivideOpr const &);
  virtual void visitModuloOpr(ModuloOpr const &);
  virtual void visitShiftLeftOpr(ShiftLeftOpr const &);
  virtual void visitShiftRightOpr(ShiftRightOpr const &);
  virtual void visitVectorAccess(VectorAccess const &);
  virtual void visitIfThenElse(IfThenElse const &);
  virtual void visitForEach(ForEach const &);
  virtual void visitUnique(Unique const &);
  virtual void visitBitslice(Bitslice const &);

  virtual void makeAssertion(Node const &);
  virtual void makeSoftAssertion(Node const &);
  virtual void makeSuggestion(Node const &);
  virtual void makeAssumption(Node const &);
  virtual std::vector<unsigned int> analyseSofts();
  virtual std::vector<std::vector<unsigned int> > analyseContradiction(std::map<unsigned int, NodePtr> const &);
  virtual bool solve(bool ignoreSofts);
  virtual bool read(Node const &var, AssignResult &);
  virtual bool read(Node const &var, std::string &);
  virtual bool readVector(const std::vector<VariablePtr> &vec, __rand_vec_base *rand_vec);

 private:  // typedefs
  typedef typename SolverType::result_type result_type;
  typedef std::pair<result_type, bool> stack_entry;
  typedef std::map<int, result_type> result_map;

 private:  // methods
  inline void pop(stack_entry &fst);
  inline void pop2(stack_entry &fst, stack_entry &snd);
  inline void pop3(stack_entry &fst, stack_entry &snd, stack_entry &trd);
  void evalBinExpr(BinaryExpression const &expr, stack_entry &fst, stack_entry &snd);
  void evalTernExpr(TernaryExpression const &expr, stack_entry &fst, stack_entry &snd, stack_entry &trd);

 private:  // data
  SolverType solver_;
  std::stack<stack_entry> exprStack_;
  result_map terminals_;
  std::vector<result_type> softs_;
  std::vector<result_type> assumptions_;
  std::vector<result_type> suggestions_;
};

template <typename SolverType>
inline void metaSMTVisitorImpl<SolverType>::pop(stack_entry &fst) {
  assert(exprStack_.size() >= 1);
  fst = exprStack_.top();
  exprStack_.pop();
}

template <typename SolverType>
inline void metaSMTVisitorImpl<SolverType>::pop2(stack_entry &fst, stack_entry &snd) {
  assert(exprStack_.size() >= 2);
  fst = exprStack_.top();
  exprStack_.pop();
  snd = exprStack_.top();
  exprStack_.pop();
}

template <typename SolverType>
inline void metaSMTVisitorImpl<SolverType>::pop3(stack_entry &fst, stack_entry &snd, stack_entry &trd) {
  assert(exprStack_.size() >= 3);
  fst = exprStack_.top();
  exprStack_.pop();
  snd = exprStack_.top();
  exprStack_.pop();
  trd = exprStack_.top();
  exprStack_.pop();
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::evalBinExpr(BinaryExpression const &expr, stack_entry &fst, stack_entry &snd) {
  visitBinaryExpr(expr);
  pop2(snd, fst);
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::evalTernExpr(TernaryExpression const &expr, stack_entry &fst, stack_entry &snd,
                                                  stack_entry &trd) {
  visitTernaryExpr(expr);
  pop3(trd, snd, fst);
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitNode(Node const &) {}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitTerminal(Terminal const &) {}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitUnaryExpr(UnaryExpression const &ue) {
  ue.child()->visit(this);
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitUnaryOpr(UnaryOperator const &) {}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitBinaryExpr(BinaryExpression const &be) {
  be.lhs()->visit(this);
  be.rhs()->visit(this);
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitBinaryOpr(BinaryOperator const &) {}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitTernaryExpr(TernaryExpression const &te) {
  te.a()->visit(this);
  te.b()->visit(this);
  te.c()->visit(this);
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitPlaceholder(Placeholder const &) {}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitVariableExpr(VariableExpr const &ve) {
  typename result_map::iterator ite(terminals_.lower_bound(ve.id()));

  result_type result;
  if (ite != terminals_.end() && !(terminals_.key_comp()(ve.id(), ite->first))) {
    // &t already exists
    result = ite->second;
  } else {
    if (ve.isBool())
      result = solver_(metaSMT::logic::new_variable());
    else
      result = solver_(metaSMT::logic::QF_BV::new_bitvector(ve.bitsize()));
    terminals_.insert(ite, std::make_pair(ve.id(), result));
  }

  exprStack_.push(std::make_pair(result, ve.sign()));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitConstant(Constant const &c) {
  result_type result = c.isBool() ? (c.value() ? solver_(true) : solver_(false))
                                  : (c.sign() ? solver_(qf_bv::bvsint_tag{}, c.value(), c.bitsize())
                                              : solver_(qf_bv::bvuint_tag{}, c.value(), c.bitsize()));
  exprStack_.push(std::make_pair(result, c.sign()));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitNotOpr(NotOpr const &o) {
  visitUnaryExpr(o);

  stack_entry entry;
  pop(entry);

  result_type result = solver_(preds::not_tag{}, entry.first);
  exprStack_.push(std::make_pair(result, entry.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitNegOpr(NegOpr const &o) {
  visitUnaryExpr(o);

  stack_entry entry;
  pop(entry);

  result_type result = solver_(qf_bv::bvneg_tag{}, entry.first);
  exprStack_.push(std::make_pair(result, entry.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitComplementOpr(ComplementOpr const &o) {
  visitUnaryExpr(o);

  stack_entry entry;
  pop(entry);

  result_type result = solver_(qf_bv::bvnot_tag{}, entry.first);
  exprStack_.push(std::make_pair(result, entry.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitInside(Inside const &in) {
  visitUnaryExpr(in);

  stack_entry entry;
  pop(entry);

  result_type result = solver_(false);
  for (Constant c : in.collection()) {
    stack_entry constExpr;
    c.visit(this);
    pop(constExpr);
    result = solver_(preds::equal_tag{}, entry.first, constExpr.first);
  }

  exprStack_.push(std::make_pair(result, false));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitExtendExpr(ExtendExpression const &e) {
  visitUnaryExpr(e);

  stack_entry entry;
  pop(entry);

  result_type result;
  if (entry.second)
    result = solver_(qf_bv::sign_extend_tag{}, e.value(), entry.first);
  else
    result = solver_(qf_bv::zero_extend_tag{}, e.value(), entry.first);

  exprStack_.push(std::make_pair(result, entry.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitAndOpr(AndOpr const &o) {
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = solver_(qf_bv::bvand_tag{}, fst.first, snd.first);
  exprStack_.push(std::make_pair(result, fst.second || snd.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitOrOpr(OrOpr const &o) {
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = solver_(qf_bv::bvor_tag{}, fst.first, snd.first);
  exprStack_.push(std::make_pair(result, fst.second || snd.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitLogicalAndOpr(LogicalAndOpr const &o) {
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = solver_(preds::and_tag{}, fst.first, snd.first);
  exprStack_.push(std::make_pair(result, fst.second || snd.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitLogicalOrOpr(LogicalOrOpr const &o) {
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = solver_(preds::or_tag{}, fst.first, snd.first);
  exprStack_.push(std::make_pair(result, fst.second || snd.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitXorOpr(XorOpr const &o) {
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = solver_(qf_bv::bvxor_tag{}, fst.first, snd.first);
  exprStack_.push(std::make_pair(result, fst.second || snd.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitEqualOpr(EqualOpr const &o) {
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = solver_(preds::equal_tag{}, fst.first, snd.first);
  exprStack_.push(std::make_pair(result, false));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitNotEqualOpr(NotEqualOpr const &o) {
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = solver_(preds::nequal_tag{}, fst.first, snd.first);
  exprStack_.push(std::make_pair(result, false));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitLessOpr(LessOpr const &o) {
  stack_entry rhs_p, lhs_p;
  evalBinExpr(o, lhs_p, rhs_p);

  result_type lhs = lhs_p.first;
  result_type rhs = rhs_p.first;
  bool lhs_signed = lhs_p.second;
  bool rhs_signed = rhs_p.second;

  result_type result;
  if (lhs_signed && rhs_signed) {
    result = solver_(qf_bv::bvslt_tag{}, lhs, rhs);
  } else if (!(lhs_signed || rhs_signed)) {
    result = solver_(qf_bv::bvult_tag{}, lhs, rhs);
  } else if (lhs_signed) {
    result = solver_(qf_bv::bvslt_tag{}, solver_(qf_bv::sign_extend_tag{}, 1, lhs),
                     solver_(qf_bv::zero_extend_tag{}, 1, rhs));
  } else {
    result = solver_(qf_bv::bvslt_tag{}, solver_(qf_bv::zero_extend_tag{}, 1, lhs),
                     solver_(qf_bv::sign_extend_tag{}, 1, rhs));
  }
  exprStack_.push(std::make_pair(result, false));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitLessEqualOpr(LessEqualOpr const &o) {
  stack_entry rhs_p, lhs_p;
  evalBinExpr(o, lhs_p, rhs_p);

  result_type lhs = lhs_p.first;
  result_type rhs = rhs_p.first;
  bool lhs_signed = lhs_p.second;
  bool rhs_signed = rhs_p.second;

  result_type result;
  if (lhs_signed && rhs_signed) {
    result = solver_(qf_bv::bvsle_tag{}, lhs, rhs);
  } else if (!(lhs_signed || rhs_signed)) {
    result = solver_(qf_bv::bvule_tag{}, lhs, rhs);
  } else if (lhs_signed) {
    result = solver_(qf_bv::bvsle_tag{}, solver_(qf_bv::sign_extend_tag{}, 1, lhs),
                     solver_(qf_bv::zero_extend_tag{}, 1, rhs));
  } else {
    result = solver_(qf_bv::bvsle_tag{}, solver_(qf_bv::zero_extend_tag{}, 1, lhs),
                     solver_(qf_bv::sign_extend_tag{}, 1, rhs));
  }
  exprStack_.push(std::make_pair(result, false));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitGreaterOpr(GreaterOpr const &o) {
  stack_entry rhs_p, lhs_p;
  evalBinExpr(o, lhs_p, rhs_p);

  result_type lhs = lhs_p.first;
  result_type rhs = rhs_p.first;
  bool lhs_signed = lhs_p.second;
  bool rhs_signed = rhs_p.second;

  result_type result;
  if (lhs_signed && rhs_signed) {
    result = solver_(qf_bv::bvsgt_tag{}, lhs, rhs);
  } else if (!(lhs_signed || rhs_signed)) {
    result = solver_(qf_bv::bvugt_tag{}, lhs, rhs);
  } else if (lhs_signed) {
    result = solver_(qf_bv::bvsgt_tag{}, solver_(qf_bv::sign_extend_tag{}, 1, lhs),
                     solver_(qf_bv::zero_extend_tag{}, 1, rhs));
  } else {
    result = solver_(qf_bv::bvsgt_tag{}, solver_(qf_bv::zero_extend_tag{}, 1, lhs),
                     solver_(qf_bv::sign_extend_tag{}, 1, rhs));
  }
  exprStack_.push(std::make_pair(result, false));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitGreaterEqualOpr(GreaterEqualOpr const &o) {
  stack_entry rhs_p, lhs_p;
  evalBinExpr(o, lhs_p, rhs_p);

  result_type lhs = lhs_p.first;
  result_type rhs = rhs_p.first;
  bool lhs_signed = lhs_p.second;
  bool rhs_signed = rhs_p.second;

  result_type result;
  if (lhs_signed && rhs_signed) {
    result = solver_(qf_bv::bvsge_tag{}, lhs, rhs);
  } else if (!(lhs_signed || rhs_signed)) {
    result = solver_(qf_bv::bvuge_tag{}, lhs, rhs);
  } else if (lhs_signed) {
    result = solver_(qf_bv::bvsge_tag{}, solver_(qf_bv::sign_extend_tag{}, 1, lhs),
                     solver_(qf_bv::zero_extend_tag{}, 1, rhs));
  } else {
    result = solver_(qf_bv::bvsge_tag{}, solver_(qf_bv::zero_extend_tag{}, 1, lhs),
                     solver_(qf_bv::sign_extend_tag{}, 1, rhs));
  }
  exprStack_.push(std::make_pair(result, false));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitPlusOpr(PlusOpr const &o) {
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = solver_(qf_bv::bvadd_tag{}, fst.first, snd.first);
  exprStack_.push(std::make_pair(result, fst.second || snd.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitMinusOpr(MinusOpr const &o) {
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = solver_(qf_bv::bvsub_tag{}, fst.first, snd.first);
  exprStack_.push(std::make_pair(result, fst.second || snd.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitMultipliesOpr(MultipliesOpr const &o) {
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = solver_(qf_bv::bvmul_tag{}, fst.first, snd.first);
  exprStack_.push(std::make_pair(result, fst.second || snd.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitDivideOpr(DivideOpr const &o) {
  stack_entry rhs_p, lhs_p;
  evalBinExpr(o, lhs_p, rhs_p);

  result_type lhs = lhs_p.first;
  result_type rhs = rhs_p.first;
  bool lhs_signed = lhs_p.second;
  bool rhs_signed = rhs_p.second;

  result_type result;
  if (lhs_signed && rhs_signed) {
    result = solver_(qf_bv::bvsdiv_tag{}, lhs, rhs);
  } else if (!(lhs_signed || rhs_signed)) {
    result = solver_(qf_bv::bvudiv_tag{}, lhs, rhs);
  } else if (lhs_signed) {
    result = solver_(qf_bv::bvsdiv_tag{}, solver_(qf_bv::sign_extend_tag{}, 1, lhs),
                     solver_(qf_bv::zero_extend_tag{}, 1, rhs));
  } else {
    result = solver_(qf_bv::bvsdiv_tag{}, solver_(qf_bv::zero_extend_tag{}, 1, lhs),
                     solver_(qf_bv::sign_extend_tag{}, 1, rhs));
  }
  exprStack_.push(std::make_pair(result, lhs_signed || rhs_signed));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitModuloOpr(ModuloOpr const &o) {
  stack_entry rhs_p, lhs_p;
  evalBinExpr(o, lhs_p, rhs_p);

  result_type lhs = lhs_p.first;
  result_type rhs = rhs_p.first;
  bool lhs_signed = lhs_p.second;

  result_type result;
  if (lhs_signed)
    result = solver_(qf_bv::bvsrem_tag{}, lhs, rhs);
  else
    result = solver_(qf_bv::bvurem_tag{}, lhs, rhs);

  exprStack_.push(std::make_pair(result, lhs_signed || rhs_p.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitShiftLeftOpr(ShiftLeftOpr const &o) {
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = solver_(qf_bv::bvshl_tag{}, fst.first, snd.first);
  exprStack_.push(std::make_pair(result, fst.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitShiftRightOpr(ShiftRightOpr const &o) {
  stack_entry fst, snd;
  evalBinExpr(o, fst, snd);

  result_type result = solver_(qf_bv::bvshr_tag{}, fst.first, snd.first);
  exprStack_.push(std::make_pair(result, fst.second));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitIfThenElse(IfThenElse const &ite) {
  stack_entry fst, snd, trd;
  evalTernExpr(ite, fst, snd, trd);
  result_type result =
      solver_(preds::ite_tag{}, solver_(preds::equal_tag{}, fst.first, solver_(true)), snd.first, trd.first);
  exprStack_.push(std::make_pair(result, false));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitBitslice(Bitslice const &b) {
  visitUnaryExpr(b);

  stack_entry entry;
  pop(entry);

  result_type result = solver_(qf_bv::extract_tag{}, b.r(), b.l(), entry.first);

  exprStack_.push(std::make_pair(result, false));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitVectorAccess(VectorAccess const &) {
  throw std::runtime_error("VectorAccess is not allowed in metaSMTNodeVisitor.");
}
template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitVectorExpr(VectorExpr const &) {
  throw std::runtime_error("VectorExpr is not allowed in metaSMTNodeVisitor.");
}
template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitForEach(ForEach const &) {
  throw std::runtime_error("ForEach is not allowed in metaSMTNodeVisitor.");
}
template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::visitUnique(Unique const &) {
  throw std::runtime_error("Unique is not allowed in metaSMTNodeVisitor.");
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::makeAssertion(Node const &expr) {
  expr.visit(this);

  stack_entry entry;
  pop(entry);

  metaSMT::assertion(solver_, solver_(preds::equal_tag{}, entry.first, solver_(true)));
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::makeSoftAssertion(Node const &expr) {
  expr.visit(this);

  stack_entry entry;
  pop(entry);

  softs_.push_back(entry.first);
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::makeSuggestion(Node const &expr) {
  expr.visit(this);

  stack_entry entry;
  pop(entry);

  suggestions_.push_back(entry.first);
}

template <typename SolverType>
void metaSMTVisitorImpl<SolverType>::makeAssumption(Node const &expr) {
  expr.visit(this);

  stack_entry entry;
  pop(entry);

  assumptions_.push_back(solver_(preds::equal_tag{}, entry.first, solver_(true)));
}

template <typename SolverType>
std::vector<unsigned int> metaSMTVisitorImpl<SolverType>::analyseSofts() {
  std::vector<unsigned int> result;
  // implements soft constraint semantics of the HVL e
  for (unsigned int i = 0; i < softs_.size(); i++) {
    metaSMT::assumption(solver_, solver_(preds::equal_tag{}, softs_[i], solver_(true)));
    if (metaSMT::solve(solver_)) {
      // accept
      metaSMT::assertion(solver_, solver_(preds::equal_tag{}, softs_[i], solver_(true)));
    } else {
      // reject
      result.push_back(i);
    }
  }
  softs_.clear();
  return result;
}

template <typename SolverType>
std::vector<std::vector<unsigned int> > metaSMTVisitorImpl<SolverType>::analyseContradiction(
    std::map<unsigned int, NodePtr> const &s) {
  std::vector<std::vector<unsigned int> > results;
  std::map<unsigned int, result_type> result_map;

  typedef std::pair<unsigned int, NodePtr> NodePair;

  for (NodePair entry : s) {
    entry.second->visit(this);
    stack_entry st_entry;
    pop(st_entry);

    result_type var(solver_(metaSMT::logic::new_variable()));
    metaSMT::assertion(solver_, solver_(preds::implies_tag{}, var, st_entry.first));
    result_map.insert(std::make_pair(entry.first, var));
  }

  results = metaSMT::analyze_conflicts(solver_, result_map, solver_(true), results);

  return results;
}

template <typename SolverType>
bool metaSMTVisitorImpl<SolverType>::solve(bool ignoreSofts) {
  bool result = false;

  std::random_shuffle(assumptions_.begin(), assumptions_.end(), crave::random_unsigned);
  std::random_shuffle(suggestions_.begin(), suggestions_.end(), crave::random_unsigned);

  while (true) {
    for (result_type const &item : assumptions_) {
      metaSMT::assumption(solver_, item);
    }

    for (result_type const &item : suggestions_) {
      metaSMT::assumption(solver_, item);
    }

    if (!ignoreSofts) {
      for (result_type const &item : softs_) {
        metaSMT::assumption(solver_, solver_(preds::equal_tag{}, item, solver_(true)));
      }
    }

    if (metaSMT::solve(solver_)) {
      result = true;
      break;
    }

    if (suggestions_.empty()) break;

    unsigned select = crave::random_unsigned(suggestions_.size());
    std::swap(suggestions_[select], suggestions_.back());
    suggestions_.pop_back();
  }

  assumptions_.clear();
  suggestions_.clear();

  return result;
}

template <typename SolverType>
bool metaSMTVisitorImpl<SolverType>::read(Node const &v, AssignResult &assign) {
  std::string res;
  if (read(v, res)) {
    assign.set_value(res);
    return true;
  } else
    return false;
}

template <typename SolverType>
bool metaSMTVisitorImpl<SolverType>::read(Node const &v, std::string &str) {
  // assert(static_cast<VariableExpr const *>(&var) != 0);
  VariableExpr const &var = *static_cast<VariableExpr const *>(&v);
  typename result_map::const_iterator ite(terminals_.find(var.id()));
  if (ite == terminals_.end()) return false;
  result_type var_expr = ite->second;
  std::string tmp = metaSMT::read_value(solver_, var_expr);
  str = tmp;
  return true;
}

template <typename SolverType>
bool metaSMTVisitorImpl<SolverType>::readVector(const std::vector<VariablePtr> &vec, __rand_vec_base *rand_vec) {
  std::vector<std::string> sv;
  for (VariablePtr var : vec) {
    typename result_map::const_iterator ite(terminals_.find(var->id()));
    if (ite == terminals_.end()) return false;

    result_type var_expr = ite->second;
    sv.push_back(metaSMT::read_value(solver_, var_expr));
  }

  rand_vec->set_values(sv);

  return true;
}

}  // namespace crave
