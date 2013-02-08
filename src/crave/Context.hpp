#pragma once

#include "bitsize_traits.hpp"
#include "Constraint.hpp"
#include "VectorConstraint.hpp"
#include "ExpressionTraits.hpp"
#include "expression/Node.hpp"

#include <boost/foreach.hpp>
#include <boost/proto/core.hpp>
#include <boost/proto/debug.hpp>
#include <boost/proto/context/callable.hpp>
#include <boost/proto/context/null.hpp>
#include <boost/range/value_type.hpp>

#include <map>
#include <set>

namespace crave {

struct Context: proto::callable_context<Context, proto::null_context> {

public:
  typedef Node expression;
  typedef boost::intrusive_ptr<expression> result_type;

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

  template<typename value_type>
  result_type operator()(proto::tag::terminal t, write_ref_tag<value_type> const & ref) {

    std::map<int, result_type>::const_iterator ite = variables_.find(ref.id);
    if ( ite != variables_.end() ) {
      return ite->second;
    } else {
      result_type var = (*this)(t, static_cast<var_tag<value_type> >(ref));

      variables_.insert(std::make_pair(ref.id, var));
      write_references_.push_back(var);

      // FIXME: implement ne way to handle write references
      // boost::function0<void> f = writeReference<value_type>(ref.ref, *this, ref.id);
      // _post_hook.push_back(f);
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

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::assign, Expr1 const & e1, Expr2 const & e2) {
    //std::cout << "eval assign" << std::endl;
    result_type expr1, expr2;
    expr1 = proto::eval(e1, *this);
    expr2 = proto::eval(e2, *this);

    return new AssignOpr(expr1, expr2);
    //FIXME: Generate IR // boost::function0<result_type> f = getID( evaluate(solver,
    //FIXME: Generate IR //     preds::equal(expr1, expr2)) );
    //FIXME: Generate IR // _lazy[proto::value(e1).id] = f;

    // TODO: maybe new syntax for this feature: gen.assign( x(), 3)
    // FIXME: Generate IR // return evaluate( solver, preds::True);
  }

  result_type operator()(proto::tag::terminal t, randomize_tag const & tag) {
    return new Constant(1, 1, false);
    // FIXME: reverts previous function
    //_lazy.erase(tag.id);
    //return evaluate( solver, preds::True);
  }

  template<typename Integer>
  result_type operator()(proto::tag::terminal t, read_ref_tag<Integer> const & ref) {

    std::map<int, result_type>::const_iterator ite = variables_.find(ref.id);
    if ( ite != variables_.end() ) {
      return ite->second;
    } else {
      result_type var = (*this)(t, static_cast<var_tag<Integer> >(ref));

      variables_.insert(std::make_pair(ref.id, var));
      read_references_.push_back(var);

      // FIXME: implement new way to handle references //
      //boost::function0<result_type> f = getLazyReference<Integer>(ref.ref, solver, var);
      //_lazy[ref.id] = f;

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
    // FIXME: implement new way to specify distributions //
  }

  template<typename Expr1, typename Expr2>
  result_type operator()(proto::tag::subscript, Expr1 const & e1, Expr2 const & e2) {
    // e1 should be a terminal vector_tag
    BOOST_STATIC_ASSERT(( boost::is_same<
        typename proto::tag_of<Expr1>::type,
        proto::tag::terminal >::value ));
    // FIXME: Generate IR //
  }

  bool getNodeByID(int const idx, result_type& var) const {
    std::map<int, result_type>::const_iterator ite(variables_.find(idx));
    if (ite == variables_.end())
      return false;

    var = ite->second;
    return true;
  }
private:
  std::map<int, result_type> variables_; // result_type = expression* / variable_expression*
  std::vector<result_type> read_references_;
  std::vector<result_type> write_references_;
};
// Context

}// namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

