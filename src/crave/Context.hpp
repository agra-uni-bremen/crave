#pragma once

#include "bitsize_traits.hpp"
#include "Constraint.hpp"
#include "VectorConstraint.hpp"
#include "ExpressionTraits.hpp"

#include <boost/foreach.hpp>
#include <boost/proto/core.hpp>
#include <boost/proto/debug.hpp>
#include <boost/proto/context/callable.hpp>
#include <boost/proto/context/null.hpp>
#include <boost/function.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_01.hpp>
#include <metaSMT/DirectSolver_Context.hpp>
// #include <metaSMT/Priority_Context.hpp>
#include <metaSMT/UnpackFuture_Context.hpp>
// #include <metaSMT/BitBlast.hpp>
#include <metaSMT/backend/Boolector.hpp>
// #include <metaSMT/backend/CUDD_Distributed.hpp>
#include <metaSMT/frontend/QF_BV.hpp>
#include <metaSMT/support/contradiction_analysis.hpp>

#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

namespace preds = metaSMT::logic;
namespace qf_bv = metaSMT::logic::QF_BV;

using metaSMT::assumption;
using metaSMT::evaluate;
using metaSMT::solve;


namespace crave {

  extern boost::mt19937 rng;
  extern boost::function0<bool> random_bit;

  std::string default_solver();

  namespace proto = boost::proto;

  struct vectorSubscript_Context
    : proto::callable_context< vectorSubscript_Context, proto::null_context > {

    vectorSubscript_Context() {}

    typedef vecIdx result_type;

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::minus, Expr1 const & e1, Expr2 const &  e2) {
      return proto::eval( e1, *this ) - proto::eval( e2, *this );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::plus, Expr1 const & e1, Expr2 const &  e2) {
      return proto::eval( e1, *this ) + proto::eval( e2, *this );
    }

    template<typename value_type>
    result_type operator() (proto::tag::terminal, Variable<value_type> const & tag ) {
      return vecIdx(0, proto::value(tag).id);
    }

    template<typename value_type>
    result_type operator() (proto::tag::terminal, var_tag<value_type> const & tag ) {
      return vecIdx(0, tag.id);
    }

    template< typename Integer>
    result_type operator() (proto::tag::terminal, Integer const & i) {
      return vecIdx(i, 0);
    }

  };

  template< typename derived_context >
  struct metaSMT_Context_base
    : proto::callable_context< metaSMT_Context_base<derived_context>, proto::null_context >
  {
    metaSMT_Context_base()
    : solver()
    , _soft( evaluate(solver, preds::True) )
    {}

    ~metaSMT_Context_base() {
    }

    private:
    metaSMT_Context_base(metaSMT_Context_base const & );

    public:

//     typedef metaSMT::DirectSolver_Context <
//       metaSMT::UnpackFuture_Context <
//       metaSMT::BitBlast <
//       metaSMT::solver::CUDD_Distributed
//       > >
//     > SolverType1;
    typedef metaSMT::DirectSolver_Context <
      metaSMT::UnpackFuture_Context <
      metaSMT::solver::Boolector
      >
    > SolverType2;

//     typedef metaSMT::Priority_Context< SolverType1, SolverType2 > SolverType;
//     typedef  SolverType1 SolverType; // BDD Only
    typedef  SolverType2 SolverType; // SMT Only

    typedef SolverType::result_type result_type;


    template<typename value_type>
    result_type operator() (proto::tag::terminal, var_tag<value_type> const & tag ) {
      std::map<int, qf_bv::bitvector>::const_iterator ite
        = _variables.find(tag.id);
      if ( ite != _variables.end() ) {
          return evaluate(solver, ite->second);
      } else {
        std::ostringstream buf;
        buf << "var_" << tag.id;
        unsigned width=bitsize_traits<value_type>::value;
        //std::cout << "creating " << buf.str() << ", width= " << width << std::endl;
        qf_bv::bitvector bv = qf_bv::new_bitvector(width);
        _variables.insert( std::make_pair(tag.id, bv) );
        return evaluate(solver, bv);
      }
    }

    template<typename value_type>
    struct writeReference {
      writeReference(value_type & i, derived_context & ctx, unsigned id)
      : _i (i), _ctx(ctx), _id(id)
      {}
      void operator() () {
        _ctx.read(_i, _id);
      }
      value_type & _i;
      derived_context & _ctx;
      unsigned _id;
    };

    template<typename value_type>
    result_type operator() (proto::tag::terminal t, write_ref_tag<value_type> const & ref ) {
      std::map<int, qf_bv::bitvector>::const_iterator ite
        = _variables.find(ref.id);
      if ( ite != _variables.end() ) {
        return evaluate(solver, ite->second);
      } else {
        result_type var = (ctx())(t, static_cast<var_tag<value_type> >(ref));
        boost::function0<void> f = writeReference<value_type>(ref.ref, ctx(), ref.id);
        _post_hook.push_back(f);
        return var;
      }
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::equal_to, Expr1 const & e1, Expr2 const &  e2) {
      return evaluate(solver, preds::equal(
          proto::eval( e1, ctx() )
        , proto::eval( e2, ctx() )
      ));
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::not_equal_to, Expr1 const & e1, Expr2 const &  e2) {
      return evaluate(solver, preds::nequal(
          proto::eval( e1, ctx() )
        , proto::eval( e2, ctx() )
      ));
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::less_equal, Expr1 const & e1, Expr2 const &  e2) {
      bool sign1 = proto::matches< Expr1, IsSigned >::value;
      bool sign2 = proto::matches< Expr2, IsSigned >::value;

      if( sign1 && sign2 ) {
        return evaluate( solver, qf_bv::bvsle(
            proto::eval( e1, ctx() )
          , proto::eval( e2, ctx() )
        ));
      } else if ( !sign1 && !sign2 ) {
        return evaluate( solver, qf_bv::bvule(
            proto::eval( e1, ctx() )
          , proto::eval( e2, ctx() )
        ));
      } else if ( sign1 ) {
        return evaluate( solver, qf_bv::bvsle(
            qf_bv::sign_extend(1, proto::eval( e1, ctx() ))
          , qf_bv::zero_extend(1, proto::eval( e2, ctx() ))
        ));
      } else {
        return evaluate( solver, qf_bv::bvsle(
            qf_bv::zero_extend(1, proto::eval( e1, ctx() ))
          , qf_bv::sign_extend(1, proto::eval( e2, ctx() ))
        ));
      }
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::less, Expr1 const & e1, Expr2 const &  e2) {
      bool sign1 = proto::matches< Expr1, IsSigned >::value;
      bool sign2 = proto::matches< Expr2, IsSigned >::value;

      if( sign1 && sign2 ) {
        return evaluate( solver, qf_bv::bvslt(
            proto::eval( e1, ctx() )
          , proto::eval( e2, ctx() )
        ));
      } else if ( !sign1 && !sign2 ) {
        return evaluate( solver, qf_bv::bvult(
            proto::eval( e1, ctx() )
          , proto::eval( e2, ctx() )
        ));
      } else if ( sign1 ) {
        return evaluate( solver, qf_bv::bvslt(
            qf_bv::sign_extend(1, proto::eval( e1, ctx() ))
          , qf_bv::zero_extend(1, proto::eval( e2, ctx() ))
        ));
      } else {
        return evaluate( solver, qf_bv::bvslt(
            qf_bv::zero_extend(1, proto::eval( e1, ctx() ))
          , qf_bv::sign_extend(1, proto::eval( e2, ctx() ))
        ));
      }
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::greater, Expr1 const & e1, Expr2 const &  e2) {
      bool sign1 = proto::matches< Expr1, IsSigned >::value;
      bool sign2 = proto::matches< Expr2, IsSigned >::value;

      if( sign1 && sign2 ) {
        return evaluate( solver, qf_bv::bvsgt(
            proto::eval( e1, ctx() )
          , proto::eval( e2, ctx() )
        ));
      } else if ( !sign1 && !sign2 ) {
        return evaluate( solver, qf_bv::bvugt(
            proto::eval( e1, ctx() )
          , proto::eval( e2, ctx() )
        ));
      } else if ( sign1 ) {
        return evaluate( solver, qf_bv::bvsgt(
            qf_bv::sign_extend(1, proto::eval( e1, ctx() ))
          , qf_bv::zero_extend(1, proto::eval( e2, ctx() ))
        ));
      } else {
        return evaluate( solver, qf_bv::bvsgt(
            qf_bv::zero_extend(1, proto::eval( e1, ctx() ))
          , qf_bv::sign_extend(1, proto::eval( e2, ctx() ))
        ));
      }
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::greater_equal, Expr1 const & e1, Expr2 const &  e2) {
      bool sign1 = proto::matches< Expr1, IsSigned >::value;
      bool sign2 = proto::matches< Expr2, IsSigned >::value;

      if( sign1 && sign2 ) {
        return evaluate( solver, qf_bv::bvsge(
            proto::eval( e1, ctx() )
          , proto::eval( e2, ctx() )
        ));
      } else if ( !sign1 && !sign2 ) {
        return evaluate( solver, qf_bv::bvuge(
            proto::eval( e1, ctx() )
          , proto::eval( e2, ctx() )
        ));
      } else if ( sign1 ) {
        return evaluate( solver, qf_bv::bvsge(
            qf_bv::sign_extend(1, proto::eval( e1, ctx() ))
          , qf_bv::zero_extend(1, proto::eval( e2, ctx() ))
        ));
      } else {
        return evaluate( solver, qf_bv::bvsge(
            qf_bv::zero_extend(1, proto::eval( e1, ctx() ))
          , qf_bv::sign_extend(1, proto::eval( e2, ctx() ))
        ));
      }
    }


    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::logical_and, Expr1 const & e1, Expr2 const &  e2) {
      return evaluate( solver, preds::And(
          proto::eval( e1, ctx() )
        , proto::eval( e2, ctx() )
      ));
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::logical_or, Expr1 const & e1, Expr2 const &  e2) {
      return evaluate( solver, preds::Or(
          proto::eval( e1, ctx() )
        , proto::eval( e2, ctx() )
      ));
    }

    template< typename Expr>
    result_type operator() (proto::tag::logical_not, Expr const & e) {
      return evaluate( solver, preds::Not( proto::eval( e, ctx() ) ) );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::plus, Expr1 const & e1, Expr2 const &  e2) {
      return evaluate( solver, qf_bv::bvadd(
          proto::eval( e1, ctx() )
        , proto::eval( e2, ctx() )
      ));
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::minus, Expr1 const & e1, Expr2 const &  e2) {
      return evaluate( solver, qf_bv::bvsub(
          proto::eval( e1, ctx() )
        , proto::eval( e2, ctx() )
      ));
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::modulus, Expr1 const & e1, Expr2 const &  e2) {
    // assume e2 >= 0
      bool sign1 = proto::matches< Expr1, IsSigned >::value;
      if( sign1 ) {
        return evaluate( solver, qf_bv::bvsrem(
            proto::eval( e1, ctx() )
          , proto::eval( e2, ctx() )
        ));
      } else {
        return evaluate( solver, qf_bv::bvurem(
            proto::eval( e1, ctx() )
          , proto::eval( e2, ctx() )
        ));
      }
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::divides, Expr1 const & e1, Expr2 const &  e2) {
      bool sign1 = proto::matches< Expr1, IsSigned >::value;
      bool sign2 = proto::matches< Expr2, IsSigned >::value;
      if( sign1 && sign2 ) {
        return evaluate( solver, qf_bv::bvsdiv(
            proto::eval( e1, ctx() )
          , proto::eval( e2, ctx() )
        ));
      } else if ( !sign1 && !sign2 ) {
        return evaluate( solver, qf_bv::bvudiv(
            proto::eval( e1, ctx() )
          , proto::eval( e2, ctx() )
        ));
      } else if ( sign1 ) {
        return evaluate( solver, qf_bv::bvsdiv(
            qf_bv::sign_extend(1, proto::eval( e1, ctx() ))
          , qf_bv::zero_extend(1, proto::eval( e2, ctx() ))
        ));
      } else {
        return evaluate( solver, qf_bv::bvsdiv(
            qf_bv::zero_extend(1, proto::eval( e1, ctx() ))
          , qf_bv::sign_extend(1, proto::eval( e2, ctx() ))
        ));
      }
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::shift_left, Expr1 const & e1, Expr2 const &  e2) {
      return evaluate( solver, qf_bv::bvshl(
          proto::eval( e1, ctx() )
        , proto::eval( e2, ctx() )
      ));
    }


    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::multiplies, Expr1 const & e1, Expr2 const &  e2) {
      return evaluate( solver, qf_bv::bvmul(
          proto::eval( e1, ctx() )
        , proto::eval( e2, ctx() )
      ));
    }

    template< typename Integral, typename Expr>
    result_type operator() (extend_tag, Integral const & by_width, Expr const &  e) {
      bool sign = proto::matches< Expr, IsSigned >::value;
      if (sign)
        return evaluate( solver,
          qf_bv::sign_extend( proto::value(by_width).value,
            proto::eval( e, ctx() )
        ));
      else
        return evaluate( solver,
          qf_bv::zero_extend( proto::value(by_width).value,
            proto::eval( e, ctx() )
        ));
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::assign, Expr1 const & e1, Expr2 const &  e2) {
      //std::cout << "eval assign" << std::endl;
      result_type expr1, expr2;
      expr1 = proto::eval( e1, ctx() );
      expr2 = proto::eval( e2, ctx() );

      boost::function0<result_type> f = getID( evaluate(solver,
          preds::equal(expr1, expr2)) );
      _lazy[proto::value(e1).id] = f;


      return evaluate( solver, preds::True);
    }

    result_type operator() (proto::tag::terminal t, randomize_tag const & tag )
    {
      _lazy.erase(tag.id);
      return evaluate( solver, preds::True);
    }

    struct getID {
      getID( result_type id) : id(id) {}
      result_type id;
      result_type operator() () { return id; }
    };

    template<typename Integer>
    struct getLazyReference{
      getLazyReference(Integer const & i, SolverType & solver, result_type var)
      : _i (i), solver(solver), _var(var)
      {}
      result_type operator() () {
        if (boost::is_signed<Integer>::value)
          return evaluate( solver, preds::equal(_var, qf_bv::bvsint(_i, bitsize_traits<Integer>::value) ));
        else
          return evaluate( solver, preds::equal(_var, qf_bv::bvuint(_i, bitsize_traits<Integer>::value) ));
      }
      Integer const & _i;
      SolverType & solver;
      result_type _var;
    };

    template< typename Integer>
    result_type operator() (proto::tag::terminal t, read_ref_tag<Integer> const & ref) {
      std::map<int, qf_bv::bitvector>::const_iterator ite
        = _variables.find(ref.id);
      if ( ite != _variables.end() ) {
        return evaluate(solver, ite->second);
      } else {
        result_type var = (ctx())(t, static_cast<var_tag<Integer> >(ref));
        boost::function0<result_type> f = getLazyReference<Integer>(ref.ref, solver, var);
        _lazy[ref.id] = f;

        return var;
      }
    }

    template< typename Integer>
    result_type operator() (proto::tag::terminal, Integer const & i) {
      if (boost::is_signed<Integer>::value)
        return evaluate( solver, qf_bv::bvsint(i, bitsize_traits<Integer>::value) );
      else
        return evaluate( solver, qf_bv::bvuint(i, bitsize_traits<Integer>::value) );
    }

    template< typename Value, typename CollectionTerm>
    result_type operator() (proto::tag::function,
       boost::proto::terminal<operator_inside>::type const & tag,
	Value const & value, CollectionTerm const & c)
    {
      	typedef typename proto::result_of::value< CollectionTerm >::type  Collection;
	result_type ret = evaluate( solver, False);
        BOOST_FOREACH(typename boost::range_value<Collection>::type const & i, boost::proto::value(c))
        {
   	 ret = evaluate( solver, preds::Or( ret, proto::eval( value == i, ctx() ) ) );
        }
	return ret;
    }

    struct getDistReference{
      getDistReference( SolverType & solver, result_type var, double probability)
      :  solver(solver), _var(var), _probability(probability)
      {}
      result_type operator() () {
        bool result = boost::uniform_01<double>()(rng) <= _probability;
      	return evaluate( solver, preds::equal(_var, qf_bv::bvuint(result, 1)) );
      }
      SolverType & solver;
      result_type _var;
      double _probability;
    };

    template<typename Var, typename Value>
    result_type
    operator() (proto::tag::function,
       boost::proto::terminal<operator_dist>::type const & tag,
        Var const & var_term, Value const & probability) {

	//qf_bv::bitvector bv = qf_bv::new_bitvector(1);
//	_variables.insert(std::make_pair(boost::proto::value(var_term).id, proto::eval(var_term,ctx())));
	result_type res = boost::proto::eval(var_term, ctx());
	double prob = boost::proto::value(probability);
	boost::function0<result_type> f =  getDistReference(solver, res, prob);
	//   std::cout << boost::proto::value(var_term).id << std::endl;
        _lazy[boost::proto::value(var_term).id] = f;

           return evaluate(solver, preds::True);
	   //res;
	   //proto::eval(var_term,ctx()));
    	  //return proto::eval(bv, ctx());
    }

    template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    result_type
    operator() (Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4)
    {
        std::cout << "DEBUG: \n";
	//std::cout << "    "<< typeid(a1).name() << "\n";
	std::cout << "    "<< typeid(a2).name() << "\n";
	//std::cout << "    "<< typeid(a3).name() << "\n";
	//std::cout << "    "<< typeid(a4).name() << "\n";

        typedef Constraint<boost::proto::terminal<operator_dist>::proto_grammar > Tag;
	Tag tag;
	std::cout << "tag: " << typeid(tag).name() << "\n";
	std::cout << "same: " << boost::is_same<Tag, Arg2>::value << "\n";

	 return evaluate( solver, False);
    	 //return proto::eval( value, ctx() );
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

      vectorSubscript_Context vsctx;
      vecVar vv(proto::value(e1).id, proto::eval(e2, vsctx));

      std::map<vecVar, qf_bv::bitvector>::const_iterator ite = _vector_variables.find(vv);
      if ( ite != _vector_variables.end() ) {
        return evaluate(solver, ite->second);
      } else {
        std::ostringstream buf;
        buf << vv;
        typedef typename proto::result_of::value<Expr1>::type value_type;
        unsigned width=bitsize_traits<value_type >::value;

        qf_bv::bitvector bv = qf_bv::new_bitvector(width);
        _vector_variables.insert( std::make_pair(vv, bv) );
        return evaluate(solver, bv);
      }
    }

    template<typename Expr>
    void assertion (Expr e) {
      //check(e);
      metaSMT::assertion(solver, proto::eval(e, ctx()) );
    }

    template<typename Expr>
    void assertion (std::string constraint_name, Expr e) {
      //check(e);

      result_type var;
      assert (_constraint_name_variables.find(constraint_name) == _constraint_name_variables.end() );

      var = evaluate(solver, preds::new_variable());
      _constraint_name_variables.insert( std::make_pair(constraint_name, var) );
      metaSMT::assertion(solver, preds::implies( var, proto::eval(e, ctx()) ) );
    }

    void enable_constraint(std::string constraint_name) {
      assert (_constraint_name_variables.find(constraint_name) != _constraint_name_variables.end());
      _disabled_constraint_names.erase(constraint_name);
    }

    void disable_constraint(std::string constraint_name) {
      assert (_constraint_name_variables.find(constraint_name) != _constraint_name_variables.end());
      _disabled_constraint_names.insert(constraint_name);
    }

    bool is_constraint_enabled(std::string constraint_name) {
      assert (_constraint_name_variables.find(constraint_name) != _constraint_name_variables.end());
      return _disabled_constraint_names.find(constraint_name) == _disabled_constraint_names.end();
    }

    template<typename Expr>
    void soft_assertion (Expr e) {
      //check(e);
      _soft = evaluate( solver, preds::And(_soft, proto::eval( e, ctx())));
    }

    void add_pre_hook(boost::function0<bool> f) {
      _pre_hook.push_back(f);
    }

    bool pre_solve() {

      for (uint i = 0; i < _pre_hook.size(); ++i) if(!_pre_hook[i]()) return false;

      for (
        std::map<std::string, result_type >::const_iterator
        ite = _constraint_name_variables.begin();
        ite != _constraint_name_variables.end(); ++ite) {
        if (_disabled_constraint_names.find(ite->first) == _disabled_constraint_names.end())
          assumption(solver, ite->second);
      }

      for (
        std::map<unsigned, boost::function0<result_type> > ::const_iterator
        ite = _lazy.begin();
        ite != _lazy.end(); ++ite) {
        assumption(solver, (ite->second)() );
      }

      for (
        std::map<vecVar, boost::function0<result_type> > ::const_iterator
        ite = _lazy_vec.begin();
        ite != _lazy_vec.end(); ++ite) {
        assumption(solver, (ite->second)() );
      }

      return true;
    }

    void post_solve(bool sat) {
      if (sat) for (uint i = 0; i < _post_hook.size(); i++) _post_hook[i]();
    }

    bool solve () {
      bool const ret = ctx().do_solve();
      post_solve(ret );
      return ret;
    }

    bool do_solve () {
      const unsigned limit = 0;
      // first run is for check if soft constrains are satisfiable
      // second run will be executed if softconstrains aren't satisfiable
      for ( unsigned run = 0; run < 2; ++run ) {

        if ( !pre_solve() ) return false;
        if ( run == 0 ) assumption(solver, _soft);

        if ( metaSMT::solve(solver) ) {
          for ( unsigned i = 0; i < limit; ++i ) {

            if ( !pre_solve() ) return false;
            if ( run == 0 ) assumption(solver, _soft);
            assign_random_bits();

            if ( metaSMT::solve(solver) ) return true;

          }  // end for

          if ( !pre_solve() ) return false;
          if ( run == 0 ) assumption(solver, _soft);
          return metaSMT::solve(solver);
        }
      }
      return false;
   }


    std::vector<std::vector<std::string> > analyse_contradiction() {
      std::map<unsigned, result_type> s;
      std::vector<std::vector<std::string> > vvec_str;
      typedef std::pair<std::string, result_type> Si_Pair;
      std::vector<std::string> out;
      std::vector<std::vector<unsigned> > results;

      BOOST_FOREACH(Si_Pair si, _constraint_name_variables)
      {
        s.insert( std::make_pair(s.size(), si.second) );
        out.push_back(si.first);
      }


      results = analyze_conflicts(solver, s, metaSMT::evaluate(solver,preds::True), results);

      BOOST_FOREACH(std::vector<unsigned> result, results)
      {
        std::vector<std::string> vec_str;
        BOOST_FOREACH(unsigned i, result)
        {
          vec_str.push_back(out[i]);
        }
        vvec_str.push_back(vec_str);
      }

      return vvec_str;
    }


    void assign_random_bits() {
      std::vector <result_type> bits;

      typedef std::pair<int,qf_bv::bitvector> var_pair;
      BOOST_FOREACH( var_pair const & p, _variables ) {
        if ( _lazy.find(p.first) == _lazy.end() ) {

          for(unsigned i=0; i < proto::value(p.second).width; ++i) {
            result_type tmp = evaluate(solver, qf_bv::extract(i,i, p.second));
            bits.push_back(tmp);
          }

        }
      }

      std::random_shuffle(bits.begin(),bits.end());
      int len = bits.size() > 0 ? boost::uniform_int<int>(0, bits.size() / 5)(rng) : 0;
      for(int i = 0; i < len ; ++i) {
       metaSMT::assumption(solver, preds::equal(bits[i], qf_bv::bvuint(boost::uniform_int<int>(0, 1)(rng), 1)));
      }
    }

    template<typename T>
    T read ( Variable<T> const & v) {
      T ret;
      bool success = false;
      success = read(ret, v.id());
      assert( success );
      return ret;
    }

    template<typename T>
    bool read ( T & v, unsigned id) {
      std::map<int, qf_bv::bitvector>::const_iterator ite
        = _variables.find(id);
      if ( ite != _variables.end() ) {
        AssignResult<T> assign;
        assign(v, read_value(solver, ite->second).randX(random_bit));
        return true;
      }
      return false;
    }

    template<typename T>
    bool read ( T & v, vecVar & vv) {
      std::map<vecVar, qf_bv::bitvector>::const_iterator ite
        = _vector_variables.find(vv);
      if ( ite != _vector_variables.end() ) {
        AssignResult<T> assign;
        assign(v, read_value(solver, ite->second).randX(random_bit));
        return true;
      }
      return false;
    }

    void dump_vec_var_list(std::vector<vecVar>& v) {
      for (std::map<vecVar, qf_bv::bitvector>::const_iterator ite = _vector_variables.begin(); ite != _vector_variables.end(); ++ite) {
          v.push_back(ite->first);
      }
    }

    template<typename T>
    void vec_assign (vecVar & vv, T & val) {
      boost::function0<result_type> f = getLazyReference<T>(val, solver, evaluate(solver, _vector_variables[vv]) );
      _lazy_vec[vv] = f;
    }

    void vec_free (vecVar & vv) {
      _lazy_vec.erase(vv);
    }

    template<typename T>
    struct makeUnique {
      makeUnique(__rand_vec<T> & rv, SolverType & solver, result_type var)
      : _rv(rv), solver(solver), _var(var)
      {}
      result_type operator() () {
        result_type ret = evaluate( solver, preds::True);
        for (uint i = 0; i < _rv.size(); i++) {
          if (boost::is_signed<T>::value)
            ret = evaluate( solver, preds::And(ret, evaluate( solver, preds::nequal(_var, qf_bv::bvsint(_rv[i], bitsize_traits<T>::value) ))));
          else
            ret = evaluate( solver, preds::And(ret, evaluate( solver, preds::nequal(_var, qf_bv::bvuint(_rv[i], bitsize_traits<T>::value) ))));
        }
        return ret;
      }
      __rand_vec<T> & _rv;
      SolverType & solver;
      result_type _var;
    };

    template<typename T>
    void vec_unique (vecVar & vv, __rand_vec<T> & rv) {
      boost::function0<result_type> f = makeUnique<T>(rv, solver, evaluate(solver, _vector_variables[vv]) );
      _lazy_vec[vv] = f;
    }

    void new_disjunction() { _disjunction = evaluate(solver, preds::False); }
    void end_disjunction() { metaSMT::assertion(solver, _disjunction ); }
    template<typename Expr>
    void add_to_disjunction (Expr expr) { _disjunction = evaluate( solver, preds::Or(_disjunction, proto::eval( expr, ctx() ) ) ); }
    
    bool has_read_ref() { return !(_lazy.empty() && _lazy_vec.empty()); }

    protected:
      inline derived_context & ctx() {  return  static_cast<derived_context&>(*this); }

      std::map<int, qf_bv::bitvector> _variables;
      std::map<vecVar, qf_bv::bitvector> _vector_variables;
      //metaSMT::MetaSolver* _solver;
      //metaSMT::QF_BV*      _logic;
      SolverType solver;
      result_type          _soft;
      result_type          _disjunction;

    private:
      std::map<std::string, result_type> _constraint_name_variables;
      std::set<std::string> _disabled_constraint_names;
      std::map<unsigned, boost::function0<result_type> > _lazy;
      std::map<vecVar, boost::function0<result_type> > _lazy_vec;
      std::vector<boost::function0<bool> > _pre_hook;
      std::vector<boost::function0<void> > _post_hook;

  }; // metaSMT_Context

  struct metaSMT_Context : metaSMT_Context_base<metaSMT_Context> {
    metaSMT_Context()
    : metaSMT_Context_base<metaSMT_Context>()
    { }
  };

  typedef metaSMT_Context Context;

} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

