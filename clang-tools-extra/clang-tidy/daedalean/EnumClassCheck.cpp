//===--- EnumClassCheck.cpp - clang-tidy ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "EnumClassCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void EnumClassCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(enumDecl().bind("x"), this);
}

void EnumClassCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<EnumDecl>("x");
  if (MatchedDecl->isScoped()) {
    return;
  }

  diag(MatchedDecl->getLocation(), "enum class MUST be used")
      << MatchedDecl;
  diag(MatchedDecl->getLocation(), "insert class", DiagnosticIDs::Note)
      << FixItHint::CreateInsertion(MatchedDecl->getLocation(), "class");
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
