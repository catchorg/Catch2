<a id="top"></a>
# Command line

**Contents**<br>
[Test specs](#test-specs)<br>
[Selecting tests](#selecting-tests)<br>
[Running](#running)<br>
[How tests behave](#how-tests-behave)<br>
[History and analysis](#history-and-analysis)<br>
[Output](#output)<br>
[Configuration](#configuration)<br>
[Information](#information)<br>
[Environment](#environment)<br>

This is the command line of a test binary, which the
[Catch3 runner](progmasoft-runner.md#top) reads. `--help` prints it.

Long options take their value after `=` or as the next argument:
`--jobs=4` and `--jobs 4` are the same. Short options take it attached or
after `=`: `-j4`, `-j=4`. An option the runner does not know is an error,
with a suggestion when it looks like a known one:

```text
$ ./tests --jbos=4
error: '--jbos=4' is not an option; did you mean --jobs?
```

`--` ends the options: what follows is test specs, even when it starts with
`-`.


## Test specs

Arguments that are not options select tests by name or tag, as in Catch2:

| Spec | Selects |
| --- | --- |
| `"parses lists"` | the test of that name |
| `"parses*"` | tests whose name starts with "parses"; `*` may stand at either end |
| `[parser]` | tests tagged `[parser]` |
| `[parser][fast]` | tests with both tags |
| `~[slow]` | every test but those tagged `[slow]` |
| `[parser],[lexer]` | tests with either tag |
| `"a b" [x]` | specs separated by spaces must all match |
| `[@alias]` | the tags of a [tag alias](test-cases-and-sections.md#tag-aliases) |

Without a spec, every test runs except hidden ones, whose tags start with
`.`; a spec that names a hidden test runs it. A backslash makes `,`, `[`,
`]`, `*`, `~` and `"` literal: `"commas\, and more"`.


## Selecting tests

| Option | |
| --- | --- |
| `-c`, `--section=NAME` | run only this section; repeat for nested sections |
| `-p`, `--path-filter=c:SECTION\|g:INDEX` | run one path through sections (`c:`) and generator values (`g:`, an index or `*`); repeat for each level. See [running a section](filtering-execution-path.md#top). |
| `-f`, `--input-file=FILE` | run the tests named in FILE, one exact name per line; lines starting with `#` are comments |
| `--select=WHICH` | `all` (default), `failed`, `changed`, `new` or `smart`; see [choosing tests](progmasoft-runner.md#choosing-and-ordering-tests) |
| `--changes-since=REVISION` | the Git revision `changed` and `smart` compare with (default `HEAD`) |
| `--incremental` | skip tests that passed last time and whose inputs did not change |
| `--shard=INDEX/COUNT` | run one of COUNT shards, counting from 0 |
| `--shard-durations=FILE` | balance shards by the durations in a JSON report or a history |
| `-#`, `--filenames-as-tags` | tag every test with `#` and the name of its file, as `[#parser.tests]` |


## Running

| Option | |
| --- | --- |
| `-j`, `--jobs=N` | worker processes; 0, the default, uses one per hardware thread |
| `--in-process` | run the tests in this process, one after another |
| `--isolate` | start a fresh process for every test |
| `--pass-env=NAME` | give the tests the variable `NAME`, which they do not get because its name looks like a secret (repeatable) |
| `--timeout=SECONDS` | the limit for a test without history, and the cap of learned limits; 0 for none |
| `--no-adaptive-timeout` | use `--timeout` for every test |
| `--retries=N\|auto\|off` | `auto` (default) retries what may pass again; N retries every failure up to N times |
| `--repeat=N` | run every test N times, to find flaky tests |
| `-a`, `--fail-fast[=N]` | start no test after N failed tests (default 1) |
| `--order=ORDER` | `priority` (default), `declared`, `name` or `random` |
| `--seed=N\|random` | the random seed; a new one each run by default |
| `--no-bisect` | do not search for the test that leaves state behind |


## How tests behave

| Option | |
| --- | --- |
| `-s`, `--success` | report passing assertions too, and show every assertion |
| `-b`, `--break` | break into the debugger on a failure; runs in process |
| `-e`, `--no-throw` | skip the assertions that expect exceptions |
| `-w`, `--warn=WHAT` | fail on `no-assertions`, `unmatched-specs` or `infinite-generators` |
| `--allow-no-tests` | succeed when no test runs |
| `--skip-benchmarks` | do not run `BENCHMARK` blocks |
| `--benchmark-samples=N` | samples per benchmark (default 100) |
| `--benchmark-resamples=N` | bootstrap resamples (default 100000) |
| `--benchmark-confidence-interval=X` | between 0 and 1 (default 0.95) |
| `--benchmark-no-analysis` | report the mean of the samples without statistics |
| `--benchmark-warmup-time=MILLISECONDS` | warm-up time per benchmark (default 100) |


## History and analysis

| Option | |
| --- | --- |
| `--history=DIRECTORY` | where results are remembered (default `.catch3` beside the binary, or `CATCH3_HISTORY`) |
| `--no-history` | neither read nor write the history |
| `--quarantine=MODE` | `auto` (default), `report` or `off` |
| `--replay` | run last run's failures again, each in the order and with the seed it failed with |
| `--analytics[=json]` | print what the history says about the tests, and exit |


## Output

| Option | |
| --- | --- |
| `-v`, `--verbose` | print every test as it ends, with what it printed |
| `-q`, `--quiet` | print only failures and the summary |
| `--color=WHEN` | `auto` (default), `always` or `never` |
| `--junit=FILE` | write a JUnit XML report |
| `--json=FILE` | write a JSON report with every attempt and its analysis |
| `--trace=FILE` | write a trace for `chrome://tracing` or Perfetto |
| `--slowest[=N]` | list the N slowest tests (default 10) |
| `--premature-exit-guard-file=FILE` | create FILE, private to the user, when the run starts and remove it when the run ends normally |


## Configuration

A project can keep its options in a [`catch3.json`](progmasoft-runner.md#project-configuration);
options on the command line win over it.

| Option | |
| --- | --- |
| `--config=FILE` | read this `catch3.json` instead of searching for one |
| `--no-config` | read no `catch3.json` |


## Information

| Option | |
| --- | --- |
| `-l`, `--list[=FORMAT]` | list the matching tests: `text` (default), `names` or `json` ([format](progmasoft-runner.md#formats-other-tools-read)) |
| `-t`, `--list-tags` | list the tags of the matching tests |
| `-h`, `--help` | print the options |
| `--version` | print the version of Catch3 |


## Environment

| Variable | |
| --- | --- |
| `CATCH3_HISTORY` | the history directory |
| `CATCH3_PROPERTY_DATABASE` | the example database of [property tests](progmasoft-properties.md#top) |
| `CATCH3_WORKER` | set in each worker to its number |
| `TMPDIR`, `TMP`, `TEMP` | set while a test runs to its [own directory](progmasoft-runner.md#temporary-files) |
| Bazel's test variables | see [Bazel](progmasoft-runner.md#bazel-and-ctest) |

---

[Home](Readme.md#top)
