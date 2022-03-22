//===--- FloatingPointComparisonCheck.cpp - clang-tidy --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FloatingPointComparisonCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void FloatingPointComparisonCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(binaryOperator().bind("x"), this);
}

void FloatingPointComparisonCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<BinaryOperator>("x");
  if (!MatchedDecl->isEqualityOp()) {
    return;
  }

  if(!MatchedDecl->getLHS()->getType()->isFloatingType() && !MatchedDecl->getRHS()->getType()->isFloatingType()) {
    return;
  }

  diag(MatchedDecl->getOperatorLoc(), "Floating point expressions MUST NOT be checked for equality or inequality");
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
