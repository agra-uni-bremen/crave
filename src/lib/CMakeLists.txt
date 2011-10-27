add_library (crave ConstrainedRandom.cpp)
target_link_libraries (crave
  metaSMT
  ${Boost_THREAD_LIBRARY}
)
install(TARGETS crave
  EXPORT crave
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)


