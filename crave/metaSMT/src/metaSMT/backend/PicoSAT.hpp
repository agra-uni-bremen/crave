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
#include "../result_wrapper.hpp"
#include "../Features.hpp"

extern "C"
{
#include <picosat.h>
}
#include <vector>
#include <exception>
#include <iostream>

#include <boost/fusion/sequence/intrinsic.hpp>
#include <boost/fusion/support/is_sequence.hpp>
#include <boost/variant.hpp>
#include <boost/any.hpp> 
#include <boost/foreach.hpp>
 

namespace metaSMT {
  // Forward declaration
  struct addclause_cmd;
  namespace features
  {
    struct addclause_api;
  }

  namespace solver {
    class PicoSAT
    {
      public:
        typedef SAT::tag::lit_tag result_type;
        PicoSAT ()
        {
          //          if ( in_use == USED )
          //            throw std:runtime_error ("Picosat already in use. Only single instances supported."); 

          picosat_init (); 
          //          in_use = USED;
        }

        ~PicoSAT ()
        {
          picosat_reset (); 
          //         in_use = UNUSED;
        }


        int toLit ( result_type lit )
        {
          return lit.id; 
        }

        void clause ( std::vector < result_type > const& clause)
        {
          BOOST_FOREACH ( result_type const& lit, clause )
            picosat_add ( toLit ( lit ) );
          picosat_add ( 0 ); 
        }

        void command ( addclause_cmd const&, std::vector < result_type > const& cls )
        {
          clause ( cls );
        }


        void assertion ( result_type lit )
        {
          picosat_add ( toLit ( lit ) ); 
          picosat_add ( 0 ); 
        }

        void assumption ( result_type lit )
        {
          picosat_assume ( toLit ( lit ) ); 
        }


        bool solve ( )
        {
          switch (picosat_sat (-1))
          {
            case PICOSAT_UNSATISFIABLE:
              return false;
            case PICOSAT_SATISFIABLE:
              return true;
            case PICOSAT_UNKNOWN:
              throw std::runtime_error ( "unknown return type of picosat_sat "); 
            default:
              throw std::runtime_error ( "unsupported return type of picosat_sat "); 
          }
        }

        result_wrapper read_value ( result_type lit )
        {

          switch ( picosat_deref ( toLit ( lit ) ) )
          {
            case -1:
              return result_wrapper ( '0' );
            case 1:
              return result_wrapper ( '1' );
            case 0:
              return result_wrapper ( 'X' );
            default:
              std::cerr << "Unknown result." << std::endl;
              return result_wrapper ( 'X' ); 
          }
        }

      private:
        //         enum { UNUSED, USED }; 
        //         static int in_use = UNUSED; 
    };
  } /* solver */

  namespace features {
    template<>
      struct supports< solver::PicoSAT, features::addclause_api>
      : boost::mpl::true_ {};
  } /* features */

} /* metaSMT */
// vim: ts=2 sw=2 et
