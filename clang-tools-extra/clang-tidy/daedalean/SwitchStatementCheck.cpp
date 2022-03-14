//===--- SwitchStatementCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SwitchStatementCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void SwitchStatementCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(switchStmt().bind("x"), this);
}

void SwitchStatementCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<SwitchStmt>("x");

  for (auto caseStmt = MatchedDecl->getSwitchCaseList(); caseStmt; caseStmt = caseStmt->getNextSwitchCase()) {
    const auto stmt = caseStmt->getSubStmt();
    if (!isa<CompoundStmt>(stmt)) {
      diag(caseStmt->getColonLoc(), "Case statement must be scoped statement");
    }
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
