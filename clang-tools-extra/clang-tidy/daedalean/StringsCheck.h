//===--- StringsCheck.h - clang-tidy ----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_STRINGSCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_STRINGSCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace daedalean {

/// Checks for usages of C-style string literals and warns on them.
/// User-defined literals are excluded from this checker.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/daedalean-strings.html
class StringsCheck : public ClangTidyCheck {
public:
  StringsCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  llvm::Optional<TraversalKind> getCheckTraversalKind() const override {
    return TK_IgnoreUnlessSpelledInSource;
  }
};

} // namespace daedalean
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_STRINGSCHECK_H
