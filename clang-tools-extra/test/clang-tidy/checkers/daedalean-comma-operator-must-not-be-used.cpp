// RUN: %check_clang_tidy %s daedalean-comma-operator-must-not-be-used %t

int a = (2, 3);
// CHECK-MESSAGES: :[[@LINE-1]]:11: warning: Comma operator must not be used [daedalean-comma-operator-must-not-be-used]

void foo() {
  a = 3, 4;
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: Comma operator must not be used [daedalean-comma-operator-must-not-be-used]
}

