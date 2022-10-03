//===--- TypeConversionsCheck.cpp - clang-tidy --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TypeConversionsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace daedalean {

namespace {

QualType getPointee(QualType type) {
  if (llvm::isa<PointerType>(type)) {
    return llvm::cast<PointerType>(type)->getPointeeType();
  } else if (llvm::isa<ReferenceType>(type)) {
    return llvm::cast<ReferenceType>(type)->getPointeeType();
  } else if (llvm::isa<BlockPointerType>(type)) {
    return llvm::cast<BlockPointerType>(type)->getPointeeType();
  }
  assert(false && "Not a valid pointer type!");
  return type;
}

bool hasPointerRepresentation(QualType type) {
  return llvm::isa<PointerType>(type) || llvm::isa<ReferenceType>(type) ||
         llvm::isa<BlockPointerType>(type);
}

bool onlyAddsQualifiers(QualType sourceType, QualType destType) {
  if ((sourceType.getQualifiers() != destType.getQualifiers()) &&
      !destType.isMoreQualifiedThan(sourceType)) {
    // Not more qualified or same
    return false;
  }

  if (hasPointerRepresentation(sourceType) &&
      hasPointerRepresentation(destType)) {
    // Check if pointed types are same or more qualifiers
    const QualType pointedSourceType =
        getPointee(sourceType).getCanonicalType();
    const QualType pointedDestType = getPointee(destType).getCanonicalType();

    return onlyAddsQualifiers(pointedSourceType, pointedDestType);
  } else if (sourceType.getUnqualifiedType().getCanonicalType() !=
             destType.getUnqualifiedType().getCanonicalType()) {
    // Unqualified types are not the same, so it does not just add qualifiers
    return false;
  }

  return true;
}
} // namespace

const char DynamicCastId[] = "dynamic-cast";
const char ReinterpretCastId[] = "reinterpret-cast";
const char ConstCastId[] = "const-cast";
const char CStyleCastId[] = "c-style-cast";
const char ImplicitCastId[] = "implicit-cast";
const char StaticCastId[] = "static-cast";

void TypeConversionsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(traverse(TK_IgnoreUnlessSpelledInSource,
                              cxxDynamicCastExpr().bind(DynamicCastId)),
                     this);
  Finder->addMatcher(traverse(TK_IgnoreUnlessSpelledInSource,
                              cxxReinterpretCastExpr().bind(ReinterpretCastId)),
                     this);
  Finder->addMatcher(traverse(TK_IgnoreUnlessSpelledInSource,
                              cxxConstCastExpr().bind(ConstCastId)),
                     this);
  Finder->addMatcher(traverse(TK_IgnoreUnlessSpelledInSource,
                              cStyleCastExpr().bind(CStyleCastId)),
                     this);
  Finder->addMatcher(implicitCastExpr().bind(ImplicitCastId), this);
  Finder->addMatcher(cxxStaticCastExpr().bind(StaticCastId), this);
}

void TypeConversionsCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *DynamicCastDecl =
      Result.Nodes.getNodeAs<CXXDynamicCastExpr>(DynamicCastId);
  if (DynamicCastDecl != nullptr) {
    diag(DynamicCastDecl->getOperatorLoc(), "dynamic_cast MUST not be used");
  }

  const auto *ReinterpretCastDecl =
      Result.Nodes.getNodeAs<CXXReinterpretCastExpr>(ReinterpretCastId);
  if (ReinterpretCastDecl != nullptr) {
    diag(ReinterpretCastDecl->getOperatorLoc(),
         "reinterpret_cast MUST not be used");
  }

  const auto *ConstCastDecl =
      Result.Nodes.getNodeAs<CXXConstCastExpr>(ConstCastId);
  if (ConstCastDecl != nullptr) {
    diag(ConstCastDecl->getOperatorLoc(), "const_cast MUST not be used");
  }

  const auto *CStyleCastDecl =
      Result.Nodes.getNodeAs<CStyleCastExpr>(CStyleCastId);
  if (CStyleCastDecl != nullptr) {
    diag(CStyleCastDecl->getLParenLoc(), "C-style cast MUST not be used");
  }

  const auto *ImplicitCastDecl =
      Result.Nodes.getNodeAs<ImplicitCastExpr>(ImplicitCastId);
  if (ImplicitCastDecl != nullptr) {
    const QualType destType = ImplicitCastDecl->getType();
    const QualType sourceType = ImplicitCastDecl->getSubExpr()->getType();
    if (ImplicitCastDecl->isPartOfExplicitCast()) {
      handleStaticCast(Result.Context, sourceType, destType,
                       ImplicitCastDecl->getBeginLoc());
    } else {
      handleImplicitCast(Result.Context, sourceType, destType,
                         ImplicitCastDecl->getBeginLoc());
    }
  }

  const auto *StaticCastDecl =
      Result.Nodes.getNodeAs<CXXStaticCastExpr>(StaticCastId);
  if (StaticCastDecl != nullptr) {
    const QualType destType = StaticCastDecl->getType();
    const QualType sourceType = StaticCastDecl->getSubExpr()->getType();
    handleStaticCast(Result.Context, sourceType, destType,
                     StaticCastDecl->getOperatorLoc());
  }
}

bool TypeConversionsCheck::isSafeIntegralCast(clang::ASTContext *context,
                                              QualType sourceType,
                                              QualType destType) const {
  if (sourceType->isIntegerType() && destType->isIntegerType()) {
    // Check that the size of the destination integer is larger than source

    const unsigned sourceBits = sourceType->isBitIntType()
                                    ? context->getIntWidth(sourceType)
                                    : context->getTypeSize(sourceType);
    const unsigned destBits = destType->isBitIntType()
                                  ? context->getIntWidth(destType)
                                  : context->getTypeSize(destType);

    // unsigned to signed conversion is fine if number of bits is strictly
    // larger than the source, otherwise it may not fit
    if ((destType->isSignedIntegerType() &&
         sourceType->isUnsignedIntegerType()) &&
        (destBits > sourceBits)) {
      return true;
    }

    // If they are the same sign, they need to have at least the same number of
    // bits
    if ((destType->isSignedIntegerType() ==
         sourceType->isSignedIntegerType()) &&
        (destBits >= sourceBits)) {
      // Explicit promotion of integer types is fine
      return true;
    }
  }

  return false;
}

void TypeConversionsCheck::handleStaticCast(clang::ASTContext *context,
                                            QualType sourceType,
                                            QualType destType,
                                            SourceLocation location) {
  if (destType.getCanonicalType() == sourceType.getCanonicalType()) {
    // If source and destination are the same type we can ignore the cast
    return;
  }

  if (destType->isVoidType()) {
    // Casting to void is always allowed
    return;
  }

  if (isSafeIntegralCast(context, sourceType, destType)) {
    // Integral promotion is allowed
    return;
  }

  if (sourceType->isRealFloatingType() && destType->isIntegerType()) {
    diag(location, "Floating point to integral conversion must use 'floor<T>', "
                   "'ceil<T>' or 'round<T>'");
    diag(location, "Conversion from %1 to %0", DiagnosticIDs::Note)
        << destType << sourceType;
    return;
  }

  if (sourceType->isArithmeticType() && destType->isArithmeticType()) {
    diag(location, "Conversion between numeric types must use 'clipToFit<T>'");
    diag(location, "Conversion from %1 to %0", DiagnosticIDs::Note)
        << destType << sourceType;
    return;
  }
}

void TypeConversionsCheck::handleImplicitCast(clang::ASTContext *context,
                                              QualType sourceType,
                                              QualType destType,
                                              SourceLocation location) {
  const QualType canonicalSourceType = sourceType.getCanonicalType();
  const QualType canonicalDestType = destType.getCanonicalType();
  if (canonicalDestType == canonicalSourceType) {
    // If source and destination are the same type we can ignore the cast
    return;
  }

  if (onlyAddsQualifiers(canonicalSourceType.getUnqualifiedType(),
                         canonicalDestType.getUnqualifiedType())) {
    return;
  }

  if (canonicalSourceType->isNullPtrType() &&
      (canonicalDestType->isPointerType() || canonicalDestType->isMemberPointerType())) {
    // Casting from nullptr to a pointer type is always allowed
    return;
  }

  if (canonicalSourceType->isPointerType() && canonicalDestType->isVoidPointerType()) {
    // Casting from pointer to void * is allowed
    return;
  }

  if (llvm::isa<BuiltinType>(canonicalSourceType)) {
    const BuiltinType *Builtin = llvm::dyn_cast<BuiltinType>(canonicalSourceType);
    if ((Builtin->getKind() == BuiltinType::Kind::BuiltinFn) &&
        canonicalDestType->isFunctionPointerType()) {
      return;
    }
  }

  if (canonicalSourceType->isDependentType() || canonicalDestType->isDependentType()) {
    return;
  }

  if (!canonicalSourceType->hasPointerRepresentation() &&
      (canonicalSourceType.getUnqualifiedType().getCanonicalType() ==
       canonicalDestType.getUnqualifiedType().getCanonicalType())) {
    // Removing qualifiers by value (not reference or pointer types) is ok
    return;
  }

  if (canonicalSourceType->canDecayToPointerType()) {
    const QualType decayedType = context->getDecayedType(canonicalSourceType);
    if (decayedType.getCanonicalType() == canonicalDestType.getCanonicalType()) {
      return;
    }
  }

  if (isSafeIntegralCast(context, canonicalSourceType, canonicalDestType)) {
    // Integral promotion is allowed
    return;
  }

  // Check conversion from child to parent
  const CXXRecordDecl *destRecord = canonicalDestType->getAsCXXRecordDecl();
  const CXXRecordDecl *sourceRecord = canonicalSourceType->getAsCXXRecordDecl();
  if (sourceRecord && destRecord && sourceRecord->isDerivedFrom(destRecord)) {
    // Cast pointer or reference from child to base MAY be implicit.
    return;
  }

  if (hasPointerRepresentation(canonicalSourceType) &&
      hasPointerRepresentation(canonicalDestType)) {
    const CXXRecordDecl *sourceRecord =
        getPointee(canonicalSourceType)->getAsCXXRecordDecl();
    const CXXRecordDecl *destRecord =
        getPointee(canonicalDestType)->getAsCXXRecordDecl();
    if (sourceRecord && destRecord && sourceRecord->isDerivedFrom(destRecord)) {
      // Cast pointer or reference from child to base MAY be implicit.
      return;
    }
  }

  diag(location, "Type conversions MUST be explicit");
  diag(location, "Implicit conversion from %1 to %0", DiagnosticIDs::Note)
      << destType << sourceType;
}

} // namespace daedalean
} // namespace tidy
} // namespace clang
