// RUN: %check_clang_tidy %s daedalean-preprocessing-directives %t
#pragma once

#define A
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: Preprocessor directives must not be used [daedalean-preprocessing-directives]