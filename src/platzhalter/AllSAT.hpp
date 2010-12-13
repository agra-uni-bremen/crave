#pragma once

#include "Context.hpp"
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/home/phoenix/algorithm.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>

namespace platzhalter {

  struct AllSAT : public metaSMT_Context_base<AllSAT>
  {
    AllSAT(std::string const & solvername) 
    : metaSMT_Context_base<AllSAT>(solvername), is_solved(false)
    { }

    typedef metaSMT_Context_base<AllSAT> Super;


    typedef std::map<result_type, std::string> solution_t;
    typedef std::vector< solution_t > all_solutions_t;

    void store_solution (solution_t & sol, std::pair<int, result_type> const & p ) {
        std::cout << "p " << p.first << " " << _solver->readAssignment(p.second) << std::endl;
        sol.insert( make_pair(p.second, _solver->readAssignment(p.second)));
    }

    void block_solution(solution_t const & solution) {
      using namespace boost::phoenix;
      using namespace boost::phoenix::arg_names;

      boost::function<result_type ( result_type, solution_t::value_type const &)> createOrNeq 
        = bind(&metaSMT::QF_BV::bvor, _logic
          ,  arg1
          , bind(&metaSMT::QF_BV::nequal, _logic
            , bind(&solution_t::value_type::first, arg2) 
            , bind(&metaSMT::QF_BV::bvbin, _logic, bind(&solution_t::value_type::second, arg2) )
          )
        );
      _solver->addAssertion(boost::phoenix::accumulate(solution, _logic->bit0(), createOrNeq)());
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
      std::map<int, result_type>::const_iterator ite
        = _variables.find(id);
      assert ( ite != _variables.end() );
      assert ( current != all_solutions.end() );
      solution_t::const_iterator sit = current->find(ite->second);
      assert( sit != current->end() && "ERROR: not assignment for variable" );
      v = metaSMT::bits2Cu(sit->second);
    }

    struct RNG {
      boost::mt19937 _state;
      unsigned operator()(unsigned i) {
            boost::uniform_int<> rng(0, i - 1);
            return rng(_state);
        }
      RNG() :_state(time(NULL)) {}
    } rng;

    bool is_solved;
    all_solutions_t all_solutions;
    all_solutions_t::iterator current;
  };
  
} /* platzhalter */


// vim: tabstop=2 shiftwidth=2 expandtab
