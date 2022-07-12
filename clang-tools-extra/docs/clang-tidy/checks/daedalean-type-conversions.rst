.. title:: clang-tidy - daedalean-type-conversions

daedalean-type-conversions
==========================

Implements CS.R.18:
- C-style cast MUST NOT be used
- dynamic_cast MUST NOT be used
- const_cast MUST NOT be used
- reinterpret_cast MUST NOT be used

Implements CS.R.44:
- All type conversions MUST be explicit.
- Type conversion MUST NOT lead to data loss.
