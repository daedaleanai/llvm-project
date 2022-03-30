// RUN: %check_clang_tidy %s daedalean-preprocessing-directives %t
#pragma once
#pragma once
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: Pragma once must be specified only once [daedalean-preprocessing-directives]