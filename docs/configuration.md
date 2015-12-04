Catch is designed to "just work" as much as possible. For most people the only configuration needed is telling Catch which source file should host all the implementation code (```CATCH_CONFIG_MAIN```).

Nonetheless there are still some occasions where finer control is needed. For these occasions Catch exposes a small set of macros for configuring how it is built.

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

Catch does not use ```std::cout``` and ```std::cerr``` directly but gets them from ```Catch::cout()``` and ```Catch::cerr()``` respectively. If the above identifier is defined these functions are left unimplemented and you must implement them yourself. Their signatures are:

    std::ostream& cout();
    std::ostream& cerr();

This can be useful on certain platforms that do not provide ```std::cout``` and ```std::cerr```, such as certain embedded systems.

# C++ conformance toggles

	CATCH_CONFIG_CPP11_NULLPTR 				// nullptr is supported?
	CATCH_CONFIG_CPP11_NOEXCEPT				// noexcept is supported?
	CATCH_CONFIG_CPP11_GENERATED_METHODS	// delete and default keywords for methods
	CATCH_CONFIG_CPP11_IS_ENUM				// std::is_enum is supported?
	CATCH_CONFIG_CPP11_TUPLE				// std::tuple is supported
	CATCH_CONFIG_VARIADIC_MACROS 			// Usually pre-C++11 compiler extensions are sufficient
	CATCH_CONFIG_CPP11_LONG_LONG			// generates overloads for the long long type
	CATCH_CONFIG_CPP11_OVERRIDE				// CATCH_OVERRIDE expands to override (for virtual function implementations)
	CATCH_CONFIG_CPP11_UNIQUE_PTR			// Use std::unique_ptr instead of std::auto_ptr

Catch has some basic compiler detection that will attempt to select the appropriate mix of these macros. However being incomplete - and often without access to the respective compilers - this detection tends to be conservative.
So overriding control is given to the user. If a compiler supports a feature (and Catch does not already detect it) then one or more of these may be defined to enable it (or suppress it, in some cases). If you do do this please raise an issue, specifying your compiler version (ideally with an idea of how to detect it) and stating that it has such support.
You may also suppress any of these features by using the `_NO_` form, e.g. `CATCH_CONFIG_CPP11_NO_NULLPTR`.

All C++11 support can be disabled with `CATCH_CONFIG_NO_CPP11`

---

[Home](Readme.md)