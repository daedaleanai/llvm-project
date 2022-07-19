//===--- TemplateSpecializationsCheck.cpp - clang-tidy --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TemplateSpecializationsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

namespace {
AST_POLYMORPHIC_MATCHER(isDefaultDefinedAndHasSpecializations,
                        AST_POLYMORPHIC_SUPPORTED_TYPES(FunctionTemplateDecl,
                                                        ClassTemplateDecl)) {
  if (Node.isThisDeclarationADefinition()) {
    for (const auto *Specialization : Node.specializations()) {
      if (Specialization->isThisDeclarationADefinition()) {
        return true;
      }
    }
  }
  return false;
}

} // namespace

void TemplateSpecializationsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(classTemplateDecl(isDefaultDefinedAndHasSpecializations())
                         .bind("class-template-decl"),
                     this);

  Finder->addMatcher(
      functionTemplateDecl(isDefaultDefinedAndHasSpecializations())
          .bind("function-template-decl"),
      this);
}

void TemplateSpecializationsCheck::check(
    const MatchFinder::MatchResult &Result) {

  if (const auto *ClassTemplate =
          Result.Nodes.getNodeAs<ClassTemplateDecl>("class-template-decl")) {
    const auto ClassTemplateDeclLocFile =
        Result.SourceManager->getFilename(ClassTemplate->getLocation());

    llvm::SmallVector<ClassTemplatePartialSpecializationDecl *>
        PartialSpecializations;
    ClassTemplate->getPartialSpecializations(PartialSpecializations);

    for (const auto *Specialization : PartialSpecializations) {
      const auto *Definition = Specialization->getDefinition();
      if (Definition == nullptr) {
        return;
      }

      const auto ClassTemplateSpecializationLocFile =
          Result.SourceManager->getFilename(Definition->getLocation());

      if (ClassTemplateSpecializationLocFile != ClassTemplateDeclLocFile) {
        diag(Definition->getLocation(),
             "Partial class template specialization %0 should be defined in "
             "the "
             "same file as %1")
            << Specialization << ClassTemplate;
      }
    }

    for (auto *Specialization : ClassTemplate->specializations()) {
      const auto *Definition = Specialization->getDefinition();
      if (Definition == nullptr) {
        return;
      }

      const auto ClassTemplateSpecializationLocFile =
          Result.SourceManager->getFilename(Definition->getLocation());

      if (ClassTemplateSpecializationLocFile != ClassTemplateDeclLocFile) {
        diag(Definition->getLocation(),
             "Class template specialization %0 should be defined in the "
             "same file as %1")
            << Specialization << ClassTemplate;
      }
    }
  }

  if (const auto *FunctionTemplate =
          Result.Nodes.getNodeAs<FunctionTemplateDecl>(
              "function-template-decl")) {
    const auto FunctionTemplateDeclLocFile =
        Result.SourceManager->getFilename(FunctionTemplate->getLocation());

    for (auto *Specialization : FunctionTemplate->specializations()) {
      const auto *Definition = Specialization->getDefinition();
      if (Definition == nullptr) {
        return;
      }

      const auto FunctionTemplateSpecializationLocFile =
          Result.SourceManager->getFilename(Definition->getInnerLocStart());

      if (FunctionTemplateSpecializationLocFile !=
          FunctionTemplateDeclLocFile) {
        diag(Definition->getLocation(),
             "Function template specialization %0 should be defined in the "
             "same file as %1")
            << Specialization << FunctionTemplate;

        diag(FunctionTemplate->getLocation(),
             "Function template first defined here", DiagnosticIDs::Note);
      }
    }
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
