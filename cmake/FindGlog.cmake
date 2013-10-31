#______________________________________________________________________________
#
#   This CMake package creates a Glog target
#______________________________________________________________________________
#
#  GLOG_FOUND - system has Glog
#  GLOG_INCLUDE_DIRS - the Glog include directory
#  GLOG_LIBRARIES - Link these to use Glog
#  GLOG_DEFINITIONS - Compiler switches required for using Glog

IF(GLOG_LIBRARIES AND GLOG_INCLUDE_DIRS)
  SET(GLOG_FOUND TRUE)
ELSE(GLOG_LIBRARIES AND GLOG_INCLUDE_DIRS)
  FIND_PATH(GLOG_INCLUDE_DIR
    HINTS
      $ENV{GLOG_ROOT}/include
      ${GLOG_ROOT}/include
    NAMES
      glog/logging.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/include/glog/include
    PATH_SUFFIXES
      glog
  )

  FIND_LIBRARY(GLOG_LIBRARY
    HINTS
      $ENV{GLOG_ROOT}/lib
      ${GLOG_ROOT}/lib
    NAMES
      glog
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/include/glog/lib
  )

SET(GLOG_INCLUDE_DIRS ${GLOG_INCLUDE_DIR})
SET(GLOG_LIBRARIES ${GLOG_LIBRARY})

IF(GLOG_INCLUDE_DIRS AND GLOG_LIBRARIES)
  SET(GLOG_FOUND TRUE)
ENDIF(GLOG_INCLUDE_DIRS AND GLOG_LIBRARIES)

IF(GLOG_FOUND)
  IF(NOT Glog_FIND_QUIETLY)
    MESSAGE(STATUS "Found Glog: ${GLOG_LIBRARIES}")
  ENDIF(NOT Glog_FIND_QUIETLY)
ELSE(GLOG_FOUND)
  IF(Glog_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find Glog")
  ENDIF(Glog_FIND_REQUIRED)
ENDIF(GLOG_FOUND)

# show the GLOG_INCLUDE_DIRS and GLOG_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(GLOG_INCLUDE_DIRS GLOG_LIBRARIES)

ENDIF(GLOG_LIBRARIES AND GLOG_INCLUDE_DIRS)
