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

if ( NOT CRAVE_EXAMPLES_TIMEOUT )
  set(CRAVE_EXAMPLES_TIMEOUT 120)
endif()

include_directories(${crave_SOURCE_DIR}/src)

function (add_ex_test ex cfile)
 add_executable(${ex} ${cfile})
 target_link_libraries(${ex} ${PROJECT_NAME} ${ALL_EXTERNAL_LIBS})
 add_test("${ex}" ${ex})
 set_tests_properties ("${ex}" PROPERTIES TIMEOUT ${CRAVE_EXAMPLES_TIMEOUT}) 
endfunction(add_ex_test)

add_subdirectory(crave2_api)

if (CRAVE_ENABLE_EXPERIMENTAL)
  add_subdirectory(experimental_api)  
endif()

