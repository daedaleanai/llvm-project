//===--- StructsAndClassesCheck.h - clang-tidy ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_STRUCTSANDCLASSESCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_STRUCTSANDCLASSESCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace daedalean {

/// Daedalean coding standards for structs and classes
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/daedalean-structs-and-classes.html
class StructsAndClassesCheck : public ClangTidyCheck {
public:
  StructsAndClassesCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  enum class ShouldBeAClass {
    No,
    YesNotAnAggregate,
    YesHasUserDeclaredCopyConstructor,
    YesHasUserDeclaredMoveConstructor,
    YesHasUserDeclaredCopyAssignment,
    YesHasUserDeclaredMoveAssignment,
    YesHasUserDeclaredDestructor,
    YesHasNonConstMethod,
  };
  ShouldBeAClass shouldBeAClass(const CXXRecordDecl *record);
};

} // namespace daedalean
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_STRUCTSANDCLASSESCHECK_H
