add_library (crave
  ConstrainedRandom.cpp
  EvalVisitor.cpp
  FixWidthVisitor.cpp
  metaSMTNodeVisitor.cpp
  ReplaceVisitor.cpp
  ToDotNodeVisitor.cpp
  graph/GraphVisitor.cpp	
  ConstrainedRandomGraph.cpp
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


