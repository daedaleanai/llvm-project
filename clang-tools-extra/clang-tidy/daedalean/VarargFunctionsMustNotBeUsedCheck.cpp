//===--- VarargFunctionsMustNotBeUsedCheck.cpp - clang-tidy ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "VarargFunctionsMustNotBeUsedCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/Token.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

const internal::VariadicDynCastAllOfMatcher<Stmt, VAArgExpr> VAArgExpr;

static constexpr StringRef VaArgWarningMessage =
    "do not use va_arg to define c-style vararg functions; "
    "use variadic templates instead";

static constexpr StringRef VaArgFuncWarningMessage =
    "do not define c-style vararg functions; use variadic templates instead";

namespace {
AST_MATCHER(QualType, isVAList) {
  ASTContext &Context = Finder->getASTContext();
  QualType Desugar = Node.getDesugaredType(Context);
  QualType NodeTy = Node.getUnqualifiedType();

  auto CheckVaList = [](QualType NodeTy, QualType Expected,
                        const ASTContext &Context) {
    if (NodeTy == Expected)
      return true;
    QualType Desugar = NodeTy;
    QualType Ty;
    do {
      Ty = Desugar;
      Desugar = Ty.getSingleStepDesugaredType(Context);
      if (Desugar == Expected)
        return true;
    } while (Desugar != Ty);
    return false;
  };

  // The internal implementation of __builtin_va_list depends on the target
  // type. Some targets implements va_list as 'char *' or 'void *'.
  // In these cases we need to remove all typedefs one by one to check this.
  using BuiltinVaListKind = TargetInfo::BuiltinVaListKind;
  BuiltinVaListKind VaListKind = Context.getTargetInfo().getBuiltinVaListKind();
  if (VaListKind == BuiltinVaListKind::CharPtrBuiltinVaList ||
      VaListKind == BuiltinVaListKind::VoidPtrBuiltinVaList) {
    if (CheckVaList(NodeTy, Context.getBuiltinVaListType(), Context))
      return true;
  } else if (Desugar ==
             Context.getBuiltinVaListType().getDesugaredType(Context)) {
    return true;
  }

  // We also need to check the implementation of __builtin_ms_va_list in the
  // same way, because it may differ from the va_list implementation.
  if (Desugar == Context.getBuiltinMSVaListType().getDesugaredType(Context) &&
      CheckVaList(NodeTy, Context.getBuiltinMSVaListType(), Context)) {
    return true;
  }

  return false;
}

AST_MATCHER_P(AdjustedType, hasOriginalType,
              ast_matchers::internal::Matcher<QualType>, InnerType) {
  return InnerType.matches(Node.getOriginalType(), Finder, Builder);
}

class VaArgPPCallbacks : public PPCallbacks {
public:
  VaArgPPCallbacks(VarargFunctionsMustNotBeUsedCheck *Check) : Check(Check) {}

  void MacroExpands(const Token &MacroNameTok, const MacroDefinition &MD,
                    SourceRange Range, const MacroArgs *Args) override {
    if (MacroNameTok.getIdentifierInfo()->getName() == "va_arg") {
      Check->diag(MacroNameTok.getLocation(), VaArgWarningMessage);
    }
  }

private:
  VarargFunctionsMustNotBeUsedCheck *Check;
};
} // namespace

void VarargFunctionsMustNotBeUsedCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(VAArgExpr().bind("va_use"), this);

  Finder->addMatcher(
      callExpr(callee(functionDecl(isVariadic()))).bind("callvararg"), this);

  // Match on variadic function declarations that have definitions, the
  // exception is function declaration for SFINAE without definition
  Finder->addMatcher(
      functionDecl(allOf(isVariadic(), isDefinition())).bind("varargfunc"),
      this);

  Finder->addMatcher(
      varDecl(hasType(qualType(
                  anyOf(isVAList(), decayedType(hasOriginalType(isVAList()))))))
          .bind("va_list"),
      this);
}

void VarargFunctionsMustNotBeUsedCheck::registerPPCallbacks(
    const SourceManager &SM, Preprocessor *PP, Preprocessor *ModuleExpanderPP) {
  PP->addPPCallbacks(std::make_unique<VaArgPPCallbacks>(this));
}

static bool hasSingleVariadicArgumentWithValue(const CallExpr *C, uint64_t I) {
  const auto *FDecl = dyn_cast<FunctionDecl>(C->getCalleeDecl());
  if (!FDecl)
    return false;

  auto N = FDecl->getNumParams(); // Number of parameters without '...'
  if (C->getNumArgs() != N + 1)
    return false; // more/less than one argument passed to '...'

  const auto *IntLit =
      dyn_cast<IntegerLiteral>(C->getArg(N)->IgnoreParenImpCasts());
  if (!IntLit)
    return false;

  if (IntLit->getValue() != I)
    return false;

  return true;
}

void VarargFunctionsMustNotBeUsedCheck::check(
    const MatchFinder::MatchResult &Result) {
  if (const auto *Matched = Result.Nodes.getNodeAs<CallExpr>("callvararg")) {
    if (hasSingleVariadicArgumentWithValue(Matched, 0))
      return;
    diag(Matched->getExprLoc(), "do not call c-style vararg functions");
  }

  if (const auto *Matched = Result.Nodes.getNodeAs<Expr>("va_use")) {
    diag(Matched->getExprLoc(), VaArgWarningMessage);
  }

  if (const auto *Matched = Result.Nodes.getNodeAs<VarDecl>("va_list")) {
    auto SR = Matched->getSourceRange();
    if (SR.isInvalid())
      return; // some implicitly generated builtins take va_list
    diag(SR.getBegin(), "do not declare variables of type va_list; "
                        "use variadic templates instead");
  }

  if (const auto *Matched =
          Result.Nodes.getNodeAs<FunctionDecl>("varargfunc")) {
    diag(Matched->getLocation(), VaArgFuncWarningMessage);
  }
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
