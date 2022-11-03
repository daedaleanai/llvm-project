// RUN: %check_clang_tidy %s daedalean-strings %t

struct StringView final {
  const char *ptr;
  unsigned long len;
};

StringView operator""_s(const char *ptr, const unsigned long len) {
  return StringView{.ptr = ptr, .len = len};
}

void f(const char *ptr) {
  const char *another = "";
  // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: C-String literals MUST not be used [daedalean-strings]
  StringView test = ""_s;

  f("");
  // CHECK-MESSAGES: :[[@LINE-1]]:5: warning: C-String literals MUST not be used [daedalean-strings]

  f(""_s.ptr);
}
