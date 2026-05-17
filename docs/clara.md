# Clara (command-line parsing)

Catch2 embeds **Clara** as `Catch::Clara` (`catch2/internal/catch_clara.hpp`). The
standalone [Clara repository](https://github.com/catchorg/Clara) is archived;
new development happens only inside Catch2.

## Getting started

- [Adding your own command line options](own-main.md#adding-your-own-command-line-options) —
  extend Catch2's CLI with custom flags
- [232-Cfg-CustomMain.cpp](../examples/232-Cfg-CustomMain.cpp) — full custom-`main`
  example composing `Catch::Clara` with `Catch::Session`
- [command-line.md](command-line.md) — built-in Catch2 flags and reporters
