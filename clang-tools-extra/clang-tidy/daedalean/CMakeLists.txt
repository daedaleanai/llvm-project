set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  Support
  )

add_clang_library(clangTidyDaedaleanModule

  AssignmentOperatorsCheck.cpp
  AutoCheck.cpp
  ClassMethodsCheck.cpp
  DaedaleanTidyModule.cpp
  CommaOperatorMustNotBeUsedCheck.cpp
  DerivedClassesCheck.cpp
  LocalMethodsAndTypesCheck.cpp
  TemplateSpecializationsCheck.cpp
  TypeConversionsCheck.cpp
  LambdaReturnTypeCheck.cpp
  EnumClassCheck.cpp
  FloatingPointComparisonCheck.cpp
  FriendDeclarationsCheck.cpp
  IncludeOrderCheck.cpp
  LambdaImplicitCaptureCheck.cpp
  TernaryOperatorMustNotBeUsedCheck.cpp
  PreprocessingDirectivesCheck.cpp
  OperatorOverloadingCheck.cpp
  StructsAndClassesCheck.cpp
  ProtectedMustNotBeUsedCheck.cpp
  SwitchStatementCheck.cpp
  UnionsMustNotBeUsedCheck.cpp
  VarargFunctionsMustNotBeUsedCheck.cpp
  UseNoexceptCheck.cpp
  UseNodiscardCheck.cpp

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
