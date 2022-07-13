//===--- UnionsMustNotBeUsedCheck.cpp - clang-tidy ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UnionsMustNotBeUsedCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void UnionsMustNotBeUsedCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(recordDecl(isUnion()).bind("x"), this);
}

void UnionsMustNotBeUsedCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<RecordDecl>("x");
  diag(MatchedDecl->getLocation(), "Unions must not be used");
  diag(MatchedDecl->getLocation(), "Remove union %0", DiagnosticIDs::Note)
      << MatchedDecl;
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
