foreach(ex IN ITEMS ALU4 ALU12 ALU16 ALU24 ALU32)
  add_executable(${ex} ${ex}.cpp)
  target_include_directories(
    ${ex} PRIVATE $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
                  $<INSTALL_INTERFACE:include>)

  target_link_directories(
    ${ex} PRIVATE $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/lib>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/metaSMT/lib>
    $<INSTALL_INTERFACE:lib>)
  target_link_libraries(${ex} ${PROJECT_NAME} ${${PROJECT_NAME}_LIBS})
endforeach(ex)
