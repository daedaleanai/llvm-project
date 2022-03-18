//===--- StructsAndClassesCheck.cpp - clang-tidy --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "StructsAndClassesCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void StructsAndClassesCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(cxxRecordDecl(hasDefinition()).bind("x"), this);
}

void StructsAndClassesCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<CXXRecordDecl>("x");

  if (!MatchedDecl->isStruct() && !MatchedDecl->isClass()) {
    // Skip enums, unions etc
    return;
  }

  if (MatchedDecl->isPOD()) {
    if (!MatchedDecl->isStruct()) {
      diag(MatchedDecl->getBeginLoc(), "POD type must be declared as struct")
          << MatchedDecl;
      diag(MatchedDecl->getBeginLoc(), "use struct", DiagnosticIDs::Note)
          << FixItHint::CreateReplacement(MatchedDecl->getBeginLoc(), "struct");
    }
  } else {
    if (!MatchedDecl->isClass()) {
      diag(MatchedDecl->getBeginLoc(), "Non-POD type must be declared as class")
          << MatchedDecl;
      diag(MatchedDecl->getBeginLoc(), "use class", DiagnosticIDs::Note)
          << FixItHint::CreateReplacement(MatchedDecl->getBeginLoc(), "class");
    }
  }

  if (MatchedDecl->isClass()) {
    for (const auto * field: MatchedDecl->fields()) {
      if (field->getType().isConstQualified()) {
        continue;
      }
      if (field->getAccess() != AS_private && field->getAccess() != AS_none) {
        diag(field->getBeginLoc(), "All non-const data members of class MUST be private")
            << MatchedDecl;
        diag(field->getBeginLoc(), "Make field private", DiagnosticIDs::Note)
            << FixItHint::CreateInsertion(field->getBeginLoc(), "private:");
      }
    }
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
