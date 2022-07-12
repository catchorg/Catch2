void main()
{
    import std.datetime.stopwatch : benchmark;
    import std.math, std.parallelism, std.stdio;

    auto logs = new double[100_000];
    auto bm = benchmark!({
        foreach (i, ref elem; logs)
            elem = log(1.0 + i);
    }, {
        foreach (i, ref elem; logs.parallel)
            elem = log(1.0 + i);
    })(100); // number of executions of each tested function
    writefln("Linear init: %s msecs", bm[0].total!"msecs");
    writefln("Parallel init: %s msecs", bm[1].total!"msecs");
}

// From https://dlang.org/