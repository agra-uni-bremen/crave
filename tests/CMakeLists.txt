include_directories(../src)

include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
  Doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG v2.4.9
  GIT_SHALLOW TRUE
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}
             -DDOCTEST_WITH_TESTS=OFF)

add_executable(
  ${PROJECT_NAME}_tests
  # core/test_Constraint_Management.cpp
  core/test_Context.cpp
  # core/test_Distribution.cpp
  # core/test_Evaluations.cpp
  # core/test_Operators.cpp
  # core/test_Random_Object.cpp
  # core/test_Syntax.cpp
  # core/test_UserConstraint.cpp
  # core/test_Vector_Constraint.cpp
  main.cpp)

if(SystemCLanguage_FOUND)
  # target_sources(${PROJECT_NAME}_tests PRIVATE core/test_SystemC_Data.cpp)
endif(SystemCLanguage_FOUND)

target_include_directories(
  ${PROJECT_NAME}_tests PRIVATE $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
                                $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
                                $<INSTALL_INTERFACE:include>)

target_link_directories(
  ${PROJECT_NAME}_tests PRIVATE $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/lib>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/metaSMT/lib> $<INSTALL_INTERFACE:lib>)
target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME}
                      ${${PROJECT_NAME}_LIBS})

get_target_property(LIB_DIRS ${PROJECT_NAME}_tests LINK_DIRECTORIES)
message(WARNING "Lib dirs: ${LIB_DIRS}")

get_target_property(LIBS ${PROJECT_NAME}_tests LINK_LIBRARIES)
message(WARNING "Libs: ${LIBS}")

if(metaSMT_USE_Boolector)
  add_test(NAME Boolector_backend COMMAND ${PROJECT_NAME}_tests
                                          --backend-Boolector)
  set_tests_properties(Boolector_backend PROPERTIES TIMEOUT 30)
endif()
if(metaSMT_USE_STP)
  add_test(NAME STP_backend COMMAND ${PROJECT_NAME}_tests --backend-STP)
  set_tests_properties(STP_backend PROPERTIES TIMEOUT 30)
endif()
if(metaSMT_USE_CVC4)
  add_test(NAME CVC4_backend COMMAND ${PROJECT_NAME}_tests --backend-CVC4)
  set_tests_properties(CVC4_backend PROPERTIES TIMEOUT 30)
endif()
if(metaSMT_USE_CUDD)
  add_test(NAME CUDD_backend COMMAND ${PROJECT_NAME}_tests --backend-CUDD)
  set_tests_properties(CUDD_backend PROPERTIES TIMEOUT 30)
endif()
if(metaSMT_USE_Z3)
  add_test(NAME Z3_backend COMMAND ${PROJECT_NAME}_tests --backend-Z3)
  set_tests_properties(Z3_backend PROPERTIES TIMEOUT 30)
endif()
