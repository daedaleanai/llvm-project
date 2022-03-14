set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  Support
  )

add_clang_library(clangTidyDaedaleanModule

  DaedaleanTidyModule.cpp
  CommaOperatorMustNotBeUsedCheck.cpp
  DerivedClassesCheck.cpp
  LambdaImplicitCaptureCheck.cpp
  SwitchStatementCheck.cpp
  TernaryOperatorMustNotBeUsedCheck.cpp
  LambdaReturnTypeCheck.cpp

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
