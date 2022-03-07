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
<<<<<<< HEAD
=======
#include "LambdaImplicitCaptureCheck.h"
#include "LambdaReturnTypeCheck.h"
>>>>>>> ab9bb7e9c307... Implicit lambda capture is prohibited

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
    CheckFactories.registerCheck<LambdaImplicitCaptureCheck>(
        "daedalean-lambda-implicit-capture");
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
