<a id="top"></a>
# Deprecations and incoming changes

This page documents current deprecations and upcoming planned changes
inside Catch2. The difference between these is that a deprecated feature
will be removed, while a planned change to a feature means that the
feature will behave differently, but will still be present. Obviously,
either of these is a breaking change, and thus will not happen until
at least the next major release.


## Deprecations

### SourceLineInfo::empty()

There should be no reason to ever have an empty `SourceLineInfo`, so the
method will be removed.


## Planned changes

### `CHECKED_IF` and `CHECKED_ELSE`

To make the `CHECKED_IF` and `CHECKED_ELSE` macros more useful, they will
be marked as "OK to fail" (`Catch::ResultDisposition::SuppressFail` flag
will be added), which means that their failure will not fail the test,
making the `else` actually useful.


### Change semantics of `[.]` and tag exclusion

Currently, given these 2 tests
```cpp
TEST_CASE("A", "[.][foo]") {}
TEST_CASE("B", "[.][bar]") {}
```
specifying `[foo]` as the testspec will run test "A" and specifying
`~[foo]` will run test "B", even though it is hidden. Also, specifying
`~[baz]` will run both tests. This behaviour is often surprising and will
be changed so that hidden tests are included in a run only if they
positively match a testspec.


### Console Colour API

The API for Catch2's console colour will be changed to take an extra
argument, the stream to which the colour code should be applied.


### Type erasure in the `PredicateMatcher`

Currently, the `PredicateMatcher` uses `std::function` for type erasure,
so that type of the matcher is always `PredicateMatcher<T>`, regardless
of the type of the predicate. Because of the high compilation overhead
of `std::function`, and the fact that the type erasure is used only rarely,
`PredicateMatcher` will no longer be type erased in the future. Instead,
the predicate type will be made part of the PredicateMatcher's type.


---

[Home](Readme.md#top)
