project(Aiger)

cmake_minimum_required(VERSION 2.8)

set(STATIC_OR_SHARED SHARED CACHE STRING "Build STATIC or SHARED libaries")


add_library(Aiger
  ${STATIC_OR_SHARED}
  aiger.c
)


file(WRITE ${PROJECT_BINARY_DIR}/AigerConfig.cmake 
"set(Aiger_FOUND 1)
set(Aiger_VERSION 20071012)
get_filename_component(Aiger_CONFIG_DIR "\${CMAKE_CURRENT_LIST_FILE}" PATH)
set(Aiger_INCLUDE_DIR \${Aiger_CONFIG_DIR}/../../include)
include("\${Aiger_CONFIG_DIR}/Aiger.cmake" )
")
file(WRITE ${PROJECT_BINARY_DIR}/AigerConfigVersion.cmake 
"if(\"\${PACKAGE_FIND_VERSION}\" VERSION_EQUAL 20071012)
	set(PACKAGE_VERSION_EXACT 1)
	set(PACKAGE_VERSION_COMPATIBLE 1)
endif(\"\${PACKAGE_FIND_VERSION}\" VERSION_EQUAL 20071012)
")
## create libAiger config file for internal use
file(WRITE ${PROJECT_BINARY_DIR}/Aiger.cmake 
"set(Aiger_INCLUDE_DIR 
  ${PROJECT_SOURCE_DIR}
)
")
## export target with install
INSTALL( FILES
  ${PROJECT_BINARY_DIR}/AigerConfig.cmake
  ${PROJECT_BINARY_DIR}/AigerConfigVersion.cmake
  DESTINATION share/Aiger)
install(EXPORT Aiger DESTINATION share/Aiger)

install(TARGETS 
  Aiger
  EXPORT Aiger
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)

install(FILES
  aiger.h
  DESTINATION include 
)
