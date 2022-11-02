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
  Finder->addMatcher(
      traverse(TK_IgnoreUnlessSpelledInSource,
               stringLiteral(unless(anyOf(hasParent(userDefinedLiteral()),
                                          hasParent(staticAssertDecl()))))
                   .bind("string-literal")),
      this);
}

void StringsCheck::check(const MatchFinder::MatchResult &Result) {
  const StringLiteral *StringLiteralDecl =
      Result.Nodes.getNodeAs<StringLiteral>("string-literal");

  auto parents = Result.Context->getParents(*StringLiteralDecl);

  if (parents.size() == 1) {
    if (const auto *callExpr = parents[0].get<CallExpr>()) {
      if (auto *cast =
              llvm::dyn_cast<const ImplicitCastExpr>(callExpr->getCallee())) {
        if (auto *decl =
                llvm::dyn_cast<const DeclRefExpr>(cast->getSubExpr())) {
          if (auto *func =
                  llvm::dyn_cast<const FunctionDecl>(decl->getDecl())) {
            if (auto *lit = func->getLiteralIdentifier()) {
              return;
            }
          }
        }
      }
    }
  }

  diag(StringLiteralDecl->getExprLoc(), "C-String literals MUST not be used");
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
