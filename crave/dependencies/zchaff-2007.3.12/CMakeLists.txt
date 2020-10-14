project(zChaff)
cmake_minimum_required(VERSION 2.8)
set(zChaff_VERSION)

set(STATIC_OR_SHARED SHARED CACHE STRING "Build STATIC or SHARED libaries")

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/zchaff_c_wrapper.cpp
  COMMAND sed 's/EXTERN/extern \"C\"/' ${CMAKE_CURRENT_SOURCE_DIR}/zchaff_wrapper.wrp > ${CMAKE_CURRENT_BINARY_DIR}/zchaff_c_wrapper.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/zchaff_wrapper.wrp
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/zchaff_cpp_wrapper.cpp
  COMMAND sed 's/EXTERN//' ${CMAKE_CURRENT_SOURCE_DIR}/zchaff_wrapper.wrp > ${CMAKE_CURRENT_BINARY_DIR}/zchaff_cpp_wrapper.cpp
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/zchaff_wrapper.wrp
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

set( zChaff_SOURCES
  zchaff_solver.cpp
  zchaff_base.cpp
  zchaff_dbase.cpp
  zchaff_utils.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/zchaff_c_wrapper.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/zchaff_cpp_wrapper.cpp
)

add_library(zChaff_libzChaff  
  ${STATIC_OR_SHARED}
  ${zChaff_SOURCES}
)
SET_TARGET_PROPERTIES( zChaff_libzChaff PROPERTIES
  OUTPUT_NAME zchaff
)

add_executable(zChaff
  sat_solver.cpp
  ${zChaff_SOURCES}
)
SET_TARGET_PROPERTIES( zChaff PROPERTIES
  OUTPUT_NAME zchaff
)

add_executable(zChaff_verify_df
  zverify_df.cpp
)
SET_TARGET_PROPERTIES( zChaff_verify_df PROPERTIES
  OUTPUT_NAME zverify_df
)

add_executable(zChaff_zminimal
  zminimal.cpp
  ${zChaff_SOURCES}
)
SET_TARGET_PROPERTIES( zChaff_zminimal PROPERTIES
  OUTPUT_NAME zminimal
)

add_executable(zChaff_cnf_stats
  cnf_stats.cpp
)
SET_TARGET_PROPERTIES( zChaff_cnf_stats PROPERTIES
  OUTPUT_NAME cnf_stats
)

set(zChaff_LIBRARIES zChaff_libzChaff)

configure_file(zChaffConfig.cmake ${PROJECT_BINARY_DIR}/zChaffConfig.cmake @ONLY)
configure_file(zChaffConfigVersion.cmake ${PROJECT_BINARY_DIR}/zChaffConfigVersion.cmake @ONLY)
## create libzChaff config file for internal use
file(WRITE ${PROJECT_BINARY_DIR}/zChaff.cmake 
"set(zChaff_INCLUDE_DIR 
  ${PROJECT_SOURCE_DIR}
)
")
## export target with install
INSTALL( FILES
  ${PROJECT_BINARY_DIR}/zChaffConfig.cmake
  ${PROJECT_BINARY_DIR}/zChaffConfigVersion.cmake
  DESTINATION share/zChaff)
install(EXPORT zChaff DESTINATION share/zChaff)

install(TARGETS 
  zChaff_libzChaff
  zChaff
  zChaff_verify_df
  zChaff_zminimal
  zChaff_cnf_stats
  EXPORT zChaff
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)


install(FILES
  SAT.h
  zchaff_clsgen.h
  zchaff_header.h
  zchaff_version.h
  zchaff_base.h
  zchaff_dbase.h
  zchaff_solver.h
  DESTINATION include)
