// RUN: %check_clang_tidy %s daedalean-protected-must-not-be-used %t

class A {
protected:
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: protected modifier must not be used [daedalean-protected-must-not-be-used]
};
