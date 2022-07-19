#pragma once

namespace template_specializations {

template <typename T>
concept TriviallyCopyable = __is_trivially_copyable(T);

template <typename T>
concept NotTriviallyCopyable = !__is_trivially_copyable(T);

namespace other_header {

template <typename T>
void function() {}

template <>
void function<char>() {}

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

} // namespace other_header
} // namespace template_specializations
