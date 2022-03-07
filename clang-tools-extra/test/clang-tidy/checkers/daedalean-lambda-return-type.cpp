// RUN: %check_clang_tidy %s daedalean-lambda-return-type %t

auto foo = []() { return 2; };
// CHECK-MESSAGES: :[[@LINE-1]]:17: warning: Lambda function without auto arguments must have explicit return type [daedalean-lambda-return-type]
// CHECK-FIXES: -> int

auto foo2 = []() { };

auto bar = []() -> int { return 2; };
auto bar2 = [](auto i)  { return i; };
