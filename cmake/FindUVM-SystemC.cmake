#______________________________________________________________________________
#
#   This CMake package creates a UVM-SystemC target
#______________________________________________________________________________
#
#  UVM_SystemC_FOUND - system has UVM-SystemC
#  UVM_SystemC_INCLUDE_DIRS - the UVM-SystemC include directory
#  UVM_SystemC_LIBRARIES - Link these to use UVM-SystemC

message("UVM_SYSTEMC_HOME:" $ENV{UVM_SYSTEMC_HOME})
IF(NOT EXISTS "$ENV{UVM_SYSTEMC_HOME}")
  SET(UVM_SYSTEMC_HOME  ${PROJECT_SOURCE_DIR}/../)
ENDIF(NOT EXISTS "$ENV{UVM_SYSTEMC_HOME}")

IF(UVM_SystemC_LIBRARIES AND UVM_SystemC_INCLUDE_DIRS)
  SET(UVM_SystemC_FOUND TRUE)
ELSE(UVM_SystemC_LIBRARIES AND UVM_SystemC_INCLUDE_DIRS)
  FIND_PATH(UVM_SystemC_INCLUDE_DIR
    HINTS
      ${UVM_SYSTEMC_HOME}/include
      $ENV{UVM_SYSTEMC_HOME}/include
    NAMES
      uvm.h
  )

  FIND_LIBRARY(UVM_SystemC_LIBRARY
    HINTS
      ${UVM_SYSTEMC_HOME}/lib
      ${UVM_SYSTEMC_HOME}/lib-linux
      ${UVM_SYSTEMC_HOME}/lib-linux64
      ${UVM_SYSTEMC_HOME}/lib-macos
      $ENV{UVM_SYSTEMC_HOME}/lib
      $ENV{UVM_SYSTEMC_HOME}/lib-linux
      $ENV{UVM_SYSTEMC_HOME}/lib-linux64
      $ENV{UVM_SYSTEMC_HOME}/lib-macos
    NAMES
      uvm-systemc
  )

SET(UVM_SystemC_INCLUDE_DIRS ${UVM_SystemC_INCLUDE_DIR})
SET(UVM_SystemC_LIBRARIES ${UVM_SystemC_LIBRARY})

IF(UVM_SystemC_INCLUDE_DIRS AND UVM_SystemC_LIBRARIES)
  SET(UVM_SystemC_FOUND TRUE)
ENDIF(UVM_SystemC_INCLUDE_DIRS AND UVM_SystemC_LIBRARIES)

IF(UVM_SystemC_FOUND)
  IF(NOT UVM_SystemC_FIND_QUIETLY)
    MESSAGE(STATUS "Found UVM_SystemC: ${UVM_SystemC_LIBRARIES}")
  ENDIF(NOT UVM_SystemC_FIND_QUIETLY)
ELSE(UVM_SystemC_FOUND)
  IF(UVM_SystemC_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find UVM_SystemC")
  ENDIF(UVM_SystemC_FIND_REQUIRED)
ENDIF(UVM_SystemC_FOUND)

MARK_AS_ADVANCED(UVM_SystemC_INCLUDE_DIRS UVM_SystemC_LIBRARIES)

ENDIF(UVM_SystemC_LIBRARIES AND UVM_SystemC_INCLUDE_DIRS) 
