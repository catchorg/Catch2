# Test cases and sections

While Catch fully supports the traditional, *x*Unit, style of class-based fixtures containing test case methods this is not the preferred style.

Instead Catch provides a powerful mechanism for nesting test case sections within a test case. For a more detailed discussion see the [tutorial](tutorial.md#testCasesAndSections).

Test cases and sections are very easy to use in practice:

**TEST_CASE(** _test name_ [**,** _tags_ ] **)**
**SECTION(** _section name_ **)**

_test name_ and _section name_ are free form, quoted, strings. The optional _tags_ argument is a quoted string containing one or more tags enclosed in square brackets. Tags are discussed below. Test names must be unique within the Catch executable.

For examples see the [Tutorial](tutorial.md)

## Tags

-{placeholder for documentation of tags}-

## User Story/ BDD-style test cases

In addition to Catch's take on the classic style of test cases, Catch supports an alternative syntax that allow tests to be written as "executable specifications" (one of the early goals of BDD). This set of macros map on to TEST_CASEs and SECTIONs, with a little internal support to make them smoother to work with.

**SCENARIO(** _scenario name_ )

-{placeholder for given-when-then docs}-

---

[Home](../README.md)