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

#include <sstream>
#include <unordered_map>
#include <set>

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

  for (const auto & m : pDecl->methods()) {

    if (llvm::isa<CXXDestructorDecl>(m) || m->isImplicit()) {
      continue;
    }
    if (!m->isPure()) {
      return false;
    }
  }

  return !pDecl->methods().empty();
}

bool isFinal(const CXXRecordDecl *pDecl) {
  const auto *Def = pDecl->getDefinition();
  if (!Def)
    return false;
  return Def->hasAttr<FinalAttr>();

}

struct BaseRef {
  std::string ref;
  const CXXBaseSpecifier base;

  std::string getName() const {
    return base.getType()->getAsCXXRecordDecl()->getQualifiedNameAsString();
  }

  bool operator <(const BaseRef & other) const {
    return ref < other.ref;
  }
};

void walkBases(const CXXRecordDecl *pDecl, std::unordered_map<std::string, std::set<BaseRef>> & references) {
  const auto ref = pDecl->getQualifiedNameAsString();
  for (const auto base : pDecl->bases()) {
    const auto decl = base.getType()->getAsCXXRecordDecl();
    references[decl->getQualifiedNameAsString()].insert({ref, base});
    walkBases(decl, references);
  }
}

}

void DerivedClassesCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(cxxRecordDecl(hasDefinition()).bind("x"), this);
}

void DerivedClassesCheck::check(const MatchFinder::MatchResult &Result) {

  const auto *MatchedDecl = Result.Nodes.getNodeAs<CXXRecordDecl>("x");

  if (!MatchedDecl || (MatchedDecl->methods().empty() && MatchedDecl->field_empty() && MatchedDecl->getNumBases() == 0)) {
    // Skip empty class
    return;
  }

  if (isInterface(MatchedDecl)) {
    const auto dtor = MatchedDecl->getDestructor();
    if (!dtor || (dtor->isImplicit() && !dtor->isVirtual())) {
      diag(MatchedDecl->getBraceRange().getBegin(), "Interface must have virtual defaulted destructor");
    } else {
      if (!dtor->isVirtual()) {
        diag(dtor->getLocation(), "Interface destructor must be virtual");
        diag(dtor->getLocation(), "Make destructor virtual", DiagnosticIDs::Note) << FixItHint::CreateInsertion(dtor->getLocation(), "virtual ");
      }
      if (!dtor->isDefaulted()) {
        diag(dtor->getLocation(), "Interface destructor must be defaulted");
        if (const auto body = dtor->getBody(); body) {
          diag(body->getBeginLoc(), "Make destructor default", DiagnosticIDs::Note) << FixItHint::CreateReplacement(body->getSourceRange(), "= default");
        }
      }
    }
  } else {
    if (!isFinal(MatchedDecl)) {
      diag(MatchedDecl->getLocation(), "Non-interface class must be final");
      diag(MatchedDecl->getLocation(), "Make class final", DiagnosticIDs::Note) << FixItHint::CreateInsertion(MatchedDecl->getLocation(), " final");
    }

    for (const auto & m : MatchedDecl->methods()) {
      if (m->isImplicit()) {
        continue;
      }
      if (!m->isVirtual()) {
        continue;
      }

      if (m->size_overridden_methods() == 0) {
        return;
      }

      if (!m->hasAttr<FinalAttr>()) {
        diag(m->getLocation(), "Implemented virtual methods must be final");
        if (const auto attr = m->getAttr<OverrideAttr>(); attr) {
          diag(m->getBody()->getBeginLoc(), "Make method final",
               DiagnosticIDs::Note)
              << FixItHint::CreateReplacement(attr->getLocation(),
                                            " final");
        } else {
          diag(m->getBody()->getBeginLoc(), "Make method final",
               DiagnosticIDs::Note)
              << FixItHint::CreateInsertion(m->getBody()->getBeginLoc(),
                                            " final");
        }
      }
    }
  }

  for (const auto & m : MatchedDecl->methods()) {
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
    if (!isInterface(base.getType()->getAsCXXRecordDecl())) {
      diag(base.getBeginLoc(), "Inheritance from non-interface type is forbidden");
    } else {
      if (base.getAccessSpecifier() == AS_public) {
        continue;
      }

      diag(base.getBeginLoc(),
           "Interfaces must be implemented using public inheritance");
    }
  }

  std::unordered_map<std::string, std::set<BaseRef>> baseReferences;
  walkBases(MatchedDecl, baseReferences);

  for (const auto [base, derived]: baseReferences) {
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
      diag(d.base.getBeginLoc(), "Interface " + base + " must be implemented virtually because it's extended by multiple interfaces (" + allRefs.str() + ") with root type " + MatchedDecl->getQualifiedNameAsString());
      diag(d.base.getBeginLoc(), "Make inheritance virtual",
           DiagnosticIDs::Note)
          << FixItHint::CreateInsertion(d.base.getBeginLoc(),"virtual ");
    }

  }

}


} // namespace daedalean
} // namespace tidy
} // namespace clang
