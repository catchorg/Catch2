<a id="top"></a>
# Frequently Asked Questions (FAQ)

**Contents**<br>
[How do I run global setup/teardown only if tests will be run?](#how-do-i-run-global-setupteardown-only-if-tests-will-be-run)<br>
[How do I clean up global state between running different tests?](#how-do-i-clean-up-global-state-between-running-different-tests)<br>
[Why cannot I derive from the built-in reporters?](#why-cannot-i-derive-from-the-built-in-reporters)<br>
[What is Catch2's ABI stability policy?](#what-is-catch2s-abi-stability-policy)<br>
[What is Catch2's API stability policy?](#what-is-catch2s-api-stability-policy)<br>
[Does Catch2 support running tests in parallel?](#does-catch2-support-running-tests-in-parallel)<br>
[Can I compile Catch2 into a dynamic library?](#can-i-compile-catch2-into-a-dynamic-library)<br>

## How do I run global setup/teardown only if tests will be run?

Write a custom [event listener](event-listeners.md#top) and place the
global setup/teardown code into the `testRun*` events.


## How do I clean up global state between running different tests?

Write a custom [event listener](event-listeners.md#top) and place the
cleanup code into either `testCase*` or `testCasePartial*` events,
depending on how often the cleanup needs to happen.


## Why cannot I derive from the built-in reporters?

They are not made to be overridden, in that we do not attempt to maintain
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


## Does Catch2 support running tests in parallel?

Not natively, no. We see running tests in parallel as the job of an
external test runner, that can also run them in separate processes,
support test execution timeouts and so on.

However, Catch2 provides some tools that make the job of external test
runners easier. [See the relevant section in our page on best
practices](usage-tips.md#parallel-tests).


## Can I compile Catch2 into a dynamic library?

Yes, Catch2 supports the [standard CMake `BUILD_SHARED_LIBS`
option](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html).
However, the dynamic library support is provided as-is. Catch2 does not
provide API export annotations, and so you can only use it as a dynamic
library on platforms that default to public visibility, or with tooling
support to force export Catch2's API.


---

[Home](Readme.md#top)
