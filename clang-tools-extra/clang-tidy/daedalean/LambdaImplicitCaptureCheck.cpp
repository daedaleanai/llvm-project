//===--- LambdaImplicitCaptureCheck.cpp - clang-tidy ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "LambdaImplicitCaptureCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void LambdaImplicitCaptureCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(lambdaExpr().bind("x"), this);
}

void LambdaImplicitCaptureCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<LambdaExpr>("x");

  if (MatchedDecl->getCaptureDefault() == LCD_None) {
    return;
  }

  diag(MatchedDecl->getBeginLoc(), "Lambda function capture must be explicit");
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
