// RUN: %check_clang_tidy %s daedalean-structs-and-classes %t

struct S1 {
  int a;
};

struct S2 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Non-POD type must be declared as class [daedalean-structs-and-classes]
  // CHECK-FIXES: class
  S2(int a) : b(a) {}
  int b;
};

class C1 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: POD type must be declared as struct [daedalean-structs-and-classes]
  // CHECK-FIXES: struct
public:
  const int a;
};

class C2 {
public:
  C2(int a) : b(a) {}
private:
  int b;
};

class C3 {
public:
  C3(int) {}

  int a;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: All non-const data members of class MUST be private [daedalean-structs-and-classes]
  // CHECK-FIXES: private:
};
