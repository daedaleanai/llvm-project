#include "project1/dir1/cls1.hh"
#include <project1/dir1/header1.hh>
#include <project1/dir1/header2.hh>
#include <project1/dir1/header2.hh>
#include <project1/dir2/header3.hh>
#include <project2/header3.hh>
// CHECK-MESSAGES: :[[@LINE-6]]:10: warning: Use #include<> instead of include "" [daedalean-include-order]
// CHECK-FIXES: <project1/dir1/cls1.hh>
// CHECK-MESSAGES: :[[@LINE-8]]:10: warning: <project1/dir1/cls1.hh> and <project1/dir1/header1.hh> should be in different groups [daedalean-include-order]
// CHECK-MESSAGES: :[[@LINE-6]]:10: warning: <project1/dir1/header2.hh> and <project1/dir2/header3.hh> should be in different groups [daedalean-include-order]
// CHECK-MESSAGES: :[[@LINE-6]]:10: warning: <project1/dir2/header3.hh> and <project2/header3.hh> should be in different groups [daedalean-include-order]