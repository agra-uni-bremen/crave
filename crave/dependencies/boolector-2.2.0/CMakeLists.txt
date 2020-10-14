project(Boolector)

cmake_minimum_required(VERSION 2.8)

find_package(Lingeling REQUIRED)
include_directories( ${Lingeling_INCLUDE_DIR} )

find_package(MiniSat REQUIRED)
include_directories( ${MiniSat_INCLUDE_DIR} )

set(STATIC_OR_SHARED SHARED CACHE STRING "Build STATIC or SHARED libaries")

set(OS $ENV{HOSTNAME})
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.10)
  string(TIMESTAMP COMPILED_TIMESTAMP)
endif()

set(BTOR_C_FLAGS "-DBTOR_USE_LINGELING -DBTOR_USE_MINISAT -DBTOR_ENABLE_BETA_REDUCTION_PROBING")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG -DNBTORLOG ${BTOR_C_FLAGS}")

set(SRC_PREFIX boolector-src)

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/${SRC_PREFIX}/VERSION Boolector_VERSION)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/btorconfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/${SRC_PREFIX}/btorconfig.h)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${SRC_PREFIX})

set( libboolector_HEADER
${SRC_PREFIX}/boolector.h
${SRC_PREFIX}/btortypes.h
)

set( libboolector_SRC
${SRC_PREFIX}/boolector.c
${SRC_PREFIX}/btor2horn.c
${SRC_PREFIX}/btoraig.c
${SRC_PREFIX}/btoraigvec.c
${SRC_PREFIX}/btorass.c
${SRC_PREFIX}/btorbeta.c
${SRC_PREFIX}/btorbitvec.c
${SRC_PREFIX}/btorchkclone.c
${SRC_PREFIX}/btorclone.c
${SRC_PREFIX}/btorconst.c
${SRC_PREFIX}/btorcore.c
${SRC_PREFIX}/btordbg.c
${SRC_PREFIX}/btordcr.c
${SRC_PREFIX}/btorexp.c
${SRC_PREFIX}/btorfmt.c
${SRC_PREFIX}/utils/btorhash.c
${SRC_PREFIX}/utils/btorinthash.c
${SRC_PREFIX}/utils/btoriter.c
${SRC_PREFIX}/btormain.c
${SRC_PREFIX}/utils/btormap.c
${SRC_PREFIX}/btormc.c
${SRC_PREFIX}/utils/btormem.c
${SRC_PREFIX}/utils/btormisc.c
${SRC_PREFIX}/btormodel.c
${SRC_PREFIX}/btormsg.c
${SRC_PREFIX}/btoropt.c
${SRC_PREFIX}/utils/btorparamcache.c
${SRC_PREFIX}/btorparse.c
${SRC_PREFIX}/btorprintmodel.c
${SRC_PREFIX}/btorrewrite.c
${SRC_PREFIX}/btorsat.c
${SRC_PREFIX}/btorsort.c
${SRC_PREFIX}/btortrapi.c
${SRC_PREFIX}/utils/btorutil.c
${SRC_PREFIX}/parser/btorsmt2.c
${SRC_PREFIX}/parser/btorsmt.c
${SRC_PREFIX}/parser/btorbtor.c
${SRC_PREFIX}/dumper/btordumpbtor.c
${SRC_PREFIX}/dumper/btordumpsmt.c
${SRC_PREFIX}/dumper/btordumpaig.c
${SRC_PREFIX}/simplifier/btorelimslices.c
${SRC_PREFIX}/simplifier/btorextract.c
${SRC_PREFIX}/simplifier/btormerge.c
${SRC_PREFIX}/simplifier/btorelimapplies.c
${SRC_PREFIX}/simplifier/btorack.c
${SRC_PREFIX}/simplifier/btorskel.c
${SRC_PREFIX}/simplifier/btorunconstrained.c
${SRC_PREFIX}/btorminisat.cc
)

set_source_files_properties(${SRC_PREFIX}/btorminisat.cc PROPERTIES LANGUAGE C )

add_library(Boolector_libboolector
  ${STATIC_OR_SHARED}
  ${libboolector_SRC}
)
target_link_libraries(Boolector_libboolector ${Lingeling_LIBRARIES} ${MiniSat_LIBRARIES} m) 
set_target_properties(Boolector_libboolector PROPERTIES OUTPUT_NAME boolector)

add_executable(Boolector_boolector
  ${SRC_PREFIX}/boolectormain.c
)
target_link_libraries(Boolector_boolector Boolector_libboolector)
SET_TARGET_PROPERTIES( Boolector_boolector PROPERTIES
  OUTPUT_NAME boolector
)

file(WRITE ${PROJECT_BINARY_DIR}/BoolectorConfig.cmake 
"set(Boolector_FOUND 1)
set(Boolector_VERSION ${Boolector_VERSION})
get_filename_component(Boolector_CONFIG_DIR \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)
set(Boolector_INCLUDE_DIR \${Boolector_CONFIG_DIR}/../../include)
set(Boolector_LIBRARIES Boolector_libboolector)
set(Boolector_CXXFLAGS \"${BTOR_C_FLAGS}\")
include(\"\${Boolector_CONFIG_DIR}/Boolector.cmake\" )
")
file(WRITE ${PROJECT_BINARY_DIR}/BoolectorConfigVersion.cmake 
"if(\"\${PACKAGE_FIND_VERSION}\" VERSION_EQUAL ${Boolector_VERSION})
set(PACKAGE_VERSION_EXACT 1)
set(PACKAGE_VERSION_COMPATIBLE 1)
endif(\"\${PACKAGE_FIND_VERSION}\" VERSION_EQUAL ${Boolector_VERSION})
")
## create libBoolector config file for internal use
file(WRITE ${PROJECT_BINARY_DIR}/Boolector.cmake 
"set(Boolector_INCLUDE_DIR 
  ${PROJECT_SOURCE_DIR}/include
)
")
## export target with install
INSTALL( FILES
  ${PROJECT_BINARY_DIR}/BoolectorConfig.cmake
  ${PROJECT_BINARY_DIR}/BoolectorConfigVersion.cmake
  DESTINATION share/Boolector)
install(EXPORT Boolector DESTINATION share/Boolector)

install(TARGETS 
  Boolector_libboolector
  Boolector_boolector
  EXPORT Boolector
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)

install(FILES
  ${libboolector_HEADER}
  DESTINATION include 
)

# vim: sw=2 ts=2 et:
