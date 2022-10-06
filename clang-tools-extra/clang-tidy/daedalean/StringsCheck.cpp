//===--- StringsCheck.cpp - clang-tidy ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "StringsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void StringsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(stringLiteral(unless(hasParent(userDefinedLiteral())))
                         .bind("string-literal"),
                     this);
}

void StringsCheck::check(const MatchFinder::MatchResult &Result) {
  const StringLiteral *StringLiteralDecl =
      Result.Nodes.getNodeAs<StringLiteral>("string-literal");
  diag(StringLiteralDecl->getExprLoc(), "C-String literals MUST not be used");
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
