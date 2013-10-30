#pragma once

#include "bitsize_traits.hpp"
#include "Constraint.hpp"
#include "VectorConstraint.hpp"
#include "ExpressionTraits.hpp"
#include "VariableContainer.hpp"
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

struct Context: boost::proto::callable_context<Context, boost::proto::null_context> {

public:
  typedef Node expression;
  typedef boost::intrusive_ptr<expression> result_type;

private:
  typedef std::pair<int, boost::shared_ptr<crave::ReferenceExpression> > ReadRefPair;
  typedef std::pair<int, boost::shared_ptr<crave::AssignResult> > WriteRefPair;

public:
  Context(VariableContainer& vars)
          : variables_(vars.variables), vector_variables_(vars.vector_variables),
            read_references_(vars.read_references), write_references_(vars.write_references) { }

  template<typename value_type>
  result_type operator()(boost::proto::tag::terminal, var_tag<value_type> const & tag) {

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

  result_type operator()(boost::proto::tag::terminal, placeholder_tag const & tag) {

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
  result_type operator()(boost::proto::tag::terminal t, write_ref_tag<value_type> const & ref) {

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
  result_type operator()(boost::proto::tag::equal_to, Expr1 const & e1,
      Expr2 const & e2) {
    return new EqualOpr(boost::proto::eval(e1, (*this)), boost::proto::eval(e2, (*this)));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::not_equal_to, Expr1 const & e1,
      Expr2 const & e2) {
    return new NotEqualOpr(boost::proto::eval(e1, (*this)), boost::proto::eval(e2, (*this)));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::less_equal, Expr1 const & e1,
      Expr2 const & e2) {
    return new LessEqualOpr(boost::proto::eval(e1, (*this)), boost::proto::eval(e2, (*this)));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::less, Expr1 const & e1, Expr2 const & e2) {
    return new LessOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::greater, Expr1 const & e1, Expr2 const & e2) {
    return new GreaterOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::greater_equal, Expr1 const & e1, Expr2 const & e2) {
    return new GreaterEqualOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::logical_and, Expr1 const & e1, Expr2 const & e2) {
    return new LogicalAndOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::logical_or, Expr1 const & e1, Expr2 const & e2) {
    return new LogicalOrOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::bitwise_and, Expr1 const & e1, Expr2 const & e2) {
    return new AndOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::bitwise_or, Expr1 const & e1, Expr2 const & e2) {
    return new OrOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::bitwise_xor, Expr1 const & e1, Expr2 const & e2) {
    return new XorOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::function f, boost::proto::terminal<operator_if_then>::type const& t,
      Expr1 const & e1, Expr2 const & e2) {
    return new IfThenElse(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this), new Constant(true));
  }

  template<typename Expr1, typename Expr2, typename Expr3>
  result_type operator()(boost::proto::tag::function, boost::proto::terminal<operator_if_then_else>::type const,
      Expr1 const & e1, Expr2 const & e2, Expr3 const & e3) {
    return new IfThenElse(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this), boost::proto::eval(e3, *this));
  }

  template<typename Expr>
  result_type operator()(boost::proto::tag::negate, Expr const & e) {
    return new NegOpr(boost::proto::eval(e, *this));
  }

  template<typename Expr>
  result_type operator()(boost::proto::tag::complement, Expr const & e) {
    return new ComplementOpr(boost::proto::eval(e, *this));
  }

  template<typename Expr>
  result_type operator()(boost::proto::tag::logical_not, Expr const & e) {
    return new NotOpr(boost::proto::eval(e, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::plus, Expr1 const & e1, Expr2 const & e2) {
    return new PlusOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::minus, Expr1 const & e1, Expr2 const & e2) {
    return new MinusOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::modulus, Expr1 const & e1, Expr2 const & e2) {
    return new ModuloOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::divides, Expr1 const & e1, Expr2 const & e2) {
    return new DevideOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::shift_left, Expr1 const & e1, Expr2 const & e2) {
    return new ShiftLeftOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::shift_right, Expr1 const & e1, Expr2 const & e2) {
    return new ShiftRightOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::multiplies, Expr1 const & e1, Expr2 const & e2) {
    return new MultipliesOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template<typename Integral, typename Expr>
  result_type operator()(extend_tag, Integral const & by_width, Expr const & e) {
    return new ExtendExpression(boost::proto::eval(e, *this), boost::proto::value(by_width).value);
  }

  template<typename Integer>
  result_type operator()(boost::proto::tag::terminal t, read_ref_tag<Integer> const & ref) {

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
  result_type operator()(boost::proto::tag::terminal, Integer const & i) {
    unsigned width = bitsize_traits<Integer>::value;
    bool sign = boost::is_signed<Integer>::value;
    return new Constant(i, width, sign);
  }

  template<typename Value, typename CollectionTerm>
  result_type operator()(boost::proto::tag::function,
      boost::proto::terminal<operator_inside>::type const & tag,
      Value const & value, CollectionTerm const & c) {

    typedef typename boost::proto::result_of::value<CollectionTerm>::type Collection;
    typedef typename boost::range_value<Collection>::type CollectionEntry;

    std::set<Constant> constants;
    BOOST_FOREACH( CollectionEntry const & i, boost::proto::value(c) ) {

      unsigned width = bitsize_traits<CollectionEntry>::value;
      bool sign = boost::is_signed<CollectionEntry>::value;
      constants.insert(Constant(i, width, sign));
    }
    return new Inside(boost::proto::eval(value, *this), constants);
  }

  template<typename Var, typename Value>
  result_type operator()(boost::proto::tag::function,
      boost::proto::terminal<operator_dist>::type const & tag,
      Var const & var_term, Value const & probability) {

    result_type expr = boost::proto::eval(var_term, *this);
    read_references_.push_back(std::make_pair(
        boost::proto::value(var_term).id,
        boost::shared_ptr<crave::ReferenceExpression>(
            new DistReferenceExpr(boost::proto::value(probability), expr)
    )));
    return new Constant(true);
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::function,
      boost::proto::terminal<operator_foreach>::type const & tag,
      Expr1 const & e1, placeholder const & p, Expr2 const & e2) {
      
    return new Constant(true);
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::subscript, Expr1 const & e1, Expr2 const & e2) {
    // e1 should be a terminal vector_tag
    BOOST_STATIC_ASSERT(( boost::is_same<
        typename boost::proto::tag_of<Expr1>::type,
        boost::proto::tag::terminal >::value ));

    int vec_id = boost::proto::value(e1).id;

    std::map<int, boost::intrusive_ptr<VectorExpr> >::const_iterator ite = vector_variables_.find(vec_id);
    boost::intrusive_ptr<VectorExpr> vec;
    if ( ite != vector_variables_.end() ) {
        vec = ite->second;
    } else {

      typedef typename boost::proto::result_of::value<Expr1>::type value_type;
      unsigned width = bitsize_traits<value_type>::value;
      bool sign = boost::is_signed<value_type>::value;

      vec = new VectorExpr(vec_id, width, sign);
      vector_variables_.insert( std::make_pair(vec_id, vec) );
    }

    return new VectorAccess(vec, boost::proto::eval(e2, *this));
  }

private:
  std::map<int, result_type>& variables_;
  std::map<int, boost::intrusive_ptr<VectorExpr> >& vector_variables_;
  std::vector<ReadRefPair>& read_references_;
  std::vector<WriteRefPair>& write_references_;
};
// Context

}// namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

