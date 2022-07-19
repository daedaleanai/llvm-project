//===--- StructsAndClassesCheck.cpp - clang-tidy --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "StructsAndClassesCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

StructsAndClassesCheck::ShouldBeAClass
StructsAndClassesCheck::shouldBeAClass(const CXXRecordDecl *record) {
  if (record->hasUserDeclaredCopyConstructor()) {
    return ShouldBeAClass::YesHasUserDeclaredCopyConstructor;
  }

  if (record->hasUserDeclaredMoveConstructor()) {
    return ShouldBeAClass::YesHasUserDeclaredMoveConstructor;
  }

  if (record->hasUserDeclaredCopyAssignment()) {
    return ShouldBeAClass::YesHasUserDeclaredCopyAssignment;
  }

  if (record->hasUserDeclaredMoveAssignment()) {
    return ShouldBeAClass::YesHasUserDeclaredCopyAssignment;
  }

  if (record->hasUserDeclaredDestructor()) {
    return ShouldBeAClass::YesHasUserDeclaredDestructor;
  }

  if (!record->isAggregate()) {
    return ShouldBeAClass::YesNotAnAggregate;
  }

  for (const CXXMethodDecl *method : record->methods()) {
    const bool isConstructor = llvm::isa<CXXConstructorDecl>(method);
    if (isConstructor) {
      // Constructors are ignored
      continue;
    }
    const bool isDestructor = llvm::isa<CXXDestructorDecl>(method);
    if (isDestructor) {
      auto *destructor = llvm::cast<CXXDestructorDecl>(method);
      if (!destructor->isUserProvided()) {
        // If not defined by the user, ignore it
        continue;
      }
    }

    if (method->isStatic()) {
      continue;
    }

    if (method->isCopyAssignmentOperator() && !method->isUserProvided()) {
      continue;
    }
    if (method->isMoveAssignmentOperator() && !method->isUserProvided()) {
      continue;
    }

    if (!method->isConst()) {
      return ShouldBeAClass::YesHasNonConstMethod;
    }
  }

  return ShouldBeAClass::No;
}

void StructsAndClassesCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(cxxRecordDecl(hasDefinition()).bind("x"), this);
}

void StructsAndClassesCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<CXXRecordDecl>("x");

  if (!MatchedDecl->isStruct() && !MatchedDecl->isClass()) {
    // Skip enums, unions etc
    return;
  }

  if (const ShouldBeAClass shouldBeClass = shouldBeAClass(MatchedDecl);
      shouldBeClass != ShouldBeAClass::No) {
    if (MatchedDecl->isStruct()) {
      switch (shouldBeClass) {
      case ShouldBeAClass::YesNotAnAggregate:
        diag(MatchedDecl->getBeginLoc(),
             "Non-Aggregate type %0 must be declared as a class")
            << MatchedDecl;
        break;
      case ShouldBeAClass::YesHasUserDeclaredCopyConstructor:
        diag(MatchedDecl->getBeginLoc(),
             "Type %0 with a user-provided copy constructor must be declared "
             "as a class")
            << MatchedDecl;
        break;
      case ShouldBeAClass::YesHasUserDeclaredMoveConstructor:
        diag(MatchedDecl->getBeginLoc(),
             "Type %0 with a user-provided move constructor must be declared "
             "as a class")
            << MatchedDecl;
        break;
      case ShouldBeAClass::YesHasUserDeclaredCopyAssignment:
        diag(MatchedDecl->getBeginLoc(),
             "Type %0 with a user-provided move copy assignment operator must "
             "be declared as a class")
            << MatchedDecl;
        break;
      case ShouldBeAClass::YesHasUserDeclaredMoveAssignment:
        diag(MatchedDecl->getBeginLoc(),
             "Type %0 with a a user-provided move move assignment operator "
             "must be declared as a class")
            << MatchedDecl;
        break;
      case ShouldBeAClass::YesHasUserDeclaredDestructor:
        diag(MatchedDecl->getBeginLoc(),
             "Type %0 with a user-provided destructor must be declared as a "
             "class")
            << MatchedDecl;
        break;
      case ShouldBeAClass::YesHasNonConstMethod:
        diag(MatchedDecl->getBeginLoc(),
             "Type %0 with non-const methods must be declared as a class")
            << MatchedDecl;
        break;
      default:
        break;
      }
      diag(MatchedDecl->getBeginLoc(), "use class", DiagnosticIDs::Note)
          << FixItHint::CreateInsertion(MatchedDecl->getBeginLoc(), "class");
    }
  } else {
    if (!MatchedDecl->isStruct()) {
      diag(MatchedDecl->getBeginLoc(), "Type %0 must be declared as a struct")
          << MatchedDecl;
      diag(MatchedDecl->getBeginLoc(), "use struct", DiagnosticIDs::Note)
          << FixItHint::CreateReplacement(MatchedDecl->getBeginLoc(), "struct");
    }
  }

  if (MatchedDecl->isClass()) {
    for (const auto *field : MatchedDecl->fields()) {
      if (field->getType().isConstQualified()) {
        continue;
      }
      if (field->getAccess() != AS_private && field->getAccess() != AS_none) {
        diag(field->getBeginLoc(),
             "All non-const data members of class MUST be private")
            << MatchedDecl;
        diag(field->getBeginLoc(), "Make field private", DiagnosticIDs::Note)
            << FixItHint::CreateInsertion(field->getBeginLoc(), "private:");
      }
    }
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
