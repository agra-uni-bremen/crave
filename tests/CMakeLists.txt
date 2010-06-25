include_directories (../src)

add_library (boost_test boost_test.cpp)
link_libraries (${metaSMT_LIBRARIES} boost_test)

add_executable (test_Syntax test_Syntax.cpp )
add_test("test_Syntax" test_Syntax )

add_executable (test_Context test_Context.cpp )
add_test("test_Context" test_Context )


if(SystemC_FOUND)
  find_package(Threads REQUIRED)
  include_directories(${SystemC_INCLUDE_DIR})
  add_executable (test_SystemC_Data test_SystemC_Data.cpp)
  target_link_libraries ( test_SystemC_Data ${SystemC_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  add_test("test_SystemC_Data" test_SystemC_Data)
endif(SystemC_FOUND)

