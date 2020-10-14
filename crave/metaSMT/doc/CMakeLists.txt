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



include_directories(../src)

include_directories(${metaSMT_INCLUDES})

if(DOXYGEN_FOUND)
  if(DOXYGEN_DOT_FOUND)
    set(DOXYGEN_HAVE_DOT YES)
  else(DOXYGEN_DOT_FOUND)
    set(DOXYGEN_HAVE_DOT NO)
  endif(DOXYGEN_DOT_FOUND)


  configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  add_custom_target(doxygen ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} 
    SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
  )

  set(doc_name "metaSMT-docs-${metaSMT_VERSION}") 

  add_custom_target(package_doxygen
    COMMAND ${CMAKE_COMMAND} -E copy_directory html "${doc_name}" 
    COMMAND ${CMAKE_COMMAND} -E tar zcf "${PROJECT_BINARY_DIR}/${doc_name}.tar.gz" "${doc_name}"
    COMMAND ${CMAKE_COMMAND} -E tar jcf "${PROJECT_BINARY_DIR}/${doc_name}.tar.bz2" "${doc_name}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS doxygen
  )
endif(DOXYGEN_FOUND)

