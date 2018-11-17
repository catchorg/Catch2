<a id="top"></a>
# Deprecations and incoming changes

**Contents**<br>
[Verbosities](#verbosities)<br>
[`--list-*` command line parameters](#--list--command-line-parameters)<br>
[Types passed to the reporter interface](#types-passed-to-the-reporter-interface)<br>
[Generators](#generators)<br>
[`ANON_TEST_CASE`](#anon_test_case)<br>
[Secondary description amongst tags](#secondary-description-amongst-tags)<br>

This page documents current deprecations and upcoming changes inside
Catch2. You can expect deprecated functionality to stick around until
the next major release, but not for longer.


## Verbosities

The current implementation of verbosities has been misguided and will
be removed. Note that this does not mean verbosities will be gone, just
that they will no longer be checked up-front, and a reporter can handle
verbosity however it sees fit (including ignoring it).


## `--list-*` command line parameters

There will be 3 large changes to the `--list-*` family of command line
parameters.

* Their return codes will no longer reflect the number of tests/tags/etc
that were found, instead it will be 0 for success and non-zero for failure.
* Their output will be piped through reporters, so that e.g. XML reporter
will write the output as a machine-readable XML, while the console
reporter will keep the current output.
* `--list-test-names-only` will be completely removed.


## Types passed to the reporter interface

To allow changes in internal representation, we are planning to change
the arguments provided to functions in the reporter interface.


## Generators

The current generator interface is not intended to be stable and will be
changed.


## `ANON_TEST_CASE`

`ANON_TEST_CASE` is scheduled for removal, as it can be fully replaced
by a `TEST_CASE` with no arguments.


## Secondary description amongst tags

Currently, the tags part of `TEST_CASE` (and others) macro can also
contain text that is not part of tags. As it is not actually documented,
or used, it will be removed.


---

[Home](Readme.md#top)
