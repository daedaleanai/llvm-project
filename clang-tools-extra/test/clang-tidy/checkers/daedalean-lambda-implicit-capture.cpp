// RUN: %check_clang_tidy %s daedalean-lambda-implicit-capture %t

void f() {

  auto foo = [&]() { return 2; };
  // CHECK-MESSAGES: :[[@LINE-1]]:14: warning: Lambda function capture must be explicit [daedalean-lambda-implicit-capture]

  auto foo1 = [=]() { return 2; };
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Lambda function capture must be explicit [daedalean-lambda-implicit-capture]

  int a = 0;
  auto bar1 = []() {};
  auto bar2 = [a](auto i) { return i; };
  auto bar3 = [&a](auto i) { return i; };

  auto foo3 = [=, t=a]() { return 2; };
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Lambda function capture must be explicit [daedalean-lambda-implicit-capture]

  auto foo4 = [&, t=a]() { return 2; };
  // CHECK-MESSAGES: :[[@LINE-1]]:15: warning: Lambda function capture must be explicit [daedalean-lambda-implicit-capture]
}