.. title:: clang-tidy - daedalean-preprocessing-directives

daedalean-preprocessing-directives
==================================

Only following uses of preprocessor directives are allowed:

- #pragma once MUST be used to prevent multiple inclusions of the same file
- #include <path> MUST be used to include headers
