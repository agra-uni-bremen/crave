#pragma once

#include "bitsize_traits.hpp"
#include "Constraint.hpp"

#include <boost/proto/core.hpp>
#include <boost/proto/debug.hpp>
#include <boost/proto/context/callable.hpp>
#include <boost/proto/context/null.hpp>
#include <boost/function.hpp>

#include <MetaSolver.hpp>
#include <logic/QF_BV.hpp>
#include <tools/Convert_QF_BV.hpp>

#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

namespace platzhalter {
  namespace proto = boost::proto;

  struct metaSMT_Context
    : proto::callable_context< metaSMT_Context, proto::null_context >
  {

//    metaSMT_Context(std::string const & solvername="SolverBoolector") 
    metaSMT_Context(std::string const & solvername="SolverSWORD") 
    : _solver(new metaSMT::MetaSolver())
    , _logic(_solver->logic<metaSMT::QF_BV>(solvername))
    , _soft(_logic->bit1())
    {}

    ~metaSMT_Context() {
      delete _solver;
    }

    typedef metaSMT::Expression result_type;


    template<typename value_type>
    result_type operator() (proto::tag::terminal, var_tag<value_type> const & tag ) {
      std::map<int, result_type>::const_iterator ite
        = _variables.find(tag.id);
      if ( ite != _variables.end() ) {
        return ite->second;
      } else {
        std::ostringstream buf;
        buf << "var_" << tag.id;
        unsigned width=bitsize_traits<value_type>::nbits;
        //std::cout << "creating " << buf.str() << ", width= " << width << std::endl;
        result_type var = _logic->bitvec(buf.str(), width);
        _variables.insert( std::make_pair(tag.id, var) );
        return var;
      }
    }

    template<typename value_type>
    struct writeReference{
      writeReference(value_type & i, metaSMT_Context & ctx, unsigned id) 
      : _i (i), _ctx(ctx), _id(id)
      {}
      void operator() () {
        _ctx.read(_i, _id);
      }
      value_type & _i;
      metaSMT_Context & _ctx;
      unsigned _id;
    };

    template<typename value_type>
    result_type operator() (proto::tag::terminal t, write_ref_tag<value_type> const & ref ) {
      std::map<int, result_type>::const_iterator ite
        = _variables.find(ref.id);
      if ( ite != _variables.end() ) {
        return ite->second;
      } else {
        result_type var = (*this)(t, static_cast<var_tag<value_type> >(ref));
        boost::function0<void> f = writeReference<value_type>(ref.ref, *this, ref.id);
        _post_hook.push_back(f);
        return var;
      }
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::equal_to, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->equal(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::not_equal_to, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->nequal(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::less_equal, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->bvule(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::less, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->bvult(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::greater, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->bvugt(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }
    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::greater_equal, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->bvuge(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::logical_and, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->bvand(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::logical_or, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->bvor(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::plus, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->bvadd(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::minus, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->bvsub(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::modulus, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->bvurem(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::divides, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->bvudiv(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::shift_left, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->bvshl(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }


    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::multiplies, Expr1 const & e1, Expr2 const &  e2) {
      return _logic->bvmul(
          proto::eval( e1, *this )
        , proto::eval( e2, *this )
      );
    }

    template< typename Expr1, typename Expr2>
    result_type operator() (proto::tag::assign, Expr1 const & e1, Expr2 const &  e2) {
      //std::cout << "eval assign" << std::endl;
      result_type expr1, expr2;
      expr1 = proto::eval( e1, *this );
      expr2 = proto::eval( e2, *this );

      boost::function0<result_type> f = getID( _logic->equal(expr1, expr2) );
      _lazy[proto::value(e1).id] = f;
      
      return _logic->bit1();
    }

    result_type operator() (proto::tag::terminal t, randomize_tag const & tag )
    {
      _lazy.erase(tag.id);
      return _logic->bit1();
    }

    struct getID {
      getID( result_type id) : id(id) {}
      result_type id;
      result_type operator() () { return id; }
    };

    template<typename Integer>
    struct getLazyReference{
      getLazyReference(Integer const & i, metaSMT::QF_BV* logic, result_type var) 
      : _i (i), _logic(logic), _var(var)
      {}
      result_type operator() () {
        return _logic->equal(_var, _logic->bvuint(_i, bitsize_traits<Integer>::nbits) );
      }
      Integer const & _i;
      metaSMT::QF_BV* _logic;
      result_type _var;
    };

    template< typename Integer>
    result_type operator() (proto::tag::terminal t, read_ref_tag<Integer> const & ref) {
      unsigned width=bitsize_traits<Integer>::nbits;

      std::map<int, result_type>::const_iterator ite
        = _variables.find(ref.id);
      if ( ite != _variables.end() ) {
        return ite->second;
      } else {
        result_type var = (*this)(t, static_cast<var_tag<Integer> >(ref));
        boost::function0<result_type> f = getLazyReference<Integer>(ref.ref, _logic, var);
        _lazy[ref.id] = f;

        return var;
      }
    }

    template< typename Integer>
    result_type operator() (proto::tag::terminal, Integer const & i) {
      unsigned width=bitsize_traits<Integer>::nbits;
//      std::cout << "creating int " << i << ", width= " << width << std::endl;
      return _logic->bvuint(i, width);
    }

    template<typename Expr>
    void assertion (Expr e) {
      check(e);
      _solver->addAssertion( proto::eval(e, *this) );
    }

    template<typename Expr>
    void assertion (std::string group, Expr e) {
      check(e);

      result_type var;
      if (_group_variables.find(group) != _group_variables.end() ) 
        var = _group_variables[group];
      else {
        var = _logic->bitvec(group, 1);
        _group_variables.insert( std::make_pair(group, var) );
      }

      _solver->addAssertion( _logic->implies( var, proto::eval(e, *this) ) );
    }

    bool enable_group(std::string group) {
      if (_group_variables.find(group) == _group_variables.end()) return false;
      _disabled_groups.erase(group);
      return true;
    }

    bool disable_group(std::string group) {
      if (_group_variables.find(group) == _group_variables.end()) return false;
      _disabled_groups.insert(group);
      return true;
    }

    template<typename Expr>
    void soft_assertion (Expr e) {
      check(e);
      _soft = _logic->bvand(_soft, proto::eval(e, *this) );
    }

    void pre_solve() {       
      for (
        std::map<std::string, result_type >::const_iterator 
        ite = _group_variables.begin();
        ite != _group_variables.end(); ++ite) {
        if (_disabled_groups.find(ite->first) == _disabled_groups.end())
          _solver->addAssumption( _logic->equal(ite->second, _logic->bit1()) );
        else
          _solver->addAssumption( _logic->equal(ite->second, _logic->bit0()) );
      }

      for (
        std::map<unsigned, boost::function0<result_type> > ::const_iterator 
        ite = _lazy.begin();
        ite != _lazy.end(); ++ite) {
        _solver->addAssumption( (ite->second)() );
      }
    }

    void post_solve(bool sat) {
      if (sat) for (uint i = 0; i < _post_hook.size(); i++) _post_hook[i]();
    }

    bool solve (bool soft=true) {
      pre_solve();
      _solver->addAssumption(_soft);
      // if soft constraint satisfiable
      if ( _solver->solve() ) {
         post_solve(true);
         return true;
      }
      else {
        // if not satisfiable
        pre_solve();
        bool ret = _solver->solve( );
        post_solve(ret);
        return ret;
      }
    }

    template<typename T>
    T read ( Variable<T> const & v) {
      T ret;
      read(ret, v.id());
      return ret;
    }

    template<typename T>
    void read ( T & v, unsigned id) {
      std::map<int, result_type>::const_iterator ite
        = _variables.find(id);
      assert ( ite != _variables.end() );
      std::string solution = _solver->readAssignment(ite->second);
      v = metaSMT::bits2Cu(solution);
    }

    protected:
      std::map<int, result_type> _variables;
      metaSMT::MetaSolver* _solver;
      metaSMT::QF_BV*      _logic;
    private:
      result_type          _soft;
      std::map<std::string, result_type> _group_variables;
      std::set<std::string> _disabled_groups;
      std::map<unsigned, boost::function0<result_type> > _lazy;
      std::vector<boost::function0<void> > _post_hook;

  }; // metaSMT_Context

  typedef metaSMT_Context Context;

  Context get_context(std::string const & solvername="") {
    return metaSMT_Context(solvername);
  }

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

    Generator() {};

    template<typename Expr>
    Generator(Expr expr)
    {
      (*this)(expr);
    }

    template<typename Expr>
    Generator<ContextT> & operator() (Expr expr)
    {
//      display_expr(expr);
      ctx.assertion(expr);
      return *this;
    }

    template<typename Expr>
    Generator<ContextT> & operator() (std::string group, Expr expr)
    {
      ctx.assertion(group, expr);
      return *this;
    }

    bool enable_group(std::string group) { return ctx.enable_group(group); }
    bool disable_group(std::string group) { return ctx.disable_group(group); }

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
      ctx.soft_assertion( e );
    }

    /**
     * generate soft constraints
     **/
    Soft_Generator<ContextT> operator() ( Soft const & ) {
      return Soft_Generator<ContextT> (*this);
    }

    bool next() {
      return ctx.solve();
    }

    /**
     * read variable "var"
     **/
    template<typename T>
    T operator[] (Variable<T> const & var) { return ctx.read(var); };

    private:
      ContextT ctx;
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
