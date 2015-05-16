# Logging macros

Additional messages can be logged during a test case.

## Streaming macros

All these macros allow heterogenous sequences of values to be streaming using the insertion operator (```<<```) in the same way that std::ostream, std::cout, etc support it.

E.g.:
```c++
INFO( "The number is " << i );
```

(Note that there is no initial ```<<``` - instead the insertion sequence is placed in parentheses.)
These macros come in three forms:

**INFO(** _message expression_ **)**

The message is logged to a buffer, but only reported with the next assertion that is logged. This allows you to log contextual information in case of failures which is not shown during a successful test run (for the console reporter, without -s). Messages are removed from the buffer at the end of their scope, so may be used, for example, in loops.

**WARN(** _message expression_ **)**

The message is always reported but does not fail the test.

**FAIL(** _message expression_ **)**

The message is reported and the test case fails.

**FAIL_AT(** _file_, _line_, _message expression_ **)**

Used when binding other test frameworks to report errors through Catch. Same as **FAIL(** _message expression_ **)** above, but _file_ and _line_ is a reference to the location in the source code the failure will be reported at.

**MARK_FAILED(** _message expression_ **)**

The message is reported and the test case fails, but execution continues.

**MARK_FAILED_AT(** _file_, _line_, _message expression_ **)**

Used when binding othet test frameworks to report errots through Catch. Same as **MARK_FAILED(** _message expression_ **)** above, but _file_ and _line_ is a reference to the location in the source code the failure will be reported at.

## Quickly capture a variable value

**CAPTURE(** _expression_ **)**

Sometimes you just want to log the name and value of a variable. While you can easily do this with the INFO macro, above, as a convenience the CAPTURE macro handles the stringising of the variable name for you (actually it works with any expression, not just variables).

E.g.
```c++
CAPTURE( theAnswer );
```

This would log something like:

<pre>"theAnswer := 42"</pre>

## Deprecated macros

**SCOPED_INFO and SCOPED_CAPTURE**

These macros are now deprecated and are just aliases for INFO and CAPTURE (which were not previously scoped).

---

[Home](Readme.md)