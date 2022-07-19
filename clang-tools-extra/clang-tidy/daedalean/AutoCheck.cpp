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

namespace {
bool checkIsDependent(Stmt *stmt) {
  if (!stmt) {
    return false;
  }

  if (const auto ptr = llvm::dyn_cast<ForStmt>(stmt); ptr) {
    return checkIsDependent(ptr->getBody());
  }
  if (const auto ptr = llvm::dyn_cast<WhileStmt>(stmt); ptr) {
    return checkIsDependent(ptr->getBody());
  }
  if (const auto ptr = llvm::dyn_cast<DoStmt>(stmt); ptr) {
    return checkIsDependent(ptr->getBody());
  }
  if (const auto ptr = llvm::dyn_cast<IfStmt>(stmt); ptr) {
    if (checkIsDependent(ptr->getThen())) {
      return true;
    }
    return checkIsDependent(ptr->getElse());
  }
  if (const auto ptr = llvm::dyn_cast<CompoundStmt>(stmt); ptr) {
    for (const auto s : ptr->body()) {
      if (checkIsDependent(s)) {
        return true;
      }
    }
  }

  if (const auto ptr = llvm::dyn_cast<ReturnStmt>(stmt); ptr) {
    const auto expr = ptr->getRetValue();
    return expr->getType()->isDependentType();
  }

  return false;
}
} // namespace

void AutoCheck::registerMatchers(MatchFinder *Finder) {
  // We register 4 matchers separately:
  //  - Variable declarations (which includes parameters)
  //  - Function declarations (to check return types)
  //  - Template Function declarations (to check return types)
  //  - Lambdas (which are handled separately given they have slightly different
  //  rules).

  // This matcher should not bind to implicit variables added by clang, that is
  // why it uses traverse with TK_IgnoreUnlessSpelledInSource.
  const auto hasAutoType =
      anyOf(hasType(autoType()), hasType(references(autoType())),
            hasType(pointsTo(autoType())),
            allOf(hasType(templateTypeParmType()), hasType(asString("auto"))));
  Finder->addMatcher(
      traverse(TK_IgnoreUnlessSpelledInSource,
               varDecl(hasAutoType,
                       unless(anyOf(isInitCapture(), decompositionDecl(),
                                    hasInitializer(expr(lambdaExpr()))))))
          .bind("variable-decl"),
      this);
  Finder->addMatcher(
      functionDecl(returns(autoType()),
                   unless(anyOf(hasTrailingReturn(),
                                ast_matchers::isTemplateInstantiation())))
          .bind("function-return"),
      this);
  Finder->addMatcher(functionTemplateDecl().bind("function-template-return"),
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

      if (MatchedDecl->getType().getUnqualifiedType().getAsString() != "auto") {
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
    return;
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

    // Check if there are dependent template parameters in the output
    QualType returnType = MatchedDecl->getReturnType();
    if (returnType->isDependentType() || returnType->isUndeducedType()) {
      // auto MAY be used in template functions if type depends on template
      // arguments.
      return;
    }

    diag(MatchedDecl->getLocation(), "Do not use auto as return type");

    if (auto *autoType =
            llvm::dyn_cast<AutoType>(MatchedDecl->getReturnType())) {
      auto beginLoc = MatchedDecl->getBeginLoc();
      auto endLoc = beginLoc.getLocWithOffset(sizeof("auto") - 1);
      auto range = CharSourceRange::getTokenRange(beginLoc, endLoc);
      diag(MatchedDecl->getLocation(), "Use actual type", DiagnosticIDs::Note)
          << FixItHint::CreateReplacement(
                 range, MatchedDecl->getReturnType().getAsString());
    }
    return;
  }

  if (const auto *MatchedDecl =
          Result.Nodes.getNodeAs<CXXRecordDecl>("lambda")) {
    CXXMethodDecl *callOperator = MatchedDecl->getLambdaCallOperator();
    bool hasAutoParam = false;
    for (const auto *param : callOperator->parameters()) {
      auto type = param->getType();
      // Clang can decude auto params as templated type params with name
      // "auto"
      if ((type->isTemplateTypeParmType() &&
           (type.getUnqualifiedType().getAsString() == "auto")) ||
          type->isUndeducedAutoType() || llvm::isa<AutoType>(*type)) {
        hasAutoParam = true;
      }
    }

    QualType returnType = callOperator->getReturnType();

    // Clang can dedude auto params as templated type params with name "auto"
    if ((returnType->isTemplateTypeParmType() &&
         (returnType.getUnqualifiedType().getAsString() == "auto")) ||
        returnType->isUndeducedAutoType() || llvm::isa<AutoType>(*returnType)) {
      if (hasAutoParam) {
        return;
      }

      diag(MatchedDecl->getLocation(),
           "Lambda with non-auto arguments MUST not use auto as return type");

      if (auto *autoType = llvm::dyn_cast<AutoType>(returnType)) {
        if (returnType.getUnqualifiedType().getAsString() != "auto") {
          diag(MatchedDecl->getLocation(), "Use actual type %0",
               DiagnosticIDs::Note)
              << returnType;
        }
      }
    }
    return;
  }

  if (const auto *MatchedDecl = Result.Nodes.getNodeAs<FunctionTemplateDecl>(
          "function-template-return")) {

    const FunctionDecl *Function = MatchedDecl->getAsFunction();
    if (const CXXMethodDecl *method = llvm::dyn_cast<CXXMethodDecl>(Function)) {
      if (method->getOverloadedOperator() == OO_Call) {
        if (const CXXRecordDecl *record = method->getParent()) {
          if (record->isLambda()) {
            // Handled separately
            return;
          }
        }
      }
    }

    QualType ReturnType = Function->getReturnType();
    if (!ReturnType->isUndeducedAutoType()) {
      return;
    }

    if (ReturnType->isDependentType() &&
        checkIsDependent(Function->getBody())) {
      return;
    }

    diag(MatchedDecl->getLocation(), "Do not use auto as return type");
    return;
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
