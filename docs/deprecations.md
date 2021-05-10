<a id="top"></a>
# Deprecations and incoming changes

This page documents current deprecations and upcoming planned changes
inside Catch2. The difference between these is that a deprecated feature
will be removed, while a planned change to a feature means that the
feature will behave differently, but will still be present. Obviously,
either of these is a breaking change, and thus will not happen until
at least the next major release.


### `ParseAndAddCatchTests.cmake`

The CMake/CTest integration using `ParseAndAddCatchTests.cmake` is deprecated,
as it can be replaced by `Catch.cmake` that provides the function
`catch_discover_tests` to get tests directly from a CMake target via the
command line interface instead of parsing C++ code with regular expressions.


## Planned changes

### Console Colour API

The API for Catch2's console colour will be changed to take an extra
argument, the stream to which the colour code should be applied.


---

[Home](Readme.md#top)
