// RUN: %check_clang_tidy %s daedalean-floating-point-comparison %t

float f1 = 1.0f;
float f2 = 1.0f;
double d1 = 2.0f;
double d2 = 2.0f;
int i1 = 1;
int i2 = 1;

bool r1 = f1 == f2;
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Floating point expressions MUST NOT be checked for equality or inequality [daedalean-floating-point-comparison]
bool r2 = f1 != f2;
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Floating point expressions MUST NOT be checked for equality or inequality [daedalean-floating-point-comparison]
bool r3 = d1 == d2;
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Floating point expressions MUST NOT be checked for equality or inequality [daedalean-floating-point-comparison]
bool r4 = d1 != d2;
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Floating point expressions MUST NOT be checked for equality or inequality [daedalean-floating-point-comparison]
bool r5 = d1 == f2;
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Floating point expressions MUST NOT be checked for equality or inequality [daedalean-floating-point-comparison]
bool r6 = d1 != f2;
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Floating point expressions MUST NOT be checked for equality or inequality [daedalean-floating-point-comparison]
bool r7 = d1 == i2;
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Floating point expressions MUST NOT be checked for equality or inequality [daedalean-floating-point-comparison]
bool r8 = d1 != i2;
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Floating point expressions MUST NOT be checked for equality or inequality [daedalean-floating-point-comparison]
bool r9 = i1 == i2;
bool r10 = i1 != i2;
bool r11 = 1 == 2.0;
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Floating point expressions MUST NOT be checked for equality or inequality [daedalean-floating-point-comparison]
bool r12 = 1 != 2.0;
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Floating point expressions MUST NOT be checked for equality or inequality [daedalean-floating-point-comparison]


template<typename T>
bool foo(const T a, const T b) {
  return a == b;
  // CHECK-MESSAGES: :[[@LINE-1]]:12: warning: Floating point expressions MUST NOT be checked for equality or inequality [daedalean-floating-point-comparison]
}

bool r31 = foo(f1, f2);



