# Test cases and sections

While Catch fully supports the traditional, xUnit, style of class-based fixtures containing test case methods this is not the preferred style.

Instead Catch provides a powerful mechanism for nesting test case sections within a test case. For a more detailed discussion see the [tutorial](tutorial.md#testCasesAndSections).

Test cases and sections are very easy to use in practice:

* **TEST_CASE(** _test name_ \[, _tags_ \] **)**
* **SECTION(** _section name_ **)**

_test name_ and _section name_ are free form, quoted, strings. The optional _tags_ argument is a quoted string containing one or more tags enclosed in square brackets. Tags are discussed below. Test names must be unique within the Catch executable.

For examples see the [Tutorial](tutorial.md)

## Tags

Tags allow an arbitrary number of additional strings to be associated with a test case. Test cases can be selected (for running, or just for listing) by tag - or even by an expression that combines several tags. At their most basic level they provide a simple way to group several related tests together.

As an example - given the following test cases:

	TEST_CASE( "A", "[widget]" ) { /* ... */ }
	TEST_CASE( "B", "[widget]" ) { /* ... */ }
	TEST_CASE( "C", "[gadget]" ) { /* ... */ }
	TEST_CASE( "D", "[widget][gadget]" ) { /* ... */ }

The tag expression, ```"[widget]"``` selects A, B & D. ```"[gadget]"``` selects C & D. ```"[widget][gadget]"``` selects just D and ```"[widget],[gadget]"``` selects all four test cases.

For more detail on command line selection see [the command line docs](command-line.md#specifying-which-tests-to-run)

A special tag name, ```[hide]``` causes test cases to be skipped from the default list (ie when no test cases have been explicitly selected through tag expressions or name wildcards). ```[.]``` is an alias for ```[hide]```.

Tag names are not case sensitive.

## BDD-style test cases

In addition to Catch's take on the classic style of test cases, Catch supports an alternative syntax that allow tests to be written as "executable specifications" (one of the early goals of [Behaviour Driven Development](http://dannorth.net/introducing-bdd/)). This set of macros map on to ```TEST_CASE```s and ```SECTION```s, with a little internal support to make them smoother to work with.

* **SCENARIO(** _scenario name_ \[, _tags_ \] **)**

This macro maps onto ```TEST_CASE``` and works in the same way, except that the test case name will be prefixed by "Scenario: "

* **GIVEN(** _something_ **)**
* **WHEN(** _something_ **)**
* **THEN(** _something_ **)**

These macros map onto ```SECTION```s except that the section names are the _something_s prefixed by "given: ", "when: " or "then: " respectively.

* **AND_WHEN(** _something_ **)**
* **AND_THEN(** _something_ **)**

Similar to ```WHEN``` and ```THEN``` except that the prefixes start with "and ". These are used to chain ```WHEN```s and ```THEN```s together.

When any of these macros are used the console reporter recognises them and formats the test case header such that the Givens, Whens and Thens are aligned to aid readability.

Other than the additional prefixes and the formatting in the console reporter these macros behave exactly as ```TEST_CASE```s and ```SECTION```s. As such there is nothing enforcing the correct sequencing of these macros - that's up to the programmer!

---

[Home](Readme.md)