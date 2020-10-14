set(MiniSat_FOUND 1)
set(MiniSat_VERSION @MiniSat_VERSION@)
get_filename_component(MiniSat_CONFIG_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(MiniSat_INCLUDE_DIR ${MiniSat_CONFIG_DIR}/../../include)
set(MiniSat_INCLUDE_DIRS ${MiniSat_INCLUDE_DIR})
set(MiniSat_LIBRARIES @MiniSat_LIBRARIES@)
set(MiniSat_CXXFLAGS "@MiniSat_CXXFLAGS@")
include("${MiniSat_CONFIG_DIR}/MiniSat.cmake" )

