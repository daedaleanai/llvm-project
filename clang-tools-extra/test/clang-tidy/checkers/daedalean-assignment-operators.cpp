// RUN: %check_clang_tidy %s daedalean-assignment-operators %t

#define CLASS(name) class name final
#define CLASS_NOLINT(name) class name final /* NOLINT(daedalean-assignment-operators) */

CLASS(MyClass){
    // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Non-abstract class 'MyClass' must implement copy-assignment operator [daedalean-assignment-operators]
    // CHECK-MESSAGES: :[[@LINE-2]]:1: warning: Non-abstract class 'MyClass' must implement move-assignment operator [daedalean-assignment-operators]
};

CLASS_NOLINT(MyClassWithoutLints){};

class C1 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Non-abstract class 'C1' must implement copy-assignment operator [daedalean-assignment-operators]
  // CHECK-MESSAGES: :[[@LINE-2]]:1: warning: Non-abstract class 'C1' must implement move-assignment operator [daedalean-assignment-operators]
public:
};

class C2 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Non-abstract class 'C2' must implement move-assignment operator [daedalean-assignment-operators]
public:
  C2 &operator=(const C2 &);
};

class C3 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Non-abstract class 'C3' must implement copy-assignment operator [daedalean-assignment-operators]
public:
  C3 &operator=(C3 &&);
};

class C4 {
  // CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Non-abstract class 'C4' must implement move-assignment operator [daedalean-assignment-operators]
public:
  C4 &operator=(C4 &);
};

class C5 {
public:
  C5 &operator=(const C5 &);
  C5 &operator=(C5 &&);
};

class C6 {
public:
  C6 &operator=(const C6 &) = default;
  C6 &operator=(C6 &&) = default;
};

class C7 {
public:
  C7 &operator=(const C7 &) = delete;
  C7 &operator=(C7 &&) = delete;
};

struct S {
};

class ForwardDeclaredClass;

struct ForwardDeclaredStruct;
