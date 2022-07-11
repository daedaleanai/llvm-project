// RUN: %check_clang_tidy %s daedalean-structs-and-classes %t

struct S1 {
  int a;
};

struct S2 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Type 'S2' with a user-provided copy constructor must be declared as a class [daedalean-structs-and-classes]
  // CHECK-FIXES: class
  S2(const S2 &) = default;
  int b;
};

struct S3 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Type 'S3' with a user-provided move constructor must be declared as a class [daedalean-structs-and-classes]
  // CHECK-FIXES: class
  S3(S3 &&) = default;
  int b;
};

struct S4 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Type 'S4' with a user-provided move copy assignment operator must be declared as a class [daedalean-structs-and-classes]
  // CHECK-FIXES: class
  S4 &operator=(const S4 &) = default;
  int b;
};

struct S5 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Type 'S5' with a user-provided move copy assignment operator must be declared as a class [daedalean-structs-and-classes]
  // CHECK-FIXES: class
  S5 &operator=(S5 &&) = default;
  int b;
};

struct S6 {
  bool isFive() const { return b == 5; }
  int b;
};

struct S7 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Type 'S7' with non-const methods must be declared as a class [daedalean-structs-and-classes]
  // CHECK-FIXES: class
  void set(int c) { b = c; }
  int b;
};

struct S8 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Type 'S8' with a user-provided destructor must be declared as a class [daedalean-structs-and-classes]
  // CHECK-FIXES: class
  ~S8() {}
  int b;
};

struct S9 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Non-Aggregate type 'S9' must be declared as a class [daedalean-structs-and-classes]
  S9(int val) : b(val) {}
  int b;
};

class C1 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Type 'C1' must be declared as a struct [daedalean-structs-and-classes]
public:
  const int a;
};

class C2 {
public:
  C2(const C2 &) = default;

private:
  int b;
};

class C3 {
public:
  C3(C3 &&) = default;

  int a;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: All non-const data members of class MUST be private [daedalean-structs-and-classes]
  // CHECK-FIXES: private:
};

class C4 {
public:
  C4 &operator=(const C4 &) = default;

private:
  int a;
};

class C5 {
public:
  C5 &operator=(C5 &&) = default;

private:
  int a;
};
