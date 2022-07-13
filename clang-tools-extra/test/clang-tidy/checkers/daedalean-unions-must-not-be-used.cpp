// RUN: %check_clang_tidy %s daedalean-unions-must-not-be-used %t

union UnionsAreNotOk {
  // CHECK-MESSAGES: :[[@LINE-1]]:7: warning: Unions must not be used [daedalean-unions-must-not-be-used]
  // CHECK-MESSAGES: :[[@LINE-2]]:7: note: Remove union 'UnionsAreNotOk'
  int i;
  char *ptr;
};

struct Fine {
  int i;
  char *ptr;
};

class AlsoFine {
  int i;
  char *ptr;
};
