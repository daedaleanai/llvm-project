//===--- LocalMethodsAndTypesCheck.cpp - clang-tidy -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "LocalMethodsAndTypesCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/ExecutionEngine/JITSymbol.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void LocalMethodsAndTypesCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      functionDecl(
          unless(anyOf(hasAncestor(namespaceDecl(isAnonymous())),
                       isExplicitTemplateSpecialization(), isExternC(),
                       hasAncestor(functionDecl()), hasAncestor(recordDecl()))))
          .bind("Function"),
      this);

  Finder->addMatcher(
      varDecl(unless(anyOf(hasAncestor(namespaceDecl(isAnonymous())),
                           isExplicitTemplateSpecialization(), isExternC(),
                           hasAncestor(functionDecl()),
                           hasAncestor(recordDecl()), parmVarDecl())))
          .bind("Variable"),
      this);

  Finder->addMatcher(
      recordDecl(
          unless(anyOf(hasAncestor(namespaceDecl(isAnonymous())),
                       cxxRecordDecl(isExplicitTemplateSpecialization()),
                       hasAncestor(functionDecl()), hasAncestor(recordDecl()))))
          .bind("Record"),
      this);
}

void LocalMethodsAndTypesCheck::check(const MatchFinder::MatchResult &Result) {
  checkType<VarDecl>(Result, "Variable");
  checkType<FunctionDecl>(Result, "Function");
  checkType<RecordDecl>(Result, "Record");
}

template <typename T>
void LocalMethodsAndTypesCheck::checkType(
    const ast_matchers::MatchFinder::MatchResult &Result,
    llvm::StringRef DeclType) {
  if (const auto *MatchedDecl = Result.Nodes.getNodeAs<T>(DeclType)) {
    // If the first declaration is in the cpp file, then it should be in an
    // anonymous namespace
    const auto *FirstDecl = MatchedDecl->getFirstDecl();
    auto FileName = Result.SourceManager->getFilename(FirstDecl->getLocation());

    if (FileName.endswith(".cpp") || FileName.endswith(".cc") ||
        FileName.endswith(".c")) {
      diag(MatchedDecl->getLocation(),
           "%0 %1 should be in an anonymous namespace")
          << DeclType << MatchedDecl;
    }
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
