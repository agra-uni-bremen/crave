project ( platzhalter )
cmake_minimum_required(VERSION 2.6.4)

set(CMAKE_MODULE_PATH /home/radames/sanitas/05_WorkPackages/AP2/platzhalter/)

find_package ( Boost REQUIRED )
find_package ( metaSMT REQUIRED )
find_package ( SystemC )

enable_testing ()

include_directories( ${metaSMT_INCLUDE_DIR} )

add_subdirectory ( tests )


