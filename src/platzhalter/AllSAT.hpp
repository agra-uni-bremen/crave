#pragma once

#include "Context.hpp"
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/home/phoenix/algorithm.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/foreach.hpp>

namespace platzhalter {

  struct bitvector_less {
    bool operator() (qf_bv::bitvector const & a, qf_bv::bitvector const & b) {
      return boost::proto::value(a).id < boost::proto::value(b).id;
    }
  };

  struct AllSAT : public metaSMT_Context_base<AllSAT>
  {
    AllSAT(std::string const & solvername) 
    : metaSMT_Context_base<AllSAT>(solvername), is_solved(false)
    { }

    typedef metaSMT_Context_base<AllSAT> Super;


    typedef std::map<qf_bv::bitvector, metaSMT::result_wrapper, bitvector_less> solution_t;
    typedef std::vector< solution_t > all_solutions_t;

    void store_solution (solution_t & sol, std::pair<int, qf_bv::bitvector> const & p ) {
        //std::cout << "p " << p.first << " " << _solver->readAssignment(p.second) << std::endl;
       sol.insert( make_pair(p.second, read_value(solver, p.second) ));
    }

    void block_solution(solution_t const & solution) {
      using namespace boost::phoenix;
      using namespace boost::phoenix::arg_names;

      result_type block = evaluate(solver, preds::False);
      
      BOOST_FOREACH( solution_t::value_type p, solution) {
        block = evaluate(solver, preds::Or( block, preds::nequal( p.first
            , qf_bv::bvbin(p.second))));
      }
      metaSMT::assertion( solver, block);
    }

    bool do_solve(bool _ )  {
      if(is_solved) {
        // use next shuffled solution
        ++current;
        if( current == all_solutions.end() ) {
          std::random_shuffle(all_solutions.begin(), all_solutions.end(), rng);
          current=all_solutions.begin();
        }
      } else {
        using namespace boost::phoenix;
        using namespace boost::phoenix::arg_names;
        // gather all solutions
        solution_t solution;
        while( Super::do_solve(true) ) {
          is_solved = true;

          std::for_each(_variables.begin(), _variables.end(),
            bind(&AllSAT::store_solution,this, ref(solution), arg1)
          );
          all_solutions.push_back(solution);
          block_solution(solution);
          solution.clear();
        }
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
    void read ( T & v, unsigned id) {
      assert(is_solved && "AllSAT::solve was not called or not successful");
      std::map<int, qf_bv::bitvector>::const_iterator ite
        = _variables.find(id);
      assert ( ite != _variables.end() );
      assert ( current != all_solutions.end() );
      solution_t::const_iterator sit = current->find(ite->second);
      assert( sit != current->end() && "ERROR: not assignment for variable" );
      v = sit->second;
    }

    struct RNG {
      boost::mt19937 _state;
      unsigned operator()(unsigned i) {
            boost::uniform_int<> rng(0, i - 1);
            return rng(_state);
        }
      //RNG() :_state(time(NULL)) {}
      RNG() :_state(0) {}
    } rng;

    bool is_solved;
    all_solutions_t all_solutions;
    all_solutions_t::iterator current;
  };
  
} /* platzhalter */


// vim: tabstop=2 shiftwidth=2 expandtab
