<a id="top"></a>
# Deprecations and incoming changes

This page documents current deprecations and upcoming planned changes
inside Catch2. The difference between these is that a deprecated feature
will be removed, while a planned change to a feature means that the
feature will behave differently, but will still be present. Obviously,
either of these is a breaking change, and thus will not happen until
at least the next major release.



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
