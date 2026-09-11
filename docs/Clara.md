# Clara v1.1.5


A simple to use, composable, command line parser for C++ 11 and beyond.



A parser for a single option can be created like this:

```c++
int width = 0;
// ...
using namespace clara;
auto cli
    = Opt( width, "width" )
        ["-w"]["--width"]
        ("How wide should it be?");
```

You can use this parser directly like this:

```c++
auto result = cli.parse( Args( argc, argv ) );
if( !result ) {
    std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
    exit(1);
}

// Everything was ok, width will have a value if supplied on command line
```

Note that exceptions are not used for error handling.

You can combine parsers by composing with `|`, like this:

```c++
int width = 0;
std::string name;
bool doIt = false;
std::string command;
auto cli
    = Opt( width, "width" )
        ["-w"]["--width"]
        ("How wide should it be?")
    | Opt( name, "name" )
        ["-n"]["--name"]
        ("By what name should I be known")
    | Opt( doIt )
        ["-d"]["--doit"]
        ("Do the thing" )
    | Arg( command, "command" )
        ("which command to run");
```

`Opt`s specify options that start with a short dash (`-`) or long dash (`--`).
On Windows forward slashes are also accepted (and automatically interpretted as a short dash).
Options can be argument taking (such as `-w 42`), in which case the `Opt` takes a second argument - a hint,
or they are pure flags (such as `-d`), in which case the `Opt` has only one argument - which must be a boolean.
The option names are provided in one or more sets of square brackets, and a description string can
be provided in parentheses. The first argument to an `Opt` is any variable, local, global member, of any type
that can be converted from a string using `std::ostream`.

`Arg`s specify arguments that are not tied to options, and so have no square bracket names. They otherwise work just like `Opt`s.

A, console optimised, usage string can be obtained by inserting the parser into a stream.
The usage string is built from the information supplied and is formatted for the console width.

As a convenience, the standard help options (`-h`, `--help` and `-?`) can be specified using the `Help` parser,
which just takes a boolean to bind to.

For more usage please see the unit tests or look at how it is used in the Catch code-base (catch-lib.net).
Fuller documentation will be coming soon.

Some of the key features:

- A single header file with no external dependencies (except the std library).
- Define your interface once to get parsing, type conversions and usage strings with no redundancy.
- Composable. Each `Opt` or `Arg` is an independent parser. Combine these to produce a composite parser - this can be done in stages across multiple function calls - or even projects.
- Bind parsers directly to variables that will receive the results of the parse - no intermediate dictionaries to worry about.
- Or can also bind parsers to lambdas for more custom handling.
- Deduces types from bound variables or lambdas and performs type conversions (via `ostream <<`), with error handling, behind the scenes.
- Bind parsers to vectors for args that can have multiple values.
- Uses Result types for error propagation, rather than exceptions (doesn't yet build with exceptions disabled, but that will be coming later)
- Models POSIX standards for short and long opt behaviour.


## Old version

If you used the earlier, v0.x, version of Clara please note that this is a complete rewrite which assumes C++11 and has
a different interface (composability was a big step forward). Conversion between v0.x and v1.x is a fairly simple and mechanical task, but is a bit of manual
work - so don't take this version until you're ready (and, of course, able to use C++11).

I hope you'll find the new interface an improvement - and this will be built on to offer new features moving forwards.
I don't expect to maintain v0.x any further, but it remains on a branch.
