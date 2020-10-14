#______________________________________________________________________________
#
#   This CMake package creates a SystemC target
#______________________________________________________________________________
#
#  SystemC_FOUND - system has SystemC
#  SystemC_INCLUDE_DIRS - the SystemC include directory
#  SystemC_LIBRARIES - Link these to use SystemC

IF(SystemC_LIBRARIES AND SystemC_INCLUDE_DIRS)
  SET(SystemC_FOUND TRUE)
ELSE(SystemC_LIBRARIES AND SystemC_INCLUDE_DIRS)
  FIND_PATH(SystemC_INCLUDE_DIR
    HINTS
      ${SYSTEMC_HOME}/include
      $ENV{SYSTEMC_HOME}/include
    NAMES
      systemc.h
  )

  FIND_LIBRARY(SystemC_LIBRARY
    HINTS
      ${SYSTEMC_HOME}/lib
      ${SYSTEMC_HOME}/lib-linux
      ${SYSTEMC_HOME}/lib-linux64
      ${SYSTEMC_HOME}/lib-macos
      $ENV{SYSTEMC_HOME}/lib
      $ENV{SYSTEMC_HOME}/lib-linux
      $ENV{SYSTEMC_HOME}/lib-linux64
	  $ENV{SYSTEMC_HOME}/lib-mingw64
	  $ENV{SYSTEMC_HOME}/lib-mingw-w64
      $ENV{SYSTEMC_HOME}/lib-macos
    NAMES
      libsystemc.a
  )

SET(SystemC_INCLUDE_DIRS ${SystemC_INCLUDE_DIR})
SET(SystemC_LIBRARIES ${SystemC_LIBRARY})

IF(SystemC_INCLUDE_DIRS AND SystemC_LIBRARIES)
  SET(SystemC_FOUND TRUE)
ENDIF(SystemC_INCLUDE_DIRS AND SystemC_LIBRARIES)

IF(SystemC_FOUND)
  IF(NOT SystemC_FIND_QUIETLY)
    MESSAGE(STATUS "Found SystemC: ${SystemC_LIBRARIES}")
  ENDIF(NOT SystemC_FIND_QUIETLY)
ELSE(SystemC_FOUND)
  IF(SystemC_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find SystemC")
  ENDIF(SystemC_FIND_REQUIRED)
ENDIF(SystemC_FOUND)

# show the SystemC_INCLUDE_DIRS and SystemC_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(SystemC_INCLUDE_DIRS SystemC_LIBRARIES)

ENDIF(SystemC_LIBRARIES AND SystemC_INCLUDE_DIRS)
