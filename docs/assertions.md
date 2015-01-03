# Assertion Macros

Most test frameworks have a large collection of assertion macros to capture all possible conditional forms (```_EQUALS```, ```_NOTEQUALS```, ```_GREATER_THAN``` etc).

Catch is different. Because it decomposes natural C-style conditional expressions most of these forms are reduced to one or two that you will use all the time. That said there are a rich set of auxilliary macros as well. We'll describe all of these here.

Most of these macros come in two forms:

## Natural Expressions

The ```REQUIRE``` family of macros tests an expression and aborts the test case if it fails.
The ```CHECK``` family are equivalent but execution continues in the same test case even if the assertion fails. This is useful if you have a series of essentially orthogonal assertions and it is useful to see all the results rather than stopping at the first failure.

* **REQUIRE(** _expression_ **)** and  
* **CHECK(** _expression_ **)**

Evaluates the expression and records the result. If an exception is thrown it is caught, reported, and counted as a failure. These are the macros you will use most of  the time

Examples:
```
CHECK( str == "string value" );
CHECK( thisReturnsTrue() );
REQUIRE( i == 42 );
```

* **REQUIRE_FALSE(** _expression_ **)** and  
* **CHECK_FALSE(** _expression_ **)**

Evaluates the expression and records the _logical NOT_ of the result. If an exception is thrown it is caught, reported, and counted as a failure.
(these forms exist as a workaround for the fact that ! prefixed expressions cannot be decomposed).

Example:
```
REQUIRE_FALSE( thisReturnsFalse() );
```

### Floating point comparisons

When comparing floating point numbers - especially if at least one of them has been computed - great care must be taken to allow for rounding errors and inexact representations.

Catch provides a way to perform tolerant comparisons of floating point values through use of a wrapper class called ```Approx```. ```Approx``` can be used on either side of a comparison expression. It overloads the comparisons operators to take a tolerance into account. Here's a simple example:

```
REQUIRE( performComputation() == Approx( 2.1 ) );
```

By default a small epsilon value is used that covers many simple cases of rounding errors. When this is insufficent the epsilon value (the amount within which a difference either way is ignored) can be specified by calling the ```epsilon()``` method on the ```Approx``` instance. e.g.:

```
REQUIRE( 22/7 == Approx( 3.141 ).epsilon( 0.01 ) );
```

When dealing with very large or very small numbers it can be useful to specify a scale, which can be achieved by calling the ```scale()``` method on the ```Approx``` instance.

## Exceptions

* **REQUIRE_THROWS(** _expression_ **)** and  
* **CHECK_THROWS(** _expression_ **)**

Expects that an exception (of any type) is be thrown during evaluation of the expression.

* **REQUIRE_THROWS_AS(** _expression_, _exception type_ **)** and  
* **CHECK_THROWS_AS(** _expression_, _exception type_ **)**

Expects that an exception of the _specified type_ is thrown during evaluation of the expression.

* **REQUIRE_NOTHROW(** _expression_ **)** and  
* **CHECK_NOTHROW(** _expression_ **)**

Expects that no exception is thrown during evaluation of the expression.

## Matcher expressions

To support Matchers a slightly different form is used. Matchers will be more fully documented elsewhere. *Note that Matchers are still at early stage development and are subject to change.*

* **REQUIRE_THAT(** _lhs_, _matcher call_ **)** and  
* **CHECK_THAT(** _lhs_, _matcher call_ **)**  


---

[Home](Readme.md)