Catch is designed to "just work" as much as possible. For most people the only configuration needed is telling Catch which source file should host all the implementation code (```CATCH_CONFIG_MAIN```).

Nonetheless there are still some occasions where finer control is needed. For these occasions Catch exposes a small set of macros for configuring how it is built.

#  Terminal colour

Yes, I am English, so I will continue to spell "colour" with a 'u'.

When sending output to the terminal, if it detects that it can, Catch will use colourised text. On Windows the Win32 API, ```SetConsoleTextAttribute```, is used. On POSIX systems ANSI colour escape codes are inserted into the stream.

For finer control you can define one of the following identifiers (these are mutually exclusive - but that is not checked so may behave unexpectedly if you mix them):

	CATCH_CONFIG_COLOUR_NONE	// completely disables all text colouring
	CATCH_CONFIG_COLOUR_WINDOWS	// forces the Win32 console API to be used
	CATCH_CONFIG_COLOUR_ANSI	// forces ANSI colour codes to be used

Note that when ANSI colour codes are used "unistd.h" must be includable - along with a definition of ```isatty()```

Typically you should place the ```#define``` before #including "catch.hpp" in your main source file - but if you prefer you can define it for your whole project by whatever your IDE or build system provides for you to do so.

---

[Home](Readme.md)