#pragma once

#include "Context.hpp"
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/home/phoenix/algorithm.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/foreach.hpp>

namespace crave {

  extern boost::mt19937 rng;

  struct DetectReadRef : proto::callable_context< DetectReadRef, proto::null_context > {
    DetectReadRef() : readRefFound(false) {}
    typedef void result_type;
    template<typename value_type>
    result_type operator() (proto::tag::terminal, read_ref_tag<value_type> const & ) {
      readRefFound = true;
    }
    bool readRefFound;
  };

  struct readRefExists {
    template<typename Expr>
    readRefExists(Expr expr) { proto::eval(expr, drr); }
    operator bool() const { return drr.readRefFound; }
    DetectReadRef drr;
  };

/*
  struct ReadRefExists
  : proto::or_ <
      proto::terminal<read_ref_tag<proto::_> >
    , proto::binary_expr<proto::_, ReadRefExists(proto::_), proto::_>
    , proto::binary_expr<proto::_, proto::_, ReadRefExists(proto::_) >
    , proto::unary_expr<proto::_, ReadRefExists(proto::_) >
    > {};
*/

  struct bitvector_less {
    bool operator() (qf_bv::bitvector const & a, qf_bv::bitvector const & b) {
      return boost::proto::value(a).id < boost::proto::value(b).id;
    }
  };

  struct AllSAT_base : public metaSMT_Context_base<AllSAT_base>
  {
    AllSAT_base(unsigned _sol_limit)
    : metaSMT_Context_base<AllSAT_base>()
    , read_ref_found(false)
    , is_solved(false)
    , sol_limit(_sol_limit)
    { }

    typedef metaSMT_Context_base<AllSAT_base> Super;

    // BEGIN functions that modify the constraints
    template<typename Expr>
    void assertion (Expr e) {
      read_ref_found |= readRefExists(e);
      Super::assertion(e);
      reset();
    }

    template<typename Expr>
    void assertion (std::string constraint_name, Expr e) {
      read_ref_found |= readRefExists(e);
      Super::assertion(constraint_name, e);
      reset();
    }

    void enable_constraint(std::string constraint_name) {
      Super::enable_constraint(constraint_name);
      reset();
    }

    void disable_constraint(std::string constraint_name) {
      Super::disable_constraint(constraint_name);
      reset();
    }

    template<typename Expr>
    void soft_assertion (Expr e) {
      read_ref_found |= readRefExists(e);
      Super::soft_assertion(e);
      reset();
    }

    template<typename T>
    void vec_assign (vecVar & vv, T & val) {
      Super::vec_assign(vv, val);
      reset();
    }

    void vec_free (vecVar & vv) {
      Super::vec_free(vv);
      reset();
    }

    template<typename T>
    void vec_unique (vecVar & vv, __rand_vec<T> & rv) {
      Super::vec_unique(vv, rv);
      reset();
    }
    // END functions that modify the constraints

    typedef std::map<qf_bv::bitvector, metaSMT::result_wrapper, bitvector_less> solution_t;
    typedef std::vector< solution_t > all_solutions_t;

    void store_solution (solution_t & sol, std::pair<int, qf_bv::bitvector> const & p ) {
       sol.insert( std::make_pair(p.second, read_value(solver, p.second) ));
    }

    void store_vector_solution (solution_t & sol, std::pair<vecVar, qf_bv::bitvector> const & p ) {
       sol.insert( std::make_pair(p.second, read_value(solver, p.second) ));
    }

    void block_solution(solution_t const & solution) {
      using namespace boost::phoenix;
      using namespace boost::phoenix::arg_names;

      result_type block = evaluate(solver, preds::False);

      BOOST_FOREACH( solution_t::value_type p, solution) {
        block = evaluate(solver, preds::Or( block, preds::nequal( p.first
            , qf_bv::bvbin(p.second))));
      }
      metaSMT::assumption( solver, block);
    }

    bool solve_all(bool soft) {
      using namespace boost::phoenix;
      using namespace boost::phoenix::arg_names;
      // gather all solutions
      while (true) {
        solution_t solution;

        pre_solve();
        if (soft) assumption(solver,_soft);

        for (uint i = 0; i < all_solutions.size(); i++)
           block_solution(all_solutions[i]);

        if (!metaSMT::solve(solver)) break;

        std::for_each(_variables.begin(), _variables.end(),
          bind(&AllSAT_base::store_solution, this, ref(solution), boost::phoenix::arg_names::arg1)
        );

        std::for_each(_vector_variables.begin(), _vector_variables.end(),
          bind(&AllSAT_base::store_vector_solution, this, ref(solution), boost::phoenix::arg_names::arg1)
        );

        all_solutions.push_back(solution);

        if (all_solutions.size() == sol_limit) break;
      }
      return all_solutions.size() > 0;
    }

    bool do_solve()  {
      // always reset if read references exist in the constraints
      if(read_ref_found) reset();
      if(is_solved) {
        // use next shuffled solution
        ++current;
        if( current == all_solutions.end() ) {
          std::random_shuffle(all_solutions.begin(), all_solutions.end(), rng);
          current=all_solutions.begin();
        }
      } else {
        is_solved = solve_all(true);
        // if soft constraint satisfiable
        if (!is_solved ) is_solved = solve_all(false);

//        std::cout << "AllSAT found " << all_solutions.size() << " solution(s)" << std::endl;
        std::random_shuffle(all_solutions.begin(), all_solutions.end(), rng);
        current=all_solutions.begin();

      }
      return is_solved;
    }

    template<typename T>
    T read ( Variable<T> const & v) {
      T ret;
      read(ret, v.id());
      return ret;
    }

    template<typename T>
    bool read ( T & v, unsigned id) {
      assert(is_solved && "AllSAT::solve was not called or not successful");
      std::map<int, qf_bv::bitvector>::const_iterator ite
        = _variables.find(id);
      if ( ite != _variables.end() ) {
        assert ( current != all_solutions.end() );
        solution_t::const_iterator sit = current->find(ite->second);
        assert( sit != current->end() && "ERROR: no assignment for variable" );
        AssignResult<T> assign;
        assign(v, sit->second);
        return true;
      }
      return false;
    }

    template<typename T>
    bool read ( T & v, vecVar & vv) {
      assert(is_solved && "AllSAT::solve was not called or not successful");
      std::map<vecVar, qf_bv::bitvector>::const_iterator ite
        = _vector_variables.find(vv);
      if ( ite != _vector_variables.end() ) {
        assert ( current != all_solutions.end() );
        solution_t::const_iterator sit = current->find(ite->second);
        assert( sit != current->end() && "ERROR: no assignment for variable" );
        AssignResult<T> assign;
        assign(v, sit->second);
        return true;
      }
      return false;
    }

    struct RNG {
      unsigned operator()(unsigned i) {
            boost::uniform_int<> rng(0, i - 1);
            return rng(crave::rng);
        }
    } rng;

    void reset() {
      if (is_solved) {
//        std::cout << "AllSAT reset" << std::endl;
        is_solved = false;
        all_solutions.clear();
      }
    }

    bool read_ref_found;
    bool is_solved;
    unsigned sol_limit;
    all_solutions_t all_solutions;
    all_solutions_t::iterator current;
  };

  template<unsigned N = 0>
  struct AllSAT : public AllSAT_base {
    AllSAT() : AllSAT_base(N) { }
  };

} /* crave */


// vim: tabstop=2 shiftwidth=2 expandtab
