
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
  VariableDefaultSolver.cpp
  VariableGeneratorType.cpp
  VariableSolver.cpp
  VectorGenerator.cpp
  ConstrainedRandom.cpp
  UserVectorConstraint.cpp
  UserConstraintType.cpp
  ConstraintManager.cpp
  ConstraintPartitioner.cpp
  Operators.cpp
  Distribution.cpp
  RandomBase.cpp
  VariableGeneratorMT.cpp
  ComplexityEstimationVisitor.cpp
  RandomSeedManager.cpp
)

if (CRAVE_ENABLE_EXPERIMENTAL)
  LIST(APPEND CRAVE_LIB_SRC
    experimental/Object.cpp	
    experimental/GraphVisitor.cpp	
    experimental/ConstrainedRandomGraph.cpp  
    experimental/Experimental.cpp
  )
endif (CRAVE_ENABLE_EXPERIMENTAL)

add_library (crave STATIC
  ${CRAVE_LIB_SRC}
)

SET(LINK_LIBRARIES
  metaSMT
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
)

if(SystemC_FOUND)
  LIST(APPEND LINK_LIBRARIES ${SystemC_LIBRARY})
endif(SystemC_FOUND)

target_link_libraries (crave ${LINK_LIBRARIES})

install(TARGETS crave
  EXPORT crave
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)


