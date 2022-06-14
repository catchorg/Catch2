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


### `CATCH_CONFIG_BAZEL_SUPPORT`

Catch2 supports writing the Bazel JUnit XML output file when it is aware
that is within a bazel testing environment. Originally there was no way
to accurately probe the environment for this information so the flag
`CATCH_CONFIG_BAZEL_SUPPORT` was added. This now deprecated. Bazel has now had a change
where it will export `BAZEL_TEST=1` for purposes like the above. Catch2
will now instead inspect the environment instead of relying on build configuration.

---

[Home](Readme.md#top)
