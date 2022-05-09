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

  if (MatchedDecl->isTemplateInstantiation()) {
    return;
  }

  if (!MatchedDecl->doesThisDeclarationHaveABody()) {
    return;
  }

  if (MatchedDecl->isImplicit() || MatchedDecl->isDefaulted()) {
    return;
  } 

  if (!MatchedDecl->getLexicalDeclContext()->isRecord()) {
    return;
  }

  if (const auto cls = llvm::dyn_cast_or_null<CXXRecordDecl>(MatchedDecl->getLexicalDeclContext())) {
    if (cls->isLambda()) {
        return;
    }
    if (cls->getDescribedClassTemplate()) {
        return;
    }

    if (llvm::isa<ClassTemplateSpecializationDecl>(cls)) {
      return;
    }

    if (cls->isTemplated()) {
        return;
    }

    diag(MatchedDecl->getLocation(), "function %0 must be implemented outside class definition %1")
      << MatchedDecl << cls;
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
