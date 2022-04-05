//===--- LocalMethodsAndTypesCheck.cpp - clang-tidy -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "LocalMethodsAndTypesCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

namespace {

bool isSpellingLocInHeaderFile(SourceLocation Loc, SourceManager &SM) {
  SourceLocation SpellingLoc = SM.getSpellingLoc(Loc);
  const auto fileName = SM.getFilename(SpellingLoc);

  return fileName.endswith(".h") || fileName.endswith("hh");
}

bool isInAnonymousNamespace(const DeclContext *decl) {
  if (!decl) {
    return false;
  }

  if (const auto ns = llvm::dyn_cast_or_null<NamespaceDecl>(decl); ns) {
    if (ns->isAnonymousNamespace()) {
      return true;
    }
  }

  return isInAnonymousNamespace(decl->getParent());
}

} // namespace

void LocalMethodsAndTypesCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(functionDecl().bind("x"), this);
  Finder->addMatcher(recordDecl().bind("x"), this);
}

void LocalMethodsAndTypesCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<NamedDecl>("x");

  if (!MatchedDecl->isFirstDecl()) {
    return;
  }

  if (isSpellingLocInHeaderFile(MatchedDecl->getBeginLoc(),
                                *Result.SourceManager)) {
    return;
  }

  if (isInAnonymousNamespace(MatchedDecl->getDeclContext())) {
    return;
  }

  if (MatchedDecl->isFunctionOrFunctionTemplate()) {
    diag(MatchedDecl->getLocation(),
         "Local function must be declared in anonymous namespace");
  } else {
    diag(MatchedDecl->getLocation(),
         "Local type must be declared in anonymous namespace");
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
