// RUN: %check_clang_tidy %s daedalean-friend-declarations %t
#if 0
class S {};

class A {
  friend class B;
  // CHECK-MESSAGES: :[[@LINE-1]]:10: warning: Friend declaration must not be used [daedalean-friend-declarations]
  friend void foo();
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Friend declaration must not be used [daedalean-friend-declarations]
  friend S& operator << (S&, const A&);
};

#endif
template<typename T>
class C {

  template<typename U>
  friend class C;
};

C<int> bar() {
  return {};
}