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


PROJECT(metaSMT)
cmake_minimum_required(VERSION 2.8.9)

option(metaSMT_REQUIRE_CXX11 "make C++11 a requirement" on)

if ( NOT metaSMT_REQUIRE_CXX11 AND WIN32 )
  message(FATAL_ERROR "Windows build requires CXX11")
endif()

include(CheckCXXCompilerFlag)

if (metaSMT_REQUIRE_CXX11)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  if(COMPILER_SUPPORTS_CXX11)
    add_definitions(-std=c++11)
  else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} does not support -std=c++11. Please use a different C++ compiler.")
  endif()
endif()

include(CheckIncludeFileCXX)
include(FeatureSummary)

include(CTest)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules )

set(Boost_NO_BOOST_CMAKE true)
set(Boost_NO_SYSTEM_PATHS true)
find_package(Boost REQUIRED)

# From v1.57.0 Boost.Any does not require RTTI
if (${Boost_VERSION} VERSION_LESS 105700)
  message( STATUS "metaSMT requires RTTI" )
  set(metaSMT_REQUIRE_RTTI true)
else()
  message( STATUS "metaSMT does not require RTTI" )
  set(metaSMT_REQUIRE_RTTI false)
endif()

set_package_info(Boost "Portable C++ libraries")
include_directories(${Boost_INCLUDE_DIRS})

list(INSERT CMAKE_PREFIX_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")

# Set the default version number, but try to calculate
# a proper version from git.
SET(metaSMT_VERSION 4)
include(cmake/get_git_version.cmake)

include(cmake/build_type.cmake)
include(cmake/default_compliler_flags.cmake)
include(cmake/config_file.cmake)

###  
# Options to force disable certain solvers. 
# set all solvers to "on" with good support and experimental or
# work in progress to "off"
###
option(metaSMT_USE_Boolector  "try to use Boolector solver" on)
option(metaSMT_USE_Z3         "try to use Z3 solver" on)
option(metaSMT_USE_STP        "try to use STP solver" on)
option(metaSMT_USE_CVC4        "try to use CVC4 solver" on)
option(metaSMT_USE_YICES2     "try to use Yices2 solver" on)
option(metaSMT_USE_CUDD       "try to use CUDD BDD package" on )
option(metaSMT_USE_MiniSat    "try to use minisat SAT solver" on)
option(metaSMT_USE_picosat    "try to use picosat SAT solver" on)
option(metaSMT_USE_lingeling  "try to use lingeling SAT solver" on)
option(metaSMT_USE_Aiger      "try to use aiger" on)
option(metaSMT_USE_CW         "try to use clause writers" on)

option(metaSMT_DOC_Doxygen    "try to build doxygen documentation" off)
option(metaSMT_ENABLE_TESTS   "build tests (recommended)" on)

set(metaSMT_CONFIG_DIR "share/metaSMT" CACHE PATH 
  "where to install CMake and Make Config files")

if ( NOT TIMEOUT )
  set(TIMEOUT 5 CACHE STRING "The timeout for tests")
endif()

if ( WIN32 )
  message(STATUS "Windows build: disable all solvers except z3 or CUDD!" )
  set(metaSMT_USE_Boolector  off)
  set(metaSMT_USE_STP off)
  set(metaSMT_USE_CVC4 off)
  set(metaSMT_USE_YICES2 off)
  set(metaSMT_USE_MiniSat off)
  set(metaSMT_USE_picosat off)
  set(metaSMT_USE_lingeling off)
  set(metaSMT_USE_Aiger off)
  set(metaSMT_USE_CW off)
endif()

if(metaSMT_USE_Boolector)
  find_package(Boolector QUIET)
  list(APPEND Boolector_INCLUDE_DIR ${boolector_INCLUDE_DIRS})
  list(APPEND Boolector_LIBRARIES ${boolector_LIBRARIES})
else()
  set(Boolector_FOUND 0)
endif()
set_package_info(Boolector "solver for SMT QF_BV and Arrays")

if (metaSMT_USE_Z3)
  find_package(Z3 QUIET)
else()
  set(Z3_FOUND 0)
endif()
set_package_info(Z3 "solver for various SMT theories")

if (metaSMT_USE_YICES2)
  find_package(YICES2 QUIET)
else()
  set(YICES2_FOUND 0)
endif()
set_package_info(YICES2 "solver for various SMT theories")

if(metaSMT_USE_STP)
  find_package(STP QUIET)
else()
  set(STP_FOUND 0)
endif()
set_package_info(STP "solver for SMT QF_BV and Arrays")

if(metaSMT_USE_CVC4)
  find_package(CVC4 QUIET)
else()
  set(CVC4_FOUND 0)
endif()
set_package_info(CVC4 "solver for various SMT theories")

if (metaSMT_USE_CUDD)
  find_package(CUDD QUIET)
else ()
  set(CUDD_FOUND 0)
endif ()
set_package_info(CUDD "binary decision diagrams")

if (metaSMT_USE_Aiger)
  find_package(Aiger QUIET) 
  list(APPEND Aiger_LIBRARIES Aiger)
else ()
  set(Aiger_FOUND 0)
endif()
set_package_info(Aiger "And Inverter Graph libraries")

if (metaSMT_USE_MiniSat)
  find_package(MiniSat QUIET)
else ()
  set(MiniSat_FOUND 0)
endif ()
set_package_info(MiniSat "SAT solver MiniSAT")

if (metaSMT_USE_picosat )
  find_package (PicoSAT QUIET)
endif()
set_package_info(PicoSat "SAT solver PicoSat")

if( metaSMT_USE_lingeling )
  find_package( Lingeling QUIET)
endif()
set_package_info(Lingeling "SAT solver lingeling")

find_package(GMP QUIET)
set_package_info(GMP "optional, required by some backends")
if(NOT GMP_FOUND)
  find_library(GMP_LIBRARIES gmp PATHS ${GMP_DIR})
  find_library(GMPXX_LIBRARIES gmpxx PATHS ${GMP_DIR})
else()
  set(GMP_LIBRARIES "")
  set(GMPXX_LIBRARIES "")
endif()

if(Boolector_FOUND AND (Boolector_VERSION VERSION_GREATER 2))
  find_package(Threads)
  set(Boolector_LIBRARIES ${Boolector_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()

if(Z3_FOUND)
  find_package(OpenMP)
  if (OPENMP_FOUND)
    set(Z3_CXXFLAGS ${OpenMP_CXX_FLAGS})
    if (UNIX OR MINGW)
      list(APPEND Z3_LIBRARIES ${OpenMP_CXX_FLAGS})
    endif()
    message(STATUS "Use Z3 with OpenMP")
  else()
    message(STATUS "Use Z3 without OpenMP")
  endif()

  if (UNIX)
    find_package(Threads QUIET)
    find_library(RT_LIB rt)
    set(Z3_LIBRARIES ${Z3_LIBRARIES} ${RT_LIB} ${CMAKE_THREAD_LIBS_INIT})
  endif()

  include(CheckCXXSourceRuns)

  set(CMAKE_REQUIRED_FLAGS ${Z3_CXXFLAGS})
  set(CMAKE_REQUIRED_INCLUDES ${Z3_INCLUDE_DIR})
  set(CMAKE_REQUIRED_LIBRARIES ${Z3_LIBRARIES})

  check_CXX_source_runs("#include <z3++.h>
int main(int argc, const char *argv[]) {
  z3::context c;
  z3::solver s(c);
  s.check();
  return 0;
}"
  Z3_WORKS)

  if( NOT Z3_WORKS )
    message(WARNING "Disabling Z3 due to linking errors.")
    set( Z3_FOUND 0)
  endif()

  check_CXX_source_compiles("#include <z3++.h>
int main(int argc, const char *argv[]) {
  z3::context c;
  z3::expr e(c);
  unsigned long long val = 0;
  Z3_get_numeral_uint64(0, e, &val);
  return 0;
}"
  Z3_ULL_API)

  if( Z3_ULL_API )
    set(Z3_CXXFLAGS "${Z3_CXXFLAGS} -DmetaSMT_Z3_ULL_API")
  endif()

endif()

if(CVC4_FOUND)
  if( GMPXX_LIBRARIES )
    set(CVC4_LIBRARIES ${CVC4_LIBRARIES} ${GMPXX_LIBRARIES})
  endif()

  if( GMP_LIBRARIES )
    set(CVC4_LIBRARIES ${CVC4_LIBRARIES} ${GMP_LIBRARIES})
  endif()

  if(EXISTS "${CVC4_INCLUDE_DIR}/cvc4/expr/kind.h")
    file(STRINGS ${CVC4_INCLUDE_DIR}/cvc4/expr/kind.h CVC4_KIND_IFF REGEX "IFF")
    if (NOT CVC4_KIND_IFF)
      message(STATUS "CVC4 version without boolean operator IFF")
      set(CVC4_CXXFLAGS "-DCVC4_WITHOUT_KIND_IFF")
    endif()
  endif()
endif()

if( WIN32 AND NOT Z3_FOUND )
  message(FATAL_ERROR "Windows build: Z3 is required but cannot be found!")
endif()

if (metaSMT_DOC_Doxygen)
  find_package(Doxygen)
else ()
  set(Doxygen_FOUND 0)
endif ()

#############################################################
## Set LIBS, INCLUDES and metaSMT_AVAILABLE_QF_ABV_SOLVERS  #
#############################################################

set(metaSMT_AVAILABLE_QF_ABV_SOLVERS "")
set(metaSMT_INCLUDES ${Boost_INCLUDE_DIRS})

if(Boolector_FOUND)
  config_find(Boolector ${Boolector_DIR})
  list(APPEND metaSMT_LIBS ${Boolector_LIBRARIES})
  list(APPEND metaSMT_INCLUDES ${Boolector_INCLUDE_DIR})
  if ((Boolector_VERSION VERSION_GREATER 2) OR (Boolector_VERSION VERSION_EQUAL 2))
    message( STATUS "Use Boolector ${Boolector_VERSION}" )
    if ((Boolector_VERSION VERSION_GREATER 2.2.0))
      message( STATUS "Use Boolector v2 new API" )
      set(Boolector_CXXFLAGS "${Boolector_CXXFLAGS} -DmetaSMT_BOOLECTOR_2_NEW_API")
    endif()
  else()
    message( STATUS "Use Boolector v1" )
    set(Boolector_CXXFLAGS "${Boolector_CXXFLAGS} -DmetaSMT_BOOLECTOR_1_API")
  endif()
  set(metaSMT_CXXFLAGS "${metaSMT_CXXFLAGS} ${Boolector_CXXFLAGS}")
  list(APPEND metaSMT_AVAILABLE_QF_ABV_SOLVERS "BTOR")
endif()

if(Z3_FOUND)
  config_find(Z3 ${Z3_DIR})
  list(APPEND metaSMT_LIBS ${Z3_LIBRARIES})
  list(APPEND metaSMT_INCLUDES ${Z3_INCLUDE_DIR})
  set(metaSMT_CXXFLAGS "${metaSMT_CXXFLAGS} ${Z3_CXXFLAGS}")
  list(APPEND metaSMT_AVAILABLE_QF_ABV_SOLVERS "Z3")
endif()

if(STP_FOUND)
  config_find(STP ${STP_DIR})
  message( STATUS "STP_LIBRARIES: ${STP_SHARED_LIBRARY}" )
  list(APPEND metaSMT_LIBS ${STP_SHARED_LIBRARY})
  message( STATUS "STP_INCLUDE_DIR: ${STP_INCLUDE_DIRS}" )
  list(APPEND metaSMT_INCLUDES ${STP_INCLUDE_DIRS})
  list(APPEND metaSMT_AVAILABLE_QF_ABV_SOLVERS "STP")
else()
  set( STP_FOUND 0 )
endif()

if(CVC4_FOUND)
  config_find(CVC4 ${CVC4_DIR})
  list(APPEND metaSMT_LIBS ${CVC4_LIBRARIES})
  list(APPEND metaSMT_INCLUDES ${CVC4_INCLUDE_DIR})
  set(metaSMT_CXXFLAGS "${metaSMT_CXXFLAGS} ${CVC4_CXXFLAGS}")
  list(APPEND metaSMT_AVAILABLE_QF_ABV_SOLVERS "CVC4")
endif()

if (YICES2_FOUND)
  config_find(YICES2 ${YICES2_DIR})
  list(APPEND metaSMT_LIBS ${YICES2_LIBRARIES})
  list(APPEND metaSMT_INCLUDES ${YICES2_INCLUDE_DIR})
  list(APPEND metaSMT_AVAILABLE_QF_ABV_SOLVERS "YICES2")
endif()

if(Aiger_FOUND)
  config_find(Aiger ${Aiger_DIR})
  list(APPEND metaSMT_LIBS ${Aiger_LIBRARIES})
  list(APPEND metaSMT_INCLUDES ${Aiger_INCLUDE_DIR})
endif()

if(CUDD_FOUND)
  config_find(CUDD ${CUDD_DIR})
  list(APPEND metaSMT_LIBS ${CUDD_LIBRARIES})
  list(APPEND metaSMT_INCLUDES ${CUDD_INCLUDE_DIR} ${cudd_INCLUDE_DIRS})
endif()

if(MiniSat_FOUND)
  config_find(MiniSat ${MiniSat_DIR})
  list(APPEND metaSMT_LIBS ${MiniSat_LIBRARIES})
  list(APPEND metaSMT_INCLUDES ${MiniSat_INCLUDE_DIR})
  set(metaSMT_CXXFLAGS "${metaSMT_CXXFLAGS} ${MiniSat_CXXFLAGS}")
endif()

if (PicoSAT_FOUND)
  config_find(PicoSAT ${PicoSAT_DIR})
  list(APPEND metaSMT_LIBS ${PicoSAT_LIBRARIES})
  list(APPEND metaSMT_INCLUDES ${PicoSAT_INCLUDE_DIR})
endif ()

if (Lingeling_FOUND)
  config_find(Lingeling ${Lingeling_DIR})
  list(APPEND metaSMT_LIBS ${Lingeling_LIBRARIES})
  list(APPEND metaSMT_INCLUDES ${Lingeling_INCLUDE_DIR})
endif ()

############################################################
############# RPATH ########################################
############################################################

include(cmake/library_rpath.cmake)
add_dependent_libraries(${metaSMT_LIBS})


############################################################
############# subdirectories ###############################
############################################################

add_subdirectory(src)
add_subdirectory(doc)

if(metaSMT_ENABLE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

############################################################
########## generate cmake config files #####################
############################################################

generate_config_files()

Feature_Summary(WHAT ALL)

############################################################
##                CPack settings                          ##
############################################################


# CPack version numbers for release tarball name.
set(CPACK_PACKAGE_VERSION_MAJOR ${metaSMT_VERSION})
#set(CPACK_PACKAGE_VERSION_MINOR 0)
#set(CPACK_PACKAGE_VERSION_PATCH 0)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SMT programming library")
set(CPACK_PACKAGE_VENDOR "metaSMT development team")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_GENERATOR TGZ)
set(
CPACK_SOURCE_PACKAGE_FILE_NAME
"metaSMT-${metaSMT_VERSION}"
CACHE INTERNAL "tarball basename"
)
set(CPACK_SOURCE_GENERATOR TGZ TBZ2)

set(CPACK_SOURCE_IGNORE_FILES
"/\\\\.git"
"/deps"
"/build.*/"
"/root"
)
include(CPack)


############################################################
############################################################

# vim: ft=cmake:ts=2:sw=2:expandtab
