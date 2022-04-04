set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  Support
  )

add_clang_library(clangTidyDaedaleanModule

  AssignmentOperatorsCheck.cpp
  ClassMethodsCheck.cpp
  DaedaleanTidyModule.cpp
  CommaOperatorMustNotBeUsedCheck.cpp
  DerivedClassesCheck.cpp
  LambdaReturnTypeCheck.cpp
  EnumClassCheck.cpp
  FloatingPointComparisonCheck.cpp
  FriendDeclarationsCheck.cpp
  IncludeOrderCheck.cpp
  LambdaImplicitCaptureCheck.cpp
  TernaryOperatorMustNotBeUsedCheck.cpp
  PreprocessingDirectivesCheck.cpp
  StructsAndClassesCheck.cpp
  ProtectedMustNotBeUsedCheck.cpp
  SwitchStatementCheck.cpp

  LINK_LIBS
  clangTidy
  clangTidyUtils

  DEPENDS
  omp_gen
  )

clang_target_link_libraries(clangTidyDaedaleanModule
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  )
