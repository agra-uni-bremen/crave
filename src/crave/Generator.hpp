#pragma once

#include "Context.hpp"
#include "AllSAT.hpp"

#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

namespace crave {

  typedef AllSAT<50> DefaultContext;

  template<typename ContextT=DefaultContext>
  struct Generator;

  typedef Generator<> DefaultGenerator;

  template< typename T=DefaultContext>
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
    : ctx()
    {};

    template<typename Expr>
    Generator(Expr expr)
    : ctx()
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
    //  std::vector<std::vector<std::string> > r = ctx.analyse_contradiction();
      return *this;
    }

    std::vector<std::vector<std::string> > analyse_contradiction()
    {
      return ctx.analyse_contradiction();
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

    bool is_constraint_enabled(std::string constraint_name) {
      typename std::map<std::string, ContextT*>::iterator ite = ctxOfCstr.find(constraint_name);
      assert(ite != ctxOfCstr.end());
      return ite->second->is_constraint_enabled(constraint_name);
    }

    void add_pre_hook(boost::function0<bool> f) {
      ctx.add_pre_hook(f);
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
        vecCtx.insert( std::make_pair(v().id(), new ContextT() ) );
      }
      vecCtx[v().id()]->assertion( FixWidth()(e) );
      return *this;
    }

    template<typename value_type, typename Expr>
    Generator<ContextT> & soft_foreach(const __rand_vec<value_type> & v, const IndexVariable & i, Expr e) {
      assert(i.id() == _i.id());
      if ( vecCtx.find(v().id()) == vecCtx.end() ) {
        vecCtx.insert( std::make_pair(v().id(), new ContextT() ) );
      }
      vecCtx[v().id()]->soft_assertion( FixWidth()(e) );
      return *this;
    }

    template<typename value_type, typename Expr>
    Generator<ContextT> & foreach(std::string constraint_name, const __rand_vec<value_type> & v, const IndexVariable & i, Expr e) {
      assert(i.id() == _i.id());
      if ( vecCtx.find(v().id()) == vecCtx.end() ) {
        vecCtx.insert( std::make_pair(v().id(), new ContextT() ) );
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
        vecCtx.insert( std::make_pair(v().id(), new ContextT() ) );
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
          if (0 <= idx && idx < (int)i) {
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
          case BOOL: _GEN_VEC(bool); break;
          case INT: _GEN_VEC(int); break;
          case UINT: _GEN_VEC(unsigned int); break;
          case CHAR: _GEN_VEC(char); break;
          case SCHAR: _GEN_VEC(signed char); break;
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

    void new_disjunction() { ctx.new_disjunction(); }
    void end_disjunction() { ctx.end_disjunction(); }
    template<typename Expr>
    void add_to_disjunction (Expr expr) { ctx.add_to_disjunction( FixWidth()(expr) ); }

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

} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

