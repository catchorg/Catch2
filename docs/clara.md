<a id="top"></a>
# Clara (Catch2 CLI parser)

Catch2 embeds its own CLI parser, historically called **Clara**. The
standalone [catchorg/Clara](https://github.com/catchorg/Clara) repository
is archived; Clara is maintained only inside Catch2
(`src/catch2/internal/catch_clara.hpp`).

This page documents the pieces you need when composing custom options
on top of Catch2's parser (see also [Supplying your own main()](own-main.md#top)).

## Namespace

Custom option code typically uses:

```cpp
using namespace Catch::Clara;
```

## Building a parser

Catch2's session exposes its parser via `session.cli()`. You can extend
it with `|` composition, then install the composite back with
`session.cli( cli )` before `applyCommandLine`.

```cpp
int height = 0;
auto cli
  = session.cli()
  | Opt( height, "height" )
      ["-g"]["--height"]
      ("how high?");
session.cli( cli );
```

## `Opt` — named options

* Bind a variable: `Opt( value, "hint" )`
* Bind a unary lambda: `Opt( []( std::string const& v ){ ... }, "hint" )`
* Bind a bool flag: `Opt( flag )` or `Opt( []( bool ){ ... } )`
* Names: `["-s"]["--long"]`
* Description for `--help`: `("description text")`

Multiple names on one `Opt` are aliases. The hint string is shown in help
as the option argument placeholder.

## `Arg` — positional arguments

`Arg` parses free-standing tokens (not `-`-prefixed options). Bind the
same way as `Opt` (variable or lambda + optional hint), then compose with
`|`.

## `Help`

`Help( showHelpFlag )` wires the conventional `-h` / `--help` option that
sets a `bool` when help is requested.

## Validation and errors

After `session.applyCommandLine( argc, argv )`, a non-zero return value
means the command line was invalid (or help was printed, depending on
configuration). Check that return code before calling `session.run()`.

## Further reading

* [Supplying your own main()](own-main.md#top) — full worked example
* [Command line reference](command-line.md#top) — Catch2's built-in options
