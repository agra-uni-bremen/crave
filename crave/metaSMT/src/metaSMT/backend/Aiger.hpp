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

#include "../tags/Logic.hpp" 

#include <boost/any.hpp>

extern "C" {
#include <aiger.h>
}

namespace metaSMT 
{
  class Aiger  
  {
    public:
      typedef unsigned result_type;
      
      aiger *aig; 

    public:
      Aiger () : aig (aiger_init()) {} 
      ~Aiger () 
      {
        aiger_reset ( aig ); 
      }

      result_type operator() (logic::tag::var_tag const& , boost::any )
      {
        return new_var(); 
      }

      result_type operator() (logic::tag::true_tag const& , boost::any )
      {
        return aiger_true;
      }
       
      result_type operator() (logic::tag::false_tag const& , boost::any )
      {
        return aiger_false; 
      }

      result_type operator() (logic::tag::not_tag const&, result_type operand)
      {
        return aiger_not ( operand );
      }

      result_type operator() (logic::tag::and_tag const&, result_type lhs, result_type rhs )
      {
        result_type t = new_var();
        aiger_add_and ( aig, t, lhs, rhs );
        return t; 
      }

      result_type operator() (logic::tag::nand_tag const&, result_type lhs, result_type rhs )
      {
        result_type t = new_var();
        aiger_add_and ( aig, t, lhs, rhs );
        return aiger_not ( t );
      }

      result_type operator() (logic::tag::equal_tag const&, result_type lhs, result_type rhs )
      {
        return aiger_add_xnor ( aig, lhs, rhs ); 
      }

      result_type operator() (logic::tag::nequal_tag const&, result_type lhs, result_type rhs )
      {
        return aiger_not ( aiger_add_xnor ( aig, lhs, rhs ) ); 
      }

      result_type operator() (logic::tag::distinct_tag const&, result_type lhs, result_type rhs )
      {
        return aiger_not ( aiger_add_xnor ( aig, lhs, rhs ) ); 
      }

      result_type operator() (logic::tag::xnor_tag const&, result_type lhs, result_type rhs )
      {
        return aiger_add_xnor ( aig, lhs, rhs ); 
      }

      result_type operator() (logic::tag::implies_tag const&, result_type lhs, result_type rhs )
      {
        return aiger_add_or ( aig, aiger_not (lhs), rhs ); 
      }

      result_type operator() (logic::tag::or_tag const&, result_type lhs, result_type rhs )
      {
        return aiger_add_or ( aig, lhs, rhs ); 
      }
      
      result_type operator() (logic::tag::nor_tag const&, result_type lhs, result_type rhs )
      {
        return aiger_not ( aiger_add_or ( aig, lhs, rhs ) );
      }

      result_type operator() (logic::tag::xor_tag const&, result_type lhs, result_type rhs )
      {
        return aiger_add_xor ( aig, lhs, rhs ); 
      }

      result_type operator() (logic::tag::ite_tag const&, result_type I, result_type T, result_type E)
      {
        return aiger_add_ite ( aig, I, T, E );
      }


      template<typename T>
        result_type operator() (T const& , result_type , result_type )
        {
          assert ( false ); 
          return aiger_false; 
        }

      template<typename T, typename Arg>
        result_type operator() (T const& , Arg )
        {
          assert ( false ); 
          return aiger_false; 
        }

      template<typename T>
        result_type operator() (T const& , result_type , result_type , result_type )
        {
          assert ( false ); 
          return aiger_false; 
        }

      unsigned new_var ()
      {
        ++aig->maxvar;
        return aiger_var2lit( aig->maxvar );
      }
       
      unsigned aiger_add_or ( aiger *aig, unsigned lhs, unsigned rhs )
      {
        result_type t = new_var ();
        aiger_add_and ( aig, t, aiger_not (lhs), aiger_not (rhs) ); 
        return aiger_not ( t );  
      }

      unsigned aiger_add_xor ( aiger *aig, unsigned lhs, unsigned rhs )
      {
        return aiger_not ( aiger_add_xnor ( aig, lhs, rhs ) );
      }

      unsigned aiger_add_xnor ( aiger *aig, unsigned lhs, unsigned rhs )
      {
        result_type t1 = new_var ();
        result_type t2 = new_var ();

        aiger_add_and ( aig, t1, aiger_not ( lhs ), aiger_not ( rhs ) ); 
        aiger_add_and ( aig, t2, lhs, rhs ); 

        return aiger_add_or ( aig, t1, t2 ); 
      }

      unsigned aiger_add_ite ( aiger *aig, unsigned I, unsigned T, unsigned E )
      {
        result_type t = new_var (); 
        aiger_add_and ( aig 
            , t
            , aiger_add_or ( aig, aiger_not (I), T )
            , aiger_add_or ( aig, I, E ) );
        return t;
      }
  }; 


}
