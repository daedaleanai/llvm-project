//===--- UseNodiscardCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UseNodiscardCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Type.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "daedalean-use-nodiscard"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

namespace {
AST_MATCHER(CXXMethodDecl, isOverloadedAssignmentOperator) {
  // Don't put ``[[nodiscard]]`` in front of assignment operators that return a
  // reference.
  return Node.isOverloadedOperator() &&
         (Node.getOverloadedOperator() == OverloadedOperatorKind::OO_Equal) &&
         Node.getReturnType()->isReferenceType();
}

AST_MATCHER(CXXMethodDecl, isLambda) {
  const RecordDecl *Parent = Node.getParent();
  return Parent && Parent->isLambda();
}

AST_MATCHER(FunctionDecl, isNotMethod) {
  // A function which is not a method
  return !llvm::isa<CXXMethodDecl>(Node);
}

AST_MATCHER(FunctionDecl, isDefinitionOrInline) {
  // A function definition, with optional inline but not the declaration.
  return !(Node.isThisDeclarationADefinition() && Node.isOutOfLine());
}

AST_MATCHER(CXXMethodDecl, isOverloadedPredecrementOperator) {
  // Don't put ``[[nodiscard]]`` in front of predecrement operators that return
  // a reference.
  return Node.isOverloadedOperator() &&
         (Node.getOverloadedOperator() ==
          OverloadedOperatorKind::OO_MinusMinus) &&
         Node.getReturnType()->isReferenceType() &&
         (Node.parameters().size() == 0);
}
AST_MATCHER(CXXMethodDecl, isOverloadedPreincrementOperator) {
  // Don't put ``[[nodiscard]]`` in front of preincrement operators that return
  // a reference.
  return Node.isOverloadedOperator() &&
         (Node.getOverloadedOperator() ==
          OverloadedOperatorKind::OO_PlusPlus) &&
         Node.getReturnType()->isReferenceType() &&
         (Node.parameters().size() == 0);
}
} // namespace

UseNodiscardCheck::UseNodiscardCheck(StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {}

void UseNodiscardCheck::registerMatchers(MatchFinder *Finder) {
  // Find all non-void methods which have not already been marked to
  // warn on unused result.
  //
  // Lambdas are excluded because c++20 does not have a way to mark the.
  // operator() as nodiscard in lambdas, so there is no point in warning.
  Finder->addMatcher(
      cxxMethodDecl(
          allOf(isDefinitionOrInline(),
                unless(anyOf(isLambda(), returns(voidType()), isNoReturn(),
                             isOverloadedAssignmentOperator(),
                             isOverloadedPreincrementOperator(),
                             isOverloadedPredecrementOperator()))))
          .bind("no_discard"),
      this);
  Finder->addMatcher(
      functionDecl(allOf(isNotMethod(), isDefinitionOrInline(),
                         unless(anyOf(returns(voidType()), isNoReturn()))))
          .bind("no_discard_func"),
      this);
}

void UseNodiscardCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<FunctionDecl>("no_discard");
  if (MatchedDecl == nullptr) {
    MatchedDecl = Result.Nodes.getNodeAs<FunctionDecl>("no_discard_func");
  }

  for (const auto *Attr : MatchedDecl->attrs()) {
    StringRef spelling{Attr->getSpelling()};
    LLVM_DEBUG(llvm::dbgs() << "spelling: " << spelling << '\n');
    if (spelling == "nodiscard") {
      return;
    }
  }

  // Don't make replacements if the location is invalid or in a macro.
  SourceLocation Loc = MatchedDecl->getLocation();
  if (Loc.isInvalid() || Loc.isMacroID())
    return;

  SourceLocation RetLoc = MatchedDecl->getInnerLocStart();

  auto Diag = diag(RetLoc, "function %0 should be marked [[nodiscard]]")
              << MatchedDecl;

  Diag << FixItHint::CreateInsertion(RetLoc, "[[nodiscard]] ");
}

bool UseNodiscardCheck::isLanguageVersionSupported(
    const LangOptions &LangOpts) const {
  // If we use ``[[nodiscard]]`` attribute, we require at least C++17.
  return LangOpts.CPlusPlus17;
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
