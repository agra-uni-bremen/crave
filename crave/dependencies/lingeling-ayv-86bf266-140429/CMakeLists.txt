project(Lingeling)

cmake_minimum_required(VERSION 2.8)

set(STATIC_OR_SHARED SHARED CACHE STRING "Build STATIC or SHARED libaries")

set (Lingeling_VERSION ayv-86bf266-140429)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/code)

set(HOSTNAME $ENV{HOSTNAME})
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.10)
  STRING(TIMESTAMP COMPILATION_TIMESTAMP "%Y-%m-%dT%H:%M:%S")
endif()
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -DNDBLSCR -DNLGLOG -DNDEBUG -DNCHKSOL -DNLGLPICOSAT -DNLGLDRUPLIG -DNLGLYALSAT")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/lglcfg.h.in.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/code/lglcfg.h
  @ONLY
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/lglcflags.h.in.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/code/lglcflags.h
  @ONLY
)


add_library(Lingeling_liblingeling
  ${STATIC_OR_SHARED}
  code/lglbnr.c
  code/lgldimacs.c
  code/lglib.c
)
set_target_properties(Lingeling_liblingeling PROPERTIES OUTPUT_NAME lingeling)

add_executable(Lingeling_lingeling
  code/lglbnr.c
  code/lgldimacs.c
  code/lglib.c
  code/lglmain.c
)
target_link_libraries(Lingeling_lingeling m)

SET_TARGET_PROPERTIES( Lingeling_lingeling PROPERTIES
  OUTPUT_NAME lingeling
)


file(WRITE ${PROJECT_BINARY_DIR}/LingelingConfig.cmake
"set(Lingeling_FOUND 1)
set(Lingeling_VERSION ${Lingeling_VERSION})
get_filename_component(Lingeling_CONFIG_DIR \${CMAKE_CURRENT_LIST_FILE} PATH)
set(Lingeling_INCLUDE_DIR \${Lingeling_CONFIG_DIR}/../../include)
set(Lingeling_LIBRARIES Lingeling_liblingeling)
include(\"\${Lingeling_CONFIG_DIR}/Lingeling.cmake\" )
")
file(WRITE ${PROJECT_BINARY_DIR}/LingelingConfigVersion.cmake
"if(\"\${PACKAGE_FIND_VERSION}\" VERSION_EQUAL ${Lingeling_VERSION})
set(PACKAGE_VERSION_EXACT 1)
set(PACKAGE_VERSION_COMPATIBLE 1)
endif(\"\${PACKAGE_FIND_VERSION}\" VERSION_EQUAL ${Lingeling_VERSION})
")
## create libLingeling config file for internal use
file(WRITE ${PROJECT_BINARY_DIR}/Lingeling.cmake
"set(Lingeling_INCLUDE_DIR
  ${PROJECT_SOURCE_DIR}/include
)
")
## export target with install
INSTALL( FILES
  ${PROJECT_BINARY_DIR}/LingelingConfig.cmake
  ${PROJECT_BINARY_DIR}/LingelingConfigVersion.cmake
  DESTINATION share/Lingeling)
install(EXPORT Lingeling DESTINATION share/Lingeling)

install(TARGETS
  Lingeling_liblingeling
  Lingeling_lingeling
  EXPORT Lingeling
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)

install(FILES
  code/lglcfg.h
  code/lglcflags.h
  code/lgldimacs.h
  code/lglib.h
  DESTINATION include
)

# vim: sw=2 tabstop=2 expandtab:
