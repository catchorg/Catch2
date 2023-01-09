<a id="top"></a>
# Skipping Test Cases at Runtime

> [Introduced](https://github.com/catchorg/Catch2/pull/2360) in Catch2 X.Y.Z.

In some situations it may not be possible to meaningfully execute a test case, for example when the system under test is missing certain hardware capabilities.
If the required conditions can only be determined at runtime, it often doesn't make sense to consider such a test case as either passed or failed, because it simply can not run at all.
To properly express such scenarios, Catch2 allows to explicitly _skip_ test cases, using the `SKIP` macro:

**SKIP(** _message expression_ **)**

Example usage:

```c++
TEST_CASE("copy files between drives") {
    if(getNumberOfHardDrives() < 2) {
        SKIP("at least two hard drives required");
    }
    // ...
}
```

This test case is then reported as _skipped_ instead of _passed_ or _failed_.

The `SKIP` macro behaves similarly to an explicit [`FAIL`](logging.md#top), in that it is the last expression that will be executed:

```c++
TEST_CASE("my test") {
    printf("foo");
    SKIP();
    printf("bar"); // not printed
}
```

However a failed assertion _before_ a `SKIP` still causes the entire test case to fail:

```c++
TEST_CASE("failing test") {
    CHECK(1 == 2);
    SKIP();
}
```

## Interaction with Sections and Generators

Sections, nested sections as well as test cases with [generators](generators.md#top) can all be individually skipped, with the rest executing as usual:

```c++
TEST_CASE("complex test case") {
  int value = GENERATE(2, 4, 6);
  SECTION("a") {
    SECTION("a1") { CHECK(value < 8); }
    SECTION("a2") {
      if (value == 4) {
        SKIP();
      }
      CHECK(value % 2 == 0);
    }
  }
}
```

This test case will report 5 passing assertions; one for each of the three values in section `a1`, as well as one for each in `a2`, except for when `value == 4`.

Note that as soon as one section is skipped, the entire test case will be reported as _skipped_ (unless there is a failing assertion, in which case it will be reported as _failed_ instead).

If all test cases in a run are skipped, Catch2 returns a non-zero exit code by default.
This can be overridden using the [--allow-running-no-tests](command-line.md#no-tests-override) flag.

---

[Home](Readme.md#top)
