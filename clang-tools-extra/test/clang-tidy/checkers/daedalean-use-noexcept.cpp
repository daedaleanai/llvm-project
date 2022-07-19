// RUN: %check_clang_tidy %s daedalean-use-noexcept %t -std c++20

class S {
public:
  S();
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: Method 'S' should be noexcept [daedalean-use-noexcept]
  S(const S &)
  noexcept = default;

  bool operator==(const S &) const noexcept;
  bool operator!=(const S &) const;
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: Method 'operator!=' should be noexcept [daedalean-use-noexcept]
};

void f() noexcept;

void f2();
// CHECK-MESSAGES: :[[@LINE-1]]:6: warning: Function 'f2' should be noexcept [daedalean-use-noexcept]
//
void func() noexcept {
  auto noexceptLambda = []() noexcept {

  };

  auto lambda = []() {
    // CHECK-MESSAGES: :[[@LINE-1]]:17: warning: Lambda should be noexcept [daedalean-use-noexcept]
  };
}

template <typename T>
class Wrapper {
};

template <typename T>
class MoreWrappers {
  MoreWrappers(Wrapper<T>) noexcept;
  MoreWrappers() = delete;
};

template <typename T>
MoreWrappers(Wrapper<T>) -> MoreWrappers<T>;
