<a id="top"></a>
# Tutorial

**Contents**<br>
[Getting Catch2](#getting-catch2)<br>
[Writing tests](#writing-tests)<br>
[Test cases and sections](#test-cases-and-sections)<br>
[BDD-Style](#bdd-style)<br>
[Scaling up](#scaling-up)<br>
[Type parametrised test cases](#type-parametrised-test-cases)<br>
[Next steps](#next-steps)<br>


## Getting Catch2

Ideally you should be using Catch2 through its [CMake integration](cmake-integration.md#top).
Catch2 also provides pkg-config files and single TU distribution, but this
documentation will assume you are using CMake. If you are using single-TU
distribution instead, remember to replace the included header with `catch_amalgamated.hpp`.


## Writing tests

Let's start with a really simple example ([code](../examples/010-TestCase.cpp)). Say you have written a function to calculate factorials and now you want to test it (let's leave aside TDD for now).

```c++
unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}
```

```c++
#include <catch2/catch_test_macros.hpp>

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}
```

This will compile to a complete executable which responds to [command line arguments](command-line.md#top). If you just run it with no arguments it will execute all test cases (in this case there is just one), report any failures, report a summary of how many tests passed and failed and return the number of failed tests (useful for if you just want a yes/ no answer to: "did it work").

Anyway, as the tests above as written will pass, but there is a bug.
The problem is that `Factorial(0)` should return 1 (due to [its
definition](https://en.wikipedia.org/wiki/Factorial#Factorial_of_zero)).
Let's add that as an assertion to the test case:

```c++
TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(0) == 1 );
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}
```

After another compile & run cycle, we will see a test failure. The output
will look something like:

```
Example.cpp:9: FAILED:
  REQUIRE( Factorial(0) == 1 )
with expansion:
  0 == 1
```

Note that the output contains both the original expression,
`REQUIRE( Factorial(0) == 1 )` and the actual value returned by the call
to the `Factorial` function: `0`.

We can fix this bug by slightly modifying the `Factorial` function to:
```c++
unsigned int Factorial( unsigned int number ) {
  return number > 1 ? Factorial(number-1)*number : 1;
}
```


### What did we do here?

Although this was a simple test it's been enough to demonstrate a few
things about how Catch2 is used. Let's take a moment to consider those
before we move on.

* We introduce test cases with the `TEST_CASE` macro. This macro takes
  one or two string arguments - a free form test name and, optionally,
  one or more tags (for more see [Test cases and Sections](#test-cases-and-sections)).
* The test automatically self-registers with the test runner, and user
  does not have do anything more to ensure that it is picked up by the test
  framework. _Note that you can run specific test, or set of tests,
  through the [command line](command-line#top)._
* The individual test assertions are written using the `REQUIRE` macro.
  It accepts a boolean expression, and uses expression templates to
  internally decompose it, so that it can be individually stringified
  on test failure.
  
On the last point, note that there are more testing macros available,
because not all useful checks can be expressed as a simple boolean
expression. As an example, checking that an expression throws an exception
is done with the `REQUIRE_THROWS` macro. More on that later.


## Test cases and sections

Like most test frameworks, Catch2 supports a class-based fixture mechanism,
where individual tests are methods on class and setup/teardown can be
done in constructor/destructor of the type.

However, idiomatic usage of Catch2 avoids using it in favour of free
standing test cases using _sections_ to share setup and teardown code. 
This is best explained through an example ([code](../examples/100-Fix-Section.cpp)):

```c++
TEST_CASE( "vectors can be sized and resized", "[vector]" ) {

    std::vector<int> v( 5 );

    REQUIRE( v.size() == 5 );
    REQUIRE( v.capacity() >= 5 );

    SECTION( "resizing bigger changes size and capacity" ) {
        v.resize( 10 );

        REQUIRE( v.size() == 10 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "resizing smaller changes size but not capacity" ) {
        v.resize( 0 );

        REQUIRE( v.size() == 0 );
        REQUIRE( v.capacity() >= 5 );
    }
    SECTION( "reserving bigger changes capacity but not size" ) {
        v.reserve( 10 );

        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 10 );
    }
    SECTION( "reserving smaller does not change size or capacity" ) {
        v.reserve( 0 );

        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 5 );
    }
}
```

For each `SECTION` the `TEST_CASE` is executed from the start. This means
that each section is entered with a freshly constructed vector `v`, that
we know has size 5 and capacity at least 5, because the two assertions
are also checked before the section is entered. Each run through a test
case will execute one, and only one, leaf section.

Section can also be nested, in which case the parent section can be
entered multiple times, once for each leaf section. Nested sections are
most useful when you have multiple tests that share part of the set up.
To continue on the vector example above, you could add a check that
`std::vector::reserve` does not remove unused excess capacity, like this:

```cpp
    SECTION( "reserving bigger changes capacity but not size" ) {
        v.reserve( 10 );

        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 10 );
        SECTION( "reserving down unused capacity does not change capacity" ) {
            v.reserve( 7 );
            REQUIRE( v.size() == 5 );
            REQUIRE( v.capacity() >= 10 );
        }
    }
```

Another way to look at sections is that they are a way to define a tree 
of paths through the test. Each section represents a node, and the final
tree is walked in depth-first manner, with each path only visiting only
one leaf node.

There is no practical limit on nesting sections, as long as your compiler
can handle them, but keep in mind that overly nested sections can become
unreadable. From experience, having section nest more than 3 levels is
usually very hard to follow and not worth the removed duplication.


## BDD-Style

If you name your test cases and sections appropriately you can achieve a BDD-style specification structure. This became such a useful way of working that first class support has been added to Catch. Scenarios can be specified using ```SCENARIO```, ```GIVEN```, ```WHEN``` and ```THEN``` macros, which map on to ```TEST_CASE```s and ```SECTION```s, respectively. For more details see [Test cases and sections](test-cases-and-sections.md#top).

The vector example can be adjusted to use these macros like so ([example code](../examples/120-Bdd-ScenarioGivenWhenThen.cpp)):

```c++
SCENARIO( "vectors can be sized and resized", "[vector]" ) {

    GIVEN( "A vector with some items" ) {
        std::vector<int> v( 5 );

        REQUIRE( v.size() == 5 );
        REQUIRE( v.capacity() >= 5 );

        WHEN( "the size is increased" ) {
            v.resize( 10 );

            THEN( "the size and capacity change" ) {
                REQUIRE( v.size() == 10 );
                REQUIRE( v.capacity() >= 10 );
            }
        }
        WHEN( "the size is reduced" ) {
            v.resize( 0 );

            THEN( "the size changes but not capacity" ) {
                REQUIRE( v.size() == 0 );
                REQUIRE( v.capacity() >= 5 );
            }
        }
        WHEN( "more capacity is reserved" ) {
            v.reserve( 10 );

            THEN( "the capacity changes but not the size" ) {
                REQUIRE( v.size() == 5 );
                REQUIRE( v.capacity() >= 10 );
            }
        }
        WHEN( "less capacity is reserved" ) {
            v.reserve( 0 );

            THEN( "neither size nor capacity are changed" ) {
                REQUIRE( v.size() == 5 );
                REQUIRE( v.capacity() >= 5 );
            }
        }
    }
}
```

Conveniently, these tests will be reported as follows when run:

```
Scenario: vectors can be sized and resized
     Given: A vector with some items
      When: more capacity is reserved
      Then: the capacity changes but not the size
```


## Type parametrised test cases

Test cases in Catch2 can be also parametrised by type, via the
`TEMPLATE_TEST_CASE` and `TEMPLATE_PRODUCT_TEST_CASE` macros,
which behave in the same way the `TEST_CASE` macro, but are run for
every type or type combination.

For more details, see our documentation on [test cases and
sections](test-cases-and-sections.md#type-parametrised-test-cases).


## Next steps

This has been a brief introduction to get you up and running with Catch, and to point out some of the key differences between Catch and other frameworks you may already be familiar with. This will get you going quite far already and you are now in a position to dive in and write some tests.

Of course there is more to learn - most of which you should be able to page-fault in as you go. Please see the ever-growing [Reference section](Readme.md#top) for what's available.

---

[Home](Readme.md#top)
