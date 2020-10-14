########################################################################################
##	MIT License
########################################################################################
##	Copyright (c) 2012-2020 University of Bremen, Germany. 
##  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
##  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
##
##	Permission is hereby granted, free of charge, to any person obtaining a copy
##	of this software and associated documentation files (the "Software"), to deal
##	in the Software without restriction, including without limitation the rights
##	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
##	copies of the Software, and to permit persons to whom the Software is
##	furnished to do so, subject to the following conditions:
##
##	The above copyright notice and this permission notice shall be included in all
##	copies or substantial portions of the Software.
##
##	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
##	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
##	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
##	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
##	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
##	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
##	SOFTWARE.
##########################################################################################


SET(CRAVE_LIB_SRC 
  ConstrainedRandom.cpp
  ConstrainedRandomInit.cpp
  EvalVisitor.cpp
  FixWidthVisitor.cpp
  GetSupportSetVisitor.cpp
  metaSMTNodeVisitor.cpp
  metaSMTNodeVisitorYices2.cpp
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
    experimental/ConstraintBase.cpp
    experimental/SequenceItem.cpp
    experimental/Expression.cpp
    experimental/graph/Rule.cpp
  )
endif (CRAVE_ENABLE_EXPERIMENTAL)

add_library (${PROJECT_NAME} STATIC
  ${CRAVE_LIB_SRC}
)

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE on)

SET(LINK_LIBRARIES
  metaSMT
  ${ALL_EXTERNAL_LIBS}
)

target_link_libraries (${PROJECT_NAME} ${LINK_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)


