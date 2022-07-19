// RUN: %check_clang_tidy -std c++20 %s daedalean-auto %t

template <typename T>
concept Iterator = requires(T t, T t2) {
  {*t};
  {++t};
  {t == t2};
  {t != t2};
};

template <class T>
struct vector {
  struct iter {
    T &operator*();
    iter &operator++();
    bool operator==(const iter &) const;
    bool operator!=(const iter &) const;
  };

  iter begin();
  iter end();
};

template <class T>
constexpr auto dependsOnTemplateParam(T &val) {
  auto &[a, b, c] = val;
  return a;
}

template <class T>
constexpr auto doesNotDependOnTemplateParam(T) {
  // CHECK-MESSAGES: :[[@LINE-1]]:16: warning: Do not use auto as return type [daedalean-auto]
  int a = 1;
  return a;
}

template <typename F>
void templateFuncContainsLambda() {
  static const auto var = [](void (*parm)()) -> void { parm(); };
}

void function() {
  vector<int> Vector{};
  vector<int *> Vector2{};

  struct {
    char c;
    char k;
    char j;
  } struct_value;
  dependsOnTemplateParam(struct_value);
  doesNotDependOnTemplateParam<char>('c');

  auto i = 123u;
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: Do not declare auto variables [daedalean-auto]

  // auto MAY be used for iterator type in for loops.
  for (Iterator auto iter = Vector.begin(); iter != Vector.end(); ++iter) {
  }

  for (auto iter = Vector.begin(); iter != Vector.end(); ++iter) {
    // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: Do not declare auto variables [daedalean-auto]
  }

  for (auto i = 0; i < 10; i++) {
    // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: Do not declare auto variables [daedalean-auto]
  }

  for (auto val : Vector) {
    // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: Do not declare auto variables [daedalean-auto]
  }

  for (const auto &val : Vector) {
    // CHECK-MESSAGES: :[[@LINE-1]]:20: warning: Do not declare auto variables [daedalean-auto]
  }

  for (const auto *val : Vector2) {
    // CHECK-MESSAGES: :[[@LINE-1]]:20: warning: Do not declare auto variables [daedalean-auto]
  }

  // auto MAY be used for local variables holding lambda functions.
  auto lambda = []() -> void {};

  // auto MAY be used as return of lambda functions with auto arguments.
  auto anotherLambda = [lambda](auto x) -> auto{
    return x * 2;
  };

  auto yetAnotherLambda = [lambda](auto x) -> bool {
    return x != 0;
  };

  auto badLambda = [](int x) -> auto{
    // CHECK-MESSAGES: :[[@LINE-1]]:20: warning: Lambda with non-auto arguments MUST not use auto as return type [daedalean-auto]
    return x * 2;
  };

  auto anotherBadLambda = [](int x) {
    // CHECK-MESSAGES: :[[@LINE-1]]:27: warning: Lambda with non-auto arguments MUST not use auto as return type [daedalean-auto]
    return x * 2;
  };

  // auto MAY be used for structured bindings.
  int array[2]{1, 2};
  auto [a, b] = array;
}

auto calculate(int i) {
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: Do not use auto as return type [daedalean-auto]
  return i * 12;
}

// auto MAY be used in template functions if type depends on template arguments.
template <typename T>
auto getValue(T &t) {
  return t.value;
}

template <>
auto getValue<vector<int>>(vector<int> &);

template <typename T>
void func(T arg2) {
}

auto trailingReturnType(char arg) -> int;

template <typename T>
concept Eq = requires(T t1, T t2) {
  {t1 == t2};
};

void testFunc(Eq auto val) {
  [value = val](decltype(val) val) -> bool {
    return value == val;
  }(val);
}

template <typename T>
void templatedFunc(T val) {
  [value = val](decltype(val) val) -> auto{
    // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: Lambda with non-auto arguments MUST not use auto as return type [daedalean-auto]
    auto variable = val;
    // CHECK-MESSAGES: :[[@LINE-1]]:10: warning: Do not declare auto variables [daedalean-auto]
    return value == variable;
  }
  (val);

  [val](decltype(val) value) -> bool {
    auto variable = value;
    // CHECK-MESSAGES: :[[@LINE-1]]:10: warning: Do not declare auto variables [daedalean-auto]
    return val == variable;
  }(val);
}
