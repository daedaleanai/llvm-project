//===--- TypeConversionsCheck.cpp - clang-tidy --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TypeConversionsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

const char DynamicCastId[] = "dynamic-cast";
const char ReinterpretCastId[] = "reinterpret-cast";
const char ConstCastId[] = "const-cast";
const char CStyleCastId[] = "c-style-cast";

void TypeConversionsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(cxxDynamicCastExpr().bind(DynamicCastId), this);
  Finder->addMatcher(cxxReinterpretCastExpr().bind(ReinterpretCastId), this);
  Finder->addMatcher(cxxConstCastExpr().bind(ConstCastId), this);
  Finder->addMatcher(cStyleCastExpr().bind(CStyleCastId), this);
}

void TypeConversionsCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *DynamicCastDecl =
      Result.Nodes.getNodeAs<CXXDynamicCastExpr>(DynamicCastId);
  if (DynamicCastDecl != nullptr) {
    diag(DynamicCastDecl->getOperatorLoc(), "dynamic_cast MUST not be used");
  }

  const auto *ReinterpretCastDecl =
      Result.Nodes.getNodeAs<CXXReinterpretCastExpr>(ReinterpretCastId);
  if (ReinterpretCastDecl != nullptr) {
    diag(ReinterpretCastDecl->getOperatorLoc(),
         "reinterpret_cast MUST not be used");
  }

  const auto *ConstCastDecl =
      Result.Nodes.getNodeAs<CXXConstCastExpr>(ConstCastId);
  if (ConstCastDecl != nullptr) {
    diag(ConstCastDecl->getOperatorLoc(), "const_cast MUST not be used");
  }

  const auto *CStyleCastDecl =
      Result.Nodes.getNodeAs<CStyleCastExpr>(CStyleCastId);
  if (CStyleCastDecl != nullptr) {
    diag(CStyleCastDecl->getLParenLoc(), "C-style cast MUST not be used");
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
