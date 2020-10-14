/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2011-2016 University of Bremen, Germany.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/


#pragma once

#include "../tags/SAT.hpp"
#include "../Features.hpp"

#include <vector>

#include <boost/foreach.hpp>
#include <boost/any.hpp>

namespace metaSMT
{
  // Forward declaration
  struct addclause_cmd;
  namespace features
  {
    struct addclause_api;
  }
   
  template<typename SatSolver>
  class SAT_Clause
  {
    public:
      typedef SAT::tag::lit_tag result_type; 
       
    public:
      SAT_Clause ()
      {
        true_lit.id = int(impl::new_var_id());
        //std::cout << "<true>\n";
        solver.assertion ( true_lit );
        //std::cout << "</true>\n";
      }

      template<typename Tag, typename Any>
      result_type operator() (Tag const& , Any )
      {
        return true_lit; 
      }

      result_type operator() (logic::tag::var_tag const& , boost::any )
      {
        result_type lit = { int(impl::new_var_id()) };
        return lit; 
      }

      result_type operator() (logic::tag::true_tag const& , boost::any )
      {
        //std::cout << "true " << true_lit << std::endl;
        return true_lit; 
      }

      result_type operator() (logic::tag::false_tag const& , boost::any )
      {
        //std::cout << "false " << -true_lit << std::endl;
        return -true_lit; 
      }
           
      result_type operator() (logic::tag::and_tag const& , result_type lhs, result_type rhs )
      {
        result_type out = { int(impl::new_var_id()) };
        clause3(-lhs,-rhs, out);
        clause2( rhs,-out);
        clause2( lhs,-out);
        return out;
      }

      result_type operator() (logic::tag::or_tag const& , result_type lhs, result_type rhs )
      {
        result_type out = { int(impl::new_var_id()) };
        clause3( lhs, rhs,-out);
        clause2(-rhs, out);
        clause2(-lhs, out);
        return out;
      }

      result_type operator() (logic::tag::nor_tag const& , result_type lhs, result_type rhs )
      {
        result_type out = { int(impl::new_var_id()) };
        clause3( lhs, rhs, out);
        clause2(-rhs,-out);
        clause2(-lhs,-out);
        return out;
      }

      result_type operator() (logic::tag::implies_tag const& , result_type lhs, result_type rhs )
      {
        return operator()(logic::tag::ite_tag(), lhs, rhs, true_lit);
      }

      result_type operator() (logic::tag::nand_tag const& , result_type lhs, result_type rhs )
      {
        result_type out = { int(impl::new_var_id()) };
        clause3(-lhs,-rhs,-out);
        clause2( rhs, out);
        clause2( lhs, out);
        return out;
      }

      result_type operator() (logic::tag::xnor_tag const& , result_type lhs, result_type rhs )
      {
        result_type out = { int(impl::new_var_id()) };
        clause3( lhs, rhs, out);
        clause3( lhs,-rhs,-out);
        clause3(-lhs, rhs,-out);
        clause3(-lhs,-rhs, out);
        return out;
      }

      result_type operator() (logic::tag::xor_tag const& , result_type lhs, result_type rhs )
      {
        result_type out = { int(impl::new_var_id()) };
        clause3( lhs, rhs,-out);
        clause3( lhs,-rhs, out);
        clause3(-lhs, rhs, out);
        clause3(-lhs,-rhs,-out);
        return out;
      }


      result_type operator() (logic::tag::equal_tag const& , result_type lhs, result_type rhs )
      {
        return operator()(logic::tag::xnor_tag(), lhs, rhs);
      }

      result_type operator() (logic::tag::nequal_tag const& , result_type lhs, result_type rhs )
      {
        return operator()(logic::tag::xor_tag(), lhs, rhs);
      }

      result_type operator() (logic::tag::distinct_tag const& , result_type lhs, result_type rhs )
      {
        return operator()(logic::tag::xor_tag(), lhs, rhs);        
      }

      template<typename T>
      result_type operator() (T const& , result_type , result_type )
      {
        return true_lit;
      }

      result_type operator() (logic::tag::not_tag const& , result_type lhs )
      {
        return -lhs;
      }

      template<typename T>
      result_type operator() (T const& , result_type )
      {
        return true_lit;
      }

      result_type operator() (logic::tag::ite_tag const& , result_type op1, result_type op2, result_type op3  )
      {
        result_type out = { int(impl::new_var_id()) };
        clause3( op1, op3, -out);
        clause3( op1,-op3,  out);
        clause3(-op1, op2, -out);
        clause3(-op1,-op2,  out);
        return out;
      }

      template<typename T>
      result_type operator() (T const& , result_type , result_type , result_type )
      {
        return true_lit;
      }

      void assertion ( result_type lit )
      {
        //std::cout << "assert " << lit << std::endl;
        solver.assertion( lit );
      }

      void assumption ( result_type lit )
      {
        //std::cout << "assume " << lit << std::endl;
        solver.assumption( lit );
      }

      template<typename Cmd, typename Expr>
      void command ( Cmd const& cmd, Expr& expr )
      {
        solver.command ( cmd, expr ); 
      }

      bool solve () 
      {
        return solver.solve (); 
      }

      result_wrapper read_value ( result_type lit ) 
      {
        return solver.read_value ( lit ); 
      }

      void clause2(result_type a, result_type b){
        std::vector<result_type> cls(2);
        cls[0]=a;
        cls[1]=b;
        solver.clause(cls);
      }

      void clause3(result_type a, result_type b, result_type c){
        std::vector<result_type> cls(3);
        cls[0]=a;
        cls[1]=b;
        cls[2]=c;
        solver.clause(cls);
      }

    private:
      SatSolver solver;
      result_type true_lit; 
  }; 

  namespace features {
    /* Stack supports stack api */
    template<typename Context>
      struct supports< SAT_Clause<Context>, features::addclause_api>
      : boost::mpl::true_ {};

    /* Forward all other supported operations */
    template<typename Context, typename Feature>
      struct supports< SAT_Clause<Context>, Feature>
      : supports<Context, Feature>::type {};
  }


}
