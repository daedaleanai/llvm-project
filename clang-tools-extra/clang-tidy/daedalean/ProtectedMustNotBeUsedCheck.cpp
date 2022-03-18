//===--- ProtectedMustNotBeUsedCheck.cpp - clang-tidy ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ProtectedMustNotBeUsedCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void ProtectedMustNotBeUsedCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(accessSpecDecl().bind("x"), this);
}

void ProtectedMustNotBeUsedCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<AccessSpecDecl>("x");
  if (MatchedDecl->getAccess() == AS_protected) {
    diag(MatchedDecl->getLocation(), "protected modifier must not be used");
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
