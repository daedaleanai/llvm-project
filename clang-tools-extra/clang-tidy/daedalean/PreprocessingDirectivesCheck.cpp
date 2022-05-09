//===--- PreprocessingDirectivesCheck.cpp - clang-tidy --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "PreprocessingDirectivesCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

namespace {

class PreprocessingDirectivesPPCallbacks : public PPCallbacks {
public:
  explicit PreprocessingDirectivesPPCallbacks(ClangTidyCheck &Check,
                                   const SourceManager &SM)
      : Check(Check), SM(SM) {}

  virtual void PragmaDirective(SourceLocation Loc,
                               PragmaIntroducerKind Introducer) override {
    const auto fileId = SM.getFileID(Loc);
    if (!fileId.isValid()) {
        return;
    }
    const auto fileEntry = SM.getFileEntryForID(fileId);
    if (!fileEntry) {
        return;
    }
    const auto fileName = fileEntry->getName();
    if (fileName.endswith(".hh") || fileName.endswith(".h")) {
      const char *data = SM.getCharacterData(Loc);
      std::string pragmaDeclaration;

      for (size_t i = 0; data[i] && data[i] != '\n'; ++i) {
        pragmaDeclaration += data[i];
      }

      if (pragmaDeclaration == "#pragma once") {
        if (PragmaOnce[fileId]) {
          Check.diag(Loc, "Pragma once must be specified only once");
        }

        PragmaOnce[fileId] = true;
        return;
      }
    }

    Check.diag(Loc, "Preprocessor directives must not be used");
  }

  virtual void MacroDefined(const Token &MacroNameTok,
                            const MacroDirective *MD) override {
    if (SM.isWrittenInBuiltinFile(MD->getLocation()) ||
        SM.isWrittenInCommandLineFile(MD->getLocation()))
      return;

    Check.diag(MacroNameTok.getLocation(),
               "Preprocessor directives must not be used");
  }

  virtual void MacroUndefined(const Token &MacroNameTok,
                              const MacroDefinition &MD,
                              const MacroDirective *Undef) override {
    Check.diag(Undef->getLocation(),
               "Preprocessor directives must not be used");
  }

  virtual void If(SourceLocation Loc, SourceRange ConditionRange,
                  ConditionValueKind ConditionValue) {
    Check.diag(Loc, "Preprocessor directives must not be used");
  }

  virtual void Ifdef(SourceLocation Loc, const Token &MacroNameTok,
                     const MacroDefinition &MD) override {
    Check.diag(Loc, "Preprocessor directives must not be used");
  }

  virtual void Ifndef(SourceLocation Loc, const Token &MacroNameTok,
                      const MacroDefinition &MD) override {
    Check.diag(Loc, "Preprocessor directives must not be used");
  }

  virtual void Else(SourceLocation Loc, SourceLocation IfLoc) override {
    Check.diag(Loc, "Preprocessor directives must not be used");
  }

  virtual void Endif(SourceLocation Loc, SourceLocation IfLoc) override {
    Check.diag(Loc, "Preprocessor directives must not be used");
  }

  virtual void Elif(SourceLocation Loc, SourceRange ConditionRange,
                    ConditionValueKind ConditionValue,
                    SourceLocation IfLoc) override {
    Check.diag(Loc, "Preprocessor directives must not be used");
  }

  virtual void Elifdef(SourceLocation Loc, const Token &MacroNameTok,
                       const MacroDefinition &MD) override {
    Check.diag(Loc, "Preprocessor directives must not be used");
  }

  virtual void Elifdef(SourceLocation Loc, SourceRange ConditionRange,
                       SourceLocation IfLoc) override {
    Check.diag(Loc, "Preprocessor directives must not be used");
  }

  virtual void Elifndef(SourceLocation Loc, const Token &MacroNameTok,
                        const MacroDefinition &MD) override {
    Check.diag(Loc, "Preprocessor directives must not be used");
  }

  virtual void Elifndef(SourceLocation Loc, SourceRange ConditionRange,
                        SourceLocation IfLoc) override {
    Check.diag(Loc, "Preprocessor directives must not be used");
  }

  virtual void EndOfMainFile() override {
    for (const auto &[fileId, hasGuard] : PragmaOnce) {
      if (hasGuard) {
        continue;
      }

      Check.diag(SM.getLocForStartOfFile(fileId),
                 "#pragma once must be used as include guard");
    }
  }

  void FileChanged(SourceLocation Loc, FileChangeReason Reason,
                   SrcMgr::CharacteristicKind FileType,
                   FileID PrevFID) override {
    // Record all files we enter. We'll need them to diagnose headers without
    // guards.
    if (Reason == EnterFile && FileType == SrcMgr::C_User) {
      if (const FileEntry *FE = SM.getFileEntryForID(SM.getFileID(Loc))) {
        const auto fileName = FE->getName();
        if (!fileName.endswith(".hh") && !fileName.endswith(".h")) {
          return;
        }

        const auto fileId = SM.getFileID(Loc);
        if (!PragmaOnce.count(fileId)) {
          PragmaOnce[fileId] = false;
        }
      }
    }
  }

private:
  ClangTidyCheck &Check;
  const SourceManager &SM;
  std::map<clang::FileID, bool> PragmaOnce;
};
} // namespace

void PreprocessingDirectivesCheck::registerPPCallbacks(
    const SourceManager &SM, Preprocessor *PP, Preprocessor *ModuleExpanderPP) {
  PP->addPPCallbacks(::std::make_unique<PreprocessingDirectivesPPCallbacks>(*this, SM));
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
