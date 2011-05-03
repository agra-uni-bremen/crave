#pragma once

#include "bitsize_traits.hpp"
#include "Constraint.hpp"
#include "VectorConstraint.hpp"
#include "ExpressionTraits.hpp"

#include <boost/proto/core.hpp>
#include <boost/proto/debug.hpp>
#include <boost/proto/context/callable.hpp>
#include <boost/proto/context/null.hpp>
#include <boost/function.hpp>

//#include <MetaSolver.hpp>
//#include <logic/QF_BV.hpp>
//#include <tools/Convert_QF_BV.hpp>
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/backend/SWORD_Context.hpp>
#include <metaSMT/frontend/QF_BV.hpp>

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


namespace platzhalter {

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
    metaSMT_Context_base(std::string const & solvername) 
    : solver()
    , _soft( evaluate(solver, preds::True) )
    {}

    ~metaSMT_Context_base() {
    }

    typedef metaSMT::DirectSolver_Context< metaSMT::solver::SWORD_Context > SolverType;
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
      return evaluate( solver, qf_bv::bvurem(
          proto::eval( e1, ctx() )
        , proto::eval( e2, ctx() )
      ));
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::divides, Expr1 const & e1, Expr2 const &  e2) {
      return evaluate( solver, qf_bv::bvudiv(
          proto::eval( e1, ctx() )
        , proto::eval( e2, ctx() )
      ));
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
        return evaluate( solver, preds::equal(_var, 
                  qf_bv::bvuint(_i, bitsize_traits<Integer>::value))); 
      }
      Integer const & _i;
      SolverType & solver;
      result_type _var;
    };

    template< typename Integer>
    result_type operator() (proto::tag::terminal t, read_ref_tag<Integer> const & ref) {
      unsigned width=bitsize_traits<Integer>::value;

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
      unsigned width=bitsize_traits<Integer>::value;
//      std::cout << "creating int " << i << ", width= " << width << std::endl;
      return evaluate( solver, qf_bv::bvuint(i,width) );
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

    template<typename Expr>
    void soft_assertion (Expr e) {
      //check(e);
      _soft = evaluate( solver, preds::And(_soft, proto::eval( e, ctx())));
    }

    void pre_solve() {       
      for (
        std::map<std::string, result_type >::const_iterator 
        ite = _constraint_name_variables.begin();
        ite != _constraint_name_variables.end(); ++ite) {
        if (_disabled_constraint_names.find(ite->first) == _disabled_constraint_names.end())
          assumption(solver, ite->second);
        else
          assumption(solver, preds::Not(ite->second));
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
      pre_solve();
      assumption(solver,_soft);
      // if soft constraint satisfiable
      if ( metaSMT::solve(solver) ) {
         return true;
      }
      else {
        // if not satisfiable
        pre_solve();
        bool ret =  metaSMT::solve(solver) ;
        return ret;
      }
    }

    template<typename T>
    T read ( Variable<T> const & v) {
      T ret;
      assert( read(ret, v.id()) );
      return ret;
    }

    template<typename T>
    bool read ( T & v, unsigned id) {
      std::map<int, qf_bv::bitvector>::const_iterator ite
        = _variables.find(id);
      if ( ite != _variables.end() ) {
        v = read_value(solver, ite->second);
        return true;
      }
      return false;
    }

    template<typename T>
    bool read ( T & v, vecVar & vv) {
      std::map<vecVar, qf_bv::bitvector>::const_iterator ite
        = _vector_variables.find(vv);
      if ( ite != _vector_variables.end() ) {
        v = read_value(solver, ite->second);
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
        for (uint i = 0; i < _rv.size(); i++)
          ret = evaluate( solver, preds::And(ret, evaluate( solver, preds::nequal(_var, qf_bv::bvuint(_rv[i], bitsize_traits<T>::value))))); 
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

    protected:
      inline derived_context & ctx() {  return  static_cast<derived_context&>(*this); }

      std::map<int, qf_bv::bitvector> _variables;
      std::map<vecVar, qf_bv::bitvector> _vector_variables;
      //metaSMT::MetaSolver* _solver;
      //metaSMT::QF_BV*      _logic;
      SolverType solver;

    private:
      result_type          _soft;
      std::map<std::string, result_type> _constraint_name_variables;
      std::set<std::string> _disabled_constraint_names;
      std::map<unsigned, boost::function0<result_type> > _lazy;
      std::map<vecVar, boost::function0<result_type> > _lazy_vec;
      std::vector<boost::function0<void> > _post_hook;

  }; // metaSMT_Context

  struct metaSMT_Context : metaSMT_Context_base<metaSMT_Context> {
    metaSMT_Context(std::string const & solvername) 
    : metaSMT_Context_base<metaSMT_Context>(solvername)
    { }
  };

  typedef metaSMT_Context Context;

  Context get_context(std::string const & solvername="");

  template<typename ContextT=Context>
  struct Generator;

  template< typename T=Context>
  struct Soft_Generator {
    Soft_Generator(Generator<T> & gen) : gen(gen) {}

    template< typename Expr >
    Soft_Generator<T> & operator() (Expr e);
    Generator<T> & gen;
  };

  struct Soft;

  template<typename ContextT>
  struct Generator {

    Generator() 
    : ctx(default_solver())
    {};

    template<typename Expr>
    Generator(Expr expr)
    : ctx(default_solver())
    {
      (*this)(expr);
    }

    template<typename Expr>
    Generator<ContextT> & operator() (Expr expr)
    {
//      display_expr(expr);
      ctx.assertion( FixWidth()(expr) );
      return *this;
    }

    template<typename Expr>
    Generator<ContextT> & operator() (std::string constraint_name, Expr expr)
    {
      addCstrToCtx(constraint_name, &ctx);
      ctx.assertion(constraint_name, FixWidth()(expr) );
      return *this;
    }

    void addCstrToCtx(std::string constraint_name, ContextT* context) {
      typename std::map<std::string, ContextT*>::iterator ite = ctxOfCstr.find(constraint_name);
      if (ite != ctxOfCstr.end())
        throw std::runtime_error("Constraint already exists.");
      ctxOfCstr[constraint_name] = context;
    }

    bool enable_constraint(std::string constraint_name) { 
      typename std::map<std::string, ContextT*>::iterator ite = ctxOfCstr.find(constraint_name);
      if (ite == ctxOfCstr.end()) return false;
      ite->second->enable_constraint(constraint_name);
      return true;    
    }

    bool disable_constraint(std::string constraint_name) { 
      typename std::map<std::string, ContextT*>::iterator ite = ctxOfCstr.find(constraint_name);
      if (ite == ctxOfCstr.end()) return false;
      ite->second->disable_constraint(constraint_name);
      return true;    
    }

    /**
     * generate a new assignment
     **/
    Generator<ContextT> & operator() () {
      if(! next() )
        throw std::runtime_error("Generator constraint unsatisfiable.");
      return *this;
    }

    template<typename Expr>
    Generator<ContextT> & soft( Expr e) {
      ctx.soft_assertion( FixWidth()(e) );
      return *this;
    }

    /**
     * foreach
     **/
    template<typename value_type, typename Expr>
    Generator<ContextT> & foreach(const __rand_vec<value_type> & v, const IndexVariable & i, Expr e) {
      assert(i.id() == _i.id());
      if ( vecCtx.find(v().id()) == vecCtx.end() ) {
        vecCtx.insert( std::make_pair(v().id(), new ContextT(default_solver()) ) );
      }
      vecCtx[v().id()]->assertion( FixWidth()(e) );
      return *this;
    }

    template<typename value_type, typename Expr>
    Generator<ContextT> & soft_foreach(const __rand_vec<value_type> & v, const IndexVariable & i, Expr e) {
      assert(i.id() == _i.id());
      if ( vecCtx.find(v().id()) == vecCtx.end() ) {
        vecCtx.insert( std::make_pair(v().id(), new ContextT(default_solver()) ) );
      }
      vecCtx[v().id()]->soft_assertion( FixWidth()(e) );
      return *this;
    }

    template<typename value_type, typename Expr>
    Generator<ContextT> & foreach(std::string constraint_name, const __rand_vec<value_type> & v, const IndexVariable & i, Expr e) {
      assert(i.id() == _i.id());
      if ( vecCtx.find(v().id()) == vecCtx.end() ) {
        vecCtx.insert( std::make_pair(v().id(), new ContextT(default_solver()) ) );
      }
      addCstrToCtx(constraint_name, vecCtx[v().id()]);
      vecCtx[v().id()]->assertion(constraint_name, FixWidth()(e) );
      return *this;
    }

    /**
     * unique & non_unique
     **/
    template<typename value_type>
    Generator<ContextT> & unique(const __rand_vec<value_type> & v) {
      if ( vecCtx.find(v().id()) == vecCtx.end() ) {
        vecCtx.insert( std::make_pair(v().id(), new ContextT(default_solver()) ) );
      }
      uniqueVecSet.insert(v().id());
      return *this;
    }

    template<typename value_type>
    Generator<ContextT> & non_unique(const __rand_vec<value_type> & v) {
      uniqueVecSet.erase(v().id());
      return *this;
    }

    /**
     * generate soft constraints
     **/
    Soft_Generator<ContextT> operator() ( Soft const & ) {
      return Soft_Generator<ContextT> (*this);
    }

    template<typename T> 
    bool gen_vector(__rand_vec<T>* rvp, ContextT* rvctx) {
      __rand_vec<T>& rv = *rvp;
      std::cout << "generate vector " << rv().id() << std::endl;
      unsigned int size = rv.size();
      if (!ctx.read(size, rv().size().id())) {
        std::cout << "size not specified, use current" << std::endl;
      } 
      std::cout << "size = " << size << std::endl;  

      std::vector<vecVar> vvv;
      rvctx->dump_vec_var_list(vvv);
      int _i_idx = -1;
      // check vvv
      for (unsigned int j = 0; j < vvv.size(); j++) {
        assert(vvv[j].vecId == rv().id());
        assert(vvv[j].index.bound == _i.id());
        if (vvv[j].index.val == 0) _i_idx = j;
        else assert(vvv[j].index.val < 0);
        rvctx->vec_free(vvv[j]);
      }
      assert(_i_idx != -1);

      rv.clear();
      for (unsigned int i = 0; i < size; i++) {
        // substitute the known values
        for (unsigned int j = 0; j < vvv.size(); j++) {
          int idx = i + vvv[j].index.val;
          if (0 <= idx && idx < i) {
            rvctx->vec_assign( vvv[j], rv[idx] );
          }
        }
        rvctx->assertion(_i = i);

        // unique
        if (uniqueVecSet.find(rv().id()) != uniqueVecSet.end()) rvctx->vec_unique(vvv[_i_idx], rv);      
        // solve 
        if (!rvctx->solve()) return false;
        T tmp;
        rvctx->read(tmp, vvv[_i_idx]);
        rv.push_back(tmp);
      }

      return true;
    }

#define _GEN_VEC(typename) if (!gen_vector(static_cast<__rand_vec<typename>* > (rvb), ite->second)) return false
    bool next() {
      if (!ctx.solve()) return false;
      // solve vector constraints
      for (typename std::map<int, ContextT*>::iterator ite = vecCtx.begin(); ite != vecCtx.end(); ++ite) {
        __rand_vec_base* rvb = __rand_vec_map[ite->first];
        switch (rvb->element_type()){
//          case BOOL: _GEN_VEC(bool); break;
          case INT: _GEN_VEC(int); break;
          case UINT: _GEN_VEC(unsigned int); break;
          case CHAR: _GEN_VEC(char); break;
          case UCHAR: _GEN_VEC(unsigned char); break;
          case SHORT: _GEN_VEC(short); break;
          case USHORT: _GEN_VEC(unsigned short); break;
          case LONG:  _GEN_VEC(long); break;
          case ULONG:  _GEN_VEC(unsigned long); break;
          case LLONG: _GEN_VEC(long long); break;
          case ULLONG: _GEN_VEC(unsigned long long); break;
          default:
            assert(false && "not supported yet"); 
        }
      }
      return true;
    }
#undef _GEN_VEC

    /**
     * read variable "var"
     **/
    template<typename T>
    T operator[] (Variable<T> const & var) { 
      return ctx.read(var); 
    };

    private:
      ContextT ctx;
      std::map<int, ContextT*> vecCtx;
      std::map<std::string, ContextT*> ctxOfCstr;
      std::set<int> uniqueVecSet;
  };

  template< typename T >
  template< typename Expr >
  Soft_Generator<T> &
  Soft_Generator<T>::operator() (Expr e) {
    gen.soft(e);
    return *this;
  }

  struct Soft {
    template< typename T >
    Soft_Generator<T> operator() (Generator<T> & gen) const {
      return Soft_Generator<T>( gen );
    }
  };

  const Soft soft = {};

} // namespace platzhalter
//  vim: ft=cpp:ts=2:sw=2:expandtab

