<a id="top"></a>
# Logging macros

Catch2 provides various macros for logging extra information when
running a test. These macros default to being scoped, and associate with
all assertions in the scope, regardless of whether they pass or fail.

**example**
```cpp
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Simple info") {
    INFO("Test case start");
    SECTION("A") {
        INFO("Section A");
        CHECK(false);        // 1
    }
    SECTION("B") {
        INFO("Section B");
        CHECK(false);        // 2
    }
    CHECK(false);            // 3
}
```
The first assertion will report messages "Test case start", and "Section A"
as extra information. The second one will report messages "Test case
started" and "Section B", while the third one will only report "Test case
started" as the extra info.


## Logging outside of current scope

> `UNSCOPED_INFO` was [introduced](https://github.com/catchorg/Catch2/issues/1522) in Catch2 2.7.0.

> `UNSCOPED_CAPTURE` was introduced in Catch2 3.13.0.

The `UNSCOPED_X` macros are similar to their plain `X` macro counterparts,
with two key differences:

- The lifetime of an unscoped message is not tied to its own scope.
- An unscoped message can be reported by the first following assertion only, regardless of the result of that assertion.

In other words, the `UNSCOPED_X` macros are useful to add extra information
to the next assertion, e.g. from helper functions or inner scopes.

An example:
```cpp
#include <catch2/catch_test_macros.hpp>

void print_some_info() {
    UNSCOPED_INFO("Info from helper");
}

TEST_CASE("Baz") {
    print_some_info();
    for (int i = 0; i < 2; ++i) {
        UNSCOPED_INFO("The number is " << i);
    }
    CHECK(false);
}

TEST_CASE("Qux") {
    INFO("First info");
    UNSCOPED_INFO("First unscoped info");
    CHECK(false);

    INFO("Second info");
    UNSCOPED_INFO("Second unscoped info");
    CHECK(false);
}
```

"Baz" test case prints:
```
Info from helper
The number is 0
The number is 1
```

With "Qux" test case, two messages will be printed when the first `CHECK` fails:
```
First info
First unscoped info
```

"First unscoped info" message will be cleared after the first `CHECK`, while "First info" message will persist until the end of the test case. Therefore, when the second `CHECK` fails, three messages will be printed:
```
First info
Second info
Second unscoped info
```

Note that unscoped messages are not passed between test cases, even if
there were no assertions between them.


## Streaming macros

Apart from `CAPTURE` (and its close sibling, `UNSCOPED_CAPTURE`), message
macros support gradual streaming of messages and values in the same way
that the standard streams do.


E.g.:
```c++
INFO( "The number is " << i );
```

(Note that there is no initial ```<<``` - instead the insertion sequence is placed in parentheses.)
These macros come in three forms:

**INFO(** _message expression_ **)**

The message is logged to a buffer, but only reported with next assertions that are logged. This allows you to log contextual information in case of failures which is not shown during a successful test run (for the console reporter, without -s). Messages are removed from the buffer at the end of their scope, so may be used, for example, in loops.

**UNSCOPED_INFO(** _message expression_ **)**

> [Introduced](https://github.com/catchorg/Catch2/issues/1522) in Catch2 2.7.0.

Similar to `INFO`, but messages are not limited to their own scope: They are removed from the buffer after each assertion, section or test case, whichever comes first.

**WARN(** _message expression_ **)**

The message is always reported but does not fail the test.

**SUCCEED(** _message expression_ **)**

The message is reported and the test case succeeds.

**FAIL(** _message expression_ **)**

The message is reported and the test case fails.

**FAIL_CHECK(** _message expression_ **)**

AS `FAIL`, but does not abort the test

## Quickly capture value of variables or expressions

**CAPTURE(** _expression1_, _expression2_, ... **)**

**UNSCOPED_CAPTURE(** _expression1_, _expression2_, ... **)**

> `UNSCOPED_CAPTURE` was introduced in Catch2 3.13.0.

Sometimes you just want to log a value of variable, or expression. For
convenience, we provide the `CAPTURE` macro, that can take a variable,
or an expression, and prints out that variable/expression and its value
at the time of capture.

e.g. `CAPTURE( theAnswer );` will log message "theAnswer := 42", while
```cpp
int a = 1, b = 2, c = 3;
CAPTURE( a, b, c, a + b, c > b, a == 1);
```
will log a total of 6 messages:
```
a := 1
b := 2
c := 3
a + b := 3
c > b := true
a == 1 := true
```

You can also capture expressions that use commas inside parentheses
(e.g. function calls), brackets, or braces (e.g. initializers). To
properly capture expression that contains template parameters list
(in other words, it contains commas between angle brackets), you need
to enclose the expression inside parentheses:
`CAPTURE( (std::pair<int, int>{1, 2}) );`


---

[Home](Readme.md#top)
