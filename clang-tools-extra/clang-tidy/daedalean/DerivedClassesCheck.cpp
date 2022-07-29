//===--- DerivedClassesCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "DerivedClassesCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <set>
#include <sstream>
#include <unordered_map>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

namespace {

bool isInterface(const CXXRecordDecl *pDecl) {
  if (!pDecl->field_empty()) {
    return false;
  }

  if (pDecl->isEffectivelyFinal()) {
    return false;
  }

  for (const auto *m : pDecl->methods()) {
    if (m == nullptr) {
      continue;
    }

    if (llvm::isa<CXXDestructorDecl>(m) || m->isImplicit()) {
      continue;
    }
    if (!m->isPure()) {
      return false;
    }
  }

  return pDecl->isAbstract();
}

bool isFinal(const CXXRecordDecl *pDecl) {
  const auto *Def = pDecl->getDefinition();
  if (!Def)
    return false;
  return Def->hasAttr<FinalAttr>();
}

} // namespace

void DerivedClassesCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(cxxRecordDecl(hasDefinition()).bind("x"), this);
}

void DerivedClassesCheck::check(const MatchFinder::MatchResult &Result) {

  const auto *MatchedDecl = Result.Nodes.getNodeAs<CXXRecordDecl>("x");

  if (!MatchedDecl ||
      (MatchedDecl->methods().empty() && MatchedDecl->field_empty() &&
       MatchedDecl->getNumBases() == 0)) {
    // Skip empty class
    return;
  }

  if (MatchedDecl->isLambda()) {
    return;
  }

  if (isInterface(MatchedDecl)) {
    const auto dtor = MatchedDecl->getDestructor();
    // 1. Each interface MUST have a virtual default destructor.
    if (!dtor || (dtor->isImplicit() && !dtor->isVirtual())) {
      diag(MatchedDecl->getBraceRange().getBegin(),
           "Interface %0 must have virtual defaulted destructor")
          << MatchedDecl;
    } else {
      if (!dtor->isVirtual()) {
        diag(dtor->getLocation(), "Interface destructor must be virtual");
        diag(dtor->getLocation(), "Make destructor virtual",
             DiagnosticIDs::Note)
            << FixItHint::CreateInsertion(dtor->getLocation(), "virtual ");
      }
      if (!dtor->isDefaulted()) {
        diag(dtor->getLocation(), "Interface destructor must be defaulted");
        if (const auto body = dtor->getBody(); body) {
          diag(body->getBeginLoc(), "Make destructor default",
               DiagnosticIDs::Note)
              << FixItHint::CreateReplacement(body->getSourceRange(),
                                              "= default");
        }
      }
    }
  } else {
    // 7. If a class contains at least one non-pure-virtual method other than
    // destructor it MUST be declared final.
    if (!isFinal(MatchedDecl)) {
      diag(MatchedDecl->getLocation(), "Non-interface class %0 must be final")
          << MatchedDecl;
      diag(MatchedDecl->getLocation(), "Make class final", DiagnosticIDs::Note)
          << FixItHint::CreateInsertion(MatchedDecl->getLocation(), " final");
    }

    for (const auto &m : MatchedDecl->methods()) {
      if (m->isImplicit()) {
        continue;
      }
      if (!m->isVirtual()) {
        continue;
      }
      if (m->isPure()) {
        continue;
      }

      if (llvm::isa<CXXDestructorDecl>(m) && m->isDefaulted()) {
        continue;
      }
      // 6. Virtual functions MUST contain exactly one of two specifiers:
      // virtual for new function or final if function overrides method from
      // base class.
      if (!m->hasAttr<FinalAttr>()) {
        diag(m->getLocation(), "Implemented virtual methods must be final");
        if (const auto attr = m->getAttr<OverrideAttr>(); attr) {
          diag(m->getLocation(), "Make method final", DiagnosticIDs::Note)
              << FixItHint::CreateReplacement(attr->getLocation(), " final");
        } else {
          diag(m->getLocation(), "Make method final", DiagnosticIDs::Note)
              << FixItHint::CreateInsertion(m->getLocation(), " final");
        }
      }
    }
  }

  // 5. All entity names MUST be unique in all inherited interfaces. If two
  // interfaces share the same methods they MUST extend the common base
  // interface.
  for (const auto &m : MatchedDecl->methods()) {
    if (m->isImplicit()) {
      continue;
    }
    if (!m->isVirtual()) {
      continue;
    }
    if (llvm::isa<CXXDestructorDecl>(m)) {
      continue;
    }

    if (m->size_overridden_methods() > 1) {
      std::ostringstream oss;
      oss << "Method " << m->getNameAsString() << " is declared in ";
      bool first = true;
      for (const auto base : m->overridden_methods()) {
        if (first) {
          first = false;
        } else {
          oss << ", ";
        }
        oss << base->getParent()->getQualifiedNameAsString();
      }
      oss << ". Common interface must be introduced.";
      diag(m->getLocation(), oss.str());
    }
  }

  for (const auto base : MatchedDecl->bases()) {
    // 2. Inheritance from base class with non-pure-virtual methods other than
    // destructor and/or data members is forbidden.
    const CXXRecordDecl *baseRecord = getRecordFromBase(base);
    if (baseRecord == nullptr) {
      continue;
    }

    if (!isInterface(baseRecord)) {
      diag(base.getBeginLoc(),
           "Inheritance from non-interface type is forbidden");
    } else {
      // 3. Public inheritance MUST be used to implement interfaces.
      if (base.getAccessSpecifier() == AS_public) {
        continue;
      }

      diag(base.getBeginLoc(),
           "Interfaces must be implemented using public inheritance");
    }
  }

  std::unordered_map<std::string, std::set<BaseRef>> baseReferences;
  walkBases(MatchedDecl, baseReferences);
  // 4. Base class MUST be declared virtual if it is in diamond inheritance.
  for (const auto [base, derived] : baseReferences) {
    if (derived.size() <= 1) {
      continue;
    }

    std::ostringstream allRefs;
    bool first = true;

    for (const auto d : derived) {
      if (first) {
        first = false;
      } else {
        allRefs << ", ";
      }
      allRefs << d.getName();
    }

    for (const auto d : derived) {
      diag(d.base.getBeginLoc(), "Interface " + base +
                                     " must be implemented virtually because "
                                     "it's extended by multiple interfaces (" +
                                     allRefs.str() + ") with root type " +
                                     MatchedDecl->getQualifiedNameAsString());
      diag(d.base.getBeginLoc(), "Make inheritance virtual",
           DiagnosticIDs::Note)
          << FixItHint::CreateInsertion(d.base.getBeginLoc(), "virtual ");
    }
  }
}

const CXXRecordDecl *
DerivedClassesCheck::getRecordFromBase(const CXXBaseSpecifier &base) {
  if (const CXXRecordDecl *baseRecord = base.getType()->getAsCXXRecordDecl()) {
    return baseRecord;
  }

  if (const auto spec = llvm::dyn_cast_or_null<TemplateSpecializationType>(
          base.getType().getTypePtr())) {
    if (const auto cls = llvm::dyn_cast_or_null<ClassTemplateDecl>(
            spec->getTemplateName().getAsTemplateDecl())) {
      return cls->getTemplatedDecl();
    } else {
      diag(base.getBeginLoc(), "Unable to get base template spec %0 as record")
          << spec->getTemplateName();
      return nullptr;
    }
  } else {
    diag(base.getBeginLoc(),
         "Unable to get base of type %0 as record or template")
        << base.getType();
    return nullptr;
  }
}

void DerivedClassesCheck::walkBases(
    const CXXRecordDecl *pDecl,
    std::unordered_map<std::string, std::set<BaseRef>> &references) {
  const auto ref = pDecl->getQualifiedNameAsString();
  for (const auto base : pDecl->bases()) {
    const auto decl = getRecordFromBase(base);
    if (!decl) {
      continue;
    }
    references[decl->getQualifiedNameAsString()].insert({ref, base});
    walkBases(decl, references);
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
