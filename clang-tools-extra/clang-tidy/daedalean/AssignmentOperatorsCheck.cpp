//===--- AssignmentOperatorsCheck.cpp - clang-tidy ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "AssignmentOperatorsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void AssignmentOperatorsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(cxxRecordDecl(hasDefinition()).bind("x"), this);
}

void AssignmentOperatorsCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<CXXRecordDecl>("x");

  if (MatchedDecl->isAbstract()) {
    return;
  }

  if (MatchedDecl->isStruct()) {
    return;
  }

  bool hasCopy = false;
  bool hasMove = false;

  for (const auto member: MatchedDecl->methods()) {
    if (!member->isOverloadedOperator()) {
      continue;
    }

    if (member->getOverloadedOperator() != OO_Equal) {
      continue;
    }

    if (member->isImplicit()) {
      continue;
    }

    const auto argType = member->getParamDecl(0)->getType();

    if (argType.getNonReferenceType().getUnqualifiedType() != MatchedDecl->getTypeForDecl()->getCanonicalTypeUnqualified()) {
      continue;
    }


    if (argType->isRValueReferenceType()) {
      hasMove = true;
    }
    if (argType->isLValueReferenceType() && argType.getNonReferenceType().isConstQualified()) {
      hasCopy = true;
    }
  }

  if (!hasMove) {
    diag(MatchedDecl->getLocation(), "Non-abstract class must implement move-assignment operator");
  }

  if (!hasCopy) {
    diag(MatchedDecl->getLocation(), "Non-abstract class must implement copy-assignment operator");
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
