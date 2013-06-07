# Why do we need yet another C++ test framework?

Good question. For C++ there are quite a number of established frameworks, including (but not limited to), [CppUnit](http://sourceforge.net/apps/mediawiki/cppunit/index.php?title=Main_Page), [Google Test](http://code.google.com/p/googletest/), [Boost.Test](http://www.boost.org/doc/libs/1_49_0/libs/test/doc/html/index.html), [Aeryn](https://launchpad.net/aeryn), [Cute](http://r2.ifs.hsr.ch/cute), [Fructose](http://fructose.sourceforge.net/) and [many, many more](http://en.wikipedia.org/wiki/List_of_unit_testing_frameworks#C.2B.2B). Even for Objective-C there are a few, including OCUnit - which now comes bundled with XCode.

So what does Catch bring to the party that differentiates it from these? Apart from a Catchy name, of course.

## Key Features

* Really easy to get started. Just download catch.hpp, #include it and you're away. 
* No external dependencies. As long as you can compile C++98 and have a C++ standard library available.
* Write test cases as, self-registering, functions or methods.
* Divide test cases into sections, each of which is run in isolation (eliminates the need for fixtures!)
* Use BDD-style GIVEN-WHEN-THEN in place of test cases and sections.
* Only one core assertion macro for comparisons. Standard C/C++ operators are used for the comparison - yet the full expression is decomposed and lhs and rhs values are logged.

## Other core features

* Tests are named using free-form strings - no more couching names in legal identifiers.
* Tests can be tagged for easily running ad-hoc groups of tests.
* Failures can (optionally) break into the debugger on Windows and Mac.
* Output is through modular reporter objects. Basic textual and XML reporters are included. Custom reporters can easily be added.
* JUnit xml output is supported for integration with third-party tools, such as CI servers.
* A default main() function is provided (in a header), but you can supply your own for complete control (e.g. integration into your own test runner GUI).
* A command line parser is provided and can still be used if you choose to provided your own main() function.
* Catch can test itself.
* Alternative assertion macro(s) report failures but don't abort the test case
* Floating point tolerance comparisons are built in using an expressive Approx() syntax.
* Internal and friendly macros are isolated so name clashes can be managed
* Support for Matchers (early stages)

## Objective-C-specific features

* Automatically detects if you are using it from an Objective-C project
* Works with and without ARC with no additional configuration
* Implement test fixtures using Obj-C classes too (like OCUnit)
* Additional built in matchers that work with Obj-C types (e.g. string matchers)

See the [tutorial](tutorial.md) to get more of a taste of using CATCH in practice 

---

[Home](../README.md)