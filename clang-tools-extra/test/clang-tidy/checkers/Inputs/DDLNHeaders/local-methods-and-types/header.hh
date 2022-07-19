#pragma once

namespace local::methods::plus::types {
void thisIsAPublicFunction();

struct GoodS {
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

class GoodC;

extern int GoodGlobalVar;

template <typename T>
class TemplatedClass;

} // namespace local::methods::plus::types
