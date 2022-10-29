// RUN: clang-tidy %s -checks='-*,clang-analyzer-core.*' -- | FileCheck %s

unsigned long foo() {
  int * ptr = nullptr;
  return 5 / (__builtin_bit_cast(unsigned long, ptr) % 10);
  // CHECK: [[@LINE-1]]:12: warning: Division by zero [clang-analyzer-core.DivideZero]
}
