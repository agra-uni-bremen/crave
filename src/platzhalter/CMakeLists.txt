add_library (${PROJECT_NAME} ConstrainedRandom.cpp)
target_link_libraries (${PROJECT_NAME} metaSMT)

install(FILES
  AllSAT.hpp
  Constraint.hpp
  SystemC.hpp
  Context.hpp
  VectorConstraint.hpp
  ExpressionTraits.hpp
  bitsize_traits.hpp
  ConstrainedRandom.hpp
  Generator.hpp
  DESTINATION include/${PROJECT_NAME}
)

install(TARGETS ${PROJECT_NAME} 
  EXPORT ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)

