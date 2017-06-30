#!/usr/bin/env python
from os import getenv
from conan.packager import ConanMultiPackager


if __name__ == "__main__":
    username = getenv("CONAN_USERNAME", "philsquared")
    builder = ConanMultiPackager(username=username)
    builder.add_common_builds(pure_c=False)
    builder.run()
