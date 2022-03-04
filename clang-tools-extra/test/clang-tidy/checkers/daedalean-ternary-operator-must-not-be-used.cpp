// RUN: %check_clang_tidy %s daedalean-ternary-operator-must-not-be-used %t

int a = 1 < 2 ? 1 : 2;
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: Ternary operator must not be used [daedalean-ternary-operator-must-not-be-used]
