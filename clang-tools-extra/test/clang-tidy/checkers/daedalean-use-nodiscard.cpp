// RUN: %check_clang_tidy -std c++20 %s daedalean-use-nodiscard %t

namespace std {
template <class>
class function;
class string {};
} // namespace std

namespace boost {
template <class>
class function;
}

typedef unsigned my_unsigned;
typedef unsigned &my_unsigned_reference;
typedef const unsigned &my_unsigned_const_reference;

struct [[nodiscard]] NoDiscardStruct {};

class Foo {
public:
  using size_type = unsigned;

  bool f1() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f1' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f1() const;

  bool f2(int) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f2' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f2(int) const;

  bool f3(const int &) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f3' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f3(const int &) const;

  bool f4(void) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f4' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f4(void) const;

  void f5() const;

  bool f6();
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f6' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f6();

  bool f7(int &);
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f7' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f7(int &);

  bool f8(int &) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f8' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f8(int &) const;

  bool f9(int *) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f9' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f9(int *) const;

  bool f10(const int &, int &) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f10' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f10(const int &, int &) const;

  [[nodiscard]] bool f12() const;

  __attribute__((warn_unused_result)) bool f13() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f13' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]

  [[nodiscard]] bool f11() const;

  [[clang::warn_unused_result]] bool f11a() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:33: warning: function 'f11a' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]

  [[gnu::warn_unused_result]] bool f11b() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:31: warning: function 'f11b' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]

  bool _f20() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function '_f20' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool _f20() const;

  [[noreturn]] bool f21() const;

  ~Foo();

  // Reference returned from prefix increment and decrement MAY be discarded
  Foo &operator++() const;

  // Reference returned from prefix increment and decrement MAY be discarded
  Foo &operator--() const;

  Foo operator--(int) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'operator--' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] Foo operator--(int) const;

  Foo &operator++(int) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'operator++' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] Foo &operator++(int) const;

  bool operator+=(int) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'operator+=' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool operator+=(int) const;

  // extra keywords (virtual,inline,const) on return type

  virtual bool f14() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f14' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] virtual bool f14() const;

  const bool f15() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f15' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] const bool f15() const;

  inline const bool f16() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f16' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] inline const bool f16() const;

  inline const std::string &f45() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f45' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] inline const std::string &f45() const;

  inline virtual const bool f17() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f17' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] inline virtual const bool f17() const;

  // inline with body
  bool f18() const
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f18' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f18() const
  {
    return true;
  }

  bool f19() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f19' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f19() const;

  bool f24(size_type) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f24' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f24(size_type) const;

  bool f28(my_unsigned) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f28' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f28(my_unsigned) const;

  bool f29(my_unsigned_reference) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f29' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f29(my_unsigned_reference) const;

  bool f30(my_unsigned_const_reference) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f30' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f30(my_unsigned_const_reference) const;

  template <class F>
  F f37(F a, F b) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f37' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] F f37(F a, F b) const;

  template <class F>
  bool f38(F a) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f38' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f38(F a) const;

  bool f39(const std::function<bool()> &predicate) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f39' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f39(const std::function<bool()> &predicate) const;

  bool f39a(std::function<bool()> predicate) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f39a' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f39a(std::function<bool()> predicate) const;

  bool f39b(const std::function<bool()> predicate) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f39b' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f39b(const std::function<bool()> predicate) const;

  bool f45(const boost::function<bool()> &predicate) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f45' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f45(const boost::function<bool()> &predicate) const;

  bool f45a(boost::function<bool()> predicate) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f45a' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f45a(boost::function<bool()> predicate) const;

  bool f45b(const boost::function<bool()> predicate) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f45b' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f45b(const boost::function<bool()> predicate) const;

  template <class... Args>
  bool ParameterPack(Args... args) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'ParameterPack' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool ParameterPack(Args... args) const;

  template <typename... Targs>
  bool ParameterPack2(Targs... Fargs) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'ParameterPack2' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool ParameterPack2(Targs... Fargs) const;

  bool VariadicFunctionTest(const int &, ...) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'VariadicFunctionTest' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool VariadicFunctionTest(const int &, ...) const;

  static bool not_empty();
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'not_empty' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] static bool not_empty();

  explicit operator bool() const { return true; }
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'operator bool' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] explicit operator bool() const { return true; }

  NoDiscardStruct f50() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f50' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] NoDiscardStruct f50() const;
};

// Do not add ``[[nodiscard]]`` to Lambda.
const auto nonConstReferenceType = [] {
  return true;
};

auto lambda1 = [](int a, int b) { return a < b; };
auto lambda1a = [](int a) { return a; };
auto lambda1b = []() { return true; };

auto get_functor = [](bool check) {
  return [&](const std::string &sr) -> std::string {
    if (check) {
      return std::string();
    }
    return std::string();
  };
};

// Do not add ``[[nodiscard]]`` to function definition.
bool Foo::f19() const {
  return true;
}

template <class T>
class Bar {
public:
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;

  operator bool() const { return true; }
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'operator bool' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] operator bool() const { return true; }

  bool empty() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'empty' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool empty() const;

  bool f25(value_type) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f25' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f25(value_type) const;

  bool f27(reference) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f27' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f27(reference) const;

  typename T::value_type f35() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f35' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] typename T::value_type f35() const;

  T f34() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f34' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] T f34() const;

  bool f31(T) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f31' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f31(T) const;

  bool f33(T &) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f33' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f33(T &) const;

  bool f26(const_reference) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f26' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f26(const_reference) const;

  bool f32(const T &) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f32' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f32(const T &) const;
};

template <typename _Tp, int cn>
class Vec {
public:
  Vec(_Tp v0, _Tp v1); //!< 2-element vector constructor

  Vec cross(const Vec &v) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'cross' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] Vec cross(const Vec &v) const;

  template <typename T2>
  operator Vec<T2, cn>() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'operator Vec<type-parameter-1-0, cn>' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] operator Vec<T2, cn>() const;
};

template <class T>
class Bar2 {
public:
  typedef T value_type;
  typedef value_type &reference;
  typedef const value_type &const_reference;

  bool f40(value_type) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f40' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f40(value_type) const;

  bool f41(reference) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f41' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f41(reference) const;

  value_type f42() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f42' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] value_type f42() const;

  typename T::value_type f43() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f43' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] typename T::value_type f43() const;

  bool f44(const_reference) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'f44' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool f44(const_reference) const;
};

template <class T>
bool Bar<T>::empty() const {
  return true;
}

// don't mark typical ``[[nodiscard]]`` candidates if the class
// has mutable member variables
class MutableExample {
  mutable bool m_isempty;

public:
  bool empty() const;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: function 'empty' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
  // CHECK-FIXES{LITERAL}: [[nodiscard]] bool empty() const;
};

bool f1();
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: function 'f1' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
// CHECK-FIXES{LITERAL}: [[nodiscard]] bool f1();

template <typename T>
typename T::value f2(T &) noexcept;
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: function 'f2' should be marked {{\[\[nodiscard\]\]}} [daedalean-use-nodiscard]
// CHECK-FIXES{LITERAL}: [[nodiscard]] typename T::value f2(T &) noexcept;

template <typename T>
[[nodiscard]] typename T::value f3(T &) noexcept;

[[nodiscard]] bool f4();

[[nodiscard]] bool callBuiltin() {
  return __builtin_isinf(0.0) != 0;
}
