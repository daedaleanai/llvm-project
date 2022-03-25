// RUN: %check_clang_tidy %s daedalean-include-order %t -- -header-filter=.* -- -I %S/Inputs/DDLNHeaders

#include "project1/dir1/cls1.cc"
// CHECK-MESSAGES: :[[@LINE-1]]:10: warning: Only header files (.h, .hh) should be included [daedalean-include-order]
// CHECK-MESSAGES: :[[@LINE-2]]:10: warning: Use #include<> instead of include "" [daedalean-include-order]
// CHECK-FIXES: <project1/dir1/cls1.cc>

