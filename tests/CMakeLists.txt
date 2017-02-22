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
    add_test("${prog}/${test}" ${prog} -t ${test})
    set_tests_properties ("${prog}/${test}" PROPERTIES TIMEOUT ${CRAVE_TIMEOUT})
  endforeach(test)
  
endfunction(add_all_tests)

add_library (${PROJECT_NAME}_boost_test ${STATIC_OR_SHARED} boost_test.cpp)
link_libraries (${PROJECT_NAME}_boost_test ${ALL_EXTERNAL_LIBS} ${PROJECT_NAME})

add_executable(solver_independent_tests core/solver_independent_tests.cpp)
add_all_tests(solver_independent_tests core/solver_independent_tests.cpp)

if (CRAVE_ENABLE_EXPERIMENTAL)
  add_executable(experimental_tests experimental/experimental_tests.cpp)
  add_all_tests(experimental_tests experimental/experimental_tests.cpp)
endif (CRAVE_ENABLE_EXPERIMENTAL)

if (metaSMT_USE_Boolector)
  add_executable(use_Boolector core/use_Boolector.cpp)
  add_all_tests(use_Boolector core/use_Boolector.cpp)
endif (metaSMT_USE_Boolector)

if (metaSMT_USE_CVC4)
  add_executable(use_CVC4 core/use_CVC4.cpp)
  add_all_tests(use_CVC4 core/use_CVC4.cpp)
endif (metaSMT_USE_CVC4)

if (metaSMT_USE_STP)
  add_executable(use_STP core/use_STP.cpp)
  add_all_tests(use_STP core/use_STP.cpp)
endif (metaSMT_USE_STP)

if (metaSMT_USE_SWORD)
  add_executable(use_SWORD core/use_SWORD.cpp)
  add_all_tests(use_SWORD core/use_SWORD.cpp)
endif (metaSMT_USE_SWORD)

if (metaSMT_USE_YICES2)
  add_executable(use_YICES2 core/use_Yices2.cpp)
  add_all_tests(use_YICES2 core/use_Yices2.cpp)
endif (metaSMT_USE_YICES2)

if (metaSMT_USE_Z3)
  add_executable(use_Z3 core/use_Z3.cpp)
  add_all_tests(use_Z3 core/use_Z3.cpp)
endif (metaSMT_USE_Z3)

#if (metaSMT_USE_CUDD)
#  add_executable(use_Cudd use_Cudd.cpp)
#  add_all_tests(use_Cudd use_Cudd.cpp)
#endif (metaSMT_USE_CUDD)

if(SystemC_FOUND)
  add_executable (SystemC_Data core/test_SystemC_Data.cpp)
  add_all_tests( SystemC_Data core/test_SystemC_Data.cpp)
  target_link_libraries ( SystemC_Data ${ALL_EXTERNAL_LIBS} ${PROJECT_NAME})
endif(SystemC_FOUND)


