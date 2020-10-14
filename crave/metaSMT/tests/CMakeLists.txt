########################################################################################
##	MIT License
########################################################################################
##	Copyright (c) 2011-2016 University of Bremen, Germany.
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

include(CMakeParseArguments)

include_directories(../src ${metaSMT_INCLUDES})

# From v1.59.0 Boost.Test does not require RTTI
if (${Boost_VERSION} VERSION_LESS 105900)
  message( STATUS "Compile metaSMT tests with RTTI" )
else()
  CHECK_CXX_COMPILER_FLAG("-fno-rtti" COMPILER_SUPPORTS_FNO_RTTI)
  if (COMPILER_SUPPORTS_FNO_RTTI)
    add_definitions(-fno-rtti)
    message( STATUS "Compile metaSMT tests without RTTI" )
  else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} does not support -fno-rtti, compile metaSMT tests with RTTI")
  endif()
endif()

add_library(boost_test boost_test.cpp)
link_libraries(boost_test metaSMT)

if (WIN32)
  add_definitions("-DBOOST_TEST_INCLUDED") # this is to prevent boost autolink
endif()

find_program(AWK awk)
if (AWK)
  message(STATUS "Use awk to search for tests")
  set(search_test_cmd ${AWK} -f search_tests.awk)
else()
  find_program(PYTHON python)
  if(PYTHON)
    message(STATUS "Use python to search for tests")
    set(search_test_cmd ${PYTHON} search_tests.py)
  endif()
endif()

if (NOT search_test_cmd)
  message(FATAL_ERROR "Cannot search for tests, either awk or python is required!")
endif()

function (add_all_tests prog cfile)
  cmake_parse_arguments(opts "" "" "ENVIRONMENT;ARGS" ${ARGN})
  execute_process( COMMAND
    ${search_test_cmd} ${cfile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE test_cases
  )
  string(REPLACE "\n" ";" test_cases "${test_cases}")

  foreach(test  ${test_cases})
    #message(STATUS "${prog}/${test} ${prog} -t ${test}")
    add_test("${prog}/${test}" ${prog} -t ${test} ${opts_ARGS})
    set_tests_properties ("${prog}/${test}" PROPERTIES TIMEOUT ${TIMEOUT})
    if(opts_ENVIRONMENT)
      set_tests_properties("${prog}/${test}" PROPERTIES ENVIRONMENT ${opts_ENVIRONMENT})
    endif()
  endforeach(test)
endfunction(add_all_tests)

function (add_test_executable name source )
  cmake_parse_arguments(req "" "" "REQUIRES;PROPERTIES;ARGS;LIBRARIES;ENVIRONMENT" ${ARGN})

  set(req_all 1)

  foreach( req ${req_REQUIRES} )
    if(NOT ${req} )
      set(req_all 0)
      # message("UNSATISFIED DEPENDENCY: " ${req} " of " ${name})
    endif(NOT ${req} )
  endforeach( req )

  if(req_all)
    add_executable(${name} ${source})
    add_all_tests(${name} ${source} ARGS ${req_ARGS} ENVIRONMENT ${req_ENVIRONMENT})
    if(req_LIBRARIES)
      target_link_libraries(${name} metaSMT ${req_LIBRARIES})
    endif()
    if(req_PROPERTIES)
      set_target_properties(${name} PROPERTIES ${req_PROPERTIES})
    endif()
  endif(req_all)
endfunction(add_test_executable)

add_test_executable( result_wrapper test_result_wrapper.cpp)

add_test_executable( direct_CUDD direct_CUDD.cpp REQUIRES CUDD_FOUND )
add_test_executable( direct_BitBlast_CUDD direct_BitBlast_CUDD.cpp
  REQUIRES CUDD_FOUND)
add_test_executable(direct_CUDD_Distributed direct_CUDD_Distributed.cpp 
  REQUIRES CUDD_FOUND)

add_test_executable( direct_Boolector direct_Boolector.cpp
  REQUIRES Boolector_FOUND 
  PROPERTIES COMPILE_FLAGS "${Boolector_CXXFLAGS}"
  )

add_test_executable( direct_Yices2 direct_Yices2.cpp REQUIRES YICES2_FOUND )

if (NOT Z3_CXXFLAGS)
  set(Z3_CXXFLAGS "-DZ3_DUMMY_FLAG")
endif()
add_test_executable( direct_Z3 direct_Z3.cpp
  REQUIRES Z3_FOUND
  PROPERTIES COMPILE_FLAGS "${Z3_CXXFLAGS}"
  )

add_test_executable( direct_STP direct_STP.cpp REQUIRES STP_FOUND )

if (NOT CVC4_CXXFLAGS)
  set(CVC4_CXXFLAGS "-DCVC4_DUMMY_FLAG")
endif()
add_test_executable( direct_CVC4 direct_CVC4.cpp
  REQUIRES CVC4_FOUND
  PROPERTIES COMPILE_FLAGS "${CVC4_CXXFLAGS}"
  )

add_test_executable( direct_MiniSAT direct_MiniSAT.cpp
  REQUIRES MiniSat_FOUND
  PROPERTIES COMPILE_FLAGS "${MiniSat_CXXFLAGS}"
  )

add_test_executable( direct_PicoSAT direct_PicoSAT.cpp
  REQUIRES PicoSAT_FOUND
)

add_test_executable( direct_Lingeling direct_Lingeling.cpp
  REQUIRES Lingeling_FOUND
)

add_test_executable( direct_Aiger_MiniSAT direct_Aiger_MiniSAT.cpp
  REQUIRES MiniSat_FOUND Aiger_FOUND
  PROPERTIES COMPILE_FLAGS "${MiniSat_CXXFLAGS}"
  )

add_test_executable( direct_Aiger_PicoSAT direct_Aiger_PicoSAT.cpp
  REQUIRES PicoSAT_FOUND Aiger_FOUND )

# vim: ts=2 sw=2 et
