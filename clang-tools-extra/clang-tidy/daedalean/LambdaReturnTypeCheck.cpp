//===--- LambdaReturnTypeCheck.cpp - clang-tidy ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "LambdaReturnTypeCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void LambdaReturnTypeCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(lambdaExpr().bind("x"), this);
}

void LambdaReturnTypeCheck::check(const MatchFinder::MatchResult &Result)  {
  // FIXME: Add callback implementation.
  const auto *MatchedDecl = Result.Nodes.getNodeAs<LambdaExpr>("x");
  if (MatchedDecl->hasExplicitResultType())
    return;

  if (MatchedDecl->isGenericLambda())
    return;

  if (MatchedDecl->getCallOperator()->getReturnType()->isVoidType())
    return;

  diag(MatchedDecl->getBody()->getBeginLoc(), "Lambda function without auto arguments must have explicit return type");

  diag(MatchedDecl->getBody()->getBeginLoc(), "add return type", DiagnosticIDs::Note)
      << FixItHint::CreateInsertion(MatchedDecl->getBody()->getBeginLoc(), " -> " + MatchedDecl->getCallOperator()->getReturnType().getAsString());
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
