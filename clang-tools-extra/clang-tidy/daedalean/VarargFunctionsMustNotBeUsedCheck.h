//===--- VarargFunctionsMustNotBeUsedCheck.h - clang-tidy -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_VARARGFUNCTIONSMUSTNOTBEUSEDCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_VARARGFUNCTIONSMUSTNOTBEUSEDCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace daedalean {

/// This check flags all calls to c-style variadic functions and all use
/// of va_arg.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/daedalean-vararg-functions-must-not-be-used.html
class VarargFunctionsMustNotBeUsedCheck : public ClangTidyCheck {
public:
  VarargFunctionsMustNotBeUsedCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  bool isLanguageVersionSupported(const LangOptions &LangOpts) const override {
    return LangOpts.CPlusPlus;
  }
  void registerPPCallbacks(const SourceManager &SM, Preprocessor *PP,
                           Preprocessor *ModuleExpanderPP) override;
};

} // namespace daedalean
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_DAEDALEAN_VARARGFUNCTIONSMUSTNOTBEUSEDCHECK_H
