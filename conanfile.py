#!/usr/bin/env python
from conans import ConanFile, CMake


class CatchConan(ConanFile):
    name = "Catch"
    version = "2.4.1"
    description = "A modern, C++-native, header-only, framework for unit-tests, TDD and BDD"
    author = "philsquared"
    generators = "cmake"
    # Only needed until conan 1.5 is released
    settings = "compiler", "arch"
    exports_sources = "single_include/*", "CMakeLists.txt", "CMake/catch2.pc.in", "LICENSE.txt"
    url = "https://github.com/catchorg/Catch2"
    license = "Boost Software License - Version 1.0. http://www.boost.org/LICENSE_1_0.txt"

    def build(self):
        pass

    def package(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_TESTING"] = "OFF"
        cmake.definitions["CATCH_INSTALL_DOCS"] = "OFF"
        cmake.definitions["CATCH_INSTALL_HELPERS"] = "ON"
        cmake.configure()
        cmake.install()

        self.copy(pattern="LICENSE.txt", dst="licenses")

    def package_id(self):
        self.info.header_only()
