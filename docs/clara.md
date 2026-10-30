<a id="top"></a>
# Clara - Command Line Parser

Clara is the command line parser used internally by Catch2. It was originally
a separate project, but has been integrated into Catch2 and is now developed
as part of the Catch2 codebase.

**Contents**<br>
[Overview](#overview)<br>
[Options](#options)<br>
[Arguments](#arguments)<br>
[Combining Parsers](#combining-parsers)<br>
[Using with Catch2's Session](#using-with-catch2s-session)<br>

## Overview

Clara provides a simple, composable way to define command line parsers.
The main building blocks are:

* `Opt` - for options (flags and options with values)
* `Arg` - for positional arguments
* `Parser` - for combining multiple `Opt`s and `Arg`s

All Clara types are in the `Catch::Clara` namespace.

## Options

Options are defined using the `Opt` class. An option can be a simple flag
(boolean) or can take a value.

### Boolean Flags

```cpp
bool showHelp = false;
auto cli = Catch::Clara::Opt(showHelp)
    ["-h"]["--help"]
    ("Show help");
```

### Options with Values

```cpp
int width = 80;
auto cli = Catch::Clara::Opt(width, "width")
    ["-w"]["--width"]
    ("Set the width");
```

The second parameter to `Opt` is a "hint" that appears in the help text.

### Lambda Handlers

You can also use a lambda to handle the option value:

```cpp
std::string name;
auto cli = Catch::Clara::Opt([&](std::string const& n) { name = n; }, "name")
    ["-n"]["--name"]
    ("Set the name");
```

Lambdas can return `void` or `Catch::Clara::ParserResult` for custom
validation:

```cpp
int value = 0;
auto cli = Catch::Clara::Opt([&](int v) -> Catch::Clara::ParserResult {
        if (v < 0)
            return Catch::Clara::ParserResult::runtimeError("Value must be non-negative");
        value = v;
        return Catch::Clara::ParserResult::ok(Catch::Clara::ParseResultType::Matched);
    }, "value")
    ["-v"]["--value"]
    ("Set a non-negative value");
```

### Required Options

By default, options are optional. You can mark an option as required:

```cpp
std::string config;
auto cli = Catch::Clara::Opt(config, "file")
    ["-c"]["--config"]
    ("Config file (required)")
    .required();
```

## Arguments

Positional arguments are defined using the `Arg` class:

```cpp
std::string filename;
auto cli = Catch::Clara::Arg(filename, "filename")
    ("The input file");
```

### Multiple Arguments

To accept multiple values, bind to a `std::vector`:

```cpp
std::vector<std::string> files;
auto cli = Catch::Clara::Arg(files, "files")
    ("Input files");
```

## Combining Parsers

Parsers can be combined using the `|` operator:

```cpp
bool verbose = false;
int count = 1;
std::string name;

auto cli
    = Catch::Clara::Opt(verbose)
        ["-v"]["--verbose"]
        ("Enable verbose output")
    | Catch::Clara::Opt(count, "count")
        ["-c"]["--count"]
        ("Number of iterations")
    | Catch::Clara::Arg(name, "name")
        ("The name to use");
```

## Using with Catch2's Session

When [supplying your own main](own-main.md#top), you can extend Catch2's
parser with your own options:

```cpp
#include <catch2/catch_session.hpp>

int main(int argc, char* argv[]) {
    Catch::Session session;

    int height = 0;

    // Build a new parser on top of Catch2's
    using namespace Catch::Clara;
    auto cli
        = session.cli()           // Get Catch2's command line parser
        | Opt(height, "height")   // bind variable to a new option
            ["-g"]["--height"]    // the option names it will respond to
            ("how high?");        // description string for the help output

    // Now pass the new composite back to Catch2 so it uses that
    session.cli(cli);

    // Let Catch2 (using Clara) parse the command line
    int returnCode = session.applyCommandLine(argc, argv);
    if (returnCode != 0)
        return returnCode;

    // if set on the command line then 'height' is now set at this point
    if (height > 0)
        std::cout << "height: " << height << std::endl;

    return session.run();
}
```

---

[Home](Readme.md#top)
