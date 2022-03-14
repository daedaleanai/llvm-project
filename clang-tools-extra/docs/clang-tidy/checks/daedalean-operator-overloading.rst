.. title:: clang-tidy - daedalean-operator-overloading

daedalean-operator-overloading
==============================

Daedalean coding standards for operator overloading
 - Operators , && || MUST not be overloaded.
 - Unary operator & MUST not be overloaded.
 - If operator[] overloaded with non-const version, const version MUST be present.
 - Assignment operators MUST return reference to self.
 - When comparison operators overloaded, set of operators MUST be complete:
   - If == overloaded, != MUST be overloaded as well
   - If < overloaded, > MUST be overloaded as well
   - If <= overloaded, >= MUST be overloaded as well
   - If <= are overloaded, == MUST be overloaded
   - If < and == are overloaded, <= MUST be overloaded
