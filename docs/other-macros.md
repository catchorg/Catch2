<a id="top"></a>
# Other macros

This page serves as a reference for macros that are not documented
elsewhere. For now, these macros are separated into 2 rough categories,
"assertion related macros" and "test case related macros".

## Assertion related macros

* `CHECKED_IF` and `CHECKED_ELSE`

`CHECKED_IF( expr )` is an `if` replacement, that also applies Catch2's
stringification machinery to the _expr_ and records the result. As with
`if`, the block after a `CHECKED_IF` is entered only if the expression
evaluates to `true`. `CHECKED_ELSE( expr )` work similarly, but the block
is entered only if the _expr_ evaluated to `false`.

Example:
```cpp
int a = ...;
int b = ...;
CHECKED_IF( a == b ) {
    // This block is entered when a == b
} CHECKED_ELSE ( a == b ) {
    // This block is entered when a != b
}
```

* `CHECK_NOFAIL`

`CHECK_NOFAIL( expr )` is a variant of `CHECK` that does not fail the test
case if _expr_ evaluates to `false`. This can be useful for checking some
assumption, that might be violated without the test neccessarily failing.

Example output:
```
main.cpp:6:
FAILED - but was ok:
  CHECK_NOFAIL( 1 == 2 )

main.cpp:7:
PASSED:
  CHECK( 2 == 2 )
```

* `SUCCEED`

`SUCCEED( msg )` is mostly equivalent with `INFO( msg ); REQUIRE( true );`.
In other words, `SUCCEED` is for cases where just reaching a certain line
means that the test has been a success.

Example usage:
```cpp
TEST_CASE( "SUCCEED showcase" ) {
    int I = 1;
    SUCCEED( "I is " << I );
}
```

## Test case related macros

* `METHOD_AS_TEST_CASE`
* `REGISTER_TEST_CASE`
* `ANON_TEST_CASE`
* `DYNAMIC_SECTION`
