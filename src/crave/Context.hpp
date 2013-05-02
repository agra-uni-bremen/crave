#pragma once

#include "bitsize_traits.hpp"
#include "Constraint.hpp"
#include "VectorConstraint.hpp"
#include "ExpressionTraits.hpp"
#include "expression/Node.hpp"
#include "expression/AssignResultImpl.hpp"
#include "expression/ReferenceExpression.hpp"

#include <boost/foreach.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/proto/core.hpp>
#include <boost/proto/debug.hpp>
#include <boost/proto/context/callable.hpp>
#include <boost/proto/context/null.hpp>
#include <boost/range/value_type.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <utility>
#include <set>
#include <vector>

namespace crave {

struct Context: proto::callable_context<Context, proto::null_context> {

public:
  typedef Node expression;
  typedef boost::intrusive_ptr<expression> result_type;

private:
  typedef std::pair<int, boost::shared_ptr<crave::ReferenceExpression> > ReadRefPair;
  typedef std::pair<int, boost::shared_ptr<crave::AssignResult> > WriteRefPair;

public:
  Context(std::map<int, result_type>& vars, std::map<int, result_type>& vec_vars,
          std::vector<ReadRefPair>& read_refs, std::vector<WriteRefPair>& write_refs)
          : variables_(vars), vector_variables_(vec_vars),
            read_references_(read_refs), write_references_(write_refs) { }

  template<typename value_type>
  result_type operator()(proto::tag::terminal, var_tag<value_type> const & tag) {

    std::map<int, result_type>::iterator ite(variables_.lower_bound(tag.id));
    if (ite == variables_.end() || tag.id < ite->first) {

      unsigned width = bitsize_traits<value_type>::value;
      bool sign = boost::is_signed<value_type>::value;

      result_type var = new VariableExpr(tag.id, width, sign);
      variables_.insert(ite, std::make_pair(tag.id, var));
      return var;

    } else {
      return ite->second;
    }
  }

  result_type operator()(proto::tag::terminal, placeholder_tag const & tag) {

    std::map<int, result_type>::iterator ite(variables_.lower_bound(tag.id));
    if (ite == variables_.end() || tag.id < ite->first) {

      result_type ph = new Placeholder(tag.id);
      variables_.insert(ite, std::make_pair(tag.id, ph));
      return ph;

    } else {
      return ite->second;
    }
  }

  template<typename value_type>
  result_type operator()(proto::tag::terminal t, write_ref_tag<value_type> const & ref) {

    std::map<int, result_type>::const_iterator ite = variables_.find(ref.id);
    if ( ite != variables_.end() ) {
      return ite->second;
    } else {
      result_type var = (*this)(t, static_cast<var_tag<value_type> >(ref));

      write_references_.push_back( std::make_pair(
              ref.id, boost::shared_ptr<crave::AssignResult>(
                  new AssignResultToRef<value_type>(ref.ref))
              ) );

      return var;
    }
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::equal_to, Expr1 const & e1,
      Expr2 const & e2) {
    return new EqualOpr(proto::eval(e1, (*this)), proto::eval(e2, (*this)));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::not_equal_to, Expr1 const & e1,
      Expr2 const & e2) {
    return new NotEqualOpr(proto::eval(e1, (*this)), proto::eval(e2, (*this)));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::less_equal, Expr1 const & e1,
      Expr2 const & e2) {
    return new LessEqualOpr(proto::eval(e1, (*this)), proto::eval(e2, (*this)));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::less, Expr1 const & e1, Expr2 const & e2) {
    return new LessOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::greater, Expr1 const & e1, Expr2 const & e2) {
    return new GreaterOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::greater_equal, Expr1 const & e1, Expr2 const & e2) {
    return new GreaterEqualOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::logical_and, Expr1 const & e1, Expr2 const & e2) {
    return new LogicalAndOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::logical_or, Expr1 const & e1, Expr2 const & e2) {
    return new LogicalOrOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::bitwise_and, Expr1 const & e1, Expr2 const & e2) {
    return new AndOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::bitwise_or, Expr1 const & e1, Expr2 const & e2) {
    return new OrOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::bitwise_xor, Expr1 const & e1, Expr2 const & e2) {
    return new XorOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::function f, proto::terminal<operator_if_then>::type const& t,
      Expr1 const & e1, Expr2 const & e2) {
   return new IfThenElse(proto::eval(e1, *this), proto::eval(e2, *this), new Constant(1, 1, false));
  }

  template<typename Expr1, typename Expr2, typename Expr3>
  result_type operator()(proto::tag::function, proto::terminal<operator_if_then_else>::type const,
      Expr1 const & e1, Expr2 const & e2, Expr3 const & e3) {
    return new IfThenElse(proto::eval(e1, *this), proto::eval(e2, *this), proto::eval(e3, *this));
  }

  template<typename Expr>
  result_type operator()(proto::tag::negate, Expr const & e) {
    return new NegOpr(proto::eval(e, *this));
  }

  template<typename Expr>
  result_type operator()(proto::tag::complement, Expr const & e) {
    return new ComplementOpr(proto::eval(e, *this));
  }

  template<typename Expr>
  result_type operator()(proto::tag::logical_not, Expr const & e) {
    return new NotOpr(proto::eval(e, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::plus, Expr1 const & e1, Expr2 const & e2) {
    return new PlusOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::minus, Expr1 const & e1, Expr2 const & e2) {
    return new MinusOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::modulus, Expr1 const & e1, Expr2 const & e2) {
    return new ModuloOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::divides, Expr1 const & e1, Expr2 const & e2) {
    return new DevideOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::shift_left, Expr1 const & e1, Expr2 const & e2) {
    return new ShiftLeftOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::shift_right, Expr1 const & e1, Expr2 const & e2) {
    return new ShiftRightOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::multiplies, Expr1 const & e1, Expr2 const & e2) {
    return new MultipliesOpr(proto::eval(e1, *this), proto::eval(e2, *this));
  }

  template<typename Integral, typename Expr>
  result_type operator()(extend_tag, Integral const & by_width, Expr const & e) {
    return new ExtendExpression(proto::eval(e, *this), proto::value(by_width).value);
  }

  template<typename Integer>
  result_type operator()(proto::tag::terminal t, read_ref_tag<Integer> const & ref) {

    std::map<int, result_type>::const_iterator ite = variables_.find(ref.id);
    if ( ite != variables_.end() ) {
      return ite->second;
    } else {
      result_type var = (*this)(t, static_cast<var_tag<Integer> >(ref));

      boost::shared_ptr<crave::ReferenceExpression> refExpr(new ReferenceExpressionImpl<Integer>(ref.ref, var));
      read_references_.push_back(std::make_pair(ref.id, refExpr));

      return var;
    }
  }

  template<typename Integer>
  result_type operator()(proto::tag::terminal, Integer const & i) {
    unsigned width = bitsize_traits<Integer>::value;
    bool sign = boost::is_signed<Integer>::value;
    return new Constant(i, width, sign);
  }

  template<typename Value, typename CollectionTerm>
  result_type operator()(proto::tag::function,
      boost::proto::terminal<operator_inside>::type const & tag,
      Value const & value, CollectionTerm const & c) {

    typedef typename proto::result_of::value<CollectionTerm>::type Collection;
    typedef typename boost::range_value<Collection>::type CollectionEntry;

    std::set<Constant> constants;
    BOOST_FOREACH( CollectionEntry const & i, boost::proto::value(c) ) {

      unsigned width = bitsize_traits<CollectionEntry>::value;
      bool sign = boost::is_signed<CollectionEntry>::value;
      constants.insert(Constant(i, width, sign));
    }
    return new Inside(proto::eval(value, *this), constants);
  }

  template<typename Var, typename Value>
  result_type operator()(proto::tag::function,
      boost::proto::terminal<operator_dist>::type const & tag,
      Var const & var_term, Value const & probability) {

    result_type expr = proto::eval(var_term, *this);
    read_references_.push_back(std::make_pair(
        proto::value(var_term).id,
        boost::shared_ptr<crave::ReferenceExpression>(
            new DistReferenceExpr(proto::value(probability), expr)
    )));
    return new Constant(1, 1, false);
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::subscript, Expr1 const & e1, Expr2 const & e2) {
    // e1 should be a terminal vector_tag
    BOOST_STATIC_ASSERT(( boost::is_same<
        typename proto::tag_of<Expr1>::type,
        proto::tag::terminal >::value ));

    int vec_id = proto::value(e1).id;

    std::map<int, result_type>::const_iterator ite = vector_variables_.find(vec_id);
    result_type vec;
    if ( ite != vector_variables_.end() ) {
        vec = ite->second;
    } else {

      typedef typename proto::result_of::value<Expr1>::type value_type;
      unsigned width = bitsize_traits<value_type>::value;
      bool sign = boost::is_signed<value_type>::value;

      vec = new VectorExpr(vec_id, width, sign);
      vector_variables_.insert( std::make_pair(vec_id, vec) );
    }

    return new VectorAccess(vec, proto::eval(e2, *this));
  }

private:
  std::map<int, result_type>& variables_;
  std::map<int, result_type>& vector_variables_;
  std::vector<ReadRefPair>& read_references_;
  std::vector<WriteRefPair>& write_references_;
};
// Context

}// namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

