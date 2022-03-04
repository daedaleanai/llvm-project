//===--- CommaOperatorMustNotBeUsedCheck.cpp - clang-tidy -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "CommaOperatorMustNotBeUsedCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

namespace {

const internal::VariadicDynCastAllOfMatcher<Stmt, BinaryOperator>
    binaryOperator;

} // namespace

void CommaOperatorMustNotBeUsedCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(binaryOperator().bind("x"), this);
}

void CommaOperatorMustNotBeUsedCheck::check(
    const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<BinaryOperator>("x");
  if (!MatchedDecl->isCommaOp()) {
    return;
  }

  diag(MatchedDecl->getOperatorLoc(), "Comma operator must not be used");
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
