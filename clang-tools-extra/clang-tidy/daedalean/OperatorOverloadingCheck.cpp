//===--- OperatorOverloadingCheck.cpp - clang-tidy ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "OperatorOverloadingCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

namespace  {
AST_MATCHER(FunctionDecl, isOverloadedOperator) {
  if (const auto *CXXMethodNode = dyn_cast<CXXMethodDecl>(&Node)) {
    if (CXXMethodNode->isImplicit())
      return false;
  }
  return Node.isOverloadedOperator();
}

bool hasOperator(const DeclContext * ctx, OverloadedOperatorKind kind, const QualType & type, const QualType & arg) {
  for (const auto & d: ctx->decls()) {
    if (const auto func = llvm::dyn_cast<FunctionDecl>(d); func) {
      if (!func->isOverloadedOperator() || func->getOverloadedOperator() != kind) {
        continue;
      }


      if (func->getParamDecl(0)->getType().getNonReferenceType().getUnqualifiedType() != type.getNonReferenceType().getUnqualifiedType()) {
        continue;
      }

      if (func->getParamDecl(1)->getType() != arg) {
        continue;
      }

      return true;
    }
  }

  return false;
}

bool hasOperator(const CXXRecordDecl * cls, OverloadedOperatorKind kind, const QualType & arg, bool onlyConst = false) {
  for (const auto & m: cls->methods()) {
    if (!m->isOverloadedOperator() ||
        m->getOverloadedOperator() != kind) {
      continue;
    }

    if (onlyConst && !m->isConst()) {
      continue;
    }

    if (m->getParamDecl(0)->getType() != arg) {
      continue;
    }

    return true;
  }

  return hasOperator(cls->getParent(), kind, cls->getTypeForDecl()->getCanonicalTypeUnqualified(), arg);
}

bool hasOperator(const FunctionDecl * function, OverloadedOperatorKind kind) {
  if (auto method = llvm::dyn_cast<CXXMethodDecl>(function); method) {
    return hasOperator(method->getParent(), kind, function->getParamDecl(0)->getType());
  } else {
    const auto firstType = function->getParamDecl(0)->getType();
    if (firstType.getNonReferenceType()->isRecordType()) {
      return hasOperator(firstType.getNonReferenceType()->getAsCXXRecordDecl(), kind, function->getParamDecl(1)->getType());
    } else {
      return hasOperator(function->getParent(), kind, firstType.getNonReferenceType(), function->getParamDecl(1)->getType());
    }
  }
}

}

void OperatorOverloadingCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(functionDecl(isOverloadedOperator()).bind("x"), this);
}

void OperatorOverloadingCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *function = Result.Nodes.getNodeAs<FunctionDecl>("x");
  const auto *method = llvm::dyn_cast<CXXMethodDecl>(function);

  switch (function->getOverloadedOperator()) {
  case OO_Equal:
    [[fallthrough]];
  case OO_PlusEqual:
    [[fallthrough]];
  case OO_MinusEqual:
    [[fallthrough]];
  case OO_PipeEqual:
    [[fallthrough]];
  case OO_AmpEqual:
    [[fallthrough]];
  case OO_CaretEqual:
    [[fallthrough]];
  case OO_LessLessEqual:
    [[fallthrough]];
  case OO_GreaterGreaterEqual:
    [[fallthrough]];
  case OO_SlashEqual:
    [[fallthrough]];
  case OO_StarEqual:
    [[fallthrough]];
  case OO_PercentEqual: {
    checkReturnSelf(method->getDefinition()->getBody());
    break;
  }
  case OO_Amp:
      if (function->getNumParams() != 0) { // Skip binary form
        break;
      }
      [[fallthrough]];
  case OO_AmpAmp:
    [[fallthrough]];
  case OO_PipePipe:
    diag(function->getBeginLoc(), "overloading %0 is disallowed") << function;
    break;
  case OO_Subscript:
    if (!method->isConst()) {
      if (hasOperator(method->getParent(), OO_Subscript,
                      function->getParamDecl(0)->getType(), true)) {
        break;
      }

      diag(function->getBeginLoc(), "If operator[] overloaded with non-const version, const version MUST be present.") << function;
    }
    break;
  case OO_EqualEqual:
    checkOperatorPair(function, OO_ExclaimEqual, "If == overloaded, != MUST be overloaded as well");
    if (hasOperator(function, OO_Less) && !hasOperator(function, OO_LessEqual)) {
      diag(function->getBeginLoc(), "If == and < overloaded, <= MUST be overloaded as well") << function;
    }
    break;
  case OO_ExclaimEqual:
     checkOperatorPair(function, OO_EqualEqual, "If != overloaded, == MUST be overloaded as well");
    break;
  case OO_Less:
    checkOperatorPair(function, OO_Greater, "If < overloaded, > MUST be overloaded as well");
    break;
  case OO_Greater:
    checkOperatorPair(function, OO_Less, "If > overloaded, < MUST be overloaded as well");
    break;
  case OO_LessEqual:
    checkOperatorPair(function, OO_GreaterEqual, "If <= overloaded, >= MUST be overloaded as well");
    checkOperatorPair(function, OO_EqualEqual, "If <= overloaded, == MUST be overloaded as well");
    break;
  case OO_GreaterEqual:
    checkOperatorPair(function, OO_LessEqual, "If >= overloaded, <= MUST be overloaded as well");
    checkOperatorPair(function, OO_EqualEqual, "If >= overloaded, == MUST be overloaded as well");
    break;
  default:
    break;
  };
}

void OperatorOverloadingCheck::checkOperatorPair(const FunctionDecl * function, OverloadedOperatorKind kind, const std::string & message) {
  if (hasOperator(function, kind)) {
    return;
  }

  diag(function->getBeginLoc(), message) << function;
}
void OperatorOverloadingCheck::checkReturnSelf(const Stmt *stmt) {
  if (!stmt) {
    return;
  }

  if (const auto ptr = llvm::dyn_cast<ForStmt>(stmt); ptr) {
    checkReturnSelf(ptr->getBody());
  }
  if (const auto ptr = llvm::dyn_cast<WhileStmt>(stmt); ptr) {
    checkReturnSelf(ptr->getBody());
  }
  if (const auto ptr = llvm::dyn_cast<DoStmt>(stmt); ptr) {
    checkReturnSelf(ptr->getBody());
  }
  if (const auto ptr = llvm::dyn_cast<IfStmt>(stmt); ptr) {
    checkReturnSelf(ptr->getThen());
    checkReturnSelf(ptr->getElse());
  }
  if (const auto ptr = llvm::dyn_cast<CompoundStmt>(stmt); ptr) {
    for (const auto s : ptr->body()) {
      checkReturnSelf(s);
    }
  }

  if (const auto ptr = llvm::dyn_cast<ReturnStmt>(stmt); ptr) {
    const auto expr = ptr->getRetValue();
    if (const auto uOp = llvm::dyn_cast<UnaryOperator>(expr); uOp) {
      if (uOp->getOpcode() != UO_Deref || !llvm::isa<CXXThisExpr>(uOp->getSubExpr())) {
        diag(ptr->getBeginLoc(), "Assignment operator must return reference to this");
      }
    } else {
      diag(ptr->getBeginLoc(), "Assignment operator must return reference to this");
    }
    // If there is no return at all compiler will generate warning/error
  }

}

} // namespace daedalean
} // namespace tidy
} // namespace clang
