.. title:: clang-tidy - daedalean-auto

daedalean-auto
==============

Checks for any uses of auto in either variable declarations or return types and warns about them.

Exceptions:
- Iterator type in for loops. (which clang actually desugars into iterators).
- As return type of lambda with auto arguments.
- As return type in template functions if type depends on template arguments.
- Structured bindings in range-based for loops and condition initializers.
- Local variables holding lambda functions.
