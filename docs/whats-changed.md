## What's new in Catch

This page has been added following quite a large (hopefully the last such) merge from the integration branch. Please read this summary through so you know what to expect (and whether any changes - breaking in some cases - will affect you).

* Calling Catch from your own ```main()``` has changed - please review [the updated docs](own-main.md)
* The command line has changed. The biggest change is that test case names and tags should now only be supplied as primary arguments - in fact the ```-t``` option has been repurposed to mean "list tags". There are [updated docs for this too](command-line.md)
* There is a new reporter interface. If you have written a custom reporter you can use the ```LegacyReporterAdapter``` to minimise any differences. Ideally you should update to the new interface - especially as it has been designed to be more robust in the face of future changes (which should be minimal).
* The docs have moved from the wiki to the repository itself. They consist of a set of markdown files in the docs folder and are referenced directly from the README in the root. You can still read them online from GitHub.
* Lots of new goodness - more documentation for which is coming. The existing docs have been updated to account for some of the changes already (e.g. variadic macros). A quick rundown:
	* Variadic macros are used, where possible, so that, e.g. you can write a ```TEST_CASE``` with just a name - or even no name at all (making it an anonymous test case).
	* The hierarchical naming convention is deprecated in favour of using tags (see next)
	* ```TEST_CASE```s (but not ```SECTION```s) can now be tagged by placing keywords in square brackets in the second argument - e.g.: ```TEST_CASE( "A nice name", "[tag1][tag2]")```. The old style is still supported but please consider using this new style.
	* Tests can still be "hidden" using the ```./``` prefix as before, but the preferred way now is to give it the ```[hide]``` tag (hidden tests are skipped if you run the test process without specifying any test specs).
	* As well as ```TEST_CASE```s and ```SECTION```s you can now also use BDD-style ```SCENARIO``` (in place of ```TEST_CASE```) and ```GIVEN```, ```WHEN``` and ```THEN``` macros (in place of ```SECTION```s).
	* New command line parser. Under the hood it is a complete rewrite - now powered by a command line library that will soon be spun out as a separate project: Clara. The options themselves are largely the same but there are some notable differences (as already discussed).
	* Completely overhauled output from the textual reporter (now the Console reporter). This now features a much clearer, cleaner format, including good use of indentation.

More information can be found in [this blog post](http://www.levelofindirection.com/journal/2013/6/28/catch-10.html).

If you find any issues please raise issue tickets on the [issue tracker on GitHub](https://github.com/philsquared/Catch/issues) as before. For general questions, comments and suggestions, though, please use the [new forums on Google Groups](https://groups.google.com/forum/?fromgroups#!forum/catch-forum).

---

[Home](../README.md)
