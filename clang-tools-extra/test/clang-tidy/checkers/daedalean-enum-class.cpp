// RUN: %check_clang_tidy %s daedalean-enum-class %t


enum E {
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: enum class MUST be used [daedalean-enum-class]
  // CHECK-FIXES: class
  E1
};

enum class EC {
    EC1
};