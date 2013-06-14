CATCH works quite nicely without any command line options at all - but for those times when you want greater control the following options are available.
Note that options are described according to the following pattern:

<a href="#test">     `    <test-spec> ...`</a><br />
<a href="#reporter"> `    -r, --reporter`</a><br />
<a href="#break">    `    -b, --break`</a><br />
<a href="#success">  `    -s, --success`</a><br />
<a href="#abort">    `    -a, --abort`</a><br />
<a href="#list">     `    -l, --list`</a><br />
<a href="#output">   `    -o, --out`</a><br />
<a href="#name">     `    -n, --name`</a><br />
<a href="#nothrow">  `    -e, --nothrow`</a><br />
<a href="#usage">    `    -h, -?, --help`</a><br />

<a name="test" />
## Specifying which tests to run

<pre>&lt;test-spec> ...</pre>

Test cases, wildcarded test cases, tags and tag expressions are all passed directly as arguments. Tags are distinguished by being enclosed in square brackets.

If no test specs are supplied then all test cases, except "hidden" tests (tagged ```[hide]``` or, in the legacy case, prefixed by `'./'`) are run

Specs must be enclosed in quotes if they contain spaces. If they do not contain spaces the quotes are optional.

Wildcards consist of the `*` character at the beginning and/or end of test case names and can substitute for any number of any characters (including none).

Test specs are case insensitive.

If a spec is prefixed with `exclude:` or the `~` character then the pattern matches an exclusion. This means that tests matching the pattern are excluded from the set - even if a prior inclusion spec included them. Subsequent inclusion specs will take precendence, however.
Inclusions and exclusions are evaluated in left-to-right order.

Test case examples:

<pre>thisTestOnly            Matches the test case called, 'thisTestOnly'
"this test only"        Matches the test case called, 'this test only'
these*                  Matches all cases starting with 'these'
exclude:notThis         Matches all tests except, 'notThis'
~notThis                Matches all tests except, 'notThis'
~*private*              Matches all tests except those that contain 'private'
a* ~ab* abc             Matches all tests that start with 'a', except those that
                        start with 'ab', except 'abc', which is included
</pre>

Names within square brackets are interpreted as tags.
A series of tags form an AND expression wheras a comma seperated sequence forms an OR expression. e.g.:

<pre>[one][two],[three]</pre>
This matches all tests tagged `[one]` and `[two]`, as well as all tests tagged `[three]`


<a name="reporter" />
## Choosing a reporter to use

<pre>-r, --reporter &lt;reporter></pre>

A reporter is an object that formats and structures the output of running tests, and potentially summarises the results. By default a console reporter is used that writes, IDE friendly, textual output. Catch comes bundled with some alternative reporters, but more can be added in client code.<br />
The bundled reporters are:

<pre>-r console
-r xml
-r junit
</pre>

The JUnit reporter is an xml format that follows the structure of the JUnit XML Report ANT task, as consumed by a number of third-party tools, including Continuous Integration servers such as Hudson. If not otherwise needed, the standard XML reporter is preferred as this is a streaming reporter, whereas the Junit reporter needs to hold all its results until the end so it can write the overall results into attributes of the root node.

<a name="break" />
## Breaking into the debugger
<pre>-b, --break</pre>

In some IDEs (currently XCode and Visual Studio) it is possible for Catch to break into the debugger on a test failure. This can be very helpful during debug sessions - especially when there is more than one path through a particular test.
In addition to the command line option, ensure you have built your code with the DEBUG preprocessor symbol

<a name="success" />
## Showing results for successful tests
<pre>-s, --success</pre>

Usually you only want to see reporting for failed tests. Sometimes it's useful to see *all* the output (especially when you don't trust that that test you just added worked first time!).
To see successul, as well as failing, test results just pass this option. Note that each reporter may treat this option differently. The Junit reporter, for example, logs all results regardless.

<a name="abort" />
## Aborting after a certain number of failures
<pre>-a, --abort
-x, --abortx [&lt;failure threshold>]
</pre>

If a ```REQUIRE``` assertion fails the test case aborts, but subsequent test cases are still run.
If a ```CHECK``` assertion fails even the current test case is not aborted.

Sometimes this results in a flood of failure messages and you'd rather just see the first few. Specifying ```-a``` or ```--abort``` on its own will abort the whole test run on the first failed assertion of any kind. Use ```-x``` or ```--abortx``` followed by a number to abort after that number of assertion failures.

<a name="list" />
## Listing available tests, tags or reporters
<pre>-l, --list-tests
-t, --list-tags
--list-reporters
</pre>

```-l``` or ```--list-tests`` will list all registered tests, along with any tags.
If one or more test-specs have been supplied too then only the matching tests will be listed.

```-t``` or ```--list-tags``` lists all available tags, along with the number of test cases they match. Again, supplying test specs limits the tags that match.

```--list-reporters``` lists the available reporters.

<a name="output" />
## Sending output to a file
<pre>-o, --out &lt;filename>
</pre>

Use this option to send all output to a file. By default output is sent to stdout (note that uses of stdout and stderr *from within test cases* are redirected and included in the report - so even stderr will effectively end up on stdout).

<a name="name" />
## Naming a test run
<pre>-n, --name &lt;name for test run></pre>

If a name is supplied it will be used by the reporter to provide an overall name for the test run. This can be useful if you are sending to a file, for example, and need to distinguish different test runs - either from different Catch executables or runs of the same executable with different options. If not supplied the name is defaulted to the name of the executable.

<a name="nothrow" />
## Eliding assertions expected to throw
<pre>-e, --nothrow</pre>

Skips all assertions that test that an exception is thrown, e.g. ```REQUIRE_THROWS```.

These can be a nuisance in certain debugging environments that may break when exceptions are thrown (while this is usually optional for handled exceptions, it can be useful to have enabled if you are trying to track down something unexpected).

When running with this option any throw checking assertions are skipped so as not to contribute additional noise. Be careful if this affects the behaviour of subsequent tests.

<a name="usage" />
## Usage
<pre>-h, -?, --help</pre>

Prints the command line arguments to stdout

---

[Home](../README.md)