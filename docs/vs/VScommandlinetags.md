If you've used the Catch command line, you might know that Catch can selectively include and exclude tests using tags.  For example, you might want to exclude long running tests from each check-in to your CI server, so you might tag those tests with `[slow]` and run Catch with `example.exe ~[slow]`.

It doesn't look like it's possible to reproduce the flexibility of Catch but it is possible to do some basic filtering using Catch tags.
  

---

[Home](../../README.md)