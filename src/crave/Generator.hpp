#pragma once

#include "Context.hpp"
#include "expression/ToDotNodeVisitor.hpp"
#include "expression/Node.hpp"

#include <boost/foreach.hpp>
#include <boost/function.hpp>

#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

namespace crave {

  struct Generator;
  typedef Generator DefaultGenerator;

  struct Soft_Generator {
    Soft_Generator(Generator & gen) : gen(gen) {}

    template< typename Expr >
    Soft_Generator & operator() (Expr e);
    Generator & gen;
  };

  struct Soft;
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
    Generator & operator() (Expr expr)
    {
      constraints_.push_back( boost::proto::eval( expr, ctx ) );
      return *this;
    }

    template<typename Expr>
    Generator & operator() (std::string constraint_name, Expr expr)
    {
      // FIXME: addCstrToCtx(constraint_name, &ctx);
      // FIXME: ctx.assertion(constraint_name, FixWidth()(expr) );
    //  std::vector<std::vector<std::string> > r = ctx.analyse_contradiction();
      return *this;
    }

    std::vector<std::vector<std::string> > analyse_contradiction()
    {
      //FIXME: return ctx.analyse_contradiction();
    }

    void addCstrToCtx(std::string constraint_name, Context* context) {
      // FIXME: typename std::map<std::string, Context*>::iterator ite = ctxOfCstr.find(constraint_name);
      // FIXME: if (ite != ctxOfCstr.end())
        // FIXME: throw std::runtime_error("Constraint already exists.");
      // FIXME: ctxOfCstr[constraint_name] = context;
    }

    bool enable_constraint(std::string constraint_name) {
      // FIXME: typename std::map<std::string, Context*>::iterator ite = ctxOfCstr.find(constraint_name);
      // FIXME: if (ite == ctxOfCstr.end()) return false;
      // FIXME: ite->second->enable_constraint(constraint_name);
      // FIXME: return true;
    }

    bool disable_constraint(std::string constraint_name) {
      // FIXME: typename std::map<std::string, Context*>::iterator ite = ctxOfCstr.find(constraint_name);
      // FIXME: if (ite == ctxOfCstr.end()) return false;
      // FIXME: ite->second->disable_constraint(constraint_name);
      // FIXME: return true;
    }

    bool is_constraint_enabled(std::string constraint_name) {
      // FIXME: typename std::map<std::string, Context*>::iterator ite = ctxOfCstr.find(constraint_name);
      // FIXME: assert(ite != ctxOfCstr.end());
      // FIXME: return ite->second->is_constraint_enabled(constraint_name);
    }

    void add_pre_hook(boost::function0<bool> f) {
      // FIXME: ctx.add_pre_hook(f);
    }

    /**
     * generate a new assignment
     **/
    Generator & operator() () {
      if(! next() )
        throw std::runtime_error("Generator constraint unsatisfiable.");
      return *this;
    }

    template<typename Expr>
    Generator & soft( Expr e) {
      // FIXME: ctx.soft_assertion( FixWidth()(e) );
      return *this;
    }

    /**
     * foreach
     **/
      // FIXME: change IndexVariable to Placeholder
    template<typename value_type, typename Expr>
    Generator & foreach(const __rand_vec<value_type> & v, const IndexVariable & i, Expr e) {
      assert(i.id() == _i.id());
      // FIXME
      return *this;
    }

    template<typename value_type, typename Expr>
    Generator & soft_foreach(const __rand_vec<value_type> & v, const IndexVariable & i, Expr e) {
      assert(i.id() == _i.id());
      // FIXME
      return *this;
    }

    template<typename value_type, typename Expr>
    Generator & foreach(std::string constraint_name, const __rand_vec<value_type> & v, const IndexVariable & i, Expr e) {
      // FIXME
      return *this;
    }

    /**
     * unique & non_unique
     **/
    template<typename value_type>
    Generator & unique(const __rand_vec<value_type> & v) {
      // FIXME
      return *this;
    }

    template<typename value_type>
    Generator & non_unique(const __rand_vec<value_type> & v) {
      // FIXME
      return *this;
    }

    /**
     * generate soft constraints
     **/
    Soft_Generator operator() ( Soft const & ) {
      return Soft_Generator (*this);
    }

    bool next() {
      // FIXME: new backend for solving
      return false;
    }

    /**
     * read variable "var"
     **/
    template<typename T>
    T operator[] (Variable<T> const & var) {
      // FIXME: new backend for solving
      //return ctx.read(var);
    }

    void new_disjunction() { /* FIXME: new backend for enums */ }
    void end_disjunction() { /* FIXME: new backend for enums */ }
    template<typename Expr>
    void add_to_disjunction (Expr expr) { /* FIXME: new backend for enums */ }

    std::ostream& get_ast( std::ostream& os )
    {
      os << "digraph AST {" << std::endl;
      ToDotVisitor visitor( os );
      BOOST_FOREACH ( boost::intrusive_ptr<Node> n, constraints_ ) n->visit( visitor );
      os << "}" << std::endl;
      return os;
    }

    private:
      Context ctx;
      std::vector<boost::intrusive_ptr<Node> > constraints_;
//      std::map<int, Context*> vecCtx;
//      std::map<std::string, Context*> ctxOfCstr;
//      std::set<int> uniqueVecSet;
  };

  template< typename Expr >
  Soft_Generator &
  Soft_Generator::operator() (Expr e) {
    gen.soft(e);
    return *this;
  }

  struct Soft {
    Soft_Generator operator() (Generator & gen) const {
      return Soft_Generator( gen );
    }
  };

  const Soft soft = {};

} // namespace crave
//  vim: ft=cpp:ts=2:sw=2:expandtab

