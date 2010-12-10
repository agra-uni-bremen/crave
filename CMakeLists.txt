project ( platzhalter )
cmake_minimum_required(VERSION 2.6.4)

find_package ( Boost REQUIRED )
find_package ( metaSMT REQUIRED )
find_package ( SystemC )

include_directories( ${metaSMT_INCLUDE_DIR} )

add_subdirectory ( src/platzhalter )

enable_testing ()

add_subdirectory ( tests )


