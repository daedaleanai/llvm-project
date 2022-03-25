// RUN: %check_clang_tidy %s daedalean-class-methods %t

class A {
  void foo() {}
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: function 'foo' must be implemented outside class definition [daedalean-class-methods]
  void bar();
};

void A::bar() {}

template <typename>
class B {
  void foo() {}
};
