//===--- FriendDeclarationsCheck.cpp - clang-tidy -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FriendDeclarationsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void FriendDeclarationsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(friendDecl().bind("x"), this);
}

void FriendDeclarationsCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<FriendDecl>("x");

  const auto decl = MatchedDecl->getFriendDecl();
  if (!decl) {
    diag(MatchedDecl->getLocation(), "Unresolved friend declaration");
    return;
  }

  if (MatchedDecl->isFunctionOrFunctionTemplate()) {
    if (const auto method = llvm::dyn_cast_or_null<FunctionDecl>(decl);
        method) {
      if (method->isOverloadedOperator()) {
        if (method->getNumParams() == 2) {
          if (const auto cls = llvm::dyn_cast_or_null<CXXRecordDecl>(
                  MatchedDecl->getLexicalDeclContext())) {
            const auto type =
                cls->getTypeForDecl()->getCanonicalTypeUnqualified();

            if (type == method->getParamDecl(1)
                            ->getType()
                            .getNonReferenceType()
                            ->getCanonicalTypeUnqualified()) {
              if (type != method->getParamDecl(0)
                              ->getType()
                              .getNonReferenceType()
                              ->getCanonicalTypeUnqualified()) {
                return;
              }
            }
          }
        }
      }
    }
  }

  if (decl->isTemplated()) {
    const auto ctx = MatchedDecl->getLexicalDeclContext();
    if (const auto cls = llvm::dyn_cast_or_null<const CXXRecordDecl>(ctx);
        cls && (cls->isTemplated() || llvm::isa<ClassTemplateSpecializationDecl>(cls)) ) {
      if (const auto friendCls =
              llvm::dyn_cast_or_null<ClassTemplateDecl>(decl);
          friendCls) {
        if (cls->getQualifiedNameAsString() ==
            friendCls->getQualifiedNameAsString()) {
          return;
        }
      }
    }
  }

  diag(MatchedDecl->getLocation(), "Friend declaration must not be used");
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
