// RUN: %check_clang_tidy %s daedalean-type-conversions %t -- -- -frtti
//
using uint8_t = __UINT8_TYPE__;
using uint16_t = __UINT16_TYPE__;
using uint32_t = __UINT32_TYPE__;
using uint64_t = __UINT64_TYPE__;
using int8_t = __INT8_TYPE__;
using int16_t = __INT16_TYPE__;
using int32_t = __INT32_TYPE__;
using int64_t = __INT64_TYPE__;

template <typename T, typename S>
T clipToFit(S Source);

template <>
float clipToFit<float, double>(double Source);

class A {
public:
  A() = default;

  A(const A &) = delete;
  A(A &&) = delete;
  A &operator=(const A &) = delete;
  A &operator=(A &&) = delete;

  virtual ~A() = default;

  virtual void doStuff() {}
};

class B final : public A {
public:
  B() = default;

  B(const B &) = delete;
  B(B &&) = delete;
  B &operator=(const B &) = delete;
  B &operator=(B &&) = delete;

  ~B() final = default;

  void doStuff() final {}
};

void testReinterpretCast() {
  static_cast<void>(reinterpret_cast<int *>(0xf));
  // CHECK-MESSAGES: :[[@LINE-1]]:21: warning: reinterpret_cast MUST not be used [daedalean-type-conversions]
}

void testConstCast() {
  const int I = 0;
  static_cast<void>(const_cast<int *>(&I));
  // CHECK-MESSAGES: :[[@LINE-1]]:21: warning: const_cast MUST not be used [daedalean-type-conversions]
}

void testDynamicCast() {
  B Obj{};

  static_cast<void>(dynamic_cast<B *>(static_cast<A *>(&Obj)));
  // CHECK-MESSAGES: :[[@LINE-1]]:21: warning: dynamic_cast MUST not be used [daedalean-type-conversions]
}

void testCStyleCast() {
  B Obj{};

  static_cast<void>((A *)(&Obj));
  // CHECK-MESSAGES: :[[@LINE-1]]:21: warning: C-style cast MUST not be used [daedalean-type-conversions]
}

void testImplicitCastFromDerivedPtrToBase() {
  B Obj{};

  // Implicit B to A is fine
  A &NonConstA{Obj};
  static_cast<void>(NonConstA);
}

void testImplicitCastAddingQualifiers() {
  A NonConstA{};

  // Implicit A to const A is fine
  const A &ConstA{NonConstA};
  static_cast<void>(ConstA);

  // Implicit A to const volatile A is fine
  const volatile A &ConstVolatileA{NonConstA};
  static_cast<void>(ConstVolatileA);
}

void testImplicitIntegralPromotion() {
  uint8_t Uint8Val{};
  uint16_t Uint16Val{Uint8Val};
  uint32_t Uint32Val{Uint16Val};
  uint64_t Uint64Val{Uint32Val};

  // unsigned from unsigned
  Uint8Val = Uint8Val;
  Uint16Val = Uint8Val;
  Uint16Val = Uint16Val;
  Uint32Val = Uint8Val;
  Uint32Val = Uint16Val;
  Uint32Val = Uint32Val;
  Uint64Val = Uint8Val;
  Uint64Val = Uint16Val;
  Uint64Val = Uint32Val;
  Uint64Val = Uint64Val;

  int8_t Int8Val{};
  int16_t Int16Val{Int8Val};
  int32_t Int32Val{Int16Val};
  int64_t Int64Val{Int32Val};

  // Signed from signed
  Int8Val = Int8Val;
  Int16Val = Int8Val;
  Int16Val = Int16Val;
  Int32Val = Int8Val;
  Int32Val = Int16Val;
  Int32Val = Int32Val;
  Int64Val = Int8Val;
  Int64Val = Int16Val;
  Int64Val = Int32Val;
  Int64Val = Int64Val;

  // Signed from unsigned (destination strictly larger than source)
  Int16Val = Uint8Val;
  Int32Val = Uint8Val;
  Int32Val = Uint16Val;
  Int64Val = Uint8Val;
  Int64Val = Uint16Val;
  Int64Val = Uint32Val;
}

void testExplicitIntegralPromotion() {
  uint8_t Uint8Val{};
  uint16_t Uint16Val{};
  uint32_t Uint32Val{};
  uint64_t Uint64Val{};

  static_cast<uint8_t>(Uint8Val);
  static_cast<uint16_t>(Uint8Val);
  static_cast<uint16_t>(Uint16Val);
  static_cast<uint32_t>(Uint8Val);
  static_cast<uint32_t>(Uint16Val);
  static_cast<uint32_t>(Uint32Val);
  static_cast<uint64_t>(Uint8Val);
  static_cast<uint64_t>(Uint16Val);
  static_cast<uint64_t>(Uint32Val);
  static_cast<uint64_t>(Uint64Val);

  int8_t Int8Val{};
  int16_t Int16Val{};
  int32_t Int32Val{};
  int64_t Int64Val{};

  // Signed from signed
  static_cast<int8_t>(Int8Val);
  static_cast<int16_t>(Int8Val);
  static_cast<int16_t>(Int16Val);
  static_cast<int32_t>(Int8Val);
  static_cast<int32_t>(Int16Val);
  static_cast<int32_t>(Int32Val);
  static_cast<int64_t>(Int8Val);
  static_cast<int64_t>(Int16Val);
  static_cast<int64_t>(Int32Val);
  static_cast<int64_t>(Int64Val);

  // Signed from unsigned (destination strictly larger than source)
  static_cast<int16_t>(Uint8Val);
  static_cast<int32_t>(Uint8Val);
  static_cast<int32_t>(Uint16Val);
  static_cast<int64_t>(Uint8Val);
  static_cast<int64_t>(Uint16Val);
  static_cast<int64_t>(Uint32Val);
}

void testImplicitIntegralLossyConversions() {
  uint8_t Uint8Val{};
  uint16_t Uint16Val{};
  uint32_t Uint32Val{};
  uint64_t Uint64Val{};

  // Unsigned to unsigned

  Uint8Val = Uint16Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'uint16_t' (aka 'unsigned short') to 'uint8_t' (aka 'unsigned char')

  Uint8Val = Uint32Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'uint32_t' (aka 'unsigned int') to 'uint8_t' (aka 'unsigned char')

  Uint8Val = Uint64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'uint64_t' (aka 'unsigned long') to 'uint8_t' (aka 'unsigned char')

  Uint16Val = Uint32Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'uint32_t' (aka 'unsigned int') to 'uint16_t' (aka 'unsigned short')

  Uint16Val = Uint64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'uint64_t' (aka 'unsigned long') to 'uint16_t' (aka 'unsigned short')

  Uint32Val = Uint64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'uint64_t' (aka 'unsigned long') to 'uint32_t' (aka 'unsigned int')

  int8_t Int8Val{};
  int16_t Int16Val{};
  int32_t Int32Val{};
  int64_t Int64Val{};

  // Signed to signed
  Int8Val = Int16Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:13: note: Implicit conversion from 'int16_t' (aka 'short') to 'int8_t' (aka 'signed char')

  Int8Val = Int32Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:13: note: Implicit conversion from 'int32_t' (aka 'int') to 'int8_t' (aka 'signed char')

  Int8Val = Int64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:13: note: Implicit conversion from 'int64_t' (aka 'long') to 'int8_t' (aka 'signed char')

  Int16Val = Int32Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'int32_t' (aka 'int') to 'int16_t' (aka 'short')

  Int16Val = Int64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'int64_t' (aka 'long') to 'int16_t' (aka 'short')

  Int32Val = Int64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'int64_t' (aka 'long') to 'int32_t' (aka 'int')

  // Signed to unsigned
  Uint8Val = Int8Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'int8_t' (aka 'signed char') to 'uint8_t' (aka 'unsigned char')

  Uint8Val = Int16Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'int16_t' (aka 'short') to 'uint8_t' (aka 'unsigned char')

  Uint8Val = Int32Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'int32_t' (aka 'int') to 'uint8_t' (aka 'unsigned char')

  Uint8Val = Int64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'int64_t' (aka 'long') to 'uint8_t' (aka 'unsigned char')

  Uint16Val = Int8Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'int8_t' (aka 'signed char') to 'uint16_t' (aka 'unsigned short')

  Uint16Val = Int16Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'int16_t' (aka 'short') to 'uint16_t' (aka 'unsigned short')

  Uint16Val = Int32Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'int32_t' (aka 'int') to 'uint16_t' (aka 'unsigned short')

  Uint16Val = Int64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'int64_t' (aka 'long') to 'uint16_t' (aka 'unsigned short')

  Uint32Val = Int8Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'int8_t' (aka 'signed char') to 'uint32_t' (aka 'unsigned int')

  Uint32Val = Int16Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'int16_t' (aka 'short') to 'uint32_t' (aka 'unsigned int')

  Uint32Val = Int32Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'int32_t' (aka 'int') to 'uint32_t' (aka 'unsigned int')

  Uint32Val = Int64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'int64_t' (aka 'long') to 'uint32_t' (aka 'unsigned int')

  Uint64Val = Int8Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'int8_t' (aka 'signed char') to 'uint64_t' (aka 'unsigned long')

  Uint64Val = Int16Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'int16_t' (aka 'short') to 'uint64_t' (aka 'unsigned long')

  Uint64Val = Int32Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'int32_t' (aka 'int') to 'uint64_t' (aka 'unsigned long')

  Uint64Val = Int64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:15: note: Implicit conversion from 'int64_t' (aka 'long') to 'uint64_t' (aka 'unsigned long')

  // Unsigned to signed
  Int8Val = Uint8Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:13: note: Implicit conversion from 'uint8_t' (aka 'unsigned char') to 'int8_t' (aka 'signed char')

  Int8Val = Uint16Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:13: note: Implicit conversion from 'uint16_t' (aka 'unsigned short') to 'int8_t' (aka 'signed char')

  Int8Val = Uint32Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:13: note: Implicit conversion from 'uint32_t' (aka 'unsigned int') to 'int8_t' (aka 'signed char')

  Int8Val = Uint64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:13: note: Implicit conversion from 'uint64_t' (aka 'unsigned long') to 'int8_t' (aka 'signed char')

  Int16Val = Uint16Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'uint16_t' (aka 'unsigned short') to 'int16_t' (aka 'short')

  Int16Val = Uint32Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'uint32_t' (aka 'unsigned int') to 'int16_t' (aka 'short')

  Int16Val = Uint64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'uint64_t' (aka 'unsigned long') to 'int16_t' (aka 'short')

  Int32Val = Uint32Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'uint32_t' (aka 'unsigned int') to 'int32_t' (aka 'int')

  Int32Val = Uint64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'uint64_t' (aka 'unsigned long') to 'int32_t' (aka 'int')

  Int64Val = Uint64Val;
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:14: note: Implicit conversion from 'uint64_t' (aka 'unsigned long') to 'int64_t' (aka 'long')
}

void testExplicitIntegralLossyConversions() {
  uint8_t Uint8Val{};
  uint16_t Uint16Val{};
  uint32_t Uint32Val{};
  uint64_t Uint64Val{};

  // Unsigned to unsigned
  static_cast<uint8_t>(Uint16Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'uint16_t' (aka 'unsigned short') to 'uint8_t' (aka 'unsigned char')

  static_cast<uint8_t>(Uint32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'uint32_t' (aka 'unsigned int') to 'uint8_t' (aka 'unsigned char')

  static_cast<uint8_t>(Uint64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'uint64_t' (aka 'unsigned long') to 'uint8_t' (aka 'unsigned char')

  static_cast<uint16_t>(Uint32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'uint32_t' (aka 'unsigned int') to 'uint16_t' (aka 'unsigned short')

  static_cast<uint16_t>(Uint64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'uint64_t' (aka 'unsigned long') to 'uint16_t' (aka 'unsigned short')

  static_cast<uint32_t>(Uint64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'uint64_t' (aka 'unsigned long') to 'uint32_t' (aka 'unsigned int')

  int8_t Int8Val{};
  int16_t Int16Val{};
  int32_t Int32Val{};
  int64_t Int64Val{};

  // Signed to signed
  static_cast<int8_t>(Int16Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:23: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:23: note: Conversion from 'int16_t' (aka 'short') to 'int8_t' (aka 'signed char')

  static_cast<int8_t>(Int32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:23: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:23: note: Conversion from 'int32_t' (aka 'int') to 'int8_t' (aka 'signed char')

  static_cast<int8_t>(Int64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:23: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:23: note: Conversion from 'int64_t' (aka 'long') to 'int8_t' (aka 'signed char')

  static_cast<int16_t>(Int32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'int32_t' (aka 'int') to 'int16_t' (aka 'short')

  static_cast<int16_t>(Int64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'int64_t' (aka 'long') to 'int16_t' (aka 'short')

  static_cast<int32_t>(Int64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'int64_t' (aka 'long') to 'int32_t' (aka 'int')

  // Signed to unsigned
  static_cast<uint8_t>(Int8Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'int8_t' (aka 'signed char') to 'uint8_t' (aka 'unsigned char')

  static_cast<uint8_t>(Int16Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'int16_t' (aka 'short') to 'uint8_t' (aka 'unsigned char')

  static_cast<uint8_t>(Int32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'int32_t' (aka 'int') to 'uint8_t' (aka 'unsigned char')

  static_cast<uint8_t>(Int64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'int64_t' (aka 'long') to 'uint8_t' (aka 'unsigned char')

  static_cast<uint16_t>(Int8Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'int8_t' (aka 'signed char') to 'uint16_t' (aka 'unsigned short')

  static_cast<uint16_t>(Int16Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'int16_t' (aka 'short') to 'uint16_t' (aka 'unsigned short')

  static_cast<uint16_t>(Int32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'int32_t' (aka 'int') to 'uint16_t' (aka 'unsigned short')

  static_cast<uint16_t>(Int64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'int64_t' (aka 'long') to 'uint16_t' (aka 'unsigned short')

  static_cast<uint32_t>(Int8Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'int8_t' (aka 'signed char') to 'uint32_t' (aka 'unsigned int')

  static_cast<uint32_t>(Int16Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'int16_t' (aka 'short') to 'uint32_t' (aka 'unsigned int')

  static_cast<uint32_t>(Int32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'int32_t' (aka 'int') to 'uint32_t' (aka 'unsigned int')

  static_cast<uint32_t>(Int64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'int64_t' (aka 'long') to 'uint32_t' (aka 'unsigned int')

  static_cast<uint64_t>(Int8Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'int8_t' (aka 'signed char') to 'uint64_t' (aka 'unsigned long')

  static_cast<uint64_t>(Int16Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'int16_t' (aka 'short') to 'uint64_t' (aka 'unsigned long')

  static_cast<uint64_t>(Int32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'int32_t' (aka 'int') to 'uint64_t' (aka 'unsigned long')

  static_cast<uint64_t>(Int64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'int64_t' (aka 'long') to 'uint64_t' (aka 'unsigned long')

  // Unsigned to signed
  static_cast<int8_t>(Uint8Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:23: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:23: note: Conversion from 'uint8_t' (aka 'unsigned char') to 'int8_t' (aka 'signed char')

  static_cast<int8_t>(Uint16Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:23: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:23: note: Conversion from 'uint16_t' (aka 'unsigned short') to 'int8_t' (aka 'signed char')

  static_cast<int8_t>(Uint32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:23: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:23: note: Conversion from 'uint32_t' (aka 'unsigned int') to 'int8_t' (aka 'signed char')

  static_cast<int8_t>(Uint64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:23: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:23: note: Conversion from 'uint64_t' (aka 'unsigned long') to 'int8_t' (aka 'signed char')

  static_cast<int16_t>(Uint16Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'uint16_t' (aka 'unsigned short') to 'int16_t' (aka 'short')

  static_cast<int16_t>(Uint32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'uint32_t' (aka 'unsigned int') to 'int16_t' (aka 'short')

  static_cast<int16_t>(Uint64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'uint64_t' (aka 'unsigned long') to 'int16_t' (aka 'short')

  static_cast<int32_t>(Uint32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'uint32_t' (aka 'unsigned int') to 'int32_t' (aka 'int')

  static_cast<int32_t>(Uint64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'uint64_t' (aka 'unsigned long') to 'int32_t' (aka 'int')

  static_cast<int64_t>(Uint64Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'uint64_t' (aka 'unsigned long') to 'int64_t' (aka 'long')
}

void testClipToFitIntegers() {
  uint8_t Uint8Val{};
  uint16_t Uint16Val{};
  uint32_t Uint32Val{};
  uint64_t Uint64Val{};

  // Unsigned to unsigned
  clipToFit<uint8_t>(Uint16Val);
  clipToFit<uint8_t>(Uint32Val);
  clipToFit<uint8_t>(Uint64Val);
  clipToFit<uint16_t>(Uint32Val);
  clipToFit<uint16_t>(Uint64Val);
  clipToFit<uint32_t>(Uint64Val);

  int8_t Int8Val{};
  int16_t Int16Val{};
  int32_t Int32Val{};
  int64_t Int64Val{};

  // Signed to signed
  clipToFit<int8_t>(Int16Val);
  clipToFit<int8_t>(Int32Val);
  clipToFit<int8_t>(Int64Val);
  clipToFit<int16_t>(Int32Val);
  clipToFit<int16_t>(Int64Val);
  clipToFit<int32_t>(Int64Val);

  // Signed to unsigned
  clipToFit<uint8_t>(Int8Val);
  clipToFit<uint8_t>(Int16Val);
  clipToFit<uint8_t>(Int32Val);
  clipToFit<uint8_t>(Int64Val);
  clipToFit<uint16_t>(Int8Val);
  clipToFit<uint16_t>(Int16Val);
  clipToFit<uint16_t>(Int32Val);
  clipToFit<uint16_t>(Int64Val);
  clipToFit<uint32_t>(Int8Val);
  clipToFit<uint32_t>(Int16Val);
  clipToFit<uint32_t>(Int32Val);
  clipToFit<uint32_t>(Int64Val);
  clipToFit<uint64_t>(Int8Val);
  clipToFit<uint64_t>(Int16Val);
  clipToFit<uint64_t>(Int32Val);
  clipToFit<uint64_t>(Int64Val);

  // Unsigned to signed
  clipToFit<int8_t>(Uint8Val);
  clipToFit<int8_t>(Uint16Val);
  clipToFit<int8_t>(Uint32Val);
  clipToFit<int8_t>(Uint64Val);
  clipToFit<int16_t>(Uint16Val);
  clipToFit<int16_t>(Uint32Val);
  clipToFit<int16_t>(Uint64Val);
  clipToFit<int32_t>(Uint32Val);
  clipToFit<int32_t>(Uint64Val);
  clipToFit<int64_t>(Uint64Val);
}

void testFloatingPointImplicitConversion() {
  float f{};
  double d{};
  long double ld{};

  f = f;
  d = d;
  ld = ld;

  f = d;
  // CHECK-MESSAGES: :[[@LINE-1]]:7: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:7: note: Implicit conversion from 'double' to 'float'
  f = ld;
  // CHECK-MESSAGES: :[[@LINE-1]]:7: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:7: note: Implicit conversion from 'long double' to 'float'
  d = f;
  // CHECK-MESSAGES: :[[@LINE-1]]:7: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:7: note: Implicit conversion from 'float' to 'double'
  d = ld;
  // CHECK-MESSAGES: :[[@LINE-1]]:7: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:7: note: Implicit conversion from 'long double' to 'double'
  ld = f;
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:8: note: Implicit conversion from 'float' to 'long double'
  ld = d;
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: Type conversions MUST be explicit [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:8: note: Implicit conversion from 'double' to 'long double'
}

void testFloatingPointExplicitConversion() {
  float f{};
  double d{};
  long double ld{};

  static_cast<float>(f);
  static_cast<double>(d);
  static_cast<long double>(ld);

  static_cast<float>(d);
  // CHECK-MESSAGES: :[[@LINE-1]]:22: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:22: note: Conversion from 'double' to 'float'
  static_cast<float>(ld);
  // CHECK-MESSAGES: :[[@LINE-1]]:22: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:22: note: Conversion from 'long double' to 'float'
  static_cast<double>(f);
  // CHECK-MESSAGES: :[[@LINE-1]]:23: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:23: note: Conversion from 'float' to 'double'
  static_cast<double>(ld);
  // CHECK-MESSAGES: :[[@LINE-1]]:23: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:23: note: Conversion from 'long double' to 'double'
  static_cast<long double>(f);
  // CHECK-MESSAGES: :[[@LINE-1]]:28: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:28: note: Conversion from 'float' to 'long double'
  static_cast<long double>(d);
  // CHECK-MESSAGES: :[[@LINE-1]]:28: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:28: note: Conversion from 'double' to 'long double'
}

void testFloatingPointClipToFit() {
  float f{};
  double d{};
  long double ld{};

  clipToFit<float>(d);
  clipToFit<float>(ld);
  clipToFit<double>(ld);
  clipToFit<double>(f);
  clipToFit<long double>(f);
  clipToFit<long double>(d);
}

void testExplicitFloatingPointToIntegral() {
  float f{};
  double d{};
  long double ld{};

  static_cast<uint64_t>(f);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Floating point to integral conversion must use 'floor<T>', 'ceil<T>' or 'round<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'float' to 'uint64_t' (aka 'unsigned long')
  //
  static_cast<uint32_t>(f);
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: Floating point to integral conversion must use 'floor<T>', 'ceil<T>' or 'round<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:25: note: Conversion from 'float' to 'uint32_t' (aka 'unsigned int')

  static_cast<int32_t>(f);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Floating point to integral conversion must use 'floor<T>', 'ceil<T>' or 'round<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'float' to 'int32_t' (aka 'int')

  static_cast<int32_t>(d);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Floating point to integral conversion must use 'floor<T>', 'ceil<T>' or 'round<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'double' to 'int32_t' (aka 'int')

  static_cast<int32_t>(ld);
  // CHECK-MESSAGES: :[[@LINE-1]]:24: warning: Floating point to integral conversion must use 'floor<T>', 'ceil<T>' or 'round<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:24: note: Conversion from 'long double' to 'int32_t' (aka 'int')
}

void testExplicitIntegralToFloatingPoint() {
  uint32_t Uint32Val{};

  static_cast<float>(Uint32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:22: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:22: note: Conversion from 'uint32_t' (aka 'unsigned int') to 'float'

  static_cast<double>(Uint32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:23: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:23: note: Conversion from 'uint32_t' (aka 'unsigned int') to 'double'

  static_cast<long double>(Uint32Val);
  // CHECK-MESSAGES: :[[@LINE-1]]:28: warning: Conversion between numeric types must use 'clipToFit<T>' [daedalean-type-conversions]
  // CHECK-MESSAGES: :[[@LINE-2]]:28: note: Conversion from 'uint32_t' (aka 'unsigned int') to 'long double'
}

enum class TestEnum {
};

template <typename T>
struct HiddenType {
  using type = T;
};

template <typename T>
void takesHiddenType(typename HiddenType<T>::type);

template <typename T>
typename HiddenType<T>::type givesHiddenType();

void dependentConstTypes() {
  const typename HiddenType<const TestEnum>::type val = givesHiddenType<const TestEnum>();
  takesHiddenType<TestEnum>(val);
}

void pointerDecay() {
  void (*funcPtr)() = pointerDecay;

  char charArray[10];
  char *charPtr = charArray;
  const char *constCharPtr = charArray;
  const char *const constCharPtrConst = charArray;
  const volatile char *const volatile constVolatileCharPtrConstVolatile = charArray;
}
