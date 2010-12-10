set(TIMEOUT 5)
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
    set_tests_properties ("${prog}/${test}" PROPERTIES TIMEOUT ${TIMEOUT})
  endforeach(test)
endfunction(add_all_tests)

add_executable(demo test)
target_link_libraries(demo ${metaSMT_LIBRARIES})

add_library (boost_test boost_test.cpp)
link_libraries (${metaSMT_LIBRARIES} boost_test)

add_executable (Syntax test_Syntax.cpp )
add_all_tests(Syntax test_Syntax.cpp )

add_executable (Context test_Context.cpp )
add_all_tests(Context test_Context.cpp )

add_executable (Inheritence test_Inheritence.cpp )
add_all_tests(Inheritence test_Inheritence.cpp )

add_executable (Constraint_Management test_Constraint_Management.cpp )
add_all_tests(Constraint_Management test_Constraint_Management.cpp )

if(SystemC_FOUND)
  find_package(Threads REQUIRED)
  include_directories(${SystemC_INCLUDE_DIR})
  add_executable (test_SystemC_Data test_SystemC_Data.cpp)
  target_link_libraries ( test_SystemC_Data ${SystemC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  add_test("test_SystemC_Data" test_SystemC_Data)
endif(SystemC_FOUND)

