// RUN: %check_clang_tidy -std c++20 %s daedalean-template-specializations %t -- -- -I %S/Inputs/DDLNHeaders

#include <template-specializations/header.hh>

namespace template_specializations {

// Function specialization
template <>
void justDeclaration<int>() {}

template <>
void function<int>() {}
// CHECK-MESSAGES: [[@LINE-1]]:6: warning: Function template specialization 'function<int>' should be defined in the same file as 'function' [daedalean-template-specializations]

template <typename T>
void functionTemplateDeclaredInCppFile() {}

template <>
void functionTemplateDeclaredInCppFile<int>() {}

namespace other_header {
template <>
void function<const char *>() {}
// CHECK-MESSAGES: [[@LINE-1]]:6: warning: Function template specialization 'function<const char *>' should be defined in the same file as 'function' [daedalean-template-specializations]
} // namespace other_header

template <typename T>
void definedInSource() {}

template void definedInSource<float>();
template void definedInSource<int>();
template void definedInSource<double>();

// Class specialization

// Partial specialization
template <NotTriviallyCopyable T>
class ClassTemplateOnlyDeclared<T> {
public:
  void doSomething() {}
};

// Full specialization
template <>
class ClassTemplateOnlyDeclared<char> {
public:
  void doSomething() {}
};

template <>
class SomeClass<int> {
  // CHECK-MESSAGES: [[@LINE-1]]:7: warning: Class template specialization 'SomeClass<int>' should be defined in the same file as 'SomeClass' [daedalean-template-specializations]
public:
  void doSomething() {}
};

template <TriviallyCopyable T>
class SomeClass<T> {
  // CHECK-MESSAGES: [[@LINE-1]]:7: warning: Partial class template specialization 'SomeClass<T>' should be defined in the same file as 'SomeClass' [daedalean-template-specializations]
public:
  void doSomething() {}
};

template <typename T>
class SomeOtherClass {
public:
  void doSomething() {}
};

template <>
class SomeOtherClass<int> {
public:
  void doSomething() {}
};

template <TriviallyCopyable T>
class SomeOtherClass<T> {
public:
  void doSomething() {}
};

// type alias template decl

template <typename T>
using CustomType = SomeClass<T>;

// Use of specialized decls

void test() {
  SomeClass<int> A;
  SomeClass<char> B;

  static_cast<void>(A);
  static_cast<void>(B);

  function<SomeClass<int>>();
}

} // namespace template_specializations
