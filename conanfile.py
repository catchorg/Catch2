#!/usr/bin/env python
from conans import ConanFile


class CatchConan(ConanFile):
    name = "Catch"
    version = "1.10.0"
    description = "A modern, C++-native, header-only, framework for unit-tests, TDD and BDD"
    author = "philsquared"
    generators = "cmake"
    exports_sources = "single_include/*"
    url = "https://github.com/philsquared/Catch"
    license = "Boost Software License - Version 1.0. http://www.boost.org/LICENSE_1_0.txt"

    def package(self):
        self.copy(pattern="catch.hpp", src="single_include", dst="include")
    
    def package_id(self):
            self.info.header_only()
