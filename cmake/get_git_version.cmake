
function( get_git_version OUTPUT_VARIABLE TAG_PREFIX ) 

  find_package(Git QUIET)
  if( GIT_FOUND )
    execute_process(
      COMMAND ${GIT_EXECUTABLE} describe --tags --match "${TAG_PREFIX}*" HEAD
      OUTPUT_VARIABLE _GIT_VERSION_DESCRIPTION
      OUTPUT_STRIP_TRAILING_WHITESPACE
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      ERROR_QUIET
      RESULT_VARIABLE _GIT_DESCRIBE_RESULT
    )
  
    if(_GIT_DESCRIBE_RESULT EQUAL 0)
    string(REPLACE "${TAG_PREFIX}" "" _git_VERSiON "${_GIT_VERSION_DESCRIPTION}")
    endif()

    set(${OUTPUT_VARIABLE} ${_git_VERSiON} PARENT_SCOPE)
  endif()

endfunction(get_git_version)
