# 1.7.0

### Features/ Changes:
* Catch now runs significantly faster for passing tests
  * Microbenchmark focused on Catch's overhead went from ~3.4s to ~0.7s.
  * Real world test using [JSON for Modern C++](https://github.com/nlohmann/json)'s test suite went from ~6m 25s to ~4m 14s.
* Catch can now run specific sections within test cases.
  * For now the support is only basic (no wildcards or tags), for details see the [documentation](docs/command-line.md).
* Catch now supports SEH on Windows as well as signals on Linux.
  * After receiving a signal, Catch reports failing assertion and then passes the signal onto the previous handler.
* Approx can be used to compare values against strong typedefs (available in C++11 mode only).
  * Strong typedefs mean types that are explicitly convertible to double.
* CHECK macro no longer stops executing section if an exception happens.
* Certain characters (space, tab, etc) are now pretty printed.
  * This means that a `char c = ' '; REQUIRE(c == '\t');` would be printed as `' ' == '\t'`, instead of ` == 9`.

### Fixes:
* Text formatting no longer attempts to access out-of-bounds characters under certain conditions.
* THROW family of assertions no longer trigger `-Wunused-value` on expressions containing explicit cast.
* Breaking into debugger under OS X works again and no longer required `DEBUG` to be defined.
* Compilation no longer breaks under certain compiler if a lambda is used inside assertion macro.

### Other:
* Catch's CMakeLists now defines install command.
* Catch's CMakeLists now generates projects with warnings enabled.





# Older versions
Release notes were not maintained prior to v1.6.0, but you should be able to work them out from the Git history

## 1.6.1

### Features/ Changes:
* Catch now supports breaking into debugger on Linux

### Fixes:
* Generators no longer leak memory (generators are still unsupported in general)
* JUnit reporter now reports UTC timestamps, instead of "tbd"
* `CHECK_THAT` macro is now properly defined as `CATCH_CHECK_THAT` when using `CATCH_` prefixed macros

### Other:
* Types with overloaded `&&` operator are no longer evaluated twice when used in an assertion macro.
* The use of `__COUNTER__` is supressed when Catch is parsed by CLion
  * This change is not active when compiling a binary
* Approval tests can now be run on Windows
* CMake will now warn if a file is present in the `include` folder but not is not enumerated as part of the project
* Catch now defines `NOMINMAX` and `WIN32_LEAN_AND_MEAN` before including `windows.h`
  * This can be disabled if needed, see [documentation](docs/configuration.md) for details.


## 1.6.0

### Cmake/ projects:
* Moved CMakeLists.txt to root, made it friendlier for CLion and generating XCode and VS projects, and removed the manually maintained XCode and VS projects.

### Features/ Changes:
* Approx now supports `>=` and `<=`
* Can now use `\` to escape chars in test names on command line
* Standardize C++11 feature toggles

### Fixes:
* Blue shell colour
* Missing argument to `CATCH_CHECK_THROWS`
* Don't encode extended ASCII in XML
* use `std::shuffle` on more compilers (fixes deprecation warning/error)
* Use `__COUNTER__` more consistently (where available)

### Other:
* Tweaks and changes to scripts - particularly for Approval test - to make them more portable


---

[Home](Readme.md)
