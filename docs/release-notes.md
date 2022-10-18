<a id="top"></a>

# Release notes
**Contents**<br>
[3.1.1](#311)<br>
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



## 3.1.1

### Improvements
* Added `Catch::getSeed` function that user code can call to retrieve current rng-seed
* Better detection of compiler support for `-ffile-prefix-map` (#2517)
* Catch2's shared libraries now have `SOVERSION` set (#2516)
* `catch2/catch_all.hpp` convenience header no longer transitively includes `windows.h` (#2432, #2526)


### Fixes
* Fixed compilation on Universal Windows Platform
* Fixed compilation on VxWorks (#2515)
* Fixed compilation on Cygwin (#2540)
* Remove unused variable in reporter registration (#2538)
* Fixed some symbol visibility issues with dynamic library on Windows (#2527)
* Suppressed `-Wuseless-cast` warnings in `REQUIRE_THROWS*` macros (#2520, #2521)
  * This was triggered when the potentially throwing expression evaluates to `void`
* Fixed "warning: storage class is not first" with `nvc++` (#2533)
* Fixed handling of `DL_PATHS` argument to `catch_discover_tests` on MacOS (#2483)
* Suppressed `*-avoid-c-arrays` clang-tidy warning in `TEMPLATE_TEST_CASE` (#2095, #2536)


### Miscellaneous
* Fixed CMake install step for Catch2 build as dynamic library (#2485)
* Raised minimum CMake version to 3.10 (#2523)
  * Expect the minimum CMake version to increase once more in next few releases.
* Whole bunch of doc updates and fixes
  * #1444, #2497, #2547, #2549, and more
* Added support for building Catch2 with Meson (#2530, #2539)



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



## 2.11.0

### Improvements
* JUnit reporter output now contains more details in case of failure (#1347, #1719)
* Added SonarQube Test Data reporter (#1738)
  * It is in a separate header, just like the TAP, Automake, and TeamCity reporters
* `range` generator now allows floating point numbers (#1776)
* Reworked part of internals to increase throughput


### Fixes
* The single header version should contain full benchmarking support (#1800)
* `[.foo]` is now properly parsed as `[.][foo]` when used on the command line (#1798)
* Fixed compilation of benchmarking on platforms where `steady_clock::period` is not `std::nano` (#1794)



## 2.10.2

### Improvements
* Catch2 will now compile on platform where `INFINITY` is double (#1782)


### Fixes
* Warning suppressed during listener registration will no longer leak



## 2.10.1

### Improvements
* Catch2 now guards itself against `min` and `max` macros from `windows.h` (#1772)
* Templated tests will now compile with ICC (#1748)
* `WithinULP` matcher now uses scientific notation for stringification (#1760)


### Fixes
* Templated tests no longer trigger `-Wunused-templates` (#1762)
* Suppressed clang-analyzer false positive in context getter (#1230, #1735)


### Miscellaneous
* CMake no longer prohibits in-tree build when Catch2 is used as a subproject (#1773, #1774)



## 2.10.0

### Fixes
* `TEMPLATE_LIST_TEST_CASE` now properly handles non-copyable and non-movable types (#1729)
* Fixed compilation error on Solaris caused by a system header defining macro `TT` (#1722, #1723)
* `REGISTER_ENUM` will now fail at compilation time if the registered enum is too large
* Removed use of `std::is_same_v` in C++17 mode (#1757)
* Fixed parsing of escaped special characters when reading test specs from a file (#1767, #1769)


### Improvements
* Trailing and leading whitespace in test/section specs are now ignored.
* Writing to Android debug log now uses `__android_log_write` instead of `__android_log_print`
* Android logging support can now be turned on/off at compile time (#1743)
  * The toggle is `CATCH_CONFIG_ANDROID_LOGWRITE`
* Added a generator that returns elements of a range
  * Use via `from_range(from, to)` or `from_range(container)`
* Added support for CRTs that do not provide `std::nextafter` (#1739)
  * They must still provide global `nextafter{f,l,}`
  * Enabled via `CATCH_CONFIG_GLOBAL_NEXTAFTER`
* Special cased `Approx(inf)` not to match non-infinite values
  * Very strictly speaking this might be a breaking change, but it should match user expectations better
* The output of benchmarking through the Console reporter when `--benchmark-no-analysis` is set is now much simpler (#1768)
* Added a matcher that can be used for checking an exceptions message (#1649, #1728)
  * The matcher helper function is called `Message`
  * The exception must publicly derive from `std::exception`
  * The matching is done exactly, including case and whitespace
* Added a matcher that can be used for checking relative equality of floating point numbers (#1746)
  * Unlike `Approx`, it considers both sides when determining the allowed margin
  * Special cases `NaN` and `INFINITY` to match user expectations
  * The matcher helper function is called `WithinRel`
* The ULP matcher now allows for any possible distance between the two numbers
* The random number generators now use Catch-global instance of RNG (#1734, #1736)
  * This means that nested random number generators actually generate different numbers


### Miscellaneous
* In-repo PNGs have been optimized to lower overhead of using Catch2 via git clone
* Catch2 now uses its own implementation of the URBG concept
  * In the future we also plan to use our own implementation of the distributions from `<random>` to provide cross-platform repeatability of random results



## 2.9.2

### Fixes
* `ChunkGenerator` can now be used with chunks of size 0 (#1671)
* Nested subsections are now run properly when specific section is run via the `-c` argument (#1670, #1673)
* Catch2 now consistently uses `_WIN32` to detect Windows platform (#1676)
* `TEMPLATE_LIST_TEST_CASE` now support non-default constructible type lists (#1697)
* Fixed a crash in the XMLReporter when a benchmark throws exception during warmup (#1706)
* Fixed a possible infinite loop in CompactReporter (#1715)
* Fixed `-w NoTests` returning 0 even when no tests were matched (#1449, #1683, #1684)
* Fixed matcher compilation under Obj-C++ (#1661)

### Improvements
* `RepeatGenerator` and `FixedValuesGenerator` now fail to compile when used with `bool` (#1692)
  * Previously they would fail at runtime.
* Catch2 now supports Android's debug logging for its debug output (#1710)
* Catch2 now detects and configures itself for the RTX platform (#1693)
  * You still need to pass `--benchmark-no-analysis` if you are using benchmarking under RTX
* Removed a "storage class is not first" warning when compiling Catch2 with PGI compiler (#1717)

### Miscellaneous
* Documentation now contains indication when a specific feature was introduced (#1695)
  * These start with Catch2 v2.3.0, (a bit over a year ago).
  * `docs/contributing.md` has been updated to provide contributors guidance on how to add these to newly written documentation
* Various other documentation improvements
  * ToC fixes
  * Documented `--order` and `--rng-seed` command line options
  * Benchmarking documentation now clearly states that it requires opt-in
  * Documented `CATCH_CONFIG_CPP17_OPTIONAL` and `CATCH_CONFIG_CPP17_BYTE` macros
  * Properly documented built-in vector matchers
  * Improved `*_THROWS_MATCHES` documentation a bit
* CMake config file is now arch-independent even if `CMAKE_SIZEOF_VOID_P` is in CMake cache (#1660)
* `CatchAddTests` now properly escapes `[` and `]` in test names (#1634, #1698)
* Reverted `CatchAddTests` adding tags as CTest labels (#1658)
  * The script broke when test names were too long
  * Overwriting `LABELS` caused trouble for users who set them manually
  * CMake does not let users append to `LABELS` if the test name has spaces


## 2.9.1

### Fixes
* Fix benchmarking compilation failure in files without `CATCH_CONFIG_EXTERNAL_INTERFACES` (or implementation)


## 2.9.0

### Improvements
* The experimental benchmarking support has been replaced by integrating Nonius code (#1616)
  * This provides a much more featurefull micro-benchmarking support.
  * Due to the compilation cost, it is disabled by default. See the documentation for details.
  * As far as backwards compatibility is concerned, this feature is still considered experimental in that we might change the interface based on user feedback.
* `WithinULP` matcher now shows the acceptable range (#1581)
* Template test cases now support type lists (#1627)


## 2.8.0

### Improvements
* Templated test cases no longer check whether the provided types are unique (#1628)
  * This allows you to e.g. test over `uint32_t`, `uint64_t`, and `size_t` without compilation failing
* The precision of floating point stringification can be modified by user (#1612, #1614)
* We now provide `REGISTER_ENUM` convenience macro for generating `StringMaker` specializations for enums
  * See the "String conversion" documentation for details
* Added new set of macros for template test cases that enables the use of NTTPs (#1531, #1609)
  * See "Test cases and sections" documentation for details

### Fixes
* `UNSCOPED_INFO` macro now has a prefixed/disabled/prefixed+disabled versions (#1611)
* Reporting errors at startup should no longer cause a segfault under certain circumstances (#1626)


### Miscellaneous
* CMake will now prevent you from attempting in-tree build (#1636, #1638)
  * Previously it would break with an obscure error message during the build step


## 2.7.2

### Improvements
* Added an approximate vector matcher (#1499)

### Fixes
* Filters will no longer be shown if there were none
* Fixed compilation error when using Homebrew GCC on OS X (#1588, #1589)
* Fixed the console reporter not showing messages that start with a newline (#1455, #1470)
* Modified JUnit reporter's output so that rng seed and filters are reported according to the JUnit schema (#1598)
* Fixed some obscure warnings and static analysis passes

### Miscellaneous
* Various improvements to `ParseAndAddCatchTests` (#1559, #1601)
  * When a target is parsed, it receives `ParseAndAddCatchTests_TESTS` property which summarizes found tests
  * Fixed problem with tests not being found if the `OptionalCatchTestLauncher` variables is used
  * Including the script will no longer forcefully modify `CMAKE_MINIMUM_REQUIRED_VERSION`
  * CMake object libraries are ignored when parsing to avoid needless warnings
* `CatchAddTests` now adds test's tags to their CTest labels (#1600)
* Added basic CPack support to our build

## 2.7.1

### Improvements
* Reporters now print out the filters applied to test cases (#1550, #1585)
* Added `GENERATE_COPY` and `GENERATE_REF` macros that can use variables inside the generator expression
  * Because of the significant danger of lifetime issues, the default `GENERATE` macro still does not allow variables
* The `map` generator helper now deduces the mapped return type (#1576)

### Fixes
* Fixed ObjC++ compilation (#1571)
* Fixed test tag parsing so that `[.foo]` is now parsed as `[.][foo]`.
* Suppressed warning caused by the Windows headers defining SE codes in different manners (#1575)

## 2.7.0

### Improvements
* `TEMPLATE_PRODUCT_TEST_CASE` now uses the resulting type in the name, instead of the serial number (#1544)
* Catch2's single header is now strictly ASCII (#1542)
* Added generator for random integral/floating point types
  * The types are inferred within the `random` helper
* Added back RangeGenerator (#1526)
  * RangeGenerator returns elements within a certain range
* Added ChunkGenerator generic transform (#1538)
  * A ChunkGenerator returns the elements from different generator in chunks of n elements
* Added `UNSCOPED_INFO` (#415, #983, #1522)
  * This is a variant of `INFO` that lives until next assertion/end of the test case.


### Fixes
* All calls to C stdlib functions are now `std::` qualified (#1541)
  * Code brought in from Clara was also updated.
* Running tests will no longer open the specified output file twice (#1545)
  * This would cause trouble when the file was not a file, but rather a named pipe
  * Fixes the CLion/Resharper integration with Catch
* Fixed `-Wunreachable-code` occurring with (old) ccache+cmake+clang combination (#1540)
* Fixed `-Wdefaulted-function-deleted` warning with Clang 8 (#1537)
* Catch2's type traits and helpers are now properly namespaced inside `Catch::` (#1548)
* Fixed std{out,err} redirection for failing test (#1514, #1525)
  * Somehow, this bug has been present for well over a year before it was reported


### Contrib
* `ParseAndAddCatchTests` now properly escapes commas in the test name



## 2.6.1

### Improvements
* The JUnit reporter now also reports random seed (#1520, #1521)

### Fixes
* The TAP reporter now formats comments with test name properly (#1529)
* `CATCH_REQUIRE_THROWS`'s internals were unified with `REQUIRE_THROWS` (#1536)
  * This fixes a potential `-Wunused-value` warning when used
* Fixed a potential segfault when using any of the `--list-*` options (#1533, #1534)


## 2.6.0

**With this release the data generator feature is now fully supported.**


### Improvements
* Added `TEMPLATE_PRODUCT_TEST_CASE` (#1454, #1468)
  * This allows you to easily test various type combinations, see documentation for details
* The error message for `&&` and `||` inside assertions has been improved (#1273, #1480)
* The error message for chained comparisons inside assertions has been improved (#1481)
* Added `StringMaker` specialization for `std::optional` (#1510)
* The generator interface has been redone once again (#1516)
  * It is no longer considered experimental and is fully supported
  * The new interface supports "Input" generators
  * The generator documentation has been fully updated
  * We also added 2 generator examples


### Fixes
* Fixed `-Wredundant-move` on newer Clang (#1474)
* Removed unreachable mentions `std::current_exception`, `std::rethrow_exception` in no-exceptions mode (#1462)
  * This should fix compilation with IAR
* Fixed missing `<type_traits>` include (#1494)
* Fixed various static analysis warnings
  * Unrestored stream state in `XmlWriter` (#1489)
  * Potential division by zero in `estimateClockResolution` (#1490)
  * Uninitialized member in `RunContext` (#1491)
  * `SourceLineInfo` move ops are now marked `noexcept`
  * `CATCH_BREAK_INTO_DEBUGGER` is now always a function
* Fix double run of a test case if user asks for a specific section (#1394, #1492)
* ANSI colour code output now respects `-o` flag and writes to the file as well (#1502)
* Fixed detection of `std::variant` support for compilers other than Clang (#1511)


### Contrib
* `ParseAndAddCatchTests` has learned how to use `DISABLED` CTest property (#1452)
* `ParseAndAddCatchTests` now works when there is a whitspace before the test name (#1493)


### Miscellaneous
* We added new issue templates for reporting issues on GitHub
* `contributing.md` has been updated to reflect the current test status (#1484)



## 2.5.0

### Improvements
* Added support for templated tests via `TEMPLATE_TEST_CASE` (#1437)


### Fixes
* Fixed compilation of `PredicateMatcher<const char*>` by removing partial specialization of `MatcherMethod<T*>`
* Listeners now implicitly support any verbosity (#1426)
* Fixed compilation with Embarcadero builder by introducing `Catch::isnan` polyfill (#1438)
* Fixed `CAPTURE` asserting for non-trivial captures (#1436, #1448)


### Miscellaneous
* We should now be providing first party Conan support via https://bintray.com/catchorg/Catch2 (#1443)
* Added new section "deprecations and planned changes" to the documentation
  * It contains summary of what is deprecated and might change with next major version
* From this release forward, the released headers should be pgp signed (#430)
  * KeyID `E29C 46F3 B8A7 5028 6079 3B7D ECC9 C20E 314B 2360`
  * or https://codingnest.com/files/horenmar-publickey.asc


## 2.4.2

### Improvements
* XmlReporter now also outputs the RNG seed that was used in a run (#1404)
* `Catch::Session::applyCommandLine` now also accepts `wchar_t` arguments.
  * However, Catch2 still does not support unicode.
* Added `STATIC_REQUIRE` macro (#1356, #1362)
* Catch2's singleton's are now cleaned up even if tests are run (#1411)
  * This is mostly useful as a FP prevention for users who define their own main.
* Specifying an invalid reporter via `-r` is now reported sooner (#1351, #1422)


### Fixes
* Stringification no longer assumes that `char` is signed (#1399, #1407)
  * This caused a `Wtautological-compare` warning.
* SFINAE for `operator<<` no longer sees different overload set than the actual insertion (#1403)


### Contrib
* `catch_discover_tests` correctly adds tests with comma in name (#1327, #1409)
* Added a new customization point in how the tests are launched to `catch_discover_tests`


## 2.4.1

### Improvements
* Added a StringMaker for `std::(w)string_view` (#1375, #1376)
* Added a StringMaker for `std::variant` (#1380)
  * This one is disabled by default to avoid increased compile-time drag
* Added detection for cygwin environment without `std::to_string` (#1396, #1397)

### Fixes
* `UnorderedEqualsMatcher` will no longer accept erroneously accept
vectors that share suffix, but are not permutation of the desired vector
* Abort after (`-x N`) can no longer be overshot by nested `REQUIRES` and
subsequently ignored (#1391, #1392)


## 2.4.0

**This release brings two new experimental features, generator support
and a `-fno-exceptions` support. Being experimental means that they
will not be subject to the usual stability guarantees provided by semver.**

### Improvements
* Various small runtime performance improvements
* `CAPTURE` macro is now variadic
* Added `AND_GIVEN` macro (#1360)
* Added experimental support for data generators
  * See [their documentation](generators.md) for details
* Added support for compiling and running Catch without exceptions
  * Doing so limits the functionality somewhat
  * Look [into the documentation](configuration.md#disablingexceptions) for details

### Fixes
* Suppressed `-Wnon-virtual-dtor` warnings in Matchers (#1357)
* Suppressed `-Wunreachable-code` warnings in floating point matchers (#1350)

### CMake
* It is now possible to override which Python is used to run Catch's tests (#1365)
* Catch now provides infrastructure for adding tests that check compile-time configuration
* Catch no longer tries to install itself when used as a subproject (#1373)
* Catch2ConfigVersion.cmake is now generated as arch-independent (#1368)
  * This means that installing Catch from 32-bit machine and copying it to 64-bit one works
  * This fixes conan installation of Catch


## 2.3.0

**This release changes the include paths provided by our CMake and
pkg-config integration. The proper include path for the single-header
when using one of the above is now `<catch2/catch.hpp>`. This change
also necessitated changes to paths inside the repository, so that the
single-header version is now at `single_include/catch2/catch.hpp`, rather
than `single_include/catch.hpp`.**



### Fixes
* Fixed Objective-C++ build
* `-Wunused-variable` suppression no longer leaks from Catch's header under Clang
* Implementation of the experimental new output capture can now be disabled (#1335)
  * This allows building Catch2 on platforms that do not provide things like `dup` or `tmpfile`.
* The JUnit and XML reporters will no longer skip over successful tests when running without `-s`  (#1264, #1267, #1310)
  * See improvements for more details

### Improvements
* pkg-config and CMake integration has been rewritten
  * If you use them, the new include path is `#include <catch2/catch.hpp>`
  * CMake installation now also installs scripts from `contrib/`
  * For details see the [new documentation](cmake-integration.md#top)
* Reporters now have a new customization point, `ReporterPreferences::shouldReportAllAssertions`
  * When this is set to `false` and the tests are run without `-s`, passing assertions are not sent to the reporter.
  * Defaults to `false`.
* Added `DYNAMIC_SECTION`, a section variant that constructs its name using stream
  * This means that you can do `DYNAMIC_SECTION("For X := " << x)`.


## 2.2.3

**To fix some of the bugs, some behavior had to change in potentially breaking manner.**
**This means that even though this is a patch release, it might not be a drop-in replacement.**

### Fixes
* Listeners are now called before reporter
  * This was always documented to be the case, now it actually works that way
* Catch's commandline will no longer accept multiple reporters
  * This was done because multiple reporters never worked properly and broke things in non-obvious ways
  * **This has potential to be a breaking change**
* MinGW is now detected as Windows platform w/o SEH support (#1257)
  * This means that Catch2 no longer tries to use POSIX signal handling when compiled with MinGW
* Fixed potential UB in parsing tags using non-ASCII characters (#1266)
  * Note that Catch2 still supports only ASCII test names/tags/etc
* `TEST_CASE_METHOD` can now be used on classnames containing commas (#1245)
  * You have to enclose the classname in extra set of parentheses
* Fixed insufficient alt stack size for POSIX signal handling (#1225)
* Fixed compilation error on Android due to missing `std::to_string` in C++11 mode (#1280)
* Fixed the order of user-provided `FALLBACK_STRINGIFIER` in stringification machinery (#1024)
  * It was intended to be replacement for built-in fallbacks, but it was used _after_ them.
  * **This has potential to be a breaking change**
* Fixed compilation error when a type has an `operator<<` with templated lhs (#1285, #1306)

### Improvements
* Added a new, experimental, output capture (#1243)
  * This capture can also redirect output written via C apis, e.g. `printf`
  * To opt-in, define `CATCH_CONFIG_EXPERIMENTAL_REDIRECT` in the implementation file
* Added a new fallback stringifier for classes derived from `std::exception`
  * Both `StringMaker` specialization and `operator<<` overload are given priority

### Miscellaneous
* `contrib/` now contains dbg scripts that skip over Catch's internals (#904, #1283)
  * `gdbinit` for gdb `lldbinit` for lldb
* `CatchAddTests.cmake` no longer strips whitespace from tests (#1265, #1281)
* Online documentation now describes `--use-colour` option (#1263)


## 2.2.2

### Fixes
* Fixed bug in `WithinAbs::match()` failing spuriously (#1228)
* Fixed clang-tidy diagnostic about virtual call in destructor (#1226)
* Reduced the number of GCC warnings suppression leaking out of the header (#1090, #1091)
  * Only `-Wparentheses` should be leaking now
* Added upper bound on the time benchmark timer calibration is allowed to take (#1237)
  * On platforms where `std::chrono::high_resolution_clock`'s resolution is low, the calibration would appear stuck
* Fixed compilation error when stringifying static arrays of `unsigned char`s (#1238)

### Improvements
* XML encoder now hex-encodes invalid UTF-8 sequences (#1207)
  * This affects xml and junit reporters
  * Some invalid UTF-8 parts are left as is, e.g. surrogate pairs. This is because certain extensions of UTF-8 allow them, such as WTF-8.
* CLR objects (`T^`) can now be stringified (#1216)
  * This affects code compiled as C++/CLI
* Added `PredicateMatcher`, a matcher that takes an arbitrary predicate function (#1236)
  * See [documentation for details](https://github.com/catchorg/Catch2/blob/devel/docs/matchers.md)

### Others
* Modified CMake-installed pkg-config to allow `#include <catch.hpp>`(#1239)
  * The plans to standardize on `#include <catch2/catch.hpp>` are still in effect


## 2.2.1

### Fixes
* Fixed compilation error when compiling Catch2 with `std=c++17` against libc++ (#1214)
  * Clara (Catch2's CLI parsing library) used `std::optional` without including it explicitly
* Fixed Catch2 return code always being 0 (#1215)
  * In the words of STL, "We feel superbad about letting this in"


## 2.2.0

### Fixes
* Hidden tests are not listed by default when listing tests (#1175)
  * This makes `catch_discover_tests` CMake script work better
* Fixed regression that meant `<windows.h>` could potentially not be included properly (#1197)
* Fixed installing `Catch2ConfigVersion.cmake` when Catch2 is a subproject.

### Improvements
* Added an option to warn (+ exit with error) when no tests were ran (#1158)
  * Use as `-w NoTests`
* Added provisional support for Emscripten (#1114)
* [Added a way to override the fallback stringifier](https://github.com/catchorg/Catch2/blob/devel/docs/configuration.md#fallback-stringifier) (#1024)
  * This allows project's own stringification machinery to be easily reused for Catch
* `Catch::Session::run()` now accepts `char const * const *`, allowing it to accept array of string literals (#1031, #1178)
  * The embedded version of Clara was bumped to v1.1.3
* Various minor performance improvements
* Added support for DJGPP DOS crosscompiler (#1206)


## 2.1.2

### Fixes
* Fixed compilation error with `-fno-rtti` (#1165)
* Fixed NoAssertion warnings
* `operator<<` is used before range-based stringification (#1172)
* Fixed `-Wpedantic` warnings (extra semicolons and binary literals) (#1173)


### Improvements
* Added `CATCH_VERSION_{MAJOR,MINOR,PATCH}` macros (#1131)
* Added `BrightYellow` colour for use in reporters (#979)
  * It is also used by ConsoleReporter for reconstructed expressions

### Other changes
* Catch is now exported as a CMake package and linkable target (#1170)

## 2.1.1

### Improvements
* Static arrays are now properly stringified like ranges across MSVC/GCC/Clang
* Embedded newer version of Clara -- v1.1.1
  * This should fix some warnings dragged in from Clara
* MSVC's CLR exceptions are supported


### Fixes
* Fixed compilation when comparison operators do not return bool (#1147)
* Fixed CLR exceptions blowing up the executable during translation (#1138)


### Other changes
* Many CMake changes
  * `NO_SELFTEST` option is deprecated, use `BUILD_TESTING` instead.
  * Catch specific CMake options were prefixed with `CATCH_` for namespacing purposes
  * Other changes to simplify Catch2's packaging



## 2.1.0

### Improvements
* Various performance improvements
  * On top of the performance regression fixes
* Experimental support for PCH was added (#1061)
* `CATCH_CONFIG_EXTERNAL_INTERFACES` now brings in declarations of Console, Compact, XML and JUnit reporters
* `MatcherBase` no longer has a pointless second template argument
* Reduced the number of warning suppressions that leak into user's code
  * Bugs in g++ 4.x and 5.x mean that some of them have to be left in


### Fixes
* Fixed performance regression from Catch classic
  * One of the performance improvement patches for Catch classic was not applied to Catch2
* Fixed platform detection for iOS (#1084)
* Fixed compilation when `g++` is used together with `libc++` (#1110)
* Fixed TeamCity reporter compilation with the single header version
  * To fix the underlying issue we will be versioning reporters in single_include folder per release
* The XML reporter will now report `WARN` messages even when not used with `-s`
* Fixed compilation when `VectorContains` matcher was combined using `&&` (#1092)
* Fixed test duration overflowing after 10 seconds (#1125, #1129)
* Fixed `std::uncaught_exception` deprecation warning (#1124)


### New features
* New Matchers
  * Regex matcher for strings, `Matches`.
  * Set-equal matcher for vectors, `UnorderedEquals`
  * Floating point matchers, `WithinAbs` and `WithinULP`.
* Stringification now attempts to decompose all containers (#606)
  * Containers are objects that respond to ADL `begin(T)` and `end(T)`.


### Other changes
* Reporters will now be versioned in the `single_include` folder to ensure their compatibility with the last released version




## 2.0.1

### Breaking changes
* Removed C++98 support
* Removed legacy reporter support
* Removed legacy generator support
  * Generator support will come back later, reworked
* Removed `Catch::toString` support
  * The new stringification machinery uses `Catch::StringMaker` specializations first and `operator<<` overloads second.
* Removed legacy `SCOPED_MSG` and `SCOPED_INFO` macros
* Removed `INTERNAL_CATCH_REGISTER_REPORTER`
  * `CATCH_REGISTER_REPORTER` should be used to register reporters
* Removed legacy `[hide]` tag
  * `[.]`, `[.foo]` and `[!hide]` are still supported
* Output into debugger is now colourized
* `*_THROWS_AS(expr, exception_type)` now unconditionally appends `const&` to the exception type.
* `CATCH_CONFIG_FAST_COMPILE` now affects the `CHECK_` family of assertions as well as `REQUIRE_` family of assertions
  * This is most noticeable in `CHECK(throws())`, which would previously report failure, properly stringify the exception and continue. Now it will report failure and stop executing current section.
* Removed deprecated matcher utility functions `Not`, `AllOf` and `AnyOf`.
  * They are superseded by operators `!`, `&&` and `||`, which are natural and do not have limited arity
* Removed support for non-const comparison operators
  * Non-const comparison operators are an abomination that should not exist
  * They were breaking support for comparing function to function pointer
* `std::pair` and `std::tuple` are no longer stringified by default
  * This is done to avoid dragging in `<tuple>` and `<utility>` headers in common path
  * Their stringification can be enabled per-file via new configuration macros
* `Approx` is subtly different and hopefully behaves more as users would expect
  * `Approx::scale` defaults to `0.0`
  * `Approx::epsilon` no longer applies to the larger of the two compared values, but only to the `Approx`'s value
  * `INFINITY == Approx(INFINITY)` returns true


### Improvements
* Reporters and Listeners can be defined in files different from the main file
  * The file has to define `CATCH_CONFIG_EXTERNAL_INTERFACES` before including catch.hpp.
* Errors that happen during set up before main are now caught and properly reported once main is entered
  * If you are providing your own main, you can access and use these as well.
* New assertion macros, *_THROWS_MATCHES(expr, exception_type, matcher) are provided
  * As the arguments suggest, these allow you to assert that an expression throws desired type of exception and pass the exception to a matcher.
* JUnit reporter no longer has significantly different output for test cases with and without sections
* Most assertions now support expressions containing commas (ie `REQUIRE(foo() == std::vector<int>{1, 2, 3});`)
* Catch now contains experimental micro benchmarking support
  * See `projects/SelfTest/Benchmark.tests.cpp` for examples
  * The support being experiment means that it can be changed without prior notice
* Catch uses new CLI parsing library (Clara)
  * Users can now easily add new command line options to the final executable
  * This also leads to some changes in `Catch::Session` interface
* All parts of matchers can be removed from a TU by defining `CATCH_CONFIG_DISABLE_MATCHERS`
  * This can be used to somewhat speed up compilation times
* An experimental implementation of `CATCH_CONFIG_DISABLE` has been added
  * Inspired by Doctest's `DOCTEST_CONFIG_DISABLE`
  * Useful for implementing tests in source files
    * ie for functions in anonymous namespaces
  * Removes all assertions
  * Prevents `TEST_CASE` registrations
  * Exception translators are not registered
  * Reporters are not registered
  * Listeners are not registered
* Reporters/Listeners are now notified of fatal errors
  * This means specific signals or structured exceptions
  * The Reporter/Listener interface provides default, empty, implementation to preserve backward compatibility
* Stringification of `std::chrono::duration` and `std::chrono::time_point` is now supported
  * Needs to be enabled by a per-file compile time configuration option
* Add `pkg-config` support to CMake install command


### Fixes
* Don't use console colour if running in XCode
* Explicit constructor in reporter base class
* Swept out `-Wweak-vtables`, `-Wexit-time-destructors`, `-Wglobal-constructors` warnings
* Compilation for Universal Windows Platform (UWP) is supported
  * SEH handling and colorized output are disabled when compiling for UWP
* Implemented a workaround for `std::uncaught_exception` issues in libcxxrt
  * These issues caused incorrect section traversals
  * The workaround is only partial, user's test can still trigger the issue by using `throw;` to rethrow an exception
* Suppressed C4061 warning under MSVC


### Internal changes
* The development version now uses .cpp files instead of header files containing implementation.
  * This makes partial rebuilds much faster during development
* The expression decomposition layer has been rewritten
* The evaluation layer has been rewritten
* New library (TextFlow) is used for formatting text to output


## Older versions

### 1.12.x

#### 1.12.2
##### Fixes
* Fixed missing <cassert> include

#### 1.12.1

##### Fixes
* Fixed deprecation warning in `ScopedMessage::~ScopedMessage`
* All uses of `min` or `max` identifiers are now wrapped in parentheses
  * This avoids problems when Windows headers define `min` and `max` macros

#### 1.12.0

##### Fixes
* Fixed compilation for strict C++98 mode (ie not gnu++98) and older compilers (#1103)
* `INFO` messages are included in the `xml` reporter output even without `-s` specified.


### 1.11.x

#### 1.11.0

##### Fixes
* The original expression in `REQUIRE_FALSE( expr )` is now reporter properly as `!( expr )` (#1051)
  * Previously the parentheses were missing and `x != y` would be expanded as `!x != x`
* `Approx::Margin` is now inclusive (#952)
  * Previously it was meant and documented as inclusive, but the check itself wasn't
  * This means that `REQUIRE( 0.25f == Approx( 0.0f ).margin( 0.25f ) )` passes, instead of fails
* `RandomNumberGenerator::result_type` is now unsigned (#1050)

##### Improvements
* `__JETBRAINS_IDE__` macro handling is now CLion version specific (#1017)
  * When CLion 2017.3 or newer is detected, `__COUNTER__` is used instead of
* TeamCity reporter now explicitly flushes output stream after each report (#1057)
  * On some platforms, output from redirected streams would show up only after the tests finished running
* `ParseAndAddCatchTests` now can add test files as dependency to CMake configuration
  * This means you do not have to manually rerun CMake configuration step to detect new tests

### 1.10.x

#### 1.10.0

##### Fixes
* Evaluation layer has been rewritten (backported from Catch 2)
  * The new layer is much simpler and fixes some issues (#981)
* Implemented workaround for VS 2017 raw string literal stringification bug (#995)
* Fixed interaction between `[!shouldfail]` and `[!mayfail]` tags and sections
  * Previously sections with failing assertions would be marked as failed, not failed-but-ok

##### Improvements
* Added [libidentify](https://github.com/janwilmans/LibIdentify) support
* Added "wait-for-keypress" option

### 1.9.x

#### 1.9.6

##### Improvements
* Catch's runtime overhead has been significantly decreased (#937, #939)
* Added `--list-extra-info` cli option (#934).
  * It lists all tests together with extra information, ie filename, line number and description.



#### 1.9.5

##### Fixes
* Truthy expressions are now reconstructed properly, not as booleans (#914)
* Various warnings are no longer erroneously suppressed in test files (files that include `catch.hpp`, but do not define `CATCH_CONFIG_MAIN` or `CATCH_CONFIG_RUNNER`) (#871)
* Catch no longer fails to link when main is compiled as C++, but linked against Objective-C (#855)
* Fixed incorrect gcc version detection when deciding to use `__COUNTER__` (#928)
  * Previously any GCC with minor version less than 3 would be incorrectly classified as not supporting `__COUNTER__`.
* Suppressed C4996 warning caused by upcoming updated to MSVC 2017, marking `std::uncaught_exception` as deprecated. (#927)

##### Improvements
* CMake integration script now incorporates debug messages and registers tests in an improved way (#911)
* Various documentation improvements



#### 1.9.4

##### Fixes
* `CATCH_FAIL` macro no longer causes compilation error without variadic macro support
* `INFO` messages are no longer cleared after being reported once

##### Improvements and minor changes
* Catch now uses `wmain` when compiled under Windows and `UNICODE` is defined.
  * Note that Catch still officially supports only ASCII

#### 1.9.3

##### Fixes
* Completed the fix for (lack of) uint64_t in earlier Visual Studios

#### 1.9.2

##### Improvements and minor changes
* All of `Approx`'s member functions now accept strong typedefs in C++11 mode (#888)
  * Previously `Approx::scale`, `Approx::epsilon`, `Approx::margin` and `Approx::operator()` didn't.


##### Fixes
* POSIX signals are now disabled by default under QNX (#889)
  * QNX does not support current enough (2001) POSIX specification
* JUnit no longer counts exceptions as failures if given test case is marked as ok to fail.
* `Catch::Option` should now have its storage properly aligned.
* Catch no longer attempts to define `uint64_t` on windows (#862)
  * This was causing trouble when compiled under Cygwin

##### Other
* Catch is now compiled under MSVC 2017 using `std:c++latest` (C++17 mode) in CI
* We now provide cmake script that autoregisters Catch tests into ctest.
  * See `contrib` folder.


#### 1.9.1

##### Fixes
* Unexpected exceptions are no longer ignored by default (#885, #887)


#### 1.9.0


##### Improvements and minor changes
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

##### Fixes:
* Clang's `-Wexit-time-destructors` should be suppressed for Catch's internals
* GCC's `-Wparentheses` is now suppressed for all TU's that include `catch.hpp`.
  * This is functionally a revert of changes made in 1.8.0, where we tried using `_Pragma` based suppression. This should have kept the suppression local to Catch's assertions, but bugs in GCC's handling of `_Pragma`s in C++ mode meant that it did not always work.
* You can now tell Catch to use C++11-based check when checking whether a type can be streamed to output.
  * This fixes cases when an unstreamable type has streamable private base (#877)
  * [Details can be found in documentation](configuration.md#catch_config_cpp11_stream_insertable_check)


##### Other notes:
* We have added VS 2017 to our CI
* Work on Catch 2 should start soon



### 1.8.x

#### 1.8.2


##### Improvements and minor changes
* TAP reporter now behaves as if `-s` was always set
  * This should be more consistent with the protocol desired behaviour.
* Compact reporter now obeys `-d yes` argument (#780)
  * The format is "XXX.123 s: <section-name>" (3 decimal places are always present).
  * Before it did not report the durations at all.
* XML reporter now behaves the same way as Console reporter in regards to `INFO`
  * This means it reports `INFO` messages on success, if output on success (`-s`) is enabled.
  * Previously it only reported `INFO` messages on failure.
* `CAPTURE(expr)` now stringifies `expr` in the same way assertion macros do (#639)
* Listeners are now finally [documented](event-listeners.md#top).
  * Listeners provide a way to hook into events generated by running your tests, including start and end of run, every test case, every section and every assertion.


##### Fixes:
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


##### Other notes:
* We have added VS 2013 and 2015 to our CI
* Catch Classic (1.x.x) now contains its own, forked, version of Clara (the argument parser).



#### 1.8.1

##### Fixes

Cygwin issue with `gettimeofday` - `#define` was not early enough

#### 1.8.0

##### New features/ minor changes

* Matchers have new, simpler (and documented) interface.
  * Catch provides string and vector matchers.
  * For details see [Matchers documentation](matchers.md#top).
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
  * For details see [documentation about integrating with build systems](build-systems.md#top).
*  XML reporter now reports filename as part of the `Section` and `TestCase` tags.
* `Approx` now supports an optional margin of absolute error
  * It has also received [new documentation](assertions.md#top).

##### Fixes
* Silenced C4312 ("conversion from int to 'ClassName *") warnings in the evaluate layer.
* Fixed C4512 ("assignment operator could not be generated") warnings under VS2013.
* Cygwin compatibility fixes
  * Signal handling is no longer compiled by default.
  * Usage of `gettimeofday` inside Catch should no longer cause compilation errors.
* Improved `-Wparentheses` suppression for gcc (#674)
  * When compiled with gcc 4.8 or newer, the suppression is localized to assertions only
  * Otherwise it is suppressed for the whole TU
* Fixed test spec parser issue (with escapes in multiple names)

##### Other
* Various documentation fixes and improvements


### 1.7.x

#### 1.7.2

##### Fixes and minor improvements
Xml:

(technically the first two are breaking changes but are also fixes and arguably break few if any people)
* C-escape control characters instead of XML encoding them (which requires XML 1.1)
* Revert XML output to XML 1.0
* Can provide stylesheet references by extending the XML reporter
* Added description and tags attributes to XML Reporter
* Tags are closed and the stream flushed more eagerly to avoid stdout interpolation


Other:
* `REQUIRE_THROWS_AS` now catches exception by `const&` and reports expected type
* In `SECTION`s the file/ line is now of the `SECTION`. not the `TEST_CASE`
* Added std:: qualification to some functions from C stdlib
* Removed use of RTTI (`dynamic_cast`) that had crept back in
* Silenced a few more warnings in different circumstances
* Travis improvements

#### 1.7.1

##### Fixes:
* Fixed inconsistency in defining `NOMINMAX` and `WIN32_LEAN_AND_MEAN` inside `catch.hpp`.
* Fixed SEH-related compilation error under older MinGW compilers, by making Windows SEH handling opt-in for compilers other than MSVC.
  * For specifics, look into the [documentation](configuration.md#top).
* Fixed compilation error under MinGW caused by improper compiler detection.
* Fixed XML reporter sometimes leaving an empty output file when a test ends with signal/structured exception.
* Fixed XML reporter not reporting captured stdout/stderr.
* Fixed possible infinite recursion in Windows SEH.
* Fixed possible compilation error caused by Catch's operator overloads being ambiguous in regards to user-defined templated operators.

#### 1.7.0

##### Features/ Changes:
* Catch now runs significantly faster for passing tests
  * Microbenchmark focused on Catch's overhead went from ~3.4s to ~0.7s.
  * Real world test using [JSON for Modern C++](https://github.com/nlohmann/json)'s test suite went from ~6m 25s to ~4m 14s.
* Catch can now run specific sections within test cases.
  * For now the support is only basic (no wildcards or tags), for details see the [documentation](command-line.md#top).
* Catch now supports SEH on Windows as well as signals on Linux.
  * After receiving a signal, Catch reports failing assertion and then passes the signal onto the previous handler.
* Approx can be used to compare values against strong typedefs (available in C++11 mode only).
  * Strong typedefs mean types that are explicitly convertible to double.
* CHECK macro no longer stops executing section if an exception happens.
* Certain characters (space, tab, etc) are now pretty printed.
  * This means that a `char c = ' '; REQUIRE(c == '\t');` would be printed as `' ' == '\t'`, instead of ` == 9`.

##### Fixes:
* Text formatting no longer attempts to access out-of-bounds characters under certain conditions.
* THROW family of assertions no longer trigger `-Wunused-value` on expressions containing explicit cast.
* Breaking into debugger under OS X works again and no longer required `DEBUG` to be defined.
* Compilation no longer breaks under certain compiler if a lambda is used inside assertion macro.

##### Other:
* Catch's CMakeLists now defines install command.
* Catch's CMakeLists now generates projects with warnings enabled.


### 1.6.x

#### 1.6.1

##### Features/ Changes:
* Catch now supports breaking into debugger on Linux

##### Fixes:
* Generators no longer leak memory (generators are still unsupported in general)
* JUnit reporter now reports UTC timestamps, instead of "tbd"
* `CHECK_THAT` macro is now properly defined as `CATCH_CHECK_THAT` when using `CATCH_` prefixed macros

##### Other:
* Types with overloaded `&&` operator are no longer evaluated twice when used in an assertion macro.
* The use of `__COUNTER__` is suppressed when Catch is parsed by CLion
  * This change is not active when compiling a binary
* Approval tests can now be run on Windows
* CMake will now warn if a file is present in the `include` folder but not is not enumerated as part of the project
* Catch now defines `NOMINMAX` and `WIN32_LEAN_AND_MEAN` before including `windows.h`
  * This can be disabled if needed, see [documentation](configuration.md#top) for details.


#### 1.6.0

##### Cmake/ projects:
* Moved CMakeLists.txt to root, made it friendlier for CLion and generating XCode and VS projects, and removed the manually maintained XCode and VS projects.

##### Features/ Changes:
* Approx now supports `>=` and `<=`
* Can now use `\` to escape chars in test names on command line
* Standardize C++11 feature toggles

##### Fixes:
* Blue shell colour
* Missing argument to `CATCH_CHECK_THROWS`
* Don't encode extended ASCII in XML
* use `std::shuffle` on more compilers (fixes deprecation warning/error)
* Use `__COUNTER__` more consistently (where available)

##### Other:
* Tweaks and changes to scripts - particularly for Approval test - to make them more portable


## Even Older versions
Release notes were not maintained prior to v1.6.0, but you should be able to work them out from the Git history

---

[Home](Readme.md#top)
