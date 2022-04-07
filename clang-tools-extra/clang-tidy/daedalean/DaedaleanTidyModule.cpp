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
#include "CommaOperatorMustNotBeUsedCheck.h"
#include "DerivedClassesCheck.h"
#include "LambdaReturnTypeCheck.h"
#include "LambdaImplicitCaptureCheck.h"
#include "TernaryOperatorMustNotBeUsedCheck.h"
#include "ProtectedMustNotBeUsedCheck.h"
#include "StructsAndClassesCheck.h"
#include "SwitchStatementCheck.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

/// This module is for Daedalean-specific checks.
class DaedaleanModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<CommaOperatorMustNotBeUsedCheck>(
        "daedalean-comma-operator-must-not-be-used");
    CheckFactories.registerCheck<DerivedClassesCheck>(
        "daedalean-derived-classes");
    CheckFactories.registerCheck<LambdaReturnTypeCheck>(
        "daedalean-lambda-return-type");
    CheckFactories.registerCheck<LambdaImplicitCaptureCheck>(
        "daedalean-lambda-implicit-capture");
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
