<a id="top"></a>
# Frequently Asked Questions (FAQ)

**Contents**<br>
[How do I run global setup/teardown only if tests will be run?](#how-do-i-run-global-setupteardown-only-if-tests-will-be-run)<br>
[How do I clean up global state between running different tests?](#how-do-i-clean-up-global-state-between-running-different-tests)<br>
[Why cannot I derive from the built-in reporters?](#why-cannot-i-derive-from-the-built-in-reporters)<br>
[What is Catch2's ABI stability policy?](#what-is-catch2s-abi-stability-policy)<br>
[What is Catch2's API stability policy?](#what-is-catch2s-api-stability-policy)<br>

## How do I run global setup/teardown only if tests will be run?

Write a custom [event listener](event-listeners.md#top) and place the
global setup/teardown code into the `testRun*` events.


## How do I clean up global state between running different tests?

Write a custom [event listener](event-listeners.md#top) and place the
cleanup code into either `testCase*` or `testCasePartial*` events,
depending on how often the cleanup needs to happen.


## Why cannot I derive from the built-in reporters?

They are not made to be overriden, in that we do not attempt to maintain
a consistent internal state if a member function is overriden, and by
forbidding users from using them as a base class, we can refactor them
as needed later.


## What is Catch2's ABI stability policy?

Catch2 provides no ABI stability guarantees whatsoever. Catch2 provides
rich C++ interface, and trying to freeze its ABI would take a lot of
pointless work.

Catch2 is not designed to be distributed as dynamic library, and you
should really be able to compile everything with the same compiler binary.


## What is Catch2's API stability policy?

Catch2 follows [semver](https://semver.org/) to the best of our ability.
This means that we will not knowingly make backwards-incompatible changes
without incrementing the major version number.


---

[Home](Readme.md#top)
