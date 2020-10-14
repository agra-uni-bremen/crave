project(PicoSAT)

cmake_minimum_required(VERSION 2.8)

set(STATIC_OR_SHARED SHARED CACHE STRING "Build STATIC or SHARED libaries")

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/VERSION PicoSAT_VERSION)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/config.h
"#define PICOSAT_CC \"${CMAKE_C_COMPILER}\"
#define PICOSAT_CFLAGS \"${CMAKE_C_FLAGS}\"
#define PICOSAT_VERSION \"${PicoSAT_VERSION}\"
")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(PicoSAT_libpicosat  
  ${STATIC_OR_SHARED}
  picosat.c
  version.c
)
set_target_properties(PicoSAT_libpicosat PROPERTIES OUTPUT_NAME picosat)

add_executable(PicoSAT_picosat
  picosat.c
  version.c
  app.c
  main.c
)
SET_TARGET_PROPERTIES( PicoSAT_picosat PROPERTIES
  OUTPUT_NAME picosat
)

add_executable(PicoSAT_picomus
  picomus.c
  picosat.c
  version.c
)
SET_TARGET_PROPERTIES( PicoSAT_picomus PROPERTIES
  OUTPUT_NAME picomus
)
  

file(WRITE ${PROJECT_BINARY_DIR}/PicoSATConfig.cmake 
"set(PicoSAT_FOUND 1)
set(PicoSAT_VERSION ${PicoSAT_VERSION})
get_filename_component(PicoSAT_CONFIG_DIR \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)
set(PicoSAT_INCLUDE_DIR \${PicoSAT_CONFIG_DIR}/../../include)
set(PicoSAT_LIBRARIES PicoSAT_libpicosat)
include(\${PicoSAT_CONFIG_DIR}/PicoSAT.cmake)
")
file(WRITE ${PROJECT_BINARY_DIR}/PicoSATConfigVersion.cmake 
"if(\"\${PACKAGE_FIND_VERSION}\" VERSION_EQUAL ${PicoSAT_VERSION})
set(PACKAGE_VERSION_EXACT 1)
set(PACKAGE_VERSION_COMPATIBLE 1)
endif(\"\${PACKAGE_FIND_VERSION}\" VERSION_EQUAL ${PicoSAT_VERSION})
")
## create libPicoSAT config file for internal use
file(WRITE ${PROJECT_BINARY_DIR}/PicoSAT.cmake 
"set(PicoSAT_INCLUDE_DIR 
  ${PROJECT_SOURCE_DIR}/include
)
")
## export target with install
INSTALL( FILES
  ${PROJECT_BINARY_DIR}/PicoSATConfig.cmake
  ${PROJECT_BINARY_DIR}/PicoSATConfigVersion.cmake
  DESTINATION share/PicoSAT)
install(EXPORT PicoSAT DESTINATION share/PicoSAT)

install(TARGETS 
  PicoSAT_libpicosat
  PicoSAT_picosat
  PicoSAT_picomus
  EXPORT PicoSAT
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)

install(FILES
  picosat.h
  DESTINATION include 
)
