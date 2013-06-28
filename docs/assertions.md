# Assertion Macros

Most test frameworks have a large collection of assertion macros to capture all possible conditional forms (_EQUALS, _NOTEQUALS, _GREATER_THAN etc).

Catch is different. Because it decomposes natural C-style conditional expressions most of these forms are reduced to one or two that you will use all the time. That said there are a rich set of auxilliary macros as well. We'll describe all of these here.

Most of these macros come in two forms:

## Natural Expressions

The REQUIRE family of macros tests an expression and aborts the test case if it fails.
The CHECK family are equivalent but execution continues in the same test case even if the assertion fails. This is useful if you have a series of essentially orthoginal assertions and it is useful to see all the results rather than stopping at the first failure.

**REQUIRE(** _expression_ **)** and  
**CHECK(** _expression_ **)**  
Evaluates the expression and records the result. If an exception is thrown it is caught, reported, and counted as a failure. These are the macros you will use most of  the time

Examples:

```c++
CHECK( str == "string value" );
CHECK( thisReturnsTrue() );
REQUIRE( i == 42 );
```
**REQUIRE_FALSE(** _expression_ **)** and  
**CHECK_FALSE(** _expression_ **)**  
Evaluates the expression and records the _logical NOT_ of the result. If an exception is thrown it is caught, reported, and counted as a failure.
(these forms exist as a workaround for the fact that ! prefixed expressions cannot be decomposed).

Example:
```c++
REQUIRE_FALSE( thisReturnsFalse() );
```

## Exceptions

**REQUIRE_THROWS(** _expression_ **)** and  
**CHECK_THROWS(** _expression_ **)**  
Expects that an exception (of any type) is be thrown during evaluation of the expression.

**REQUIRE_THROWS_AS(** _expression_ and _exception type_ **)** and  
**CHECK_THROWS_AS(** _expression_, _exception type_ **)**  
Expects that an exception of the _specified type_ is thrown during evaluation of the expression.

**REQUIRE_NOTHROW(** _expression_ **)** and  
**CHECK_NOTHROW(** _expression_ **)**  
Expects that no exception is thrown during evaluation of the expression.

## Matcher expressions

To support Matchers a slightly different form is used. Matchers will be more fully documented elsewhere. *Note that Matchers are still at early stage development and are subject to change.*

**REQUIRE_THAT(** _lhs_, __matcher call__ **)** and  
**CHECK_THAT(** _lhs_, __matcher call__ **)**  


---

[Home](../README.md)