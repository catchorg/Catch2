If you've used the Catch command line, you might know that Catch can selectively include and exclude tests using tags.  For example, you might want to exclude long running tests from each check-in to your CI server, so you might tag those tests with `[slow]` and run Catch with `example.exe ~[slow]`.

The goal here was to be able to select which tests are run using the regular Visual Studio command line tools (MSTest.exe for VS2010 and XXX for VS2012) so that suites of tests can be integrated into CI servers such as TeamCity or Jenkins with the minimum of effort. It doesn't look like it's possible to reproduce the same flexibility that is possible with Catch but it is possible to do some basic filtering using Catch tags.

## Enabling tags ##

The basic idea is to instantiate a dummy test that VS can 'see' but that isn't part of the Catch tests, like this:

```
CATCH_MAP_CATEGORY_TO_TAG(verbose, "[test1],[test2]");
```

Here, the first parameter is an identifier for a 'category' that we can pass into the command line, and the second is the set of tags that we want to use (exactly as you would specify it on the command line). We can run this suite like this (VS2010):

```
mstest /TestContainer:Debug\example.dll /category:"verbose"
```

There's one important difference between running a suite of tests and running from the VS GUI - when you run a test interactively from the GUI you want the test to stop at the first error so that you jump to the offending failure; on the other hand, when running a batch of tests, we want it to run as much as possible and produce a report at the end. CATCH_MAP_CATEGORY_TO_TAG() works in this way.

### Altering behaviour ###

Catch can also specify some other command line switches that affect the behaviour of the test, for example `-s` shows tests that pass as well as those that fail. With a little compiler magic we can do the same for our suite of tests:

```
CATCH_CONFIG_SHOW_SUCCESS(true)
CATCH_MAP_CATEGORY_TO_TAG(verbose, "[test1],[test2]");
```
Each new test manipulator affects the suite that follows. The following manipulators are available:

CATCH_CONFIG_SHOW_SUCCESS(bool) - show successful output
CATCH_CONFIG_WARN_MISSING_ASSERTIONS(bool) - warn about missing assertions
CATCH_CONFIG_ABORT_AFTER(count) - stops after 'count' failures.

### Running specific tests in  order ###

Sometimes, tags don't quite do what you need. This happened to me when I was writing the scripts that verify whether the Catch self tests do the right thing; one of the tests specifies 'abort after = 4' but Clang on OS X runs the tests n a different order to MSVC, so I got very different results.  What I needed was to be able to give a list of specific tests, to be run in exactly that order.  So I added this:

```
CATCH_INTERNAL_CONFIG_ADD_TEST("Output from all sections is reported")
CATCH_INTERNAL_CONFIG_ADD_TEST("Standard output from all sections is reported")
INTERNAL_CATCH_MAP_CATEGORY_TO_LIST(OutputFromAllSectionsIsReported);
```

Now I can run this from the command line in the same way, like this:

```
mstest /TestContainer:Debug\ManagedTestCatch.dll /category:"OutputFromAllSectionsIsReported"
```

### VS2012 ###

Microsoft have changed the way that tests should be run from the command line as of VS2012 - it seems that you should be using vstest.console.exe. Sadly, this means slightly different filtering for our tests; in particular the 'TestCategory' filter cannot be used for native tests (it doesn't work). So for managed tests we can use this:

```
vstest.console.exe /Logger:Trx Debug\ManagedExample.dll /TestCaseFilter:"TestCategory=verbose"
```
but for native tests it seems like the only descriptive 'trait' that does anything that we can use is 'Owner', so we have to use that; not ideal but as a workaround for vstest.console.exe we have to do this:

```
vstest.console.exe /Logger:Trx Debug\NativeExample.dll /TestCaseFilter:"Owner=verbose"
```

---

[Home](../../README.md)