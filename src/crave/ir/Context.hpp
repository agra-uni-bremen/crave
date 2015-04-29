#pragma once

#include "../frontend/bitsize_traits.hpp"
#include "../frontend/Constraint.hpp"
#include "../frontend/AssignResultImpl.hpp"
#include "VariableContainer.hpp"
#include "Node.hpp"
#include "ReferenceExpression.hpp"

#include <boost/foreach.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/proto/core.hpp>
#include <boost/proto/debug.hpp>
#include <boost/proto/context/callable.hpp>
#include <boost/proto/context/null.hpp>
#include <boost/range/value_type.hpp>
#include <boost/shared_ptr.hpp>

#include <limits>
#include <map>
#include <set>
#include <utility>
#include <vector>

namespace crave {

struct Context : boost::proto::callable_context<Context, boost::proto::null_context> {

 public:
  typedef NodePtr result_type;

 private:
  typedef std::pair<int, boost::shared_ptr<crave::ReferenceExpression> > ReadRefPair;
  typedef std::pair<int, boost::shared_ptr<crave::AssignResult> > WriteRefPair;

 public:
  Context(VariableContainer& vars)
      : variables_(vars.variables),
        vector_variables_(vars.vector_variables),
        read_references_(vars.read_references),
        write_references_(vars.write_references),
        dist_references_(vars.dist_references) {}

  result_type new_var(unsigned id, unsigned width, bool sign) {
    return (variables_[id] = new VariableExpr(id, width, sign));
  }

  template <typename value_type>
  result_type new_var(var_tag<value_type> const& tag) {
    unsigned width = bitsize_traits<value_type>::value;
    bool sign = crave::is_signed<value_type>::value;
    return new_var(tag.id, width, sign);
  }

  template <typename value_type>
  result_type operator()(boost::proto::tag::terminal, var_tag<value_type> const& tag) {
    std::map<int, result_type>::iterator ite(variables_.find(tag.id));
    return ite != variables_.end() ? ite->second : new_var(tag);
  }

  template <typename value_type>
  result_type operator()(boost::proto::tag::terminal, vector_tag<value_type> const& tag) {
    std::map<int, result_type>::const_iterator ite = vector_variables_.find(tag.id);

    if (ite != vector_variables_.end()) {
      return ite->second;
    } else {
      unsigned width = bitsize_traits<value_type>::value;
      bool sign = crave::is_signed<value_type>::value;

      result_type vec = new VectorExpr(tag.id, width, sign);
      vector_variables_.insert(std::make_pair(tag.id, vec));
      return vec;
    }
  }

  result_type operator()(boost::proto::tag::terminal, placeholder_tag const& tag) { return new Placeholder(tag.id); }

  result_type operator()(boost::proto::tag::terminal, result_type const& r) { return r; }

  template <typename value_type>
  result_type operator()(boost::proto::tag::terminal t, write_ref_tag<value_type> const& ref) {
    std::map<int, result_type>::const_iterator ite = variables_.find(ref.id);

    if (ite != variables_.end()) {
      return ite->second;
    } else {
      result_type var = new_var(static_cast<var_tag<value_type> >(ref));

      write_references_.push_back(
          std::make_pair(ref.id, boost::shared_ptr<crave::AssignResult>(new AssignResultToRef<value_type>(ref.ref))));

      return var;
    }
  }

#if GCC_VERSION < 40600
  template <typename value_type>
  result_type operator()(boost::proto::tag::terminal t, WriteReference<value_type> const& ref) {
    return operator()(t, boost::proto::value(ref));
  }
#endif

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::equal_to, Expr1 const& e1, Expr2 const& e2) {
    return new EqualOpr(boost::proto::eval(e1, (*this)), boost::proto::eval(e2, (*this)));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::not_equal_to, Expr1 const& e1, Expr2 const& e2) {
    return new NotEqualOpr(boost::proto::eval(e1, (*this)), boost::proto::eval(e2, (*this)));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::less_equal, Expr1 const& e1, Expr2 const& e2) {
    return new LessEqualOpr(boost::proto::eval(e1, (*this)), boost::proto::eval(e2, (*this)));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::less, Expr1 const& e1, Expr2 const& e2) {
    return new LessOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::greater, Expr1 const& e1, Expr2 const& e2) {
    return new GreaterOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::greater_equal, Expr1 const& e1, Expr2 const& e2) {
    return new GreaterEqualOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::logical_and, Expr1 const& e1, Expr2 const& e2) {
    return new LogicalAndOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::logical_or, Expr1 const& e1, Expr2 const& e2) {
    return new LogicalOrOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::bitwise_and, Expr1 const& e1, Expr2 const& e2) {
    return new AndOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::bitwise_or, Expr1 const& e1, Expr2 const& e2) {
    return new OrOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::bitwise_xor, Expr1 const& e1, Expr2 const& e2) {
    return new XorOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::function f, boost::proto::terminal<operator_if_then>::type const& t,
                         Expr1 const& e1, Expr2 const& e2) {
    return new IfThenElse(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this), new Constant(true));
  }

  template <typename Expr1, typename Expr2, typename Expr3>
  result_type operator()(boost::proto::tag::function, boost::proto::terminal<operator_if_then_else>::type const,
                         Expr1 const& e1, Expr2 const& e2, Expr3 const& e3) {
    return new IfThenElse(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this), boost::proto::eval(e3, *this));
  }

  template <typename Expr>
  result_type operator()(boost::proto::tag::negate, Expr const& e) {
    return new NegOpr(boost::proto::eval(e, *this));
  }

  template <typename Expr>
  result_type operator()(boost::proto::tag::complement, Expr const& e) {
    return new ComplementOpr(boost::proto::eval(e, *this));
  }

  template <typename Expr>
  result_type operator()(boost::proto::tag::logical_not, Expr const& e) {
    return new NotOpr(boost::proto::eval(e, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::plus, Expr1 const& e1, Expr2 const& e2) {
    return new PlusOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::minus, Expr1 const& e1, Expr2 const& e2) {
    return new MinusOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::modulus, Expr1 const& e1, Expr2 const& e2) {
    return new ModuloOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::divides, Expr1 const& e1, Expr2 const& e2) {
    return new DevideOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::shift_left, Expr1 const& e1, Expr2 const& e2) {
    return new ShiftLeftOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::shift_right, Expr1 const& e1, Expr2 const& e2) {
    return new ShiftRightOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::multiplies, Expr1 const& e1, Expr2 const& e2) {
    return new MultipliesOpr(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Integer>
  result_type operator()(boost::proto::tag::terminal t, read_ref_tag<Integer> const& ref) {
    std::map<int, result_type>::const_iterator ite = variables_.find(ref.id);

    if (ite != variables_.end()) {
      return ite->second;
    } else {
      result_type var = new_var(static_cast<var_tag<Integer> >(ref));

      boost::shared_ptr<crave::ReferenceExpression> refExpr(new ReferenceExpressionImpl<Integer>(ref.ref, var));
      read_references_.push_back(std::make_pair(ref.id, refExpr));

      return var;
    }
  }

  template <typename Integer>
  static result_type new_value(Integer const& i) {
    unsigned width = bitsize_traits<Integer>::value;
    bool sign = crave::is_signed<Integer>::value;
    if (i >= 0) {
      for (int j = 1; j < width; ++j)
        if ((i >> j) == 0) return new Constant(i, j, false);
    }
    return new Constant(i, width, sign);
  }

  template <typename Integer>
  result_type operator()(boost::proto::tag::terminal, Integer const& i) {
    return new_value<Integer>(i);
  }

  template <typename Integer, typename CollectionTerm>
  result_type operator()(boost::proto::tag::function, boost::proto::terminal<operator_inside>::type const& tag,
                         WriteReference<Integer> const& var_term, CollectionTerm const& c) {

    typedef typename boost::proto::result_of::value<CollectionTerm>::type Collection;
    typedef typename boost::range_value<Collection>::type CollectionEntry;

    unsigned width = bitsize_traits<Integer>::value;
    bool sign = crave::is_signed<Integer>::value;

    std::set<Constant> constants;
    distribution<Integer> dist;
    BOOST_FOREACH(CollectionEntry const & i, boost::proto::value(c)) {
      constants.insert(Constant(i, width, sign));
      dist(weighted_value<Integer>(i, 1));
    }

    unsigned id = new_var_id();
    result_type tmp_var = new_var(id, width, sign);
    boost::shared_ptr<crave::ReferenceExpression> ref_expr(new DistReferenceExpr<Integer>(dist, tmp_var));
    dist_references_.push_back(std::make_pair(id, ref_expr));

    result_type val_equal_tmp(new EqualOpr(boost::proto::eval(var_term, *this), tmp_var));
    result_type tmp_inside(new Inside(tmp_var, constants));
    return new LogicalAndOpr(val_equal_tmp, tmp_inside);
  }

  template <typename Integer, typename Expr>
  result_type operator()(boost::proto::tag::function, boost::proto::terminal<operator_dist>::type const& tag,
                         WriteReference<Integer> const& var_term, Expr const& dist_expr) {
    result_type node = boost::proto::eval(dist_expr, *this);
    if (boost::dynamic_pointer_cast<distribution<Integer> >(node) != 0) {

      distribution<Integer>& dist = *(boost::dynamic_pointer_cast<distribution<Integer> >(node));

      unsigned width = bitsize_traits<Integer>::value;
      bool sign = crave::is_signed<Integer>::value;

      unsigned id = new_var_id();
      result_type tmp_var = new_var(id, width, sign);
      boost::shared_ptr<crave::ReferenceExpression> ref_expr(new DistReferenceExpr<Integer>(dist, tmp_var));
      dist_references_.push_back(std::make_pair(id, ref_expr));

      result_type in_ranges;
      BOOST_FOREACH(weighted_range<Integer> const & r, dist.ranges()) {
        result_type left(new Constant(r.left, width, sign));
        result_type right(new Constant(r.right, width, sign));
        result_type left_cond(new LessEqualOpr(left, tmp_var));
        result_type right_cond(new LessEqualOpr(tmp_var, right));
        result_type in_range(new LogicalAndOpr(left_cond, right_cond));
        result_type tmp(in_ranges != 0 ? new LogicalOrOpr(in_ranges, in_range) : in_range);
        in_ranges = tmp;
      }

      result_type var_equal_tmp(new EqualOpr(boost::proto::eval(var_term, *this), tmp_var));
      return dist.ranges().size() > 0 ? new LogicalAndOpr(var_equal_tmp, in_ranges) : var_equal_tmp;
    } else
      throw std::runtime_error("Distribution and variable type not compatible");
  }

  template <typename Integer>
  result_type operator()(boost::proto::tag::terminal, distribution<Integer> const& dist) {
    return new distribution<Integer>(dist);
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::function, boost::proto::terminal<operator_foreach>::type const& tag,
                         Expr1 const& e1, Expr2 const& e2) {
    return new ForEach(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Expr>
  result_type operator()(boost::proto::tag::function, boost::proto::terminal<operator_unique>::type const& tag,
                         Expr const& e) {
    return new Unique(boost::proto::eval(e, *this));
  }

  template <typename Expr1, typename Expr2>
  result_type operator()(boost::proto::tag::subscript, Expr1 const& e1, Expr2 const& e2) {
    return new VectorAccess(boost::proto::eval(e1, *this), boost::proto::eval(e2, *this));
  }

  template <typename Integer1, typename Integer2, typename Integer3>
  result_type operator()(boost::proto::tag::function, boost::proto::terminal<operator_bitslice>::type const& tag,
                         Integer1 const& r, Integer2 const& l, WriteReference<Integer3> const& var_term) {
    int rb = boost::proto::value(r);
    int lb = boost::proto::value(l);
    if ((rb < lb) || (rb >= bitsize_traits<Integer3>::value)) throw std::runtime_error("Invalid range of bitslice");
    return new Bitslice(boost::proto::eval(var_term, *this), rb, lb);
  }

 private:
  std::map<int, result_type>& variables_;
  std::map<int, result_type>& vector_variables_;
  std::vector<ReadRefPair>& read_references_;
  std::vector<WriteRefPair>& write_references_;
  std::vector<ReadRefPair>& dist_references_;
};
// Context

}  // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab
