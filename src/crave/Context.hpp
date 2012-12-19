#pragma once

#include "bitsize_traits.hpp"
#include "Constraint.hpp"
#include "VectorConstraint.hpp"
#include "ExpressionTraits.hpp"

#include <boost/proto/core.hpp>
#include <boost/proto/debug.hpp>
#include <boost/proto/context/callable.hpp>
#include <boost/proto/context/null.hpp>

#include <map>


namespace crave {
  class expression;

  struct Context
    : proto::callable_context< Context, proto::null_context >
  {

    public:
      typedef expression* result_type; // FIXME: boost::intrusive_ptr<expression>

    template<typename value_type>
    result_type operator() (proto::tag::terminal, var_tag<value_type> const & tag ) {
      std::map<int, result_type>::const_iterator ite
        = _variables.find(tag.id);
      if ( ite != _variables.end() ) {
          //FIXME: return evaluate(solver, ite->second);
      } else {
        //std::cout << "creating " << buf.str() << ", width= " << width << std::endl;
        // FIXME: Generate IR // qf_bv::bitvector bv = qf_bv::new_bitvector(width);
        // FIXME: Generate IR // _variables.insert( std::make_pair(tag.id, bv) );
        //FIXME: return evaluate(solver, bv);
      }
    }

    template<typename value_type>
    result_type operator() (proto::tag::terminal t, write_ref_tag<value_type> const & ref ) {
      std::map<int, result_type>::const_iterator ite
        = _variables.find(ref.id);
      if ( ite != _variables.end() ) {
        //FIXME: return evaluate(solver, ite->second);
      } else {
        result_type var = (*this)(t, static_cast<var_tag<value_type> >(ref));
        // FIXME: implement ne way to handle write references
        // boost::function0<void> f = writeReference<value_type>(ref.ref, *this, ref.id);
        // _post_hook.push_back(f);
        return var;
      }
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::equal_to, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR // return evaluate(solver, preds::equal( // FIXME: create new expression for equal
      // FIXME: Generate IR //     proto::eval( e1, *this ) // FIXME: recursion is correct
      // FIXME: Generate IR //   , proto::eval( e2, *this ) // FIXME: recursion is correct
      // FIXME: Generate IR // ));
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::not_equal_to, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR // return evaluate(solver, preds::nequal(
      // FIXME: Generate IR //     proto::eval( e1, *this )
      // FIXME: Generate IR //   , proto::eval( e2, *this )
      // FIXME: Generate IR // ));
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::less_equal, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR //
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::less, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR //
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::greater, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR //
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::greater_equal, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR //
    }


    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::logical_and, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR //
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::logical_or, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR //
    }

    template< typename Expr>
    result_type operator() (proto::tag::logical_not, Expr const & e) {
      // FIXME: Generate IR //
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::plus, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR //
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::minus, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR //
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::modulus, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR //
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::divides, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR //
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::shift_left, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR //
    }


    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::multiplies, Expr1 const & e1, Expr2 const &  e2) {
      // FIXME: Generate IR //
    }

    template< typename Integral, typename Expr>
    result_type operator() (extend_tag, Integral const & by_width, Expr const &  e) {
      // FIXME: Generate IR //
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::assign, Expr1 const & e1, Expr2 const &  e2) {
      //std::cout << "eval assign" << std::endl;
      result_type expr1, expr2;
      expr1 = proto::eval( e1, *this );
      expr2 = proto::eval( e2, *this );

      //FIXME: Generate IR // boost::function0<result_type> f = getID( evaluate(solver,
      //FIXME: Generate IR //     preds::equal(expr1, expr2)) );
      //FIXME: Generate IR // _lazy[proto::value(e1).id] = f;


      // TODO: maybe new syntax for this feature: gen.assign( x(), 3)
      // FIXME: Generate IR // return evaluate( solver, preds::True);
    }

    result_type operator() (proto::tag::terminal t, randomize_tag const & tag )
    {
      // FIXME: reverts previous function
      //_lazy.erase(tag.id);
      //return evaluate( solver, preds::True);
    }

    template< typename Integer>
    result_type operator() (proto::tag::terminal t, read_ref_tag<Integer> const & ref) {
      std::map<int, result_type>::const_iterator ite
        = _variables.find(ref.id);
      if ( ite != _variables.end() ) {
        //FIXME: return evaluate(solver, ite->second);
      } else {
        result_type var = (*this)(t, static_cast<var_tag<Integer> >(ref));
        // FIXME: implement new way to handle references //
        //boost::function0<result_type> f = getLazyReference<Integer>(ref.ref, solver, var);
        //_lazy[ref.id] = f;

        return var;
      }
    }

    template< typename Integer>
    result_type operator() (proto::tag::terminal, Integer const & i) {
      // FIXME: Generate IR //
    }

    template< typename Value, typename CollectionTerm>
    result_type operator() (proto::tag::function,
       boost::proto::terminal<operator_inside>::type const & tag,
	Value const & value, CollectionTerm const & c)
    {
      // FIXME: Generate IR //
    }

    template<typename Var, typename Value>
    result_type
    operator() (proto::tag::function,
       boost::proto::terminal<operator_dist>::type const & tag,
        Var const & var_term, Value const & probability) {
      // FIXME: implement new way to specify distrubutions //
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::subscript, Expr1 const & e1, Expr2 const &  e2) {
      // e1 should be a terminal vector_tag
      BOOST_STATIC_ASSERT((
        boost::is_same<
          typename proto::tag_of<Expr1>::type,
          proto::tag::terminal
        >::value
      ));
      // FIXME: Generate IR //
    }

    private:
      std::map<int, result_type> _variables; // result_type = expression* / variable_expression*
  }; // Context

} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

