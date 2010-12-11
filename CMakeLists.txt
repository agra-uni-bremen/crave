project ( platzhalter )
cmake_minimum_required(VERSION 2.6.4)

set(platzhalter_CONFIG_DIR "share/platzhalter" CACHE PATH 
  "where to install CMake and Make Config files")


find_package ( Boost REQUIRED )
find_package ( metaSMT REQUIRED )
find_package ( SystemC )

include_directories( ${metaSMT_INCLUDE_DIR} )

add_subdirectory ( src/platzhalter )

enable_testing ()
set(platzhalter_LIBS ${CMAKE_INSTALL_PREFIX}/lib/libcrub.a ${metaSMT_LIBRARIES} )
set(platzhalter_INCLUDE ${CMAKE_INSTALL_PREFIX}/include ${metaSMT_INCLUDE_DIR} )

## create metaSMT CMake config file
file(WRITE ${CMAKE_BINARY_DIR}/platzhalterConfig.cmake 
"set(platzhalter_FOUND 1)
set(platzhalter_INCLUDE_DIR ${platzhalter_INCLUDE})
set(platzhalter_LIBRARIES ${platzhalter_LIBS})
")

string(REPLACE ";" " " platzhalter_MLIBS "${platzhalter_LIBS}")
string(REPLACE ";" " -I" platzhalter_MINCLUDE "${platzhalter_INCLUDE}")
## create platzhalter CMake make config file
file(WRITE ${CMAKE_BINARY_DIR}/platzhalter.makefile
"platzhalter_INCLUDES := ${platzhalter_MINCLUDE}
platzhalter_LIBRARIES := ${platzhalter_MLIBS} 
")

## create platzhalter pkgconfig make config file
string(REPLACE ";" " " platzhalter_MLIBS "${platzhalter_LIBS}")
file(WRITE ${CMAKE_BINARY_DIR}/platzhalter.pc
"Name: platzhalter
Description: SMT Solver library
Cflags: ${platzhalter_MINCLUDE}
Libs: ${platzhalter_MLIBS} 
")

INSTALL( FILES 
	${CMAKE_BINARY_DIR}/platzhalterConfig.cmake 
	${CMAKE_BINARY_DIR}/platzhalter.makefile
	DESTINATION ${platzhalter_CONFIG_DIR})
INSTALL( FILES 
	${CMAKE_BINARY_DIR}/platzhalter.pc
	DESTINATION lib/pkgconfig/)


add_subdirectory ( tests )


