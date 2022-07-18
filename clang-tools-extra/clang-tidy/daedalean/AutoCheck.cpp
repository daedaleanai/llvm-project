//===--- AutoCheck.cpp - clang-tidy ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "AutoCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

void AutoCheck::registerMatchers(MatchFinder *Finder) {
  // We register 3 matchers separately:
  //  - Variable declarations (which includes parameters)
  //  - Function declarations (to check return types)
  //  - Lambdas (which are handled separately given they have slightly different
  //  rules).

  // This matcher should not bind to implicit variables added by clang, that is
  // why it uses traverse with TK_IgnoreUnlessSpelledInSource.
  const auto hasAutoType =
      anyOf(hasType(autoType()), hasType(references(autoType())),
            hasType(pointsTo(autoType())),
            allOf(hasType(templateTypeParmType()), hasType(asString("auto"))));
  Finder->addMatcher(traverse(TK_IgnoreUnlessSpelledInSource,
                              varDecl(hasAutoType, unless(isInitCapture())))
                         .bind("variable-decl"),
                     this);
  Finder->addMatcher(
      functionDecl(returns(autoType()), unless(hasTrailingReturn()))
          .bind("function-return"),
      this);
  Finder->addMatcher(cxxRecordDecl(isLambda()).bind("lambda"), this);
}

void AutoCheck::check(const MatchFinder::MatchResult &Result) {
  if (const auto *MatchedDecl =
          Result.Nodes.getNodeAs<VarDecl>("variable-decl")) {
    std::string actualType{""};

    // Check if it is a parameter part of a method, part of a closure record
    // representing a lambda
    if (const ParmVarDecl *parmVarDecl =
            llvm::dyn_cast<ParmVarDecl>(MatchedDecl)) {
      if (const CXXMethodDecl *method = llvm::dyn_cast<CXXMethodDecl>(
              parmVarDecl->getParentFunctionOrMethod())) {
        if (method->getParent()->isLambda()) {
          // auto lambda parameters are allowed
          return;
        }
      }
    }

    if (auto *autoType = llvm::dyn_cast<AutoType>(MatchedDecl->getType())) {
      // Check if the auto variable contains a lambda
      if (autoType->isDeduced()) {
        if (QualType deducedType = autoType->getDeducedType();
            !deducedType.isNull()) {
          if (auto *recordType = llvm::dyn_cast<RecordType>(deducedType)) {
            if (CXXRecordDecl *recordDecl =
                    dyn_cast<CXXRecordDecl>(recordType->getDecl());
                recordDecl && recordDecl->isLambda()) {
              // It is an auto variable containing a lambda
              return;
            }
          }
        }
      }

      // If it is constrained, we allow the type (just like we would for a
      // template or concept)
      if (autoType->isConstrained()) {
        return;
      }

      if (MatchedDecl->getType().getAsString() != "auto") {
        actualType = MatchedDecl->getType().getAsString();
      }
    }

    diag(MatchedDecl->getLocation(), "Do not declare auto variables");

    if (actualType != "") {
      auto beginLoc = MatchedDecl->getBeginLoc();
      auto endLoc = beginLoc.getLocWithOffset(sizeof("auto") - 1);
      auto range = CharSourceRange::getTokenRange(beginLoc, endLoc);
      diag(MatchedDecl->getLocation(), "Use actual type", DiagnosticIDs::Note)
          << FixItHint::CreateReplacement(range, actualType);
    }
  }

  if (const auto *MatchedDecl =
          Result.Nodes.getNodeAs<FunctionDecl>("function-return")) {
    if (const CXXMethodDecl *method =
            llvm::dyn_cast<CXXMethodDecl>(MatchedDecl)) {
      if (method->getParent()->isLambda()) {
        // Lambdas are handled below (see "lambda" binding)
        return;
      }
    }

    // Check if there are dependent template parametes in the output
    QualType returnType = MatchedDecl->getReturnType();
    if (returnType->isDependentType() || returnType->isUndeducedType()) {
      // auto MAY be used in template functions if type depends on template
      // arguments.
      return;
    }

    diag(MatchedDecl->getLocation(), "Do not use auto as return type")
        << MatchedDecl;

    if (auto *autoType =
            llvm::dyn_cast<AutoType>(MatchedDecl->getReturnType())) {
      auto beginLoc = MatchedDecl->getBeginLoc();
      auto endLoc = beginLoc.getLocWithOffset(sizeof("auto") - 1);
      auto range = CharSourceRange::getTokenRange(beginLoc, endLoc);
      diag(MatchedDecl->getLocation(), "Use actual type", DiagnosticIDs::Note)
          << FixItHint::CreateReplacement(
                 range, MatchedDecl->getReturnType().getAsString());
    }
  }

  if (const auto *MatchedDecl =
          Result.Nodes.getNodeAs<CXXRecordDecl>("lambda")) {
    CXXMethodDecl *callOperator = MatchedDecl->getLambdaCallOperator();
    bool hasAutoParam = false;
    for (const auto *param : callOperator->parameters()) {
      auto type = param->getType();
      // Clang can decude auto params as templated type params with name
      // "auto"
      if ((type->isTemplateTypeParmType() && (type.getAsString() == "auto")) ||
          type->isUndeducedAutoType() || llvm::isa<AutoType>(*type)) {
        hasAutoParam = true;
      }
    }

    QualType returnType = callOperator->getReturnType();

    // Clang can dedude auto params as templated type params with name "auto"
    if ((returnType->isTemplateTypeParmType() &&
         (returnType.getAsString() == "auto")) ||
        returnType->isUndeducedAutoType() || llvm::isa<AutoType>(*returnType)) {
      if (hasAutoParam) {
        return;
      }

      diag(MatchedDecl->getLocation(),
           "Lambda with non-auto arguments MUST not use auto as return type");

      if (auto *autoType = llvm::dyn_cast<AutoType>(returnType)) {
        if (returnType.getAsString() != "auto") {
          diag(MatchedDecl->getLocation(), "Use actual type %0",
               DiagnosticIDs::Note)
              << returnType;
        }
      }
    }
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
