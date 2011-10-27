add_subdirectory(lib)

install(DIRECTORY crave
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp"
)

