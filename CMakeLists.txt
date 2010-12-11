project ( platzhalter )
cmake_minimum_required(VERSION 2.6.4)

set(${PROJECT_NAME}_CONFIG_DIR "share/${PROJECT_NAME}" CACHE PATH 
  "where to install CMake and Make Config files")


find_package ( Boost REQUIRED )
find_package ( metaSMT REQUIRED )
find_package ( SystemC )

include_directories( ${metaSMT_INCLUDE_DIR} )

add_subdirectory ( src/platzhalter )

enable_testing ()
set(${PROJECT_NAME}_LIBS ${CMAKE_INSTALL_PREFIX}/lib/libcrub.a ${metaSMT_LIBRARIES} )
set(${PROJECT_NAME}_INCLUDE ${CMAKE_INSTALL_PREFIX}/include ${metaSMT_INCLUDE_DIR} )

## create metaSMT CMake config file
file(WRITE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake 
"set(${PROJECT_NAME}_FOUND 1)
set(${PROJECT_NAME}_INCLUDE_DIR ${${PROJECT_NAME}_INCLUDE})
set(${PROJECT_NAME}_LIBRARIES ${${PROJECT_NAME}_LIBS})
")

string(REPLACE ";" " " ${PROJECT_NAME}_MLIBS "${${PROJECT_NAME}_LIBS}")
string(REPLACE ";" " -I" ${PROJECT_NAME}_MINCLUDE "${${PROJECT_NAME}_INCLUDE}")
## create ${PROJECT_NAME} CMake make config file
file(WRITE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.makefile
"${PROJECT_NAME}_INCLUDES := ${${PROJECT_NAME}_MINCLUDE}
${PROJECT_NAME}_LIBRARIES := ${${PROJECT_NAME}_MLIBS} 
")

## create ${PROJECT_NAME} pkgconfig make config file
string(REPLACE ";" " " ${PROJECT_NAME}_MLIBS "${${PROJECT_NAME}_LIBS}")
file(WRITE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc
"Name: ${PROJECT_NAME}
Description: SMT Solver library
Cflags: ${${PROJECT_NAME}_MINCLUDE}
Libs: ${${PROJECT_NAME}_MLIBS} 
")

INSTALL( FILES 
	${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake 
	${CMAKE_BINARY_DIR}/${PROJECT_NAME}.makefile
	DESTINATION ${${PROJECT_NAME}_CONFIG_DIR})
INSTALL( FILES 
	${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc
	DESTINATION lib/pkgconfig/)


add_subdirectory ( tests )


