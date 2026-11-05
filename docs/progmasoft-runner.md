<a id="top"></a>
# The Catch3 runner

**Contents**<br>
[How a run works](#how-a-run-works)<br>
[Retries, flaky tests and order dependence](#retries-flaky-tests-and-order-dependence)<br>
[History, quarantine and analytics](#history-quarantine-and-analytics)<br>
[Importing the results of other frameworks](#importing-the-results-of-other-frameworks)<br>
[Choosing and ordering tests](#choosing-and-ordering-tests)<br>
[Tags the runner reads](#tags-the-runner-reads)<br>
[Time limits](#time-limits)<br>
[Temporary files](#temporary-files)<br>
[What a failure report says](#what-a-failure-report-says)<br>
[Reports and traces](#reports-and-traces)<br>
[Formats other tools read](#formats-other-tools-read)<br>
[Project configuration](#project-configuration)<br>
[Bazel and CTest](#bazel-and-ctest)<br>
[Exit codes](#exit-codes)<br>
[Coming from Catch2's runner](#coming-from-catch2s-runner)<br>

Every Catch3 test binary has the runner built in. It replaced Catch2's
session, command line and reporters; the API tests are written with stays
Catch2's: `TEST_CASE`, `SECTION`, the assertions, matchers, generators,
benchmarks, fixtures and listeners. The options are listed in the
[command line reference](command-line.md#top).

```text
$ ./tests
Catch3: 446 test(s), up to 8 worker(s), seed 2153251184

446 test(s): 423 passed, 16 failed as expected, 7 skipped
84193 assertion(s), 446 attempt(s); 1.2 s (6.0 s of tests, 5.0x in parallel)
```


## How a run works

The runner reads the registered tests, decides which to run and in which
order, and hands them to worker processes: the same binary, started again
with an internal option. A free worker takes the next test, so the work
spreads by itself and a long test does not hold back a shard of short ones.
`--jobs=N` sets the number of workers; the default is one per hardware
thread.

Running tests in their own processes gives the run what one process
cannot:

* **A crash ends the test, not the run.** A worker that dies is replaced;
  the test it was running is reported as crashed, with the signal or
  structured exception, the last assertion it started and the section it
  was in.
* **A hang is stopped.** A test that runs past its [time limit](#time-limits)
  is ended together with every process it started, and reported as timed
  out, with the section it was in.
* **Tests that need it run alone.** `--isolate`, or the tag `[!isolated]`,
  gives a test a fresh process.

The runner, its workers and every process a test starts form one group: a
job object on Windows, a process group elsewhere. Stopping a worker stops
all of it.

What a test writes to `std::cout`, `std::cerr` and `std::clog` is captured
and belongs to the test. Output of a worker outside any test, such as what a
listener prints when the run starts, is passed on to the runner's standard
error.

`--in-process` runs the tests in the runner's own process, one after
another, for a debugger. `--break` implies it. Crashes and hangs are then not
contained.

Listeners registered with `CATCH_REGISTER_LISTENER` run in every worker:
each worker is one test run for them, from `testRunStarting` to
`testRunEnded`.


## Retries, flaky tests and order dependence

With `--retries=auto`, the default, a failed test is tried once more, alone
in a fresh process. What happens then tells three kinds of failure apart:

* It fails again: the failure is deterministic, and is reported.
* It passes, and it had failed alone too: the test is **flaky**. It counts as
  passed, is marked flaky in every report, and its history remembers it.
* It passes alone, but had failed after other tests in a shared process:
  the runner runs the same tests again in a fresh process, in the same
  order. If the test fails again, it depends on state another test leaves
  behind, and the runner bisects the tests that ran before it to find which
  one. The report names it: *fails after 'X', which leaves state behind*.
  Such a failure fails the run; `--no-bisect` turns the search off.

Crashes, timeouts, tests in quarantine and tests with a flaky history get up
to two retries. `--retries=N` retries every failure up to N times,
`--retries=off` never retries, and `[!retries:N]` sets the limit of one
test. A run spends at most a tenth of its tests on retries, so a broken
build does not run everything three times.

`--repeat=N` runs every test N times; a test whose verdicts differ is flaky.


## History, quarantine and analytics

The runner remembers the results of the last 50 runs of each test: verdict,
duration, flakiness, failure signature and a fingerprint of the inputs. The
history of a binary lives in one JSON file in `.catch3` beside the binary,
or in the directory `--history` or the environment variable `CATCH3_HISTORY`
names. `--no-history` neither reads nor writes it. Runs that finish at the
same time all reach the file: it is updated under a lock, read again, and
replaced in one rename.

The history drives several features:

* **Adaptive time limits.** See [time limits](#time-limits).
* **Predictive ordering.** Each test gets a probability of failing: its
  failure rate, with recent runs weighing more, and at least 70% after a
  failure. Tests that are likely to fail run first, the most failures per
  second of running time first, so a failing run fails early. The rest run
  longest first, which keeps every worker busy until the end.
* **Quarantine.** A test with two flaky results in its last 20 runs goes
  into quarantine. It still runs and is reported, but its failure does not
  fail the run. Ten clean passes in a row release it. `[!quarantine]`
  quarantines a test by hand; `--quarantine=report` reports quarantine
  without excusing failures, and `--quarantine=off` ignores it.
* **Classification.** A failure is *new* (it passed last time),
  *persistent* (it failed last time too) or *intermittent* (it flakes).
* **Replay.** `--replay` runs the last failures again, each with the seed it
  failed with and after the same tests, in a fresh process.
* **Incremental runs.** `--incremental` skips tests that passed last time
  with the same fingerprint; they are reported as up to date.
* **Analytics.** `--analytics` prints what the history says: flaky tests,
  tests in quarantine, the tests that fail most often, the slowest tests and
  the tests getting slower. `--analytics=json` prints it as JSON.

Under `bazel test` there is no history unless `CATCH3_HISTORY` names a
directory that outlives the sandbox.


## Importing the results of other frameworks

`--import=FILE` reads the report of another test framework instead of running
tests, so that tests Catch3 does not run get the same history, flaky detection,
quarantine and analytics. Any Catch3 test binary can do it, and the option can
be repeated:

```sh
./tests --import=results/pytest.xml --import=results/vitest.ctrf.json \
        --import-name=web --junit=merged.xml
```

| `--import-format=` | Reads |
| --- | --- |
| `auto` | whichever the contents show (the default) |
| `catch3` | a `--json` report of Catch3, or [`catch3-results`](#the-catch3-results-format) |
| `junit` | JUnit XML, with Surefire's reruns; Gradle, pytest, Jest, GoogleTest and most others write it |
| `xunit` | xUnit.net v2 XML |
| `trx` | Visual Studio's TRX, with data-driven rows |
| `tap` | TAP 12, 13 and 14, with subtests and YAML diagnostics |
| `ctrf` | [CTRF](https://ctrf.io) JSON, with retries |

The results go into a history of their own, named by `--import-name` and
`imported` by default; `--analytics --import-name=NAME` reads it. The console,
`--junit`, `--json`, `--trace`, `--quarantine`, `--history` and the exit code
work as they do for a run. Reports that were read count as one run.

A test is identified by its class and its name. Results with the same
identifier, in one report or in several, are attempts at one test: a test that
both failed and passed is flaky, passes and is remembered as flaky, which is how
a report of a rerun is imported. A failure stays a failure when a later report
skips the test. Reports of the same tests on different platforms are not reruns
of each other: import each with an `--import-name` of its own, or a failure on one
reads as flakiness.

TAP names tests only within one stream, so the path given to `--import` is the
class of its tests: keep it the same from run to run for the history to follow
them. A name that comes back in one stream is another test and gets a number.

`--import-revision=REVISION` names what the reports tested, and a verdict that
changes on one revision then counts as flaky too. Without it only retries in the
reports do, because a working directory can have edits or be another checkout.

Nothing is run, so an imported failure has no replay command, and only what the
report says is known: it has no order, seed or sections. A TAP stream that bails
out or reports fewer tests than it planned adds a failed test called
`the TAP stream`, so that it does not read as a run that passed.

### The catch3-results format

For a framework without a report of these kinds, `catch3-results` is the
smallest thing to write: JSON Lines with one test per line, after a line that
names the format.

```json
{"format": "catch3-results", "version": 1}
{"name": "parses a date", "className": "Dates", "outcome": "passed", "durationMicroseconds": 1200}
{"name": "rounds down", "outcome": "failed", "message": "1.5 != 2", "details": "at Dates.rounds:12"}
```

`name` or `id`, and `outcome`, are required; `outcome` is a name from the JSON
report, such as `passed`, `failed`, `skipped`, `crashed` or `timed-out`. The other
members are `id`, `className`, `file`, `line`, `tags`, `category`, `message`,
`details`, `standardOutput` and `standardError`. A line can give `attempts`
instead, as the JSON report does.


## Choosing and ordering tests

Test specs work as in Catch2: names with `*` as a wildcard, `[tags]`, `~`
to exclude, a comma for "or". `--input-file` reads exact names from a file.
`--select` narrows the matching tests further:

| `--select=` | Runs |
| --- | --- |
| `all` | every matching test (default) |
| `failed` | tests that failed, crashed, timed out or flaked last time |
| `changed` | tests defined in files Git reports as changed since `--changes-since` (default `HEAD`), staged or not, and untracked files |
| `new` | tests without a recorded result |
| `smart` | changed, failed and new tests, those in quarantine, and those at least 50% likely to fail |

Each selected test carries the reason it was selected into the reports.

`--order=priority` (default) orders by the prediction above; `declared`,
`name` and `random` are also available. Every test is seeded with the
run's seed, wherever it runs, so the order does not change its random
numbers.

`--shard=INDEX/COUNT` runs one of COUNT shards. A test goes to the shard a
stable hash of its name chooses, so every machine agrees without talking to
the others; tests linked by `[!depends]` stay in one shard. With
`--shard-durations=FILE`, a history or a JSON report, shards are balanced by
duration instead, longest first into the least loaded shard. `--list`
honours `--shard`.


## Tags the runner reads

Tags that start with `!` are reserved, as in Catch2. The runner adds these:

| Tag | Effect |
| --- | --- |
| `[!serial]` | nothing else runs while this test runs |
| `[!isolated]` | the test runs in a process of its own |
| `[!resource:NAME]` | tests that share a resource never run at the same time |
| `[!depends:TEST]` | runs after TEST, and only if TEST passes; otherwise it is reported as blocked. TEST is selected along with it. Cycles are an error. |
| `[!timeout:SECONDS]` | the time limit of this test |
| `[!retries:N]` | the retry limit of this test |
| `[!quarantine]` | the test is in quarantine |

```cpp
TEST_CASE("migrates the schema", "[db][!resource:database]") { ... }
TEST_CASE("reads after migration", "[db][!resource:database][!depends:migrates the schema]") { ... }
TEST_CASE("renders the full catalogue", "[!timeout:120]") { ... }
```


## Time limits

A test with history gets a limit learned from it: four times its 95th
percentile or ten times its median, whichever is larger, plus two seconds,
and at least five seconds. A test without history gets 300 seconds.
`--timeout=SECONDS` sets the limit for tests without history and caps the
learned ones; `--timeout=0` turns limits off; `--no-adaptive-timeout` uses
`--timeout` for every test. `[!timeout:SECONDS]` overrides all of these.
Benchmarks get no learned limit.


## Temporary files

Every attempt of every test gets a temporary directory of its own. While the
test runs, `TMPDIR`, `TMP` and `TEMP` point at it, so
`std::filesystem::temp_directory_path()`, `tmpfile` and most libraries use
it, and tests that run at the same time never share a scratch file by
accident. `Progmasoft::Catch3::TestTemporaryDirectory()` in
`<Progmasoft/Catch3/Runner.hpp>` returns it.

The directory is removed when the attempt passes and kept when it fails;
the report says where.

## Environment variables

Workers, and the processes the runner starts for itself, inherit its
environment without the variables whose names look like secrets: those that
contain, in any case, `TOKEN`, `SECRET`, `PASSWORD`, `PASSWD`, `CREDENTIAL`,
`API_KEY`, `APIKEY`, `PRIVATE_KEY` or `ACCESS_KEY`. A test that needs one
gets it with `--pass-env=NAME`, which can be repeated, or `"pass-env"` in
the project configuration. The runner itself starts with the whole
environment, so code that runs before the first worker starts still sees
them.


## What a failure report says

```text
FAIL: parses nested lists
tests/parser.cpp:120
  tests/parser.cpp:131: FAILED:
    REQUIRE( tokens.size() == 3 )
  with expansion:
    2 == 3
  in section with commas
  analysis: new failure after 14 passing run(s); it last passed at commit 4c40a97b2f1e
  analysis: fails the same way as 'parses flat lists': probably one cause
  analysis: changed since HEAD: src/lexer.cpp
  replay: ./tests "parses nested lists" --seed=2153251184
```

Every failure has a **category**: assertion, unexpected exception, missing
exception, explicit failure, crash, timeout, dependency or infrastructure.
It has a **signature**: the category, the location of the first failure and
its message with numbers and addresses removed. Tests that share a
signature are named together, since they probably share a cause.

The analysis adds what the runner knows: how the failure relates to the
history, the commit the test last passed at, the change it follows (the
test's file, or the file of the failing assertion, changed since
`--changes-since`), where a crash or a hang happened, the test that leaves
state behind, the quarantine, and where the temporary files were kept. The
replay line runs the failure again as it happened.


## Reports and traces

* The **console report** shows progress and the failures with their
  analysis, then a summary. `--verbose` shows every test and what passing
  tests printed; `--success` also shows every assertion; `--quiet` shows
  only failures. `--slowest=N` lists the slowest tests.
* `--junit=FILE` writes JUnit XML. Retries are `<rerunFailure>` elements,
  and a flaky test is a success with `<flakyFailure>` elements, which
  Jenkins, GitLab and most CI dashboards understand.
* `--json=FILE` writes everything the runner knows: every attempt, its
  assertions, sections and benchmarks, the captured output, the analysis.
* `--trace=FILE` writes a trace for `chrome://tracing` or
  [Perfetto](https://ui.perfetto.dev): one track per worker, one slice per
  attempt and section.


## Formats other tools read

These formats carry a `format` and a `version`. Within a version they only
gain members; a reader ignores members it does not know and refuses a newer
version.

* **Discovery**, `--list=json`: `"format": "catch3-tests"`, version 1. One
  object per test with `id`, `name`, `className`, `file`, `line`, `tags`,
  `hidden`, and what the runner tags say: `serial`, `isolated`,
  `quarantined`, `resources`, `dependsOn`, `timeoutSeconds` and `retries`.
  CMake's `catch3_discover_tests` reads it.
* **Run report**, `--json`: `"format": "catch3-run"`, version 1.
* **Imported results**, `--import`: `"format": "catch3-results"`, version 1;
  see [its format](#the-catch3-results-format).
* **Analytics**, `--analytics=json`: `"format": "catch3-analytics"`,
  version 1.
* **History**: `"format": "catch3-history"`, version 1. It is the runner's
  own file; read it through `--analytics`.

The messages between the runner and its workers are internal and change
with the binary.


## Project configuration

Options a project always wants go in a `catch3.json`:

```json
{
  "version": 1,
  "tests": ["~[slow]"],
  "jobs": 8,
  "timeout": 60,
  "warn": ["no-assertions"],
  "junit": "build/test-results/junit.xml"
}
```

The runner looks for the file in the working directory and in each
directory above it, up to the root of the Git repository, and reads the
nearest one. Outside a repository it looks only in the working directory.
`--config=FILE` names a file instead, and `--no-config` reads none.

The keys are the long options, and their values go through the same checks
as on the command line, with the key named in the message:

| Value | Keys |
| --- | --- |
| `true` turns the switch on | `incremental`, `filenames-as-tags`, `in-process`, `isolate`, `success`, `no-throw`, `allow-no-tests`, `skip-benchmarks`, `benchmark-no-analysis` |
| `false` turns it off | `adaptive-timeout`, `bisect` |
| a string | `select`, `changes-since`, `order`, `quarantine`, `color` |
| a whole number | `jobs`, `repeat`, `fail-fast`, `slowest`, `benchmark-samples`, `benchmark-resamples`, `benchmark-warmup-time` |
| a whole number or a string | `retries` (`"auto"`, `"off"`), `seed` (`"random"`) |
| a number | `timeout` (seconds), `benchmark-confidence-interval` |
| a path, relative to the file | `junit`, `json`, `trace`, `shard-durations`; `history`, or `false` for none |
| an array of strings | `tests` (test specs), `warn`, `pass-env` |
| `"quiet"`, `"normal"` or `"verbose"` | `verbosity` |

`"version": 1` is required; `"$schema"` may name a schema for editors.
The file is strict JSON: no comments and no trailing commas. A key it does
not know is an error, with a suggestion when it looks like a known one.
Options that describe one run, such as `--shard`, `--section`, `--list` or
`--replay`, belong on the command line only.

Options on the command line win over the file, and test specs on the
command line replace the file's `tests` rather than adding to them. A
switch the file turns on stays on; `--no-config` leaves the file out.
`--verbose` names the file that was read. Workers, `--help` and
`--version` read no file.


## Bazel and CTest

Under `bazel test` the runner reads Bazel's environment: `XML_OUTPUT_FILE`
for a JUnit report, `TESTBRIDGE_TEST_ONLY` for `--test_filter`,
`TEST_SHARD_INDEX`, `TEST_TOTAL_SHARDS` and `TEST_SHARD_STATUS_FILE` for
sharding, `TEST_RANDOM_SEED` for the seed, `TEST_PREMATURE_EXIT_FILE` for
the exit guard and `TEST_UNDECLARED_OUTPUTS_DIR` for a JSON report.

[`catch3_discover_tests`](cmake-integration.md#automatic-test-registration)
registers each test with CTest through the discovery format. The runner
also parallelises a whole binary under one CTest test, which usually starts
fewer processes.


## Exit codes

| Code | Meaning |
| --- | --- |
| 0 | every test passed or failed as allowed by `[!mayfail]` or `[!shouldfail]`; flaky tests and failures in quarantine do not fail the run |
| 1 | the command line was wrong, the tests could not be registered, or workers could not start |
| 2 | no test ran (`--allow-no-tests` accepts this) |
| 3 | a test spec matched nothing and `--warn=unmatched-specs` was given |
| 4 | every test that ran was skipped |
| 5 | a test spec is invalid |
| 42 | a test failed, crashed, timed out or was blocked |


## Coming from Catch2's runner

The test-writing API is unchanged. The runner is new:

* Options are Catch3's own; `--help` lists them. Long options take their
  value after `=` or as the next argument. `-s`, `-b`, `-e`, `-a`, `-c`,
  `-p`, `-f`, `-#` and `-w` keep their meaning, under the names in the
  [reference](command-line.md#top).
* Reporters and `CATCH_REGISTER_REPORTER` are gone. The console, JUnit and
  JSON reports replace the built-in reporters; for anything else, read the
  JSON report, or write a listener, which still runs in every worker.
* `Catch::Session` remains for a `main` of your own: `run(argc, argv)` hands
  the command line to the runner. `Session::cli()` and `configData()` are
  gone with Clara. See [supplying main yourself](own-main.md#top).
* `main` runs in every worker too, since workers are the same binary.
* Tests run in parallel by default. Tests that share files or other state
  outside the process need `[!serial]` or `[!resource:NAME]`; per-test
  temporary directories cover the common case.

---

[Home](Readme.md#top)
