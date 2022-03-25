.. title:: clang-tidy - daedalean-include-order

daedalean-include-order
=======================

Daedalean coding standards for includes

- Only header files may be included
- Each file MUST include all necessary headers for used symbols.
- Include path MUST be relative to workspace root
- Include groups separate by empty line MUST be used in following order:
 1. Related header
 2. Other headers from same directory
 3. Other headers from same project
 4. Library headers
-Includes in one group MUST be ordered lexicographically.