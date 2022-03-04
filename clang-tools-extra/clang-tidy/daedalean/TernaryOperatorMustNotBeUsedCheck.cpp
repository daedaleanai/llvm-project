//===--- TernaryOperatorMustNotBeUsedCheck.cpp - clang-tidy ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TernaryOperatorMustNotBeUsedCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

namespace {
  const internal::VariadicDynCastAllOfMatcher<Stmt, ConditionalOperator> ternaryOperator;

}

void TernaryOperatorMustNotBeUsedCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(ternaryOperator().bind("x"), this);
}

void TernaryOperatorMustNotBeUsedCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<ConditionalOperator>("x");
  diag(MatchedDecl->getBeginLoc(), "Ternary operator must not be used");

}

} // namespace daedalean
} // namespace tidy
} // namespace clang
