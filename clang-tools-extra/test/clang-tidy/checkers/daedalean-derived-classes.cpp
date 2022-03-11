// RUN: %check_clang_tidy %s daedalean-derived-classes %t

class I1 {
public:
  virtual ~I1() = default;
  virtual void foo() = 0;
};

class I2 {
// CHECK-MESSAGES: :[[@LINE-1]]:10: warning: Interface must have virtual defaulted destructor [daedalean-derived-classes]
public:
  virtual void foo() = 0;
};

class I3 {
public:
  virtual ~I3() { }
  // CHECK-MESSAGES: :[[@LINE-1]]:11: warning: Interface destructor must be defaulted [daedalean-derived-classes]
  // CHECK-FIXES: = default;
  virtual void foo() = 0;
};

class I4 {
public:
  ~I4() = default;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: Interface destructor must be virtual [daedalean-derived-classes]
  // CHECK-FIXES: virtual
  virtual void foo() = 0;
};


class I5 : public I1{
  // CHECK-MESSAGES: :[[@LINE-1]]:12: warning: Interface I1 must be implemented virtually because it's extended by multiple interfaces (I1, I1) with root type D8 [daedalean-derived-classes]
  // CHECK-FIXES: virtual
  virtual void bar() = 0;
};

class I6 : public I1 {
  // CHECK-MESSAGES: :[[@LINE-1]]:12: warning: Interface I1 must be implemented virtually because it's extended by multiple interfaces (I1, I1) with root type D8 [daedalean-derived-classes]
  // CHECK-FIXES: virtual
  virtual void baz() = 0;
};

class B1 {
  // CHECK-MESSAGES: :[[@LINE-1]]:7: warning: Non-interface class must be final [daedalean-derived-classes]
  int a;
};

class B2 {
  // CHECK-MESSAGES: :[[@LINE-1]]:7: warning: Non-interface class must be final [daedalean-derived-classes]
  void foo();
};

class D1 final : public B1 {
  // CHECK-MESSAGES: :[[@LINE-1]]:18: warning:  Inheritance from non-interface type is forbidden [daedalean-derived-classes]
};

class D2 final : public B2 {
  // CHECK-MESSAGES: :[[@LINE-1]]:18: warning:  Inheritance from non-interface type is forbidden [daedalean-derived-classes]
};

class D3 final : private I1 {
  // CHECK-MESSAGES: :[[@LINE-1]]:18: warning: Interfaces must be implemented using public inheritance [daedalean-derived-classes]
};

class D4 final : public I1 {
public:
  void foo() final {}
};

class D5 final : public I1 {
public:
  void foo() override {}
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: Implemented virtual methods must be final  [daedalean-derived-classes]
  // CHECK-FIXES: final
};

class D6 final : public I1 {
public:
  virtual void foo() {}
  // CHECK-MESSAGES: :[[@LINE-1]]:16: warning: Implemented virtual methods must be final  [daedalean-derived-classes]
  // CHECK-FIXES: final
};

class D7 final : public I1, public I2 {
public:
  virtual void foo() final {}
  // CHECK-MESSAGES: :[[@LINE-1]]:16: warning: Method foo is declared in I1, I2. Common interface must be introduced. [daedalean-derived-classes]
};

class D8 final : public I5, public I6 {
public:
  virtual void foo() final {}
  virtual void bar() final {}
  virtual void baz() final {}
};

class S1 {
  // CHECK-MESSAGES: :[[@LINE-1]]:7: warning: Non-interface class must be final [daedalean-derived-classes]
  virtual void foo() {}
};

class S2 {
  // CHECK-MESSAGES: :[[@LINE-1]]:7: warning: Non-interface class must be final [daedalean-derived-classes]
  int a;
  void foo() {}
};
