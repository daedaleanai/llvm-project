//===--- IncludeOrderCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "IncludeOrderCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "daedalean-include-order"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

namespace {

enum class GroupType {
  RelatedHeader = 0,
  SameDirectory,
  SameProject,
  External
};

class IncludeOrderPPCallbacks : public PPCallbacks {
public:
  explicit IncludeOrderPPCallbacks(ClangTidyCheck &Check,
                                   const SourceManager &SM)
      : Check(Check), SM(SM) {}

  void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok,
                          StringRef FileName, bool IsAngled,
                          CharSourceRange FilenameRange, const FileEntry *File,
                          StringRef SearchPath, StringRef RelativePath,
                          const Module *Imported,
                          SrcMgr::CharacteristicKind FileType) override;
  void EndOfMainFile() override;

private:
  struct IncludeDirective {
    SourceLocation Loc;
    CharSourceRange Range;
    std::string Filename;

    GroupType groupType;
    std::string project;

    bool operator<(const IncludeDirective &other) const {
      if (groupType != other.groupType) {
        return static_cast<int>(groupType) < static_cast<int>(other.groupType);
      }
      return Filename < other.Filename;
    }
  };

  typedef std::vector<IncludeDirective> FileIncludes;
  std::map<clang::FileID, FileIncludes> IncludeDirectives;

  ClangTidyCheck &Check;
  const SourceManager &SM;

private:
  void printInverse(const IncludeDirective &what,
                    const IncludeDirective &beforeWhat);
};

std::string basename(const std::string &path) {
  if (const size_t pos = path.find_last_of('/'); pos == std::string::npos) {
    return path;
  } else {
    return path.substr(pos + 1);
  }
}

std::string ext(const std::string &path) {
  const std::string name = basename(path);
  if (const size_t pos = name.find_last_of("."); pos == std::string::npos) {
    return "";
  } else {
    return name.substr(pos);
  }
}

llvm::Optional<std::string> getPathInProject(StringRef fileSystemPath) {
  constexpr static StringRef SourceDir = "src";
  constexpr static StringRef IncludeDir = "include";

  auto pos = fileSystemPath.rfind(SourceDir);
  if (pos != fileSystemPath.npos) {
    return fileSystemPath.substr(pos + SourceDir.size() + 1).str();
  }

  pos = fileSystemPath.rfind(IncludeDir);
  if (pos != fileSystemPath.npos) {
    return fileSystemPath.substr(pos + IncludeDir.size() + 1).str();
  }

  return llvm::Optional<std::string>{llvm::NoneType{}};
}

std::string projectForPathInProject(StringRef pathInProject) {
  size_t pos = pathInProject.find_first_of("/");
  if (pos != pathInProject.npos) {
    return pathInProject.substr(0, pos).str();
  }
  return pathInProject.str();
}

} // namespace

void IncludeOrderCheck::registerPPCallbacks(const SourceManager &SM,
                                            Preprocessor *PP,
                                            Preprocessor *ModuleExpanderPP) {
  PP->addPPCallbacks(::std::make_unique<IncludeOrderPPCallbacks>(*this, SM));
}

void IncludeOrderPPCallbacks::InclusionDirective(
    SourceLocation HashLoc, const Token &IncludeTok, StringRef FileName,
    bool IsAngled, CharSourceRange FilenameRange, const FileEntry *File,
    StringRef SearchPath, StringRef RelativePath, const Module *Imported,
    SrcMgr::CharacteristicKind FileType) {

  if (!File) {
    Check.diag(FilenameRange.getBegin(),
               "Include <" + FileName.str() + "> not resolved",
               DiagnosticIDs::Error);
    return;
  }

  if (!IsAngled) {
    Check.diag(FilenameRange.getBegin(),
               "Use #include<> instead of include \"\"")
        << FixItHint::CreateReplacement(FilenameRange,
                                        "<" + FileName.str() + ">");
  }

  const std::string FileNameStr(FileName);
  LLVM_DEBUG(llvm::dbgs() << "Parsing included file #include <" << FileNameStr
                          << ">\n");

  const std::string Project =
      FileNameStr.substr(0, FileNameStr.find_first_of('/'));
  LLVM_DEBUG(llvm::dbgs() << "\tProject:\t" << Project << "\n");

  const std::string FileExt = ext(FileNameStr);
  LLVM_DEBUG(llvm::dbgs() << "\tExtension:\t" << FileExt << "\n");

  if (FileExt != ".hh" && FileExt != ".h") {
    Check.diag(FilenameRange.getBegin(),
               "Only header files (.h, .hh) should be included");
    LLVM_DEBUG(llvm::dbgs() << "Invalid extension\n");
  }

  const auto FileEntry = SM.getFileEntryForID(SM.getFileID(HashLoc));

  if (!FileEntry) {
    // This is not regular file
    return;
  }

  const std::string CurrentDir =
      SM.getFileManager().getCanonicalName(FileEntry->getDir()).str();
  LLVM_DEBUG(llvm::dbgs() << "\tpwd:\t" << CurrentDir << "\n");

  const std::string IncludeDir =
      SM.getFileManager().getCanonicalName(File->getDir()).str();
  LLVM_DEBUG(llvm::dbgs() << "\tincludeDir:\t" << IncludeDir << "\n");

  GroupType GroupType = GroupType::External;

  auto CurrentDirInProject = getPathInProject(CurrentDir);
  LLVM_DEBUG(llvm::dbgs() << "\tpwdInProj:\t" << CurrentDirInProject << "\n");
  auto IncludeDirInProject = getPathInProject(IncludeDir);
  LLVM_DEBUG(llvm::dbgs() << "\tincludeInProj:\t" << IncludeDirInProject
                          << "\n");

  if (CurrentDirInProject.hasValue() && IncludeDirInProject.hasValue() &&
      (CurrentDirInProject.getValue() == IncludeDirInProject.getValue())) {
    GroupType = GroupType::SameDirectory;

    const std::string IncludeBasename = basename(FileNameStr);
    LLVM_DEBUG(llvm::dbgs() << "\tincBasename:\t" << IncludeBasename << "\n");
    const std::string CurFileName =
        SM.getFileEntryForID(SM.getFileID(HashLoc))->getName().str();
    LLVM_DEBUG(llvm::dbgs() << "\tcurFileName:\t" << CurFileName << "\n");
    const std::string CurBasename = basename(CurFileName);
    LLVM_DEBUG(llvm::dbgs() << "\tcurBaseName:\t" << CurBasename << "\n");

    const std::string CurExt = ext(CurBasename);
    LLVM_DEBUG(llvm::dbgs() << "\tcurExt:\t" << CurExt << "\n");

    if ((CurExt == ".cc" || CurExt == ".c") &&
        CurBasename.substr(0, CurBasename.find_last_of('.')) ==
            IncludeBasename.substr(0, IncludeBasename.find_last_of('.'))) {
      GroupType = GroupType::RelatedHeader;
      LLVM_DEBUG(llvm::dbgs() << "Related Header\n");
    }
  } else if (CurrentDirInProject.hasValue() && IncludeDirInProject.hasValue() &&
             (projectForPathInProject(CurrentDirInProject.getValue()) ==
              projectForPathInProject(IncludeDirInProject.getValue()))) {
    GroupType = GroupType::SameProject;
    LLVM_DEBUG(llvm::dbgs() << "Same project\n");
  }

  IncludeDirective ID = {HashLoc, FilenameRange, FileNameStr, GroupType,
                         Project};

  // Bucket the include directives by the id of the file they were declared in.
  IncludeDirectives[SM.getFileID(HashLoc)].push_back(std::move(ID));
}

void IncludeOrderPPCallbacks::printInverse(const IncludeDirective &what,
                                           const IncludeDirective &beforeWhat) {
  Check.diag(what.Range.getBegin(), "<" + what.Filename +
                                        "> should be included before <" +
                                        beforeWhat.Filename + ">");
}

void IncludeOrderPPCallbacks::EndOfMainFile() {
  if (IncludeDirectives.empty())
    return;

  for (auto &Bucket : IncludeDirectives) {
    auto &FileDirectives = Bucket.second;

    if (Bucket.second.empty()) {
      continue;
    }

    GroupType currentGroup = Bucket.second.front().groupType;

    for (auto it = Bucket.second.begin() + 1; it != Bucket.second.end(); ++it) {
      if (*it < *(it - 1)) {
        // Wrong order
        for (const auto &incPos : Bucket.second) {
          if (incPos < *it) {
            continue;
          }
          printInverse(*it, incPos);
          break;
        }
      }

      if (currentGroup != it->groupType) {
        currentGroup = it->groupType;
        if (SM.getExpansionLineNumber(it->Loc) ==
            SM.getExpansionLineNumber((it - 1)->Loc) + 1) {
          Check.diag((it - 1)->Range.getBegin(),
                     "<" + (it - 1)->Filename + "> and <" + it->Filename +
                         "> should be in different include groups");
        }
      } else {
        if (SM.getExpansionLineNumber(it->Loc) !=
            SM.getExpansionLineNumber((it - 1)->Loc) + 1) {
          Check.diag((it - 1)->Range.getBegin(),
                     "<" + (it - 1)->Filename + "> and <" + it->Filename +
                         "> should be in the same include group");
        }
      }
    }
  }

  IncludeDirectives.clear();
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
