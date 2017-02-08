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

Do note that "overly complex" expressions cannot be decomposed and thus will not compile. This is done partly for practical reasons (to keep the underlying expression template machinery to minimum) and partly for philosophical reasons (assertions should be simple and deterministic).

Examples:
* `CHECK(a == 1 && b == 2);`
This expression is too complex because of the `&&` operator. If you want to check that 2 or more properties hold, you can either put the expression into parenthesis, which stops decomposition from working, or you need to decompose the expression into two assertions: `CHECK( a == 1 ); CHECK( b == 2);`
* `CHECK( a == 2 || b == 1 );`
This expression is too complex because of the `||` operator. If you want to check that one of several properties hold, you can put the expression into parenthesis (unlike with `&&`, expression decomposition into several `CHECK`s is not possible).


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

* **REQUIRE_NOTHROW(** _expression_ **)** and  
* **CHECK_NOTHROW(** _expression_ **)**

Expects that no exception is thrown during evaluation of the expression.

* **REQUIRE_THROWS(** _expression_ **)** and  
* **CHECK_THROWS(** _expression_ **)**

Expects that an exception (of any type) is be thrown during evaluation of the expression.

* **REQUIRE_THROWS_AS(** _expression_, _exception type_ **)** and  
* **CHECK_THROWS_AS(** _expression_, _exception type_ **)**

Expects that an exception of the _specified type_ is thrown during evaluation of the expression.

* **REQUIRE_THROWS_WITH(** _expression_, _string or string matcher_ **)** and  
* **CHECK_THROWS_WITH(** _expression_, _string or string matcher_ **)**

Expects that an exception is thrown that, when converted to a string, matches the _string_ or _string matcher_ provided (see next section for Matchers).

e.g.
```cpp
REQUIRE_THROWS_WITH( openThePodBayDoors(), Contains( "afraid" ) && Contains( "can't do that" ) );
REQUIRE_THROWS_WITH( dismantleHal(), "My mind is going" );
```


Please note that the `THROW` family of assertions expects to be passed a single expression, not a statement or series of statements. If you want to check a more complicated sequence of operations, you can use a C++11 lambda function.

```cpp
REQUIRE_NOTHROW([&](){
    int i = 1;
    int j = 2;
    auto k = i + j;
    if (k == 3) {
        throw 1;
    }
}());
```

## Matcher expressions

To support Matchers a slightly different form is used. Matchers will be more fully documented elsewhere. *Note that Matchers are still at early stage development and are subject to change.*

* **REQUIRE_THAT(** _lhs_, _matcher expression_ **)** and  
* **CHECK_THAT(** _lhs_, _matcher expression_ **)**  

Currently only string matchers are implemented and consist of: `Contains`, `Equals`, `StartsWith` and `EndsWith`.

Matchers can be composed using `&&`, `||` and `!` operators.

---

[Home](Readme.md)