// RUN: %check_clang_tidy %s daedalean-switch-statement %t

void bar();

void foo(int a) {

  switch (a) {
  case 1:
    // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: Case statement must be scoped statement [daedalean-switch-statement]
  case 2:
    // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: Case statement must be scoped statement [daedalean-switch-statement]
    bar();
    break;
  case 3: {
    bar();
  }
  default:
    // CHECK-MESSAGES: :[[@LINE-1]]:10: warning: Case statement must be scoped statement [daedalean-switch-statement]
    break;
  }
}