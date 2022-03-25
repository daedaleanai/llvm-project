//===--- ClassMethodsCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ClassMethodsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void ClassMethodsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(cxxMethodDecl().bind("x"), this);
}

void ClassMethodsCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<CXXMethodDecl>("x");

  if (!MatchedDecl->doesThisDeclarationHaveABody()) {
    return;
  }

  if (MatchedDecl->getParent()->isTemplated()) {
    return;
  }

  if (!MatchedDecl->getLexicalDeclContext()->isRecord()) {
    return;
  }

  diag(MatchedDecl->getLocation(), "function %0 must be implemented outside class definition")
      << MatchedDecl;
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
