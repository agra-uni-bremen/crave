project ( crave )
cmake_minimum_required(VERSION 2.8)

set(${PROJECT_NAME}_CONFIG_DIR "share/${PROJECT_NAME}" CACHE PATH 
  "where to install CMake and Make Config files")

SET( CMAKE_CXX_FLAGS_PROFILING "-O0 -g -Wall -fprofile-arcs -ftest-coverage" CACHE STRING
  "Flags used by the C++ compiler during maintainer builds."
  FORCE )
SET( CMAKE_C_FLAGS_PROFILING "-O0 -g -Wall -fprofile-arcs -ftest-coverage" CACHE STRING
  "Flags used by the C compiler during maintainer builds."
  FORCE )
SET( CMAKE_EXE_LINKER_FLAGS_PROFILING
  "-fprofile-arcs -ftest-coverage" CACHE STRING
  "Flags used for linking binaries during maintainer builds."
  FORCE )
SET( CMAKE_SHARED_LINKER_FLAGS_PROFILING
  "-fprofile-arcs -ftest-coverage" CACHE STRING
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

find_package ( Boost REQUIRED COMPONENTS thread)

find_package ( metaSMT REQUIRED )
add_definitions( ${metaSMT_CXXFLAGS} )

find_package ( SystemC )

include_directories( ${metaSMT_INCLUDE_DIR} )
include_directories( ${Boost_INCLUDE_DIRS} )

add_subdirectory(src)

enable_testing ()
set(${PROJECT_NAME}_LIBS ${CMAKE_INSTALL_PREFIX}/lib/lib${PROJECT_NAME}.a ${metaSMT_LIBRARIES} )
set(${PROJECT_NAME}_INCLUDE ${CMAKE_INSTALL_PREFIX}/include ${metaSMT_INCLUDE_DIR} )

## create metaSMT CMake config file
file(WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
"set(${PROJECT_NAME}_FOUND 1)
set(${PROJECT_NAME}_INCLUDE_DIR ${${PROJECT_NAME}_INCLUDE})
set(${PROJECT_NAME}_LIBRARIES ${${PROJECT_NAME}_LIBS})
")

string(REPLACE ";" " " ${PROJECT_NAME}_MLIBS "${${PROJECT_NAME}_LIBS}")
string(REPLACE ";" " -I" ${PROJECT_NAME}_MINCLUDE "${${PROJECT_NAME}_INCLUDE}")
## create ${PROJECT_NAME} CMake make config file
file(WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.makefile
"${PROJECT_NAME}_INCLUDES := ${${PROJECT_NAME}_MINCLUDE}
${PROJECT_NAME}_LIBRARIES := ${${PROJECT_NAME}_MLIBS} 
")

## create ${PROJECT_NAME} pkgconfig make config file
string(REPLACE ";" " " ${PROJECT_NAME}_MLIBS "${${PROJECT_NAME}_LIBS}")
file(WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc
"Name: ${PROJECT_NAME}
Description: Constraint Solver library
Cflags: ${${PROJECT_NAME}_MINCLUDE}
Libs: ${${PROJECT_NAME}_MLIBS} 
")

INSTALL( FILES 
	${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
	${PROJECT_BINARY_DIR}/${PROJECT_NAME}.makefile
	DESTINATION ${${PROJECT_NAME}_CONFIG_DIR})
INSTALL( FILES 
	${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc
	DESTINATION lib/pkgconfig/)


add_subdirectory ( tests )


