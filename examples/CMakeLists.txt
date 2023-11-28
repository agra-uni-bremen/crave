set(EXAMPLES
    # ex1_seed_dist
    ex2_constr_ref
    ex3_inh_soft_next
    # ex4_constr_mng ex5_vec_constr
    ex6_inline_gen
    # ex7_rand_enum ex8_make_expression sudoku sudoku_free ahb_mem_burst
)

foreach(ex ${EXAMPLES})
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ex}")
    add_executable(${ex} ${ex}/main.cpp)
    target_include_directories(
      ${ex} PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
                    $<INSTALL_INTERFACE:include>)

    target_link_directories(
      ${ex} PRIVATE $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/lib>
      $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/metaSMT/lib>
      $<INSTALL_INTERFACE:lib>)
    target_link_libraries(${ex} ${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})
  endif()
endforeach(ex)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sudoku_free_perf")
  add_subdirectory(sudoku_free_perf)
endif()

if(SystemCLanguage_FOUND)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ALU")
    add_subdirectory(ALU)
  endif()
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/random_stability")
    add_subdirectory(random_stability)
  endif()
endif(SystemCLanguage_FOUND)
