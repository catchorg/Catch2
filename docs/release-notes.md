<a id="top"></a>

# Release notes

**Contents**<br>
[3.1.0](#310)<br>
[3.0.1](#301)<br>
[2.13.7](#2137)<br>
[2.13.6](#2136)<br>
[2.13.5](#2135)<br>
[2.13.4](#2134)<br>
[2.13.3](#2133)<br>
[2.13.2](#2132)<br>
[2.13.1](#2131)<br>
[2.13.0](#2130)<br>
[2.12.4](#2124)<br>
[2.12.3](#2123)<br>
[2.12.2](#2122)<br>
[2.12.1](#2121)<br>
[2.12.0](#2120)<br>
[2.11.3](#2113)<br>
[2.11.2](#2112)<br>
[2.11.1](#2111)<br>
[2.11.0](#2110)<br>
[2.10.2](#2102)<br>
[2.10.1](#2101)<br>
[2.10.0](#2100)<br>
[2.9.2](#292)<br>
[2.9.1](#291)<br>
[2.9.0](#290)<br>
[2.8.0](#280)<br>
[2.7.2](#272)<br>
[2.7.1](#271)<br>
[2.7.0](#270)<br>
[2.6.1](#261)<br>
[2.6.0](#260)<br>
[2.5.0](#250)<br>
[2.4.2](#242)<br>
[2.4.1](#241)<br>
[2.4.0](#240)<br>
[2.3.0](#230)<br>
[2.2.3](#223)<br>
[2.2.2](#222)<br>
[2.2.1](#221)<br>
[2.2.0](#220)<br>
[2.1.2](#212)<br>
[2.1.1](#211)<br>
[2.1.0](#210)<br>
[2.0.1](#201)<br>
[Older versions](#older-versions)<br>
[Even Older versions](#even-older-versions)<br>


## 3.1.0

### Improvements
* Improved suppression of `-Wparentheses` for older GCCs
  * Turns out that even GCC 9 does not properly handle `_Pragma`s in the C++ frontend.
* Added type constraints onto `random` generator (#2433)
  * These constraints copy what the standard says for the underlying `std::uniform_int_distribution`
* Suppressed -Wunused-variable from nvcc (#2306, #2427)
* Suppressed -Wunused-variable from MinGW (#2132)
* Added All/Any/NoneTrue range matchers (#2319)
  * These check that all/any/none of boolean values in a range are true.
* The JUnit reporter now normalizes classnames from C++ namespaces to Java-like namespaces (#2468)
  * This provides better support for other JUnit based tools.
* The Bazel support now understands `BAZEL_TEST` environment variable (#2459)
  * The `CATCH_CONFIG_BAZEL_SUPPORT` configuration option is also still supported.
* Returned support for compiling Catch2 with GCC 5 (#2448)
  * This required removing inherited constructors from Catch2's internals.
  * I recommend updating to a newer GCC anyway.
* `catch_discover_tests` now has a new options for setting library load path(s) when running the Catch2 binary (#2467)


### Fixes
* Fixed crash when listing listeners without any registered listeners (#2442)
* Fixed nvcc compilation error in constructor benchmarking helper (#2477)
* Catch2's CMakeList supports pre-3.12 CMake again (#2428)
  * The gain from requiring CMake 3.12 was very minor, but y'all should really update to newer CMake


### Miscellaneous
* Fixed SelfTest build on MinGW (#2447)
* The in-repo conan recipe exports the CMake helper (#2460)
* Added experimental CMake script to showcase using test case sharding together with CTest
  * Compared to `catch_discover_tests`, it supports very limited number of options and customization
* Added documentation page on best practices when running Catch2 tests
* Catch2 can be built as a dynamic library (#2397, #2398)
  * Note that Catch2 does not have visibility annotations, and you are responsible for ensuring correct visibility built into the resulting library.



## 3.0.1

**Catch2 now uses statically compiled library as its distribution model.
This also means that to get all of Catch2's functionality in a test file,
you have to include multiple headers.**

You probably want to look into the [migration docs](migrate-v2-to-v3.md#top),
which were written to help people coming from v2.x.x versions to the
v3 releases.


### FAQ

* Why is Catch2 moving to separate headers?
  * The short answer is future extensibility and scalability. The long answer is complex and can be found on my blog, but at the most basic level, it is that providing single-header distribution is at odds with providing variety of useful features. When Catch2 was distributed in a single header, adding a new Matcher would cause overhead for everyone, but was useful only to a subset of users. This meant that the barrier to entry for new Matchers/Generators/etc is high in single header model, but much smaller in the new model.
* Will Catch2 again distribute single-header version in the future?
  * No. But we do provide sqlite-style amalgamated distribution option. This means that you can download just 1 .cpp file and 1 header and place them next to your own sources. However, doing this has downsides similar to using the `catch_all.hpp` header.
* Why the big breaking change caused by replacing `catch.hpp` with `catch_all.hpp`?
  * The convenience header `catch_all.hpp` exists for two reasons. One of them is to provide a way for quick migration from Catch2, the second one is to provide a simple way to test things with Catch2. Using it for migration has one drawback in that it is **big**. This means that including it _will_ cause significant compile time drag, and so using it to migrate should be a conscious decision by the user, not something they can just stumble into unknowingly.


### (Potentially) Breaking changes
* **Catch2 now uses statically compiled library as its distribution model**
  * **Including `catch.hpp` no longer works**
* **Catch2 now uses C++14 as the minimum support language version**
* `ANON_TEST_CASE` has been removed, use `TEST_CASE` with no arguments instead (#1220)
* `--list*` commands no longer have non-zero return code (#1410)
* `--list-test-names-only` has been removed (#1190)
  * You should use verbosity-modifiers for `--list-tests` instead
* `--list*` commands are now piped through the reporters
  * The top-level reporter interface provides default implementation that works just as the old one
  * XmlReporter outputs a machine-parseable XML
* `TEST_CASE` description support has been removed
  * If the second argument has text outside tags, the text will be ignored.
* Hidden test cases are no longer included just because they don't match an exclusion tag
  * Previously, a `TEST_CASE("A", "[.foo]")` would be included by asking for `~[bar]`.
* `PredicateMatcher` is no longer type erased.
  * This means that the type of the provided predicate is part of the `PredicateMatcher`'s type
* `SectionInfo` no longer contains section description as a member (#1319)
  * You can still write `SECTION("ShortName", "Long and wordy description")`, but the description is thrown away
  * The description type now must be a `const char*` or be implicitly convertible to it
* The `[!hide]` tag has been removed.
  * Use `[.]` or `[.foo]` instead.
* Lvalues of composed matchers cannot be composed further
* Uses of `REGISTER_TEST_CASE` macro need to be followed by a semicolon
  * This does not change `TEST_CASE` and friends in any way
* `IStreamingReporter::IsMulti` member function was removed
  * This is _very_ unlikely to actually affect anyone, as it was default-implemented in the interface, and only used internally
* Various classes not designed for user-extension have been made final
  * `ListeningReporter` is now `final`
  * Concrete Matchers (e.g. `UnorderedEquals` vector matcher) are now `final`
  * All Generators are now `final`
* Matcher namespacing has been redone
  * Matcher types are no longer in deeply nested namespaces
  * Matcher factory functions are no longer brought into `Catch` namespace
  * This means that all public-facing matcher-related functionality is now in `Catch::Matchers` namespace
* Defining `CATCH_CONFIG_MAIN` will no longer create main in that TU.
  * Link with `libCatch2Main.a`, or the proper CMake/pkg-config target
  * If you want to write custom main, include `catch2/catch_session.hpp`
* `CATCH_CONFIG_EXTERNAL_INTERFACES` has been removed.
  * You should instead include the appropriate headers as needed.
* `CATCH_CONFIG_IMPL` has been removed.
  * The implementation is now compiled into a static library.
* Event Listener interface has changed
  * `TestEventListenerBase` was renamed to `EventListenerBase`
  * `EventListenerBase` now directly derives from `IStreamingReporter`, instead of deriving from `StreamingReporterBase`
* `GENERATE` decays its arguments (#2012, #2040)
  * This means that `str` in `auto str = GENERATE("aa", "bb", "cc");` is inferred to `char const*` rather than `const char[2]`.
* `--list-*` flags write their output to file specified by the `-o` flag
* Many changes to reporter interfaces
  * With the exception of the XmlReporter, the outputs of first party reporters should remain the same
  * New pair of events were added
  * One obsolete event was removed
  * The base class has been renamed
  * The built-in reporter class hierarchy has been redone
* Catch2 generates a random seed if one hasn't been specified by the user
* The short flag for `--list-tests`, `-l`, has been removed.
  * This is not a commonly used flag and does not need to use up valuable single-letter space.
* The short flag for `--list-tags`, `-t`, has been removed.
  * This is not a commonly used flag and does not need to use up valuable single-letter space.
* The `--colour` option has been replaced with `--colour-mode` option


### Improvements
* Matchers have been extended with the ability to use different signatures of `match` (#1307, #1553, #1554, #1843)
  * This includes having templated `match` member function
  * See the [rewritten Matchers documentation](matchers.md#top) for details
  * Catch2 currently provides _some_ generic matchers, but there should be more before final release of v3
    * `IsEmpty`, `SizeIs` which check that the range has specific properties
    * `Contains`, which checks whether a range contains a specific element
    * `AllMatch`, `AnyMatch`, `NoneMatch` range matchers, which apply matchers over a range of elements
* Significant compilation time improvements
  * including `catch_test_macros.hpp` is 80% cheaper than including `catch.hpp`
* Some runtime performance optimizations
  * In all tested cases the v3 branch was faster, so the table below shows the speedup of v3 to v2 at the same task
<a id="v3-runtime-optimization-table"></a>

|                   task                      |  debug build | release build |
|:------------------------------------------- | ------------:| -------------:|
| Run 1M `REQUIRE(true)`                      |  1.10 ± 0.01 |   1.02 ± 0.06 |
| Run 100 tests, 3^3 sections, 1 REQUIRE each |  1.27 ± 0.01 |   1.04 ± 0.01 |
| Run 3k tests, no names, no tags             |  1.29 ± 0.01 |   1.05 ± 0.01 |
| Run 3k tests, names, tags                   |  1.49 ± 0.01 |   1.22 ± 0.01 |
| Run 1 out of 3k tests no names, no tags     |  1.68 ± 0.02 |   1.19 ± 0.22 |
| Run 1 out of 3k tests, names, tags          |  1.79 ± 0.02 |   2.06 ± 0.23 |


* POSIX platforms use `gmtime_r`, rather than `gmtime` when constructing a date string (#2008, #2165)
* `--list-*` flags write their output to file specified by the `-o` flag (#2061, #2163)
* `Approx::operator()` is now properly `const`
* Catch2's internal helper variables no longer use reserved identifiers (#578)
* `--rng-seed` now accepts string `"random-device"` to generate random seed using `std::random_device`
* Catch2 now supports test sharding (#2257)
  * You can ask for the tests to be split into N groups and only run one of them.
  * This greatly simplifies parallelization of tests in a binary through external runner.
* The embedded CLI parser now supports repeatedly callable lambdas
  * A lambda-based option parser can opt into being repeatedly specifiable.
* Added `STATIC_CHECK` macro, similar to `STATIC_REQUIRE` (#2318)
  * When deferred tu runtime, it behaves like `CHECK`, and not like `REQUIRE`.
* You can have multiple tests with the same name, as long as other parts of the test identity differ (#1915, #1999, #2175)
  * Test identity includes test's name, test's tags and and test's class name if applicable.
* Added new warning, `UnmatchedTestSpec`, to error on test specs with no matching tests
* The `-w`, `--warn` warning flags can now be provided multiple times to enable multiple warnings
* The case-insensitive handling of tags is now more reliable and takes up less memory
* Test case and assertion counting can no longer reasonably overflow on 32 bit systems
  * The count is now kept in `uint64_t` on all platforms, instead of using `size_t` type.
* The `-o`, `--out` output destination specifiers recognize `-` as stdout
  * You have to provide it as `--out=-` to avoid CLI error about missing option
  * The new reporter specification also recognizes `-` as stdout
* Multiple reporters can now run at the same time and write to different files (#1712, #2183)
  * To support this, the `-r`, `--reporter` flag now also accepts optional output destination
  * For full overview of the semantics of using multiple reporters, look into the reporter documentation
  * To enable the new syntax, reporter names can no longer contain `::`.
* Console colour support has been rewritten and significantly improved
  * The colour implementation based on ANSI colour codes is always available
  * Colour implementations respect their associated stream
    * previously e.g. Win32 impl would change console colour even if Catch2 was writing to a file
  * The colour API is resilient against changing evaluation order of expressions
  * The associated CLI flag and compile-time configuration options have changed
    * For details see the docs for command-line and compile-time Catch2 configuration
* Added a support for Bazel integration with `XML_OUTPUT_FILE` env var (#2399)
  * This has to be enabled during compilation.
* Added `--skip-benchmarks` flag to run tests without any `BENCHMARK`s (#2392, #2408)
* Added option to list all listeners in the binary via `--list-listeners`


### Fixes
* The `INFO` macro no longer contains superfluous semicolon (#1456)
* The `--list*` family of command line flags now return 0 on success (#1410, #1146)
* Various ways of failing a benchmark are now counted and reporter properly
* The ULP matcher now handles comparing numbers with different signs properly (#2152)
* Universal ADL-found operators should no longer break decomposition (#2121)
* Reporter selection is properly case-insensitive
  * Previously it forced lower cased name, which would fail for reporters with upper case characters in name
* The cumulative reporter base stores benchmark results alongside assertion results
* Catch2's SE handling should no longer interferes with ASan on Windows (#2334)
* Fixed Windows console colour handling for tests that redirect stdout (#2345)
* Fixed issue with the `random` generators returning the same value over and over again


### Other changes
* `CATCH_CONFIG_DISABLE_MATCHERS` no longer exists.
  * If you do not want to use Matchers in a TU, do not include their header.
* `CATCH_CONFIG_ENABLE_CHRONO_STRINGMAKER` no longer exists.
  * `StringMaker` specializations for `<chrono>` are always provided
* Catch2's CMake now provides 2 targets, `Catch2` and `Catch2WithMain`.
  * `Catch2` is the statically compiled implementation by itself
  * `Catch2WithMain` also links in the default main
* Catch2's pkg-config integration also provides 2 packages
  * `catch2` is the statically compiled implementation by itself
  * `catch2-with-main` also links in the default main
* Passing invalid test specifications passed to Catch2 are now reported before tests are run, and are a hard error.
* Running 0 tests (e.g. due to empty binary, or test spec not matching anything) returns non-0 exit code
  * Flag `--allow-running-no-tests` overrides this behaviour.
  * `NoTests` warning has been removed because it is fully subsumed by this change.
* Catch2's compile-time configuration options (`CATCH_CONFIG_FOO`) can be set through CMake options of the same name
  * They use the same semantics as C++ defines, including the `CATCH_CONFIG_NO_FOO` overrides,
    * `-DCATCH_CONFIG_DEFAULT_REPORTER=compact` changes default reporter to "compact"
    * `-DCATCH_CONFIG_NO_ANDROID_LOGWRITE=ON` forces android logwrite to off
    * `-DCATCH_CONFIG_ANDROID_LOGWRITE=OFF` does nothing (the define will not exist)



## 2.13.7

### Fixes
* Added missing `<iterator>` include in benchmarking. (#2231)
* Fixed noexcept build with benchmarking enabled (#2235)
* Fixed build for compilers with C++17 support but without C++17 library support (#2195)
* JUnit only uses 3 decimal places when reporting durations (#2221)
* `!mayfail` tagged tests are now marked as `skipped` in JUnit reporter output (#2116)


## 2.13.6

### Fixes
* Disabling all signal handlers no longer breaks compilation  (#2212, #2213)

### Miscellaneous
* `catch_discover_tests` should handle escaped semicolon (`;`) better (#2214, #2215)


## 2.13.5

### Improvements
* Detection of MAC and IPHONE platforms has been improved (#2140, #2157)
* Added workaround for bug in XLC 16.1.0.1 (#2155)
* Add detection for LCC when it is masquerading as GCC (#2199)
* Modified posix signal handling so it supports newer libcs (#2178)
  * `MINSIGSTKSZ` was no longer usable in constexpr context.

### Fixes
* Fixed compilation of benchmarking when `min` and `max` macros are defined (#2159)
  * Including `windows.h` without `NOMINMAX` remains a really bad idea, don't do it

### Miscellaneous
* The check whether Catch2 is being built as a subproject is now more reliable (#2202, #2204)
  * The problem was that if the variable name used internally was defined the project including Catch2 as subproject, it would not be properly overwritten for Catch2's CMake.


## 2.13.4

### Improvements
* Improved the hashing algorithm used for shuffling test cases (#2070)
  * `TEST_CASE`s that differ only in the last character should be properly shuffled
  * Note that this means that v2.13.4 gives you a different order of test cases than 2.13.3, even given the same seed.

### Miscellaneous
* Deprecated `ParseAndAddCatchTests` CMake integration (#2092)
  * It is impossible to implement it properly for all the different test case variants Catch2 provides, and there are better options provided.
  * Use `catch_discover_tests` instead, which uses runtime information about available tests.
* Fixed bug in `catch_discover_tests` that would cause it to fail when used in specific project structures (#2119)
* Added Bazel build file
* Added an experimental static library target to CMake


## 2.13.3

### Fixes
* Fixed possible infinite loop when combining generators with section filter (`-c` option) (#2025)

### Miscellaneous
* Fixed `ParseAndAddCatchTests` not finding `TEST_CASE`s without tags (#2055, #2056)
* `ParseAndAddCatchTests` supports `CMP0110` policy for changing behaviour of `add_test` (#2057)
  * This was the shortlived change in CMake 3.18.0 that temporarily broke `ParseAndAddCatchTests`


## 2.13.2

### Improvements
* Implemented workaround for AppleClang shadowing bug (#2030)
* Implemented workaround for NVCC ICE (#2005, #2027)

### Fixes
* Fixed detection of `std::uncaught_exceptions` support under non-msvc platforms (#2021)
* Fixed the experimental stdout/stderr capture under Windows (#2013)

### Miscellaneous
* `catch_discover_tests` has been improved significantly (#2023, #2039)
  * You can now specify which reporter should be used
  * You can now modify where the output will be written
  * `WORKING_DIRECTORY` setting is respected
* `ParseAndAddCatchTests` now supports `TEMPLATE_TEST_CASE` macros (#2031)
* Various documentation fixes and improvements (#2022, #2028, #2034)


## 2.13.1

### Improvements
* `ParseAndAddCatchTests` handles CMake v3.18.0 correctly (#1984)
* Improved autodetection of `std::byte` (#1992)
* Simplified implementation of templated test cases (#2007)
  * This should have a tiny positive effect on its compilation throughput

### Fixes
* Automatic stringification of ranges handles sentinel ranges properly (#2004)


## 2.13.0

### Improvements
* `GENERATE` can now follow a `SECTION` at the same level of nesting (#1938)
  * The `SECTION`(s) before the `GENERATE` will not be run multiple times, the following ones will.
* Added `-D`/`--min-duration` command line flag (#1910)
  * If a test takes longer to finish than the provided value, its name and duration will be printed.
  * This flag is overriden by setting `-d`/`--duration`.

### Fixes
* `TAPReporter` no longer skips successful assertions (#1983)


## 2.12.4

### Improvements
* Added support for MacOS on ARM (#1971)


## 2.12.3

### Fixes
* `GENERATE` nested in a for loop no longer creates multiple generators (#1913)
* Fixed copy paste error breaking `TEMPLATE_TEST_CASE_SIG` for 6 or more arguments (#1954)
* Fixed potential UB when handling non-ASCII characters in CLI args (#1943)

### Improvements
* There can be multiple calls to `GENERATE` on a single line
* Improved `fno-except` support for platforms that do not provide shims for exception-related std functions (#1950)
  * E.g. the Green Hills C++ compiler
* XmlReporter now also reports test-case-level statistics (#1958)
  * This is done via a new element, `OverallResultsCases`

### Miscellaneous
* Added `.clang-format` file to the repo (#1182, #1920)
* Rewrote contributing docs
  * They should explain the different levels of testing and so on much better


## 2.12.2

### Fixes
* Fixed compilation failure if `is_range` ADL found deleted function (#1929)
* Fixed potential UB in `CAPTURE` if the expression contained non-ASCII characters (#1925)

### Improvements
* `std::result_of` is not used if `std::invoke_result` is available (#1934)
* JUnit reporter writes out `status` attribute for tests (#1899)
* Suppresed clang-tidy's `hicpp-vararg` warning (#1921)
  * Catch2 was already suppressing the `cppcoreguidelines-pro-type-vararg` alias of the warning


## 2.12.1

### Fixes
* Vector matchers now support initializer list literals better

### Improvements
* Added support for `^` (bitwise xor) to `CHECK` and `REQUIRE`



## 2.12.0

### Improvements
* Running tests in random order (`--order rand`) has been reworked significantly (#1908)
  * Given same seed, all platforms now produce the same order
  * Given same seed, the relative order of tests does not change if you select only a subset of them
* Vector matchers support custom allocators (#1909)
* `|` and `&` (bitwise or and bitwise and) are now supported in `CHECK` and `REQUIRE`
  * The resulting type must be convertible to `bool`

### Fixes
* Fixed computation of benchmarking column widths in ConsoleReporter (#1885, #1886)
* Suppressed clang-tidy's `cppcoreguidelines-pro-type-vararg` in assertions (#1901)
  * It was a false positive trigered by the new warning support workaround
* Fixed bug in test specification parser handling of OR'd patterns using escaping (#1905)

### Miscellaneous
* Worked around IBM XL's codegen bug (#1907)
  * It would emit code for _destructors_ of temporaries in an unevaluated context
* Improved detection of stdlib's support for `std::uncaught_exceptions` (#1911)



## 2.11.3

### Fixes
* Fixed compilation error caused by lambdas in assertions under MSVC


## 2.11.2

### Improvements
* GCC and Clang now issue warnings for suspicious code in assertions (#1880)
  * E.g. `REQUIRE( int != unsigned int )` will now issue mixed signedness comparison warning
  * This has always worked on MSVC, but it now also works for GCC and current Clang versions
* Colorization of "Test filters" output should be more robust now
* `--wait-for-keypress` now also accepts `never` as an option (#1866)
* Reporters no longer round-off nanoseconds when reporting benchmarking results (#1876)
* Catch2's debug break now supports iOS while using Thumb instruction set (#1862)
* It is now possible to customize benchmark's warm-up time when running the test binary (#1844)
  * `--benchmark-warmup-time {ms}`
* User can now specify how Catch2 should break into debugger (#1846)

### Fixes
* Fixes missing `<random>` include in benchmarking (#1831)
* Fixed missing `<iterator>` include in benchmarking (#1874)
* Hidden test cases are now also tagged with `[!hide]` as per documentation (#1847)
* Detection of whether libc provides `std::nextafter` has been improved (#1854)
* Detection of `wmain` no longer incorrectly looks for `WIN32` macro (#1849)
  * Now it just detects Windows platform
* Composing already-composed matchers no longer modifies the partially-composed matcher expression
  * This bug has been present for the last ~2 years and nobody reported it



## 2.11.1

### Improvements
* Breaking into debugger is supported on iOS (#1817)
* `google-build-using-namespace` clang-tidy warning is suppressed (#1799)

### Fixes
* Clang on Windows is no longer assumed to implement MSVC's traditional preprocessor (#1806)
* `ObjectStorage` now behaves properly in `const` contexts (#1820)
* `GENERATE_COPY(a, b)` now compiles properly (#1809, #1815)
* Some more cleanups in the benchmarking support

## For more detailed released note

visit: [Released Note](https://github.com/catchorg/Catch2/blob/devel/docs/release-note-in-detail.md)