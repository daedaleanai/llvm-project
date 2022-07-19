//===--- UseNoexceptCheck.cpp - clang-tidy --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UseNoexceptCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void UseNoexceptCheck::registerMatchers(MatchFinder *Finder) {
  auto isPartOfLambda = cxxMethodDecl(ofClass(isLambda()));
  Finder->addMatcher(
      functionDecl(unless(anyOf(isNoThrow(), isPartOfLambda, cxxMethodDecl(),
                                ast_matchers::isTemplateInstantiation(),
                                isDeleted(), cxxDeductionGuideDecl())))
          .bind("function"),
      this);
  Finder->addMatcher(
      cxxMethodDecl(unless(anyOf(isNoThrow(), isPartOfLambda, isDeleted(),
                                 ast_matchers::isTemplateInstantiation(),
                                 cxxDeductionGuideDecl())))
          .bind("method"),
      this);
  Finder->addMatcher(
      cxxMethodDecl(allOf(ofClass(isLambda()), hasOverloadedOperatorName("()")),
                    unless(anyOf(isNoThrow(), isDeleted(),
                                 ast_matchers::isTemplateInstantiation())))
          .bind("lambda"),
      this);
}

void UseNoexceptCheck::check(const MatchFinder::MatchResult &Result) {
  if (const auto *MatchedDecl =
          Result.Nodes.getNodeAs<FunctionDecl>("function");
      MatchedDecl && MatchedDecl->getLocation().isValid()) {
    DeclarationName name = MatchedDecl->getDeclName();
    if (name.isIdentifier()) {
      std::string nameStr{name.getAsString()};
      llvm::StringRef nameRef{nameStr};
      if (nameRef.startswith("__builtin") || nameRef.startswith("__atomic")) {
        return;
      }
    }
    diag(MatchedDecl->getLocation(), "Function %0 should be noexcept")
        << MatchedDecl;
    diag(MatchedDecl->getLocation(), "insert 'noexcept'", DiagnosticIDs::Note);
  }

  if (const auto *MatchedDecl = Result.Nodes.getNodeAs<CXXMethodDecl>("method");
      MatchedDecl && MatchedDecl->getLocation().isValid()) {
    diag(MatchedDecl->getLocation(), "Method %0 should be noexcept")
        << MatchedDecl;
    diag(MatchedDecl->getLocation(), "insert 'noexcept'", DiagnosticIDs::Note);
  }

  if (const auto *MatchedDecl = Result.Nodes.getNodeAs<CXXMethodDecl>("lambda");
      MatchedDecl && MatchedDecl->getLocation().isValid()) {
    diag(MatchedDecl->getLocation(), "Lambda should be noexcept");
    diag(MatchedDecl->getLocation(), "insert 'noexcept'", DiagnosticIDs::Note);
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
