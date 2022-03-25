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

    bool operator <(const IncludeDirective & other) const {
      if (groupType != other.groupType) {
        return static_cast<int>(groupType) < static_cast<int>(other.groupType);
      }
      return  Filename < other.Filename;
    }
  };

  typedef std::vector<IncludeDirective> FileIncludes;
  std::map<clang::FileID, FileIncludes> IncludeDirectives;

  ClangTidyCheck &Check;
  const SourceManager &SM;

private:
  void printInverse(const IncludeDirective & what, const IncludeDirective & beforeWhat);
};

std::string basename(const std::string & path) {
  if (const size_t pos = path.find_last_of('/'); pos == std::string::npos) {
    return path;
  } else {
    return path.substr(pos + 1);
  }
}

std::string ext(const std::string & path) {
  const std::string name = basename(path);
  if (const size_t pos = name.find_last_of("."); pos == std::string::npos) {
    return "";
  } else {
    return name.substr(pos);
  }
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
    Check.diag(FilenameRange.getBegin(), "Include <" + FileName.str() + "> not resolved");
    return;
  }

  if (!IsAngled) {
    Check.diag(FilenameRange.getBegin(), "Use #include<> instead of include \"\"") << FixItHint::CreateReplacement(FilenameRange, "<" + FileName.str() + ">");
  }

  const std::string fileName(FileName);
  const std::string project = fileName.substr(0, fileName.find_first_of('/'));
  const std::string fileExt = ext(fileName);
  if (fileExt != ".hh" && fileExt != ".h") {
    Check.diag(FilenameRange.getBegin(), "Only header files (.h, .hh) should be included");
  }

  const std::string currentDir = SM.getFileManager().getCanonicalName(SM.getFileEntryForID(SM.getFileID(HashLoc))->getDir()).str();
  const std::string includeDir = SM.getFileManager().getCanonicalName(File->getDir()).str();

  const size_t prefixSize = includeDir.find(project) + project.length() + 1;

  GroupType groupType = GroupType::External;

  if (currentDir == includeDir) {
    groupType = GroupType::SameDirectory;

    const std::string includeBasename = basename(fileName);
    const std::string curFileName = SM.getFileEntryForID(SM.getFileID(HashLoc))->getName().str();
    const std::string curBasename = basename(curFileName);

    const std::string curExt = ext(curBasename);

    if ((curExt == ".cc" || curExt == ".c") && curBasename.substr(0, curBasename.find_last_of('.')) == includeBasename.substr(0, includeBasename.find_last_of('.'))) {
      groupType = GroupType::RelatedHeader;
    }

  } else if (currentDir.substr(0, prefixSize) == includeDir.substr(0, prefixSize))  {
    groupType = GroupType::SameProject;
  }

  IncludeDirective ID = {HashLoc, FilenameRange, fileName, groupType, project};

  // Bucket the include directives by the id of the file they were declared in.
  IncludeDirectives[SM.getFileID(HashLoc)].push_back(std::move(ID));
}

void IncludeOrderPPCallbacks::printInverse(const IncludeDirective & what, const IncludeDirective & beforeWhat) {
  Check.diag(what.Range.getBegin(), "<" + what.Filename + "> should be included before <" + beforeWhat.Filename + ">");
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
