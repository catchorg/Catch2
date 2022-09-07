<a id="top"></a>
# Known limitations

Over time, some limitations of Catch2 emerged. Some of these are due
to implementation details that cannot be easily changed, some of these
are due to lack of development resources on our part, and some of these
are due to plain old 3rd party bugs.


## Implementation limits
### Sections nested in loops

If you are using `SECTION`s inside loops, you have to create them with
different name per loop's iteration. The recommended way to do so is to
incorporate the loop's counter into section's name, like so:

```cpp
TEST_CASE( "Looped section" ) {
    for (char i = '0'; i < '5'; ++i) {
        SECTION(std::string("Looped section ") + i) {
            SUCCEED( "Everything is OK" );
        }
    }
}
```

or with a `DYNAMIC_SECTION` macro (that was made for exactly this purpose):

```cpp
TEST_CASE( "Looped section" ) {
    for (char i = '0'; i < '5'; ++i) {
        DYNAMIC_SECTION( "Looped section " << i) {
            SUCCEED( "Everything is OK" );
        }
    }
}
```

### Tests might be run again if last section fails

If the last section in a test fails, it might be run again. This is because
Catch2 discovers `SECTION`s dynamically, as they are about to run, and
if the last section in test case is aborted during execution (e.g. via
the `REQUIRE` family of macros), Catch2 does not know that there are no
more sections in that test case and must run the test case again.


### MinGW/CygWin compilation (linking) is extremely slow

Compiling Catch2 with MinGW can be exceedingly slow, especially during
the linking step. As far as we can tell, this is caused by deficiencies
in its default linker. If you can tell MinGW to instead use lld, via
`-fuse-ld=lld`, the link time should drop down to reasonable length
again.


## Features
This section outlines some missing features, what is their status and their possible workarounds.

### Thread safe assertions
Catch2's assertion macros are not thread safe. This does not mean that
you cannot use threads inside Catch's test, but that only single thread
can interact with Catch's assertions and other macros.

This means that this is ok
```cpp
    std::vector<std::thread> threads;
    std::atomic<int> cnt{ 0 };
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&]() {
            ++cnt; ++cnt; ++cnt; ++cnt;
        });
    }
    for (auto& t : threads) { t.join(); }
    REQUIRE(cnt == 16);
```
because only one thread passes the `REQUIRE` macro and this is not
```cpp
    std::vector<std::thread> threads;
    std::atomic<int> cnt{ 0 };
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&]() {
            ++cnt; ++cnt; ++cnt; ++cnt;
            CHECK(cnt == 16);
        });
    }
    for (auto& t : threads) { t.join(); }
    REQUIRE(cnt == 16);
```

Because C++11 provides the necessary tools to do this, we are planning
to remove this limitation in the future.

### Process isolation in a test
Catch does not support running tests in isolated (forked) processes. While this might in the future, the fact that Windows does not support forking and only allows full-on process creation and the desire to keep code as similar as possible across platforms, mean that this is likely to take significant development time, that is not currently available.


### Running multiple tests in parallel

Catch2 keeps test execution in one process strictly serial, and there
are no plans to change this. If you find yourself with a test suite
that takes too long to run and you want to make it parallel, you have
to run multiple processes side by side.

There are 2 basic ways to do that,
* you can split your tests into multiple binaries, and run those binaries
  in parallel
* you can run the same test binary multiple times, but run a different
  subset of the tests in each process

There are multiple ways to achieve the latter, the easiest way is to use
[test sharding](command-line.md#test-sharding).


## 3rd party bugs

This section outlines known bugs in 3rd party components (this means compilers, standard libraries, standard runtimes).


### Visual Studio 2017 -- raw string literal in assert fails to compile

There is a known bug in Visual Studio 2017 (VC 15), that causes compilation
error when preprocessor attempts to stringize a raw string literal
(`#` preprocessor directive is applied to it). This snippet is sufficient
to trigger the compilation error:

```cpp
#include <catch2/catch_test_macros.hpp>

TEST_CASE("test") {
    CHECK(std::string(R"("\)") == "\"\\");
}
```

Catch2 provides a workaround, by letting the user disable stringification
of the original expression by defining `CATCH_CONFIG_DISABLE_STRINGIFICATION`,
like so:
```cpp
#define CATCH_CONFIG_DISABLE_STRINGIFICATION
#include <catch2/catch_test_macros.hpp>

TEST_CASE("test") {
    CHECK(std::string(R"("\)") == "\"\\");
}
```

_Do note that this changes the output:_
```
catchwork\test1.cpp(6):
PASSED:
  CHECK( Disabled by CATCH_CONFIG_DISABLE_STRINGIFICATION )
with expansion:
  ""\" == ""\"
```


### Clang/G++ -- skipping leaf sections after an exception
Some versions of `libc++` and `libstdc++` (or their runtimes) have a bug with `std::uncaught_exception()` getting stuck returning `true` after rethrow, even if there are no active exceptions. One such case is this snippet, which skipped the sections "a" and "b", when compiled against `libcxxrt` from master
```cpp
#include <catch2/catch_test_macros.hpp>

TEST_CASE("a") {
    CHECK_THROWS(throw 3);
}

TEST_CASE("b") {
    int i = 0;
    SECTION("a") { i = 1; }
    SECTION("b") { i = 2; }
    CHECK(i > 0);
}
```

If you are seeing a problem like this, i.e. weird test paths that trigger only under Clang with `libc++`, or only under very specific version of `libstdc++`, it is very likely you are seeing this. The only known workaround is to use a fixed version of your standard library.


### libstdc++, `_GLIBCXX_DEBUG` macro and random ordering of tests

Running a Catch2 binary compiled against libstdc++ with `_GLIBCXX_DEBUG`
macro defined with `--order rand` will cause a debug check to trigger and
abort the run due to self-assignment.
[This is a known bug inside libstdc++](https://stackoverflow.com/questions/22915325/avoiding-self-assignment-in-stdshuffle/23691322)

Workaround: Don't use `--order rand` when compiling against debug-enabled
libstdc++.
