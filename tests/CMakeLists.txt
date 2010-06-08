include_directories (../src)

add_library (boost_test boost_test.cpp)
link_libraries (${metaSMT_LIBRARIES} boost_test)

add_executable (test_Syntax test_Syntax.cpp )
add_test("test_Syntax" test_Syntax )

add_executable (test_Context test_Context.cpp )
add_test("test_Context" test_Context )

