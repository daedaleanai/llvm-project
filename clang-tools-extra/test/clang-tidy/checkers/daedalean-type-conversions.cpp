// RUN: %check_clang_tidy %s daedalean-type-conversions %t -- -- -frtti

class A {
  virtual void doStuff() = 0;
};

class B : public A {
  void doStuff() final {}
};

void f() {
  const int i = 0;
  B obj{};

  reinterpret_cast<int *>(0xf);
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: reinterpret_cast MUST not be used [daedalean-type-conversions]

  const_cast<int *>(&i);
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: const_cast MUST not be used [daedalean-type-conversions]

  dynamic_cast<B *>(static_cast<A *>(&obj));
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: dynamic_cast MUST not be used [daedalean-type-conversions]

  (A *)(&obj);
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: C-style cast MUST not be used [daedalean-type-conversions]
}
