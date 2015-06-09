
SET(CRAVE_LIB_SRC 
  ConstrainedRandom.cpp
  EvalVisitor.cpp
  FixWidthVisitor.cpp
  GetSupportSetVisitor.cpp
  metaSMTNodeVisitor.cpp
  ReplaceVisitor.cpp
  ToDotNodeVisitor.cpp
  CraveSettings.cpp
  Generator.cpp
  SettingType.cpp
  VariableCoverageGenerator.cpp
  ConstraintPartition.cpp
  Evaluator.cpp
  RandObjGenerator.cpp
  CraveSettings.cpp
  Evaluator.cpp
  RandObjGenerator.cpp
  SettingType.cpp
  VariableCoverageSolver.cpp
)

if (CRAVE_ENABLE_EXPERIMENTAL)
  LIST(APPEND CRAVE_LIB_SRC
    experimental/Object.cpp	
    experimental/GraphVisitor.cpp	
    experimental/ConstrainedRandomGraph.cpp  
  )
endif (CRAVE_ENABLE_EXPERIMENTAL)

add_library (crave STATIC
  ${CRAVE_LIB_SRC}
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


