<a id="top"></a>
# Tests inside a library project

Catch2 discovers `TEST_CASE`s at static initialization time in every
translation unit that is **linked into the test executable**. If you
ship tests with a library (several `.cpp` files, each with its own
`TEST_CASE`s), they will not run unless all of those object files end up
in the same binary you execute.

This page summarizes the layout that avoids “missing” tests when using
multiple compilation units. It replaces older v2 guidance around
`CATCH_CONFIG_RUNNER`; in v3 use [`Catch::Session`](own-main.md#top)
instead (see also [Migrating from v2 to v3](migrate-v2-to-v3.md#top)).


## Recommended layout

```
mylib/
  include/
  src/
  tests/
    test_foo.cpp
    test_bar.cpp
  CMakeLists.txt
```

Keep test sources under `tests/` (or similar) and link them into **one**
test executable together with your library.


## CMake (recommended)

```cmake
add_library(mylib ...)

file(GLOB MYLIB_TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
# If you provide a custom main, exclude it from the glob and add it explicitly.
list(FILTER MYLIB_TEST_SOURCES EXCLUDE REGEX "test_main\\.cpp$")

add_executable(mylib-tests ${MYLIB_TEST_SOURCES})
target_link_libraries(mylib-tests PRIVATE mylib Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(mylib-tests)
```

`Catch2::Catch2WithMain` supplies a single `main` for the executable.
Register tests with [`catch_discover_tests`](cmake-integration.md#catch_discover_tests)
so CTest sees each `TEST_CASE`.

**Common mistake:** compiling test `.cpp` files into the `mylib` static
library but linking only `mylib` into a tiny runner that has no test
sources of its own. The test object files are then not linked into the
runner and Catch2 never sees them. Prefer linking test sources directly
into `mylib-tests`, or ensure the static library is linked with flags
that pull in all object files (for example `--whole-archive` on GCC/Clang).


## Custom `main`

If you need code to run before or after the test session, provide your
own `main` and link only `Catch2::Catch2`:

```cpp
#include <catch2/catch_session.hpp>

int main(int argc, char* argv[]) {
    // Optional library-wide setup
    const int result = Catch::Session().run(argc, argv);
    // Optional library-wide teardown
    return result;
}
```

There must be exactly **one** `main` in the test executable. See
[Supplying main() yourself](own-main.md#top) for more recipes.


## Without CMake

Compile every `.cpp` that defines `TEST_CASE`s into the same final
executable. If tests disappear after a refactor, check that each test
translation unit is still on the link line of the binary you run.


## Further reading

- [Getting Catch to work across multiple compilation units](https://groups.google.com/g/catch-forum/c/pQchmF6Pr0E) (Catch forum)
- [CATCH_CONFIG_RUNNER usage](https://groups.google.com/g/catch-forum/c/FV0Qo62DvgY) (Catch2 v2; use `Catch::Session` in v3)


---

[Home](Readme.md#top)
