if ( NOT CRAVE_TIMEOUT )
  set(CRAVE_TIMEOUT 15)
endif()
include_directories (../src)

find_program(AWK awk)
function (add_all_tests prog cfile)
  execute_process( COMMAND
    ${AWK} -f search_tests.awk ${cfile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE test_cases
  )
  
  string(REPLACE "\n" ";" test_cases "${test_cases}")

  foreach(test  ${test_cases})
    #message(STATUS "${prog}/${test} ${prog} -t ${test}")
    add_test("${prog}/${test}" ${prog} -t ${test})
    set_tests_properties ("${prog}/${test}" PROPERTIES TIMEOUT ${CRAVE_TIMEOUT})
  endforeach(test)
endfunction(add_all_tests)

find_library(PTHREAD pthread)

add_library (${PROJECT_NAME}_boost_test ${STATIC_OR_SHARED} boost_test.cpp)
link_libraries (${PROJECT_NAME}_boost_test ${PROJECT_NAME} ${Boost_THREAD_LIBRARY} ${PTHREAD} )

add_executable(use_Boolector use_Boolector.cpp)
add_all_tests(use_Boolector use_Boolector.cpp)

add_executable(use_Z3 use_Z3.cpp)
add_all_tests(use_Z3 use_Z3.cpp)

add_executable(use_SWORD use_SWORD.cpp)
add_all_tests(use_SWORD use_SWORD.cpp)

add_executable(use_Cudd use_Cudd.cpp)
add_all_tests(use_Cudd use_Cudd.cpp)

add_executable(use_Priority use_Priority.cpp)
add_all_tests(use_Priority use_Priority.cpp)

if(SystemC_FOUND)
  find_package(Threads REQUIRED)
  include_directories(${SystemC_INCLUDE_DIR})
  add_executable (SystemC_Data test_SystemC_Data.cpp)
  add_all_tests( SystemC_Data test_SystemC_Data.cpp)
  target_link_libraries ( SystemC_Data ${SystemC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${PROJECT_NAME})
endif(SystemC_FOUND)

