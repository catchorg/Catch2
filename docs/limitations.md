# Known limitations

Catch has some known limitations, that we are not planning to change. Some of these are caused by our desire to support C++98 compilers, some of these are caused by our desire to keep Catch crossplatform, some exist because their priority is seen as low compared to the development effort they would need and some other yet are compiler/runtime bugs.

## Features
This section outlines some missing features, what is their status and their possible workarounds.

### Thread safe assertions
Because threading support in standard C++98 is limited (well, non-existent), assertion macros in Catch are not thread safe. This does not mean that you cannot use threads inside Catch's test, but that only single thread can interact with Catch's assertions and other macros.

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


_This limitation is highly unlikely to be lifted before Catch 2 is released._

### Process isolation in a test
Catch does not support running tests in isolated (forked) processes. While this might in the future, the fact that Windows does not support forking and only allows full-on process creation and the desire to keep code as similar as possible across platforms, mean that this is likely to take significant development time, that is not currently available.

### Running multiple tests in parallel
Catch's test execution is strictly serial. If you find yourself with a test suite that takes too long to run and you want to make it parallel, there are 2 feasible solutions
 * You can split your tests into multiple binaries and then run these binaries in parallel.
 * You can have Catch list contained test cases and then run the same test binary multiple times in parallel, passing each instance list of test cases it should run.

Both of these solutions have their problems, but should let you wring parallelism out of your test suite.

## 3rd party bugs
This section outlines known bugs in 3rd party components (this means compilers, standard libraries, standard runtimes).

### Visual Studio 2017 -- raw string literal in assert fails to compile
There is a known bug in Visual Studio 2017 (VC 15), that causes compilation error when preprocessor attempts to stringize a raw string literal (`#` preprocessor is applied to it). This snippet is sufficient to trigger the compilation error:
```cpp
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("test") {
    CHECK(std::string(R"("\)") == "\"\\");
}
```

Catch provides a workaround, it is possible to disable stringification of original expressions by defining `CATCH_CONFIG_DISABLE_STRINGIFICATION`:
```cpp
#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_DISABLE_STRINGIFICATION
#include "catch.hpp"

TEST_CASE("test") {
    CHECK(std::string(R"("\)") == "\"\\");
}
```

_Do note that this changes the output somewhat_
```
catchwork\test1.cpp(6):
PASSED:
  CHECK( Disabled by CATCH_CONFIG_DISABLE_STRINGIFICATION )
with expansion:
  ""\" == ""\"
```


### Visual Studio 2013 -- do-while loop withing range based for fails to compile (C2059)
There is a known bug in Visual Studio 2013 (VC 12), that causes compilation error if range based for is followed by an assertion macro, without enclosing the block in braces. This snippet is sufficient to trigger the error
```cpp
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Syntax error with VC12") {
    for ( auto x : { 1 , 2, 3 } )
        REQUIRE( x < 3.14 );
}
```
An easy workaround is possible, use braces:
```cpp
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("No longer a syntax error with VC12") {
    for ( auto x : { 1 , 2, 3 } ) {
        REQUIRE( x < 3.14 );
    }
}
```

### Visual Studio 2003 -- Syntax error caused by improperly expanded `__LINE__` macro
Older version of Visual Studio can have trouble compiling Catch, not expanding the `__LINE__` macro properly when recompiling the test binary. This is caused by Edit and Continue being on.

A workaround is to turn off Edit and Continue when compiling the test binary.

### Clang/G++ -- skipping leaf sections after an exception
Some versions of `libc++` and `libstdc++` (or their runtimes) have a bug with `std::uncaught_exception()` getting stuck returning `true` after rethrow, even if there are no active exceptions. One such case is this snippet, which skipped the sections "a" and "b", when compiled against `libcxxrt` from master
```cpp
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

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

If you are seeing a problem like this, i.e. a weird test paths that trigger only under Clang with `libc++`, or only under very specific version of `libstdc++`, it is very likely you are seeing this. The only known workaround is to use a fixed version of your standard library.
