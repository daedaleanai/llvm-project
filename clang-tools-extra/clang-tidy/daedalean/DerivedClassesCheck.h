//===--- DerivedClassesCheck.h - clang-tidy ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_DERIVEDCLASSESCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_DERIVEDCLASSESCHECK_H

#include "../ClangTidyCheck.h"

#include <set>

namespace clang {
namespace tidy {
namespace daedalean {

/// Daedalean CS.R.30 Derived classes
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/daedalean-derived-classes.html
class DerivedClassesCheck : public ClangTidyCheck {
public:
  DerivedClassesCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
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

  void walkBases(const CXXRecordDecl *pDecl, std::unordered_map<std::string, std::set<BaseRef>> & references);
  const CXXRecordDecl * getRecordFromBase(const CXXBaseSpecifier & base);
};

} // namespace daedalean
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_DERIVEDCLASSESCHECK_H
