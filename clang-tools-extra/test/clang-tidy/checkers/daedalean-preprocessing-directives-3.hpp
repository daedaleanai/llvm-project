// RUN: %check_clang_tidy %s daedalean-preprocessing-directives %t
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: #pragma once must be used as include guard [daedalean-preprocessing-directives]
