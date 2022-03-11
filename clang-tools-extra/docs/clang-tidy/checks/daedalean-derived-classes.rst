.. title:: clang-tidy - daedalean-derived-classes

daedalean-derived-classes
=========================

Daedalean coding standards for derived classes
1. Each interface MUST have a virtual default destructor.
2. Inheritance from base class with non-pure-virtual methods other than destructor and/or data members is forbidden.
3. Public inheritance MUST be used to implement interfaces.
4. Base class MUST be declared virtual if it is in diamond inheritance.
5. All entity names MUST be unique in all inherited interfaces. If two interfaces share the same methods they MUST extend the common base interface.
6. Virtual functions MUST contain exactly one of two specifiers: virtual for new function or final if function overrides method from base class.
7. If a class contains at least one non-pure-virtual method other than destructor it MUST be declared final.
