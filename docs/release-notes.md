# 1.10.0

### Fixes
* Evaluation layer has been rewritten (backported from Catch 2)
  * The new layer is much simpler and fixes some issues (#981)
* Implemented workaround for VS 2017 raw string literal stringification bug (#995)
* Fixed interaction between `[!shouldfail]` and `[!mayfail]` tags and sections
  * Previously sections with failing assertions would be marked as failed, not failed-but-ok

### Improvements
* Added [libidentify](https://github.com/janwilmans/LibIdentify) support
* Added "wait-for-keypress" option


# 1.9.7

### Fixes
* Various warnings from clang-tidy, Resharper-C++ and PVS Studio have been addressed (#957)
* Dynamically generated sections are now properly reported (#963)
* Writes to `std::clog` are redirected for reporters (#989)
  * Previously only `std::cerr` writes were redirected
  * Interleaved writes to `std::cerr` and `std::clog` are combined properly
* Assertions failed before signals/structured exceptions fails test case are properly reported as failed (#990)

### Improvements
* Catch's runtime overhead has been decreased further (#940)
* Added support for IBM i ILE c++ compiler (#976)
  * This means that AS/400 is now supported.
* The default reporter can be configured at compile time (#978)
  * That is, the reporter used if no reporter is explicitly specified

### Other
* `ParseAndAddCatchTests` cmake script has couple new customization options



# 1.9.6

### Improvements
* Catch's runtime overhead has been significantly decreased (#937, #939)
* Added `--list-extra-info` cli option (#934).
  * It lists all tests together with extra information, ie filename, line number and description.



# 1.9.5

### Fixes
* Truthy expressions are now reconstructed properly, not as booleans (#914)
* Various warnings are no longer erroneously suppressed in test files (files that include `catch.hpp`, but do not define `CATCH_CONFIG_MAIN` or `CATCH_CONFIG_RUNNER`) (#871)
* Catch no longer fails to link when main is compiled as C++, but linked against Objective-C (#855)
* Fixed incorrect gcc version detection when deciding to use `__COUNTER__` (#928)
  * Previously any GCC with minor version less than 3 would be incorrectly classified as not supporting `__COUNTER__`.
* Suppressed C4996 warning caused by upcoming updated to MSVC 2017, marking `std::uncaught_exception` as deprecated. (#927)

### Improvements
* CMake integration script now incorporates debug messages and registers tests in an improved way (#911)
* Various documentation improvements



# 1.9.4

### Fixes
* `CATCH_FAIL` macro no longer causes compilation error without variadic macro support
* `INFO` messages are no longer cleared after being reported once

### Improvements and minor changes
* Catch now uses `wmain` when compiled under Windows and `UNICODE` is defined.
  * Note that Catch still officially supports only ASCII

# 1.9.3

### Fixes
* Completed the fix for (lack of) uint64_t in earlier Visual Studios

# 1.9.2

### Improvements and minor changes
* All of `Approx`'s member functions now accept strong typedefs in C++11 mode (#888)
  * Previously `Approx::scale`, `Approx::epsilon`, `Approx::margin` and `Approx::operator()` didn't.


### Fixes
* POSIX signals are now disabled by default under QNX (#889)
  * QNX does not support current enough (2001) POSIX specification
* JUnit no longer counts exceptions as failures if given test case is marked as ok to fail.
* `Catch::Option` should now have its storage properly aligned.
* Catch no longer attempts to define `uint64_t` on windows (#862)
  * This was causing trouble when compiled under Cygwin

### Other
* Catch is now compiled under MSVC 2017 using `std:c++latest` (C++17 mode) in CI
* We now provide cmake script that autoregisters Catch tests into ctest.
  * See `contrib` folder.


# 1.9.1

### Fixes
* Unexpected exceptions are no longer ignored by default (#885, #887)


# 1.9.0


### Improvements and minor changes
* Catch no longer attempts to ensure the exception type passed by user in `REQUIRE_THROWS_AS` is a constant reference.
  * It was causing trouble when `REQUIRE_THROWS_AS` was used inside templated functions
  * This actually reverts changes made in v1.7.2
* Catch's `Version` struct should no longer be double freed when multiple instances of Catch tests are loaded into single program (#858)
  * It is now a static variable in an inline function instead of being an `extern`ed struct.
* Attempt to register invalid tag or tag alias now throws instead of calling `exit()`.
  * Because this happen before entering main, it still aborts execution
  * Further improvements to this are coming
* `CATCH_CONFIG_FAST_COMPILE` now speeds-up compilation of `REQUIRE*` assertions by further ~15%.
  * The trade-off is disabling translation of unexpected exceptions into text.
* When Catch is compiled using C++11, `Approx` is now constructible with anything that can be explicitly converted to `double`.
* Captured messages are now printed on unexpected exceptions

### Fixes:
* Clang's `-Wexit-time-destructors` should be suppressed for Catch's internals
* GCC's `-Wparentheses` is now suppressed for all TU's that include `catch.hpp`.
  * This is functionally a revert of changes made in 1.8.0, where we tried using `_Pragma` based suppression. This should have kept the suppression local to Catch's assertions, but bugs in GCC's handling of `_Pragma`s in C++ mode meant that it did not always work.
* You can now tell Catch to use C++11-based check when checking whether a type can be streamed to output.
  * This fixes cases when an unstreamable type has streamable private base (#877)
  * [Details can be found in documentation](configuration.md#catch_config_cpp11_stream_insertable_check)


### Other notes:
* We have added VS 2017 to our CI
* Work on Catch 2 should start soon



# 1.8.2


### Improvements and minor changes
* TAP reporter now behaves as if `-s` was always set
  * This should be more consistent with the protocol desired behaviour.
* Compact reporter now obeys `-d yes` argument (#780)
  * The format is "XXX.123 s: <section-name>" (3 decimal places are always present).
  * Before it did not report the durations at all.
* XML reporter now behaves the same way as Console reporter in regards to `INFO`
  * This means it reports `INFO` messages on success, if output on success (`-s`) is enabled.
  * Previously it only reported `INFO` messages on failure.
* `CAPTURE(expr)` now stringifies `expr` in the same way assertion macros do (#639)
* Listeners are now finally [documented](event-listeners.md).
  * Listeners provide a way to hook into events generated by running your tests, including start and end of run, every test case, every section and every assertion.


### Fixes:
* Catch no longer attempts to reconstruct expression that led to a fatal error  (#810)
  * This fixes possible signal/SEH loop when processing expressions, where the signal was triggered by expression decomposition.
* Fixed (C4265) missing virtual destructor warning in Matchers (#844)
* `std::string`s are now taken by `const&` everywhere (#842).
  * Previously some places were taking them by-value.
* Catch should no longer change errno (#835).
  * This was caused by libstdc++ bug that we now work around.
* Catch now provides `FAIL_CHECK( ... )` macro (#765).
  * Same as `FAIL( ... )`, but does not abort the test.
* Functions like `fabs`, `tolower`, `memset`, `isalnum` are now used with `std::` qualification (#543).
* Clara no longer assumes first argument (binary name) is always present (#729)
  * If it is missing, empty string is used as default.
* Clara no longer reads 1 character past argument string (#830)
* Regression in Objective-C bindings (Matchers) fixed (#854)


### Other notes:
* We have added VS 2013 and 2015 to our CI
* Catch Classic (1.x.x) now contains its own, forked, version of Clara (the argument parser).



# 1.8.1

### Fixes

Cygwin issue with `gettimeofday` - `#define` was not early enough

# 1.8.0

### New features/ minor changes

* Matchers have new, simpler (and documented) interface.
  * Catch provides string and vector matchers.
  * For details see [Matchers documentation](matchers.md).
* Changed console reporter test duration reporting format (#322)
  * Old format: `Some simple comparisons between doubles completed in 0.000123s`
  * New format: `xxx.123s: Some simple comparisons between doubles` _(There will always be exactly 3 decimal places)_
* Added opt-in leak detection under MSVC + Windows (#439)
  * Enable it by compiling Catch's main with `CATCH_CONFIG_WINDOWS_CRTDBG`
* Introduced new compile-time flag, `CATCH_CONFIG_FAST_COMPILE`, trading features for compilation speed.
  * Moves debug breaks out of tests and into implementation, speeding up test compilation time (~10% on linux).
  * _More changes are coming_
* Added [TAP (Test Anything Protocol)](https://testanything.org/) and [Automake](https://www.gnu.org/software/automake/manual/html_node/Log-files-generation-and-test-results-recording.html#Log-files-generation-and-test-results-recording) reporters.
  * These are not present in the default single-include header and need to be downloaded from GitHub separately.
  * For details see [documentation about integrating with build systems](build-systems.md).
*  XML reporter now reports filename as part of the `Section` and `TestCase` tags.
* `Approx` now supports an optional margin of absolute error
  * It has also received [new documentation](assertions.md).

### Fixes
* Silenced C4312 ("conversion from int to 'ClassName *") warnings in the evaluate layer.
* Fixed C4512 ("assignment operator could not be generated") warnings under VS2013.
* Cygwin compatibility fixes
  * Signal handling is no longer compiled by default.
  * Usage of `gettimeofday` inside Catch should no longer cause compilation errors.
* Improved `-Wparentheses` supression for gcc (#674)
  * When compiled with gcc 4.8 or newer, the supression is localized to assertions only
  * Otherwise it is supressed for the whole TU
* Fixed test spec parser issue (with escapes in multiple names)

### Other
* Various documentation fixes and improvements


# 1.7.2

### Fixes and minor improvements
Xml:

(technically the first two are breaking changes but are also fixes and arguably break few if any people)
* C-escape control characters instead of XML encoding them (which requires XML 1.1)
* Revert XML output to XML 1.0
* Can provide stylesheet references by extending the XML reporter
* Added description and tags attribites to XML Reporter
* Tags are closed and the stream flushed more eagerly to avoid stdout interpolation


Other:
* `REQUIRE_THROWS_AS` now catches exception by `const&` and reports expected type
* In `SECTION`s the file/ line is now of the `SECTION`. not the `TEST_CASE`
* Added std:: qualification to some functions from C stdlib
* Removed use of RTTI (`dynamic_cast`) that had crept back in
* Silenced a few more warnings in different circumstances
* Travis improvements

# 1.7.1

### Fixes:
* Fixed inconsistency in defining `NOMINMAX` and `WIN32_LEAN_AND_MEAN` inside `catch.hpp`.
* Fixed SEH-related compilation error under older MinGW compilers, by making Windows SEH handling opt-in for compilers other than MSVC.
  * For specifics, look into the [documentation](configuration.md).
* Fixed compilation error under MinGW caused by improper compiler detection.
* Fixed XML reporter sometimes leaving an empty output file when a test ends with signal/structured exception.
* Fixed XML reporter not reporting captured stdout/stderr.
* Fixed possible infinite recursion in Windows SEH.
* Fixed possible compilation error caused by Catch's operator overloads being ambiguous in regards to user-defined templated operators.

## 1.7.0

### Features/ Changes:
* Catch now runs significantly faster for passing tests
  * Microbenchmark focused on Catch's overhead went from ~3.4s to ~0.7s.
  * Real world test using [JSON for Modern C++](https://github.com/nlohmann/json)'s test suite went from ~6m 25s to ~4m 14s.
* Catch can now run specific sections within test cases.
  * For now the support is only basic (no wildcards or tags), for details see the [documentation](command-line.md).
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
  * This can be disabled if needed, see [documentation](configuration.md) for details.


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


# Older versions
Release notes were not maintained prior to v1.6.0, but you should be able to work them out from the Git history

---

[Home](Readme.md)
