set(zChaff_FOUND 1)
set(zChaff_VERSION @zChaff_VERSION@)
get_filename_component(zChaff_CONFIG_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(zChaff_INCLUDE_DIR ${zChaff_CONFIG_DIR}/../../include)
set(zChaff_INCLUDE_DIRS ${zChaff_INCLUDE_DIR})
set(zChaff_LIBRARIES @zChaff_LIBRARIES@)

set(zchaff_BIN_DIR      @CMAKE_INSTALL_PREFIX@/bin )
set(zchaff_BIN_DIRS     ${zchaff_BIN_DIR} )
set(zchaff_INCLUDE_DIR  ${zChaff_INCLUDE_DIR} )
set(zchaff_INCLUDE_DIRS ${zChaff_INCLUDE_DIR} )
set(zchaff_LIBRARIES    ${zChaff_LIBRARIES} )

include("${zChaff_CONFIG_DIR}/zChaff.cmake" )

