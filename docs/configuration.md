Catch is designed to "just work" as much as possible. For most people the only configuration needed is telling Catch which source file should host all the implementation code (```CATCH_CONFIG_MAIN```).

Nonetheless there are still some occasions where finer control is needed. For these occasions Catch exposes a set of macros for configuring how it is built.

#  main()/ implementation

	CATCH_CONFIG_MAIN	// Designates this as implementation file and defines main()
	CATCH_CONFIG_RUNNER	// Designates this as implementation file

Although Catch is header only it still, internally, maintains a distinction between interface headers and headers that contain implementation. Only one source file in your test project should compile the implementation headers and this is controlled through the use of one of these macros - one of these identifiers should be defined before including Catch in *exactly one implementation file in your project*.

#  Prefixing Catch macros

	CATCH_CONFIG_PREFIX_ALL

To keep test code clean and uncluttered Catch uses short macro names (e.g. ```TEST_CASE``` and ```REQUIRE```). Occasionally these may conflict with identifiers from platform headers or the system under test. In this case the above identifier can be defined. This will cause all the Catch user macros to be prefixed with ```CATCH_``` (e.g. ```CATCH_TEST_CASE``` and ```CATCH_REQUIRE```).


#  Terminal colour

	CATCH_CONFIG_COLOUR_NONE	// completely disables all text colouring
	CATCH_CONFIG_COLOUR_WINDOWS	// forces the Win32 console API to be used
	CATCH_CONFIG_COLOUR_ANSI	// forces ANSI colour codes to be used

Yes, I am English, so I will continue to spell "colour" with a 'u'.

When sending output to the terminal, if it detects that it can, Catch will use colourised text. On Windows the Win32 API, ```SetConsoleTextAttribute```, is used. On POSIX systems ANSI colour escape codes are inserted into the stream.

For finer control you can define one of the above identifiers (these are mutually exclusive - but that is not checked so may behave unexpectedly if you mix them):

Note that when ANSI colour codes are used "unistd.h" must be includable - along with a definition of ```isatty()```

Typically you should place the ```#define``` before #including "catch.hpp" in your main source file - but if you prefer you can define it for your whole project by whatever your IDE or build system provides for you to do so.

#  Console width

	CATCH_CONFIG_CONSOLE_WIDTH = x // where x is a number

Catch formats output intended for the console to fit within a fixed number of characters. This is especially important as indentation is used extensively and uncontrolled line wraps break this.
By default a console width of 80 is assumed but this can be controlled by defining the above identifier to be a different value.

#  stdout

	CATCH_CONFIG_NOSTDOUT

Catch does not use ```std::cout```, ```std::cerr``` and ```std::clog``` directly but gets them from ```Catch::cout()```, ```Catch::cerr()``` and ```Catch::clog``` respectively. If the above identifier is defined these functions are left unimplemented and you must implement them yourself. Their signatures are:

    std::ostream& cout();
    std::ostream& cerr();
    std::ostream& clog();

This can be useful on certain platforms that do not provide the standard iostreams, such as certain embedded systems.

# Default reporter

    CATCH_CONFIG_DEFAULT_REPORTER <reporter>

The default reporter (reporter used when no reporters are explicitly specified) can be overriden during compilation time by using the above macro. Note that desired value of the macro is a C string and quotes have to be escaped during compilation: `clang++ test.cpp -DCATCH_CONFIG_DEFAULT_REPORTER=\"xml\"`.

# C++ conformance toggles

    CATCH_CONFIG_CPP11_NULLPTR                 // nullptr is supported?
    CATCH_CONFIG_CPP11_NOEXCEPT                // noexcept is supported?
    CATCH_CONFIG_CPP11_GENERATED_METHODS       // delete and default keywords for methods
    CATCH_CONFIG_CPP11_IS_ENUM                 // std::is_enum is supported?
    CATCH_CONFIG_CPP11_TUPLE                   // std::tuple is supported
    CATCH_CONFIG_VARIADIC_MACROS               // Usually pre-C++11 compiler extensions are sufficient
    CATCH_CONFIG_CPP11_LONG_LONG               // generates overloads for the long long type
    CATCH_CONFIG_CPP11_OVERRIDE                // CATCH_OVERRIDE expands to override (for virtual function implementations)
    CATCH_CONFIG_CPP11_UNIQUE_PTR              // Use std::unique_ptr instead of std::auto_ptr
    CATCH_CONFIG_CPP11_SHUFFLE                 // Use std::shuffle instead of std::random_shuffle
    CATCH_CONFIG_CPP11_TYPE_TRAITS             // Use std::enable_if and <type_traits>
    CATCH_CONFIG_CPP11_STREAM_INSERTABLE_CHECK // Use C++11 expression SFINAE to check if class can be inserted to std::ostream

Catch has some basic compiler detection that will attempt to select the appropriate mix of these macros. However being incomplete - and often without access to the respective compilers - this detection tends to be conservative.
So overriding control is given to the user. If a compiler supports a feature (and Catch does not already detect it) then one or more of these may be defined to enable it (or suppress it, in some cases). If you do do this please raise an issue, specifying your compiler version (ideally with an idea of how to detect it) and stating that it has such support.
You may also suppress any of these features by using the `_NO_` form, e.g. `CATCH_CONFIG_CPP11_NO_NULLPTR`.

All C++11 support can be disabled with `CATCH_CONFIG_NO_CPP11`

## `CATCH_CONFIG_CPP11_STREAM_INSERTABLE_CHECK`

This flag is off by default, but allows you to resolve problems caused by types with private base class that are streamable, but the classes themselves are not. Without it, the following code will cause a compilation error:
```cpp
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
struct A {};
std::ostream &operator<< (std::ostream &o, const A &v) { return o << 0; }

struct B : private A {
    bool operator==(int){ return true;}
};

B f ();
std::ostream g ();

TEST_CASE ("Error in streamable check") {
    B x;
    REQUIRE (x == 4);
}
```

# Other toggles

    CATCH_CONFIG_COUNTER                    // Use __COUNTER__ to generate unique names for test cases
    CATCH_CONFIG_WINDOWS_SEH                // Enable SEH handling on Windows
    CATCH_CONFIG_FAST_COMPILE               // Sacrifices some (rather minor) features for compilation speed
    CATCH_CONFIG_POSIX_SIGNALS              // Enable handling POSIX signals
    CATCH_CONFIG_WINDOWS_CRTDBG             // Enable leak checking using Windows's CRT Debug Heap

Currently Catch enables `CATCH_CONFIG_WINDOWS_SEH` only when compiled with MSVC, because some versions of MinGW do not have the necessary Win32 API support.

`CATCH_CONFIG_POSIX_SIGNALS` is on by default, except when Catch is compiled under `Cygwin`, where it is disabled by default (but can be force-enabled by defining `CATCH_CONFIG_POSIX_SIGNALS`).

`CATCH_CONFIG_WINDOWS_CRTDBG` is off by default. If enabled, Windows's CRT is used to check for memory leaks, and displays them after the tests finish running.

Just as with the C++11 conformance toggles, these toggles can be disabled by using `_NO_` form of the toggle, e.g. `CATCH_CONFIG_NO_WINDOWS_SEH`.

## `CATCH_CONFIG_FAST_COMPILE`
Defining this flag speeds up compilation of test files by ~20%, by making 2 changes:
* The `-b` (`--break`) flag no longer makes Catch break into debugger in the same stack frame as the failed test, but rather in a stack frame *below*.
* The `REQUIRE` family of macros (`REQUIRE`, `REQUIRE_FALSE` and `REQUIRE_THAT`) no longer uses local try-catch block. This disables exception translation, but should not lead to false negatives.

`CATCH_CONFIG_FAST_COMPILE` has to be either defined, or not defined, in all translation units that are linked into single test binary, or the behaviour of setting `-b` flag and throwing unexpected exceptions will be unpredictable.

# Windows header clutter

On Windows Catch includes `windows.h`. To minimize global namespace clutter in the implementation file, it defines `NOMINMAX` and `WIN32_LEAN_AND_MEAN` before including it. You can control this behaviour via two macros:

    CATCH_CONFIG_NO_NOMINMAX            // Stops Catch from using NOMINMAX macro 
    CATCH_CONFIG_NO_WIN32_LEAN_AND_MEAN // Stops Catch from using WIN32_LEAN_AND_MEAN macro

---

[Home](Readme.md)
