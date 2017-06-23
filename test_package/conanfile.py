#!/usr/bin/env python
from os import getenv
from conans import ConanFile
from conans import CMake


class CatchConanTest(ConanFile):
    """Build test using target package and execute all tests
    """
    target = "Catch"
    name = "%s-test" % target
    version = "1.9.5"
    description = "A modern, C++-native, header-only, framework for unit-tests, TDD and BDD"
    author = "philsquared"
    generators = "cmake"
    settings = "os", "compiler", "arch", "build_type"
    url = "https://github.com/philsquared/Catch"
    license = "BSL-1.0"
    username = getenv("CONAN_USERNAME", author)
    channel = getenv("CONAN_CHANNEL", "testing")
    requires = "%s/%s@%s/%s" % (target, version, username, channel)

    def build(self):
        cmake = CMake(self)
        cmake.configure(build_dir="./")
        cmake.build()

    def test(self):
        cmake = CMake(self)
        cmake.configure(build_dir="./")
        cmake.test()
