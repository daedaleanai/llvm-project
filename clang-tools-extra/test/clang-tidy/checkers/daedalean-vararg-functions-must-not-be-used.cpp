// RUN: %check_clang_tidy %s daedalean-vararg-functions-must-not-be-used %t

void f(int i);
void f_vararg(int i, ...);

struct C {
  void g_vararg(...) {}
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: do not define c-style vararg functions; use variadic templates instead [daedalean-vararg-functions-must-not-be-used]
  void g(const char *);
} c;

template <typename... P>
void cpp_vararg(P... p) {}

void check() {
  f_vararg(1, 7, 9);
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: do not call c-style vararg functions [daedalean-vararg-functions-must-not-be-used]
  c.g_vararg("foo");
  // CHECK-MESSAGES: :[[@LINE-1]]:5: warning: do not call c-style vararg functions [daedalean-vararg-functions-must-not-be-used]

  f(3);                // OK
  c.g("foo");          // OK
  cpp_vararg(1, 7, 9); // OK
}

template <typename T>
void CallFooIfAvailableImpl(T &t, ...) {
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: do not define c-style vararg functions; use variadic templates instead [daedalean-vararg-functions-must-not-be-used]
}
template <typename T>
void CallFooIfAvailableImpl(T &t, decltype(t.foo()) *) {
  t.foo();
}
template <typename T>
void CallFooIfAvailable(T &t) {
  CallFooIfAvailableImpl(t, 0); // OK to call variadic function when the argument is a literal 0
}

#include <stdarg.h>

void my_printf(const char *format, ...);

void my_printf(const char *format, ...) {
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: do not define c-style vararg functions; use variadic templates instead [daedalean-vararg-functions-must-not-be-used]
  va_list ap;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: do not declare variables of type va_list; use variadic templates instead [daedalean-vararg-functions-must-not-be-used]
  va_start(ap, format);
  va_list n;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: do not declare variables of type va_list; use variadic templates instead [daedalean-vararg-functions-must-not-be-used]
  va_copy(n, ap);
  int i = va_arg(ap, int);
  // CHECK-MESSAGES: :[[@LINE-1]]:11: warning: do not use va_arg to define c-style vararg functions; use variadic templates instead [daedalean-vararg-functions-must-not-be-used]
  va_end(ap);
}

int my_vprintf(const char *format, va_list arg);
// CHECK-MESSAGES: :[[@LINE-1]]:36: warning: do not declare variables of type va_list; use variadic templates instead [daedalean-vararg-functions-must-not-be-used]

void ignoredBuiltinsTest() {
  (void)__builtin_assume_aligned(0, 8);
  // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: do not call c-style vararg functions [daedalean-vararg-functions-must-not-be-used]
  (void)__builtin_fpclassify(0, 0, 0, 0, 0, 0.f);
  // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: do not call c-style vararg functions [daedalean-vararg-functions-must-not-be-used]
  (void)__builtin_isinf_sign(0.f);
  // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: do not call c-style vararg functions [daedalean-vararg-functions-must-not-be-used]
  (void)__builtin_prefetch(nullptr);
  // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: do not call c-style vararg functions [daedalean-vararg-functions-must-not-be-used]
}

// Declaring a variadic function but not defining it anywhere is fine, they can be used for SFINAE.
void declaredButNotDefinedAnywhere(const char *format, ...);

// Some implementations of __builtin_va_list and __builtin_ms_va_list desugared
// as 'char *' or 'void *'. This test checks whether we are handling this case
// correctly and not generating false positives.
void no_false_positive_desugar_va_list(char *in) {
  char *tmp1 = in;
  void *tmp2 = in;
}
