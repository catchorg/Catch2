<a id="top"></a>
![Catch2 logo](data/artwork/catch2-logo-small.png)

[![Github Releases](https://img.shields.io/github/release/catchorg/catch2.svg)](https://github.com/catchorg/catch2/releases)
[![Linux build status](https://github.com/catchorg/Catch2/actions/workflows/linux-simple-builds.yml/badge.svg)](https://github.com/catchorg/Catch2/actions/workflows/linux-simple-builds.yml)
[![Linux build status](https://github.com/catchorg/Catch2/actions/workflows/linux-other-builds.yml/badge.svg)](https://github.com/catchorg/Catch2/actions/workflows/linux-other-builds.yml)
[![MacOS build status](https://github.com/catchorg/Catch2/actions/workflows/mac-builds.yml/badge.svg)](https://github.com/catchorg/Catch2/actions/workflows/mac-builds.yml)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/catchorg/Catch2?svg=true&branch=devel)](https://ci.appveyor.com/project/catchorg/catch2)
[![Code Coverage](https://codecov.io/gh/catchorg/Catch2/branch/devel/graph/badge.svg)](https://codecov.io/gh/catchorg/Catch2)
[![Try online](https://img.shields.io/badge/try-online-blue.svg)](https://godbolt.org/z/EdoY15q9G)
[![Join the chat in Discord: https://discord.gg/4CWS9zD](https://img.shields.io/badge/Discord-Chat!-brightgreen.svg)](https://discord.gg/4CWS9zD)


## What's the Catch2?

Catch2 is mainly a unit testing framework for C++, but it also
provides basic micro-benchmarking features, and simple BDD macros.

Catch2's main advantage is that using it is both simple and natural.
Tests autoregister themselves and do not have to be named with valid
identifiers, assertions look like normal C++ code, and sections provide
a nice way to share set-up and tear-down code in tests.


## Catch2 v3 is being developed!

You are on the `devel` branch, where the next major version, v3, of
Catch2 is being developed. As it is a significant rework, you will
find that parts of this documentation are likely still stuck on v2.

For stable (and documentation-matching) version of Catch2, [go to the
`v2.x` branch](https://github.com/catchorg/Catch2/tree/v2.x).

For migrating from the v2 releases to v3, you should look at [our
documentation](docs/migrate-v2-to-v3.md#top). It provides a simple
guidelines on getting started, and collects most common migration
problems.


## How to use it
This documentation comprises these three parts:

* [Why do we need yet another C++ Test Framework?](docs/why-catch.md#top)
* [Tutorial](docs/tutorial.md#top) - getting started
* [Reference section](docs/Readme.md#top) - all the details


## More
* Issues and bugs can be raised on the [Issue tracker on GitHub](https://github.com/catchorg/Catch2/issues)
* For discussion or questions please use [our Discord](https://discord.gg/4CWS9zD)
* See who else is using Catch2 in [Open Source Software](docs/opensource-users.md#top)
or [commercially](docs/commercial-users.md#top).
