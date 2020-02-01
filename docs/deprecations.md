<a id="top"></a>
# Deprecations and incoming changes

This page documents current deprecations and upcoming planned changes
inside Catch2. The difference between these is that a deprecated feature
will be removed, while a planned change to a feature means that the
feature will behave differently, but will still be present. Obviously,
either of these is a breaking change, and thus will not happen until
at least the next major release.


### Composing lvalues of already composed matchers

Because a significant bug in this use case has persisted for 2+ years
without a bug report, and to simplify the implementation, code that
composes lvalues of composed matchers will not compile. That is,
this code will no longer work:

```cpp
            auto m1 = Contains("string");
            auto m2 = Contains("random");
            auto composed1 = m1 || m2;
            auto m3 = Contains("different");
            auto composed2 = composed1 || m3;
            REQUIRE_THAT(foo(), !composed1);
            REQUIRE_THAT(foo(), composed2);
```

Instead you will have to write this:

```cpp
            auto m1 = Contains("string");
            auto m2 = Contains("random");
            auto m3 = Contains("different");
            REQUIRE_THAT(foo(), !(m1 || m2));
            REQUIRE_THAT(foo(), m1 || m2 || m3);
```



## Planned changes

### `CHECKED_IF` and `CHECKED_ELSE`

To make the `CHECKED_IF` and `CHECKED_ELSE` macros more useful, they will
be marked as "OK to fail" (`Catch::ResultDisposition::SuppressFail` flag
will be added), which means that their failure will not fail the test,
making the `else` actually useful.


### Console Colour API

The API for Catch2's console colour will be changed to take an extra
argument, the stream to which the colour code should be applied.


---

[Home](Readme.md#top)
