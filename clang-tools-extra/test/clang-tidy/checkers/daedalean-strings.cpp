// RUN: %check_clang_tidy %s daedalean-strings %t

struct StringView final {

  constexpr StringView(const char * p, unsigned long l) : ptr(p), len(l) {}

  const char *ptr;
  unsigned long len;
};

constexpr StringView operator""_s(const char *ptr, const unsigned long len) {
  return {ptr, len};
}

void abort(const char*, unsigned long);

template <typename T>
class C {
public:
  constexpr void foo() noexcept {
    constexpr StringView STR = "HELLO"_s;
    abort(STR.ptr, STR.len);
  }
};

void b(const char *ptr);

void f(const char *ptr) {
  const char *another = "";
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: C-String literals MUST not be used [daedalean-strings]
  (void)another;
  StringView test = ""_s;
  (void)test;

  b("");
  // CHECK-MESSAGES: :[[@LINE-1]]:5: warning: C-String literals MUST not be used [daedalean-strings]

  b(""_s.ptr);

  C<int> var;
  var.foo();
}
