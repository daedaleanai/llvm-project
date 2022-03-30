// RUN: %check_clang_tidy %s daedalean-preprocessing-directives %t
#pragma once
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]

#define MACRO_NAME 1
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]

#ifdef A
// CHECK-MESSAGES: :[[@LINE-1]]:2: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]
#elifdef B
// CHECK-MESSAGES: :[[@LINE-1]]:2: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]
#else
// CHECK-MESSAGES: :[[@LINE-1]]:2: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]
#endif
// CHECK-MESSAGES: :[[@LINE-1]]:2: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]
#ifndef A
// CHECK-MESSAGES: :[[@LINE-1]]:2: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]
#elifndef B
// CHECK-MESSAGES: :[[@LINE-1]]:2: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]
#endif
// CHECK-MESSAGES: :[[@LINE-1]]:2: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]
#if 1
// CHECK-MESSAGES: :[[@LINE-1]]:2: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]
#elif 2
// CHECK-MESSAGES: :[[@LINE-1]]:2: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]
#endif
// CHECK-MESSAGES: :[[@LINE-1]]:2: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]

#undef MACRO_NAME
// CHECK-MESSAGES: :[[@LINE-1]]:8: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]

#pragma pack(push, 1)
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]
#pragma pack(pop)
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]