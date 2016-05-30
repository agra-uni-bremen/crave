project(crave)
cmake_minimum_required(VERSION 2.8.9)

set(${PROJECT_NAME}_CONFIG_DIR "share/${PROJECT_NAME}" CACHE PATH 
  "where to install CMake and Make Config files")

############################################################
##                CRAVE settings                          ##
############################################################

option(CRAVE_ENABLE_EXPERIMENTAL "enable experimental extensions of CRAVE" on)

if (CRAVE_ENABLE_EXPERIMENTAL)
  add_definitions(-std=c++11)
endif()

if (metaSMT_USE_Boolector)
  add_definitions(-DmetaSMT_USE_Boolector)
endif()
if (metaSMT_USE_CVC4)
  add_definitions(-DmetaSMT_USE_CVC4)
endif()
if (metaSMT_USE_Z3)
  add_definitions(-DmetaSMT_USE_Z3)
endif()
if (metaSMT_USE_SWORD)
  add_definitions(-DmetaSMT_USE_SWORD)
endif()
if (metaSMT_USE_CUDD)
  add_definitions(-DmetaSMT_USE_CUDD)
endif()

if (WITH_SYSTEMC)
  add_definitions(-DWITH_SYSTEMC)
endif()

### packages
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(Boost_NO_BOOST_CMAKE true)
set(Boost_NO_SYSTEM_PATHS true)
set(Boost_ADDITIONAL_VERSIONS "1.50.0")
find_package(Boost REQUIRED COMPONENTS system thread filesystem)

find_package(Glog REQUIRED)

if (WITH_SYSTEMC)
  find_package(SystemC REQUIRED)
  find_package(Threads REQUIRED)
else()
  find_package(SystemC)
endif()

add_subdirectory(metaSMT)
list(INSERT CMAKE_PREFIX_PATH 0 "${CMAKE_BINARY_DIR}")
#list(INSERT CMAKE_PREFIX_PATH 0 "${metaSMT_BINARY_DIR}/src")
find_package(metaSMT REQUIRED)
add_definitions("${metaSMT_CXXFLAGS}")

### includes
include_directories(${metaSMT_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GLOG_INCLUDE_DIRS})

if(SystemC_FOUND)
  include_directories(${SystemC_INCLUDE_DIR})
endif(SystemC_FOUND)

enable_testing ()

add_subdirectory(src)

set(${PROJECT_NAME}_LIBS ${CMAKE_INSTALL_PREFIX}/lib/lib${PROJECT_NAME}.a ${metaSMT_LIBRARIES} ${GLOG_LIBRARIES})
set(${PROJECT_NAME}_INCLUDE ${CMAKE_INSTALL_PREFIX}/include ${metaSMT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GLOG_INCLUDE_DIRS})

if(SystemC_FOUND)
  LIST(APPEND ${PROJECT_NAME}_INCLUDE ${SystemC_INCLUDE_DIR})
  LIST(APPEND ${PROJECT_NAME}_LIBS ${SystemC_LIBRARY})
endif(SystemC_FOUND)

if( Boost_FILESYSTEM_FOUND )
  list(APPEND ${PROJECT_NAME}_LIBS ${Boost_FILESYSTEM_LIBRARY})
endif()

if( Boost_THREAD_FOUND )
  list(APPEND ${PROJECT_NAME}_LIBS ${Boost_THREAD_LIBRARY})
endif()

if( Boost_SYSTEM_FOUND )
  list(APPEND ${PROJECT_NAME}_LIBS ${Boost_SYSTEM_LIBRARY})
endif()

## create CRAVE CMake config file
file(WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
"set(${PROJECT_NAME}_FOUND 1)
set(${PROJECT_NAME}_INCLUDE_DIR ${${PROJECT_NAME}_INCLUDE})
set(${PROJECT_NAME}_LIBRARIES ${${PROJECT_NAME}_LIBS})
")

## create CRAVE py config file
file(WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.py
"${PROJECT_NAME}_INCLUDES = '${${PROJECT_NAME}_INCLUDE}'
${PROJECT_NAME}_LIBRARIES = '${${PROJECT_NAME}_LIBS}'
")

string(REPLACE ";" " " ${PROJECT_NAME}_MLIBS "${${PROJECT_NAME}_LIBS}")
string(REPLACE ";" " -I" ${PROJECT_NAME}_MINCLUDE "${${PROJECT_NAME}_INCLUDE}")
set(${PROJECT_NAME}_RPATH "")
foreach(LIBPATH ${${PROJECT_NAME}_LIBS})
  get_filename_component(LIBDIR ${LIBPATH} PATH)
  LIST(APPEND ${PROJECT_NAME}_RPATH -Wl,-rpath=${LIBDIR})
endforeach()  
string(REPLACE ";" " " ${PROJECT_NAME}_RPATH "${${PROJECT_NAME}_RPATH}")

## create CRAVE CMake make config file
file(WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.makefile
"${PROJECT_NAME}_INCLUDES := ${${PROJECT_NAME}_MINCLUDE}
${PROJECT_NAME}_LIBRARIES := ${${PROJECT_NAME}_MLIBS} 
${PROJECT_NAME}_RPATH := ${${PROJECT_NAME}_RPATH}
")

## create CRAVE pkgconfig make config file
string(REPLACE ";" " " ${PROJECT_NAME}_MLIBS "${${PROJECT_NAME}_LIBS}")
file(WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc
"Name: ${PROJECT_NAME}
Description: Constraint Solver library
Cflags: ${${PROJECT_NAME}_MINCLUDE}
Libs: ${${PROJECT_NAME}_MLIBS} 
")

## install
INSTALL( FILES 
	${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
	${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.py
	${PROJECT_BINARY_DIR}/${PROJECT_NAME}.makefile
	DESTINATION ${${PROJECT_NAME}_CONFIG_DIR})
INSTALL( FILES 
	${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc
	DESTINATION lib/pkgconfig/)

add_subdirectory ( tests )
add_subdirectory ( examples )

############################################################
##                CPack settings                          ##
############################################################

option( CRAVE_BUNDLE_PACKAGE_BY_DATE "use date for package name instead of git description" on)

if( CRAVE_BUNDLE_PACKAGE_BY_DATE )

  # CPack version numbers for release tarball name.
  if(UNIX)
  execute_process(COMMAND date +%Y OUTPUT_VARIABLE CPACK_PACKAGE_VERSION_MAJOR ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND date +%m OUTPUT_VARIABLE CPACK_PACKAGE_VERSION_MINOR ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND date +%d OUTPUT_VARIABLE CPACK_PACKAGE_VERSION_PATCH ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif()
  set(VERSION "${CPACK_PACKAGE_VERSION_MAJOR}-${CPACK_PACKAGE_VERSION_MINOR}-${CPACK_PACKAGE_VERSION_PATCH}")
else()
  include(cmake/get_git_version.cmake)
  get_git_version(VERSION crave-bundle-)
endif()

message(STATUS "crave-bundle VERSION: ${VERSION}")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CRAVE - Constrained RAndom Verification Environment for SystemC/C++")
set(CPACK_PACKAGE_VENDOR "CRAVE development team")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_GENERATOR TGZ)
set(
CPACK_SOURCE_PACKAGE_FILE_NAME
"${PROJECT_NAME}-${VERSION}"
CACHE INTERNAL "tarball basename"
)
set(CPACK_SOURCE_GENERATOR TGZ)

set(CPACK_SOURCE_IGNORE_FILES
"/\\\\.git"
"/deps.*/"
"/build.*/"
"/root.*/"
"/crave-doxygen/"
"/logs/"
"crave.cfg"
"jenkins_run_crave_check.sh"
"~$"
"^${PROJECT_SOURCE_DIR}/scripts/"
"^${PROJECT_SOURCE_DIR}/crave.*"
"^${PROJECT_SOURCE_DIR}/.*.user"
)

if (NOT CRAVE_ENABLE_EXPERIMENTAL)
  LIST(APPEND CPACK_SOURCE_IGNORE_FILES "crave/experimental/" "lib/experimental")
endif()

include(CPack)

############################################################
##                CMAKE flags settings                    ##
############################################################

SET( CMAKE_CXX_FLAGS_PROFILING "-O0 -g -Wall -Wextra -pedantic --coverage" CACHE STRING
  "Flags used by the C++ compiler during maintainer builds."
  FORCE )
SET( CMAKE_C_FLAGS_PROFILING "${CMAKE_CXX_FLAGS_PROFILING}" CACHE STRING
  "Flags used by the C compiler during maintainer builds."
  FORCE )
SET( CMAKE_EXE_LINKER_FLAGS_PROFILING
  "--coverage" CACHE STRING
  "Flags used for linking binaries during maintainer builds."
  FORCE )
SET( CMAKE_SHARED_LINKER_FLAGS_PROFILING
  " --coverage" CACHE STRING
  "Flags used by the shared libraries linker during maintainer builds."
  FORCE )
MARK_AS_ADVANCED(
  CMAKE_CXX_FLAGS_PROFILING
  CMAKE_C_FLAGS_PROFILING
  CMAKE_EXE_LINKER_FLAGS_PROFILING
  CMAKE_SHARED_LINKER_FLAGS_PROFILING )
# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
  "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Profiling."
  FORCE )
  
