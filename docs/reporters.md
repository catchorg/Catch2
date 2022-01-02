<a id="top"></a>
# Reporters

Catch has a modular reporting system and comes bundled with a handful of useful reporters built in.
You can also write your own reporters.

## Using different reporters

The reporter to use can easily be controlled from the command line.
To specify a reporter use [`-r` or `--reporter`](command-line.md#choosing-a-reporter-to-use), followed by the name of the reporter, e.g.:

```
-r xml
```

If you don't specify a reporter then the console reporter is used by default.
There are four reporters built in to the single include:

* `console` writes as lines of text, formatted to a typical terminal width, with colours if a capable terminal is detected.
* `compact` similar to `console` but optimised for minimal output - each entry on one line
* `gtest` also designed for console output, but mimics the output of Google Test framework. See below for details.
* `junit` writes xml that corresponds to Ant's [junitreport](http://help.catchsoftware.com/display/ET/JUnit+Format) target. Useful for build systems that understand Junit.
Because of the way the junit format is structured the run must complete before anything is written. 
* `xml` writes an xml format tailored to Catch. Unlike `junit` this is a streaming format so results are delivered progressively.

There are a few additional reporters, for specific build systems, in the Catch repository (in `include\reporters`) which you can `#include` in your project if you would like to make use of them.
Do this in one source file - the same one you have `CATCH_CONFIG_MAIN` or `CATCH_CONFIG_RUNNER`.

* `teamcity` writes the native, streaming, format that [TeamCity](https://www.jetbrains.com/teamcity/) understands. 
Use this when building as part of a TeamCity build to see results as they happen ([code example](../examples/207-Rpt-TeamCityReporter.cpp)).
* `tap` writes in the TAP ([Test Anything Protocol](https://en.wikipedia.org/wiki/Test_Anything_Protocol)) format.
* `automake` writes in a format that correspond to [automake  .trs](https://www.gnu.org/software/automake/manual/html_node/Log-files-generation-and-test-results-recording.html) files
* `sonarqube` writes the [SonarQube Generic Test Data](https://docs.sonarqube.org/latest/analysis/generic-test/) XML format.

You see what reporters are available from the command line by running with `--list-reporters`.

By default all these reports are written to stdout, but can be redirected to a file with [`-o` or `--out`](command-line.md#sending-output-to-a-file)

### gtest reporter

The `gtest` reporter is much more verbose than `console` but it allows you to:
* See how each of your tests is run - it feels good to see a lot of green lines
* See what test is being run now - useful when some tests are slow
* See the progress is going - useful when the whole test suit is slow
* See the output of each test nested inside its scope - useful when tests prints something to stdout
* Feel comfortable if you are used to Google Test

The reporter is optimized for test cases that do not have nested sections in them. A _test case_ from Catch2 is thought of as a _test case_ from gtest and each _section_ of the test case is thought of as a _test_ from gtest. The latter is referred to as simply a "test" in the output. It is assumed that code outside sections is only used for set up and tear down and is unlikely to fail the test case.

The reporter also works perfectly fine when there are nested sections, but we need to know the general definition of a "test" to understand how it works: a test is a single run of a test case. For instance: 
* If there are no sections in the test case, Catch2 runs the test case once and thus there is only one test.
* If there are `N` non-nested sections in the test case, Catch2 runs  it `N` times and thus there is `N` tests. 
* If there are nested sections in the test case, Catch2 runs it for every leaf section and thus the number of tests depends on the structure of the sections.

Each time Catch2 enters a section the reporter prints the `[ RUN ]` line to the output. With the exception of the root section, which Catch2 implicitly creates for the body of the test case itself. It is only printed once per test case. You can set verbosity to high to remove this exception and fully see how the sections stack is formed during the execution.

Note that since Catch2 is much more flexible with its sections than Google Test with its test cases and tests, there is no single "true" way to define how the output of the reporter should look. Especially if there are nested sections. To make things even more complicated, Catch2 runner does not know the structure of the sections before they are actually executed, and thus we cannot analyze this structure beforehand to e.g. collapse the output for entering multiple nested sections.

## Writing your own reporter

You can write your own custom reporter and register it with Catch.
At time of writing the interface is subject to some changes so is not, yet, documented here.
If you are determined you shouldn't have too much trouble working it out from the existing implementations -
but do keep in mind upcoming changes (these will be minor, simplifying, changes such as not needing to forward calls to the base class).

---

[Home](Readme.md#top)
