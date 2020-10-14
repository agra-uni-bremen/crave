########################################################################################
##	MIT License
########################################################################################
##	Copyright (c) 2012-2020 University of Bremen, Germany. 
##  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
##  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
##
##	Permission is hereby granted, free of charge, to any person obtaining a copy
##	of this software and associated documentation files (the "Software"), to deal
##	in the Software without restriction, including without limitation the rights
##	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
##	copies of the Software, and to permit persons to whom the Software is
##	furnished to do so, subject to the following conditions:
##
##	The above copyright notice and this permission notice shall be included in all
##	copies or substantial portions of the Software.
##
##	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
##	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
##	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
##	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
##	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
##	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
##	SOFTWARE.
##########################################################################################

set(EXAMPLES
  ex1_seed_dist
  ex2_constr_ref
  ex3_inh_soft_next
  ex4_constr_mng
  ex5_vec_constr
  ex6_inline_gen
  ex7_rand_enum 
  ex8_make_expression
  ex9_cdg
  ex10_variable_prev
  ex11_randomize_with
  sudoku
  sudoku_free
  ahb_mem_burst
)

foreach (ex ${EXAMPLES})
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ex}")
    add_ex_test(${ex}_experimental ${ex}/main.cpp)
  endif()
endforeach(ex)

if(SystemC_FOUND)
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sysc_dt")
	  add_subdirectory(sysc_dt)
  endif()
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ALU")
    add_subdirectory(ALU)
  endif()
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/random_stability")
    add_subdirectory(random_stability)
  endif()
endif(SystemC_FOUND)

