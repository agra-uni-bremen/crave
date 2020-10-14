#______________________________________________________________________________
#
#   This CMake package creates a CRAVE target with all its dependencies
#______________________________________________________________________________
#
#  CRAVE_FOUND - system has CRAVE
#  CRAVE_INCLUDE_DIRS - the CRAVE include directory and include directories of all dependencies
#  CRAVE_LIBRARIES - Link these to use CRAVE. Contains CRAVE and dependencies

SET(crave_DIR ${PROJECT_SOURCE_DIR}/crave/build/root/share/crave/)
find_package(crave QUIET)
IF(crave_FOUND)
  SET(CRAVE_FOUND TRUE)
  SET(CRAVE_INCLUDE_DIRS ${crave_INCLUDE_DIR})
  SET(CRAVE_LIBRARIES ${crave_LIBRARIES})
ENDIF()

IF(CRAVE_FOUND)
  IF(NOT CRAVE_FIND_QUIETLY)
    MESSAGE(STATUS "Found CRAVE: ${CRAVE_LIBRARIES}")
  ENDIF(NOT CRAVE_FIND_QUIETLY)
ELSE(CRAVE_FOUND)
  IF(CRAVE_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find CRAVE")
  ENDIF(CRAVE_FIND_REQUIRED)
ENDIF(CRAVE_FOUND)

MARK_AS_ADVANCED(CRAVE_INCLUDE_DIRS CRAVE_LIBRARIES)

