#pragma once

#include <template-specializations/other-header.hh>

namespace template_specializations {

template <typename T>
void justDeclaration();

template <typename T>
void function() {}

template <typename T>
void definedInSource();

extern template void definedInSource<float>();
extern template void definedInSource<int>();
extern template void definedInSource<double>();

template <>
void function<char>() {}

template <typename T>
class ClassTemplateOnlyDeclared;

// Partial specialization
template <TriviallyCopyable T>
class ClassTemplateOnlyDeclared<T> {
public:
  void doSomething() {}
};

// Full specialization
template <>
class ClassTemplateOnlyDeclared<int> {
public:
  void doSomething() {}
};

template <typename T>
class SomeClass {
public:
  void doSomething() {}
};

template <>
class SomeClass<char> {
public:
  void doSomething() {}
};

template <NotTriviallyCopyable T>
class SomeClass<T> {
public:
  void doSomething() {}
};

namespace other_header {
template <>
void function<int>() {}
// CHECK-MESSAGES: [[@LINE-1]]:6: warning: Function template specialization 'function<int>' should be defined in the same file as 'function' [daedalean-template-specializations]

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

} // namespace other_header

} // namespace template_specializations
