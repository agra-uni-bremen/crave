add_executable(ALU4 ALU4.cpp)
target_include_directories(ALU4 PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(ALU4 ${PROJECT_NAME} ${ALL_EXTERNAL_LIBS})

add_executable(ALU12 ALU12.cpp)
target_include_directories(ALU12 PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(ALU12 ${PROJECT_NAME} ${ALL_EXTERNAL_LIBS})

add_executable(ALU16 ALU16.cpp)
target_include_directories(ALU16 PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(ALU16 ${PROJECT_NAME} ${ALL_EXTERNAL_LIBS})

add_executable(ALU24 ALU24.cpp)
target_include_directories(ALU24 PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(ALU24 ${PROJECT_NAME} ${ALL_EXTERNAL_LIBS})

add_executable(ALU32 ALU32.cpp)
target_include_directories(ALU32 PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(ALU32 ${PROJECT_NAME} ${ALL_EXTERNAL_LIBS})
