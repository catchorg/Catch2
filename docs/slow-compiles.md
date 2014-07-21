# Why do my tests take so long to compile?

Several people have reported that test code written with Catch takes much longer to compile than they would expect. Why is that?
The answer comes in three parts:

1. Catch is implemented entirely in headers. There is a little overhead due to this - but not as much as you'd think. Read on for more detail on this one.
2. Most of the "implementation" of Catch is conditionally compiled into a single translation unit. If all your tests are in this file you are rebuilding the whole of Catch every time. Read on for how to avoid this.
3. Even after taking the above into consideration some people still report slower compiles than they'd like. At this point the reasons are unknown - although the main suspect is some supporting template code necessary to workaround the way the compiler treats integer literals differently when inferring them as template arguments vs. in bare expressions.

Although Catch is implemented entirely in headers (and even stitched together into a single header) it is internally organised in a similar way to the more traditional split between header and "implementation" files. The implementation portion is conditionally compiled into only one translation unit - which is the one that ```#define```s ```CATCH_CONFIG_RUNNER``` or ```CATCH_CONFIG_MAIN```. Putting all your tests into the same file that also compiles the implementation is a convenient way to get started - and makes for good demos - but beyond that it is better to dedicate an implementation file for just #including Catch for the implementation. If you're using ```CATCH_CONFIG_MAIN``` it may just be as simple as that and the #include in the entire file. If you're providing your own ```main()``` you'll probably want to put that in there too. But no test cases.

Write your test code in other translation units (cpp files) and when you incrementally build you won't be rebuilding the whole of Catch every time.

---

[Home](../README.md)