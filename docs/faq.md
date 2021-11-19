<a id="top"></a>
# Frequently Asked Questions (FAQ)

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


---

[Home](Readme.md#top)
