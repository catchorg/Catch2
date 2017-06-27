#!/usr/bin/env python
from conans import ConanFile


class CatchConan(ConanFile):
    name = "Catch"
    version = "1.9.5"
    description = "A modern, C++-native, header-only, framework for unit-tests, TDD and BDD"
    author = "philsquared"
    generators = "cmake"
    exports_sources = "single_include/*"
    url = "https://github.com/philsquared/Catch"
    license = "BSL-1.0"

    def package(self):
        self.copy(pattern="catch.hpp", src="single_include", dst="include")
