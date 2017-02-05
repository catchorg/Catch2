# Known limitations

Catch has some known limitations, that we are not planning to change. Some of these are caused by our desire to support C++98 compilers, some of these are caused by our desire to keep Catch crossplatform and some exist because their priority is seen as low compared to the development effort they would need.

## Thread safe assertions
Because threading support in standard C++98 is limited (well, non-existent), assertion macros in Catch are not thread safe. This does not mean that you cannot use threads inside Catch's test, but that only single thread can interact with Catch's assertions and other macros.

This means that this is ok
```cpp
    std::vector<std::thread> threads;
    std::atomic<int> cnt{ 0 };
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&]() {
            ++cnt; ++cnt; ++cnt; ++cnt;
        });
    }
    for (auto& t : threads) { t.join(); }
    REQUIRE(cnt == 16);
```
because only one thread passes the `REQUIRE` macro and this is not
```cpp
    std::vector<std::thread> threads;
    std::atomic<int> cnt{ 0 };
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&]() {
            ++cnt; ++cnt; ++cnt; ++cnt;
            CHECK(cnt == 16);
        });
    }
    for (auto& t : threads) { t.join(); }
    REQUIRE(cnt == 16);
```


_This limitation is highly unlikely to be lifted before Catch 2 is released._

## Process isolation in a test
Catch does not support running tests in isolated (forked) processes. While this might in the future, the fact that Windows does not support forking and only allows full-on process creation and the desire to keep code as similar as possible across platforms, mean that this is likely to take significant development time, that is not currently available.

## Running multiple tests in parallel
Catch's test execution is strictly serial. If you find yourself with a test suite that takes too long to run and you want to make it parallel, there are 2 feasible solutions
 * You can split your tests into multiple binaries and then run these binaries in parallel.
 * You can have Catch list contained test cases and then run the same test binary multiple times in parallel, passing each instance list of test cases it should run.

Both of these solutions have their problems, but should let you wring parallelism out of your test suite.
