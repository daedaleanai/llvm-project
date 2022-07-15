//===--- DaedaleanTidyModule.cpp - clang-tidy------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "../ClangTidy.h"
#include "../ClangTidyModule.h"
#include "../ClangTidyModuleRegistry.h"
#include "AssignmentOperatorsCheck.h"
#include "AutoCheck.h"
#include "ClassMethodsCheck.h"
#include "CommaOperatorMustNotBeUsedCheck.h"
#include "DerivedClassesCheck.h"
#include "LambdaReturnTypeCheck.h"
#include "EnumClassCheck.h"
#include "FloatingPointComparisonCheck.h"
#include "FriendDeclarationsCheck.h"
#include "IncludeOrderCheck.h"
#include "LambdaImplicitCaptureCheck.h"
#include "LocalMethodsAndTypesCheck.h"
#include "OperatorOverloadingCheck.h"
#include "PreprocessingDirectivesCheck.h"
#include "ProtectedMustNotBeUsedCheck.h"
#include "StructsAndClassesCheck.h"
#include "SwitchStatementCheck.h"
#include "TernaryOperatorMustNotBeUsedCheck.h"
#include "TypeConversionsCheck.h"
#include "UnionsMustNotBeUsedCheck.h"
#include "UseNoexceptCheck.h"
#include "VarargFunctionsMustNotBeUsedCheck.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

/// This module is for Daedalean-specific checks.
class DaedaleanModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<AssignmentOperatorsCheck>(
        "daedalean-assignment-operators");
    CheckFactories.registerCheck<AutoCheck>(
        "daedalean-auto");
    CheckFactories.registerCheck<ClassMethodsCheck>(
        "daedalean-class-methods");
    CheckFactories.registerCheck<CommaOperatorMustNotBeUsedCheck>(
        "daedalean-comma-operator-must-not-be-used");
    CheckFactories.registerCheck<DerivedClassesCheck>(
        "daedalean-derived-classes");
    CheckFactories.registerCheck<LocalMethodsAndTypesCheck>(
        "daedalean-local-methods-and-types");
    CheckFactories.registerCheck<TypeConversionsCheck>(
        "daedalean-type-conversions");
    CheckFactories.registerCheck<LambdaReturnTypeCheck>(
        "daedalean-lambda-return-type");
    CheckFactories.registerCheck<EnumClassCheck>(
        "daedalean-enum-class");
    CheckFactories.registerCheck<FloatingPointComparisonCheck>(
        "daedalean-floating-point-comparison");
    CheckFactories.registerCheck<FriendDeclarationsCheck>(
        "daedalean-friend-declarations");
    CheckFactories.registerCheck<IncludeOrderCheck>(
        "daedalean-include-order");
    CheckFactories.registerCheck<LambdaImplicitCaptureCheck>(
        "daedalean-lambda-implicit-capture");
    CheckFactories.registerCheck<OperatorOverloadingCheck>(
        "daedalean-operator-overloading");
    CheckFactories.registerCheck<PreprocessingDirectivesCheck>(
        "daedalean-preprocessing-directives");
    CheckFactories.registerCheck<ProtectedMustNotBeUsedCheck>(
        "daedalean-protected-must-not-be-used");
    CheckFactories.registerCheck<StructsAndClassesCheck>(
        "daedalean-structs-and-classes");
    CheckFactories.registerCheck<SwitchStatementCheck>(
        "daedalean-switch-statement");
    CheckFactories.registerCheck<TernaryOperatorMustNotBeUsedCheck>(
        "daedalean-ternary-operator-must-not-be-used");
    CheckFactories.registerCheck<SwitchStatementCheck>(
        "daedalean-switch-statement");
    CheckFactories.registerCheck<UnionsMustNotBeUsedCheck>(
        "daedalean-unions-must-not-be-used");
    CheckFactories.registerCheck<UseNoexceptCheck>("daedalean-use-noexcept");
    CheckFactories.registerCheck<VarargFunctionsMustNotBeUsedCheck>(
        "daedalean-vararg-functions-must-not-be-used");
  }
};

// Register the DaedaleanTidyModule using this statically initialized variable.
static ClangTidyModuleRegistry::Add<DaedaleanModule>
    X("daedalean-module", "Adds Daedalean checks.");
} // namespace daedalean

// This anchor is used to force the linker to link in the generated object file
// and thus register the DaedaleanModule.
volatile int DaedaleanModuleAnchorSource = 0;

} // namespace tidy
} // namespace clang
