add_library (crave
  ConstrainedRandom.cpp
  expression/EvalVisitor.cpp
  expression/metaSMTNodeVisitor.cpp
  expression/ReplaceVisitor.cpp
  expression/ToDotNodeVisitor.cpp
)
target_link_libraries (crave
  metaSMT
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
)
install(TARGETS crave
  EXPORT crave
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)


