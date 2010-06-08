project ( platzhalter )
cmake_minimum_required(VERSION 2.8)

find_package ( Boost REQUIRED )
find_package ( metaSMT REQUIRED )

enable_testing ()

include_directories( ${metaSMT_INCLUDE_DIR} )

add_subdirectory ( tests )
