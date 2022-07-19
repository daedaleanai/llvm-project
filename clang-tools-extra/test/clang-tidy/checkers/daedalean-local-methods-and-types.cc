// RUN: %check_clang_tidy %s daedalean-local-methods-and-types %t -- -- -isystem %S/Inputs/DDLNHeaders

#include <local-methods-and-types/header.hh>

void f() {
  // CHECK-MESSAGES: [[@LINE-1]]:6: warning: Function 'f' should be in an anonymous namespace [daedalean-local-methods-and-types]
}

int GlobalVar;
// CHECK-MESSAGES: [[@LINE-1]]:5: warning: Variable 'GlobalVar' should be in an anonymous namespace [daedalean-local-methods-and-types]

struct S {
  // CHECK-MESSAGES: [[@LINE-1]]:8: warning: Record 'S' should be in an anonymous namespace [daedalean-local-methods-and-types]
  int Var;
  const char *Var2;

  void function();

  struct InnerS {
    int Var;
    const char *Var2;
    void function();
  };

  class InnerC {
    int Var;
    const char *Var2;
    void function();
  };
};

class C {
  // CHECK-MESSAGES: [[@LINE-1]]:7: warning: Record 'C' should be in an anonymous namespace [daedalean-local-methods-and-types]
  int Var;
  const char *Var2;

  void function();

  struct InnerS {
    int Var;
    const char *Var2;
    void function();
  };

  class InnerC {
    int Var;
    const char *Var2;
    void function();
  };
};

namespace {
int AlsoGlobalVar;
void awesomef2() {}
namespace does::it::really::work {
void awesomef3() {}
} // namespace does::it::really::work
} // namespace

namespace local::methods::plus::types {

void thisIsAPublicFunction() {}

void thisIsReallyAPrivateFunction() {
  // CHECK-MESSAGES: [[@LINE-1]]:6: warning: Function 'thisIsReallyAPrivateFunction' should be in an anonymous namespace [daedalean-local-methods-and-types]
}

struct AnotherS {
  // CHECK-MESSAGES: [[@LINE-1]]:8: warning: Record 'AnotherS' should be in an anonymous namespace [daedalean-local-methods-and-types]
  int Var;
  const char *Var2;

  void function();

  struct InnerS {
    int Var;
    const char *Var2;
    void function();
  };

  class InnerC {
    int Var;
    const char *Var2;
    void function();
  };
};

class AnotherC {
  // CHECK-MESSAGES: [[@LINE-1]]:7: warning: Record 'AnotherC' should be in an anonymous namespace [daedalean-local-methods-and-types]
  int Var;
  const char *Var2;

  void function();

  struct InnerS {
    int Var;
    const char *Var2;
    void function();
  };

  class InnerC {
    int Var;
    const char *Var2;
    void function();
  };
};

class GoodC {
  int Var;
  const char *Var2;

  void function();

  struct InnerS {
    int Var;
    const char *Var2;
    void function();
  };

  class InnerC {
    int Var;
    const char *Var2;
    void function();
  };
};

int GoodGlobalVar;

int BadGlobalVar;
// CHECK-MESSAGES: [[@LINE-1]]:5: warning: Variable 'BadGlobalVar' should be in an anonymous namespace [daedalean-local-methods-and-types]

void GoodS::function() {
}

template <>
class TemplatedClass<int>;

} // namespace local::methods::plus::types

extern "C" {
void cFunction() {}
}
