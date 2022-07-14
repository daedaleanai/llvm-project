.. title:: clang-tidy - daedalean-use-nodiscard

daedalean-use-nodiscard
=======================

Adds ``[[nodiscard]]`` attributes (introduced in C++17) to member functions in
order to highlight at compile time which return values should not be ignored.

Member functions need to satisfy the following conditions to be considered by
this check:

 - no ``[[nodiscard]]``, ``[[noreturn]]``,
   ``__attribute__((warn_unused_result))``,
   ``[[clang::warn_unused_result]]`` nor ``[[gcc::warn_unused_result]]``
   attribute,
 - non-void return type,
 - no Lambdas,
 - Reference returned from assignment operators may be discarded
 - Reference returned from prefix increment and decrement may be discarded

Example
-------

.. code-block:: c++

    bool empty() const;
    bool empty(int i) const;

transforms to:

.. code-block:: c++

    [[nodiscard]] bool empty() const;
    [[nodiscard]] bool empty(int i) const;

Options
-------

.. option:: ReplacementString

    Specifies a macro to use instead of ``[[nodiscard]]``. This is useful when
    maintaining source code that needs to compile with a pre-C++17 compiler.

Example
^^^^^^^

.. code-block:: c++

    bool empty() const;
    bool empty(int i) const;

transforms to:

.. code-block:: c++

    NO_DISCARD bool empty() const;
    NO_DISCARD bool empty(int i) const;

if the :option:`ReplacementString` option is set to `NO_DISCARD`.

.. note::

    If the :option:`ReplacementString` is not a C++ attribute, but instead a
    macro, then that macro must be defined in scope or the fix-it will not be
    applied.

.. note::

    For alternative ``__attribute__`` syntax options to mark functions as
    ``[[nodiscard]]`` in non-c++17 source code.
    See https://clang.llvm.org/docs/AttributeReference.html#nodiscard-warn-unused-result
