set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  Support
  )

add_clang_library(clangTidyDaedaleanModule

  AssignmentOperatorsCheck.cpp
  DaedaleanTidyModule.cpp
  CommaOperatorMustNotBeUsedCheck.cpp
  DerivedClassesCheck.cpp
  EnumClassCheck.cpp
  FloatingPointComparisonCheck.cpp
  IncludeOrderCheck.cpp
  LambdaImplicitCaptureCheck.cpp
  StructsAndClassesCheck.cpp
  ProtectedMustNotBeUsedCheck.cpp
  SwitchStatementCheck.cpp
  TernaryOperatorMustNotBeUsedCheck.cpp
  LambdaReturnTypeCheck.cpp
  OperatorOverloadingCheck.cpp

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
