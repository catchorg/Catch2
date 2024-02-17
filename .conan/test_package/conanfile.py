#!/usr/bin/env python
# -*- coding: utf-8 -*-
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import can_run
import os


class TestPackageConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "test_package")
            self.run(cmd, env="conanrun")

        # If we are on conan 2 we can check the license info is populated
        if hasattr(self, 'dependencies'):
            catch2 = self.dependencies["catch2"]
            assert os.path.exists(f'{catch2.package_folder}/licenses/LICENSE.txt')
            assert catch2.license == 'BSL-1.0'
