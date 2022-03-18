//===--- AssignmentOperatorsCheck.h - clang-tidy ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_ASSIGNMENTOPERATORSCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_ASSIGNMENTOPERATORSCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace daedalean {

/// Daedalean coding standards for assignment operators
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/daedalean-assignment-operators.html
class AssignmentOperatorsCheck : public ClangTidyCheck {
public:
  AssignmentOperatorsCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace daedalean
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_ASSIGNMENTOPERATORSCHECK_H
