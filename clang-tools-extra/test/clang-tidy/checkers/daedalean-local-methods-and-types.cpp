// RUN: %check_clang_tidy %s daedalean-local-methods-and-types %t -- -- -I`pwd`/../test/clang-tidy/checkers

#include "daedalean-local-methods-and-types.hh"

void foo1();

void foo2() {}

void bar1();
// CHECK-MESSAGES: :[[@LINE-1]]:6: warning: Local function must be declared in anonymous namespace [daedalean-local-methods-and-types]

void bar2() {}
// CHECK-MESSAGES: :[[@LINE-1]]:6: warning: Local function must be declared in anonymous namespace [daedalean-local-methods-and-types]

struct S2 {};
// CHECK-MESSAGES: :[[@LINE-1]]:8: warning: Local type must be declared in anonymous namespace [daedalean-local-methods-and-types]

namespace {

void baz();

void baz() {}

struct S1;

struct S1{
  void foo();
};

}
