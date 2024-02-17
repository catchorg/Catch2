#!/usr/bin/env python
from conan import ConanFile, tools, __version__ as conan_version
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools import files, scm
import os
import shutil
import re

required_conan_version = ">=1.53.0"

class CatchConan(ConanFile):
    name = "catch2"
    description = "A modern, C++-native, framework for unit-tests, TDD and BDD"
    topics = ("conan", "catch2", "unit-test", "tdd", "bdd")
    url = "https://github.com/catchorg/Catch2"
    homepage = url
    license = "BSL-1.0"
    version = "latest"

    exports = "LICENSE.txt"
    exports_sources = ("src/*", "CMakeLists.txt", "CMake/*", "extras/*")

    settings = "os", "compiler", "build_type", "arch"

    def set_version(self):
        pattern = re.compile(r"\w*VERSION (\d+\.\d+\.\d+) # CML version placeholder, don't delete")
        with open("CMakeLists.txt") as file:
            for line in file:
                result = pattern.search(line)
                if result:
                    self.version = result.group(1)

        self.output.info(f'Using version: {self.version}')

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def _configure_cmake(self):
        cmake = CMake(self)

        # These are option variables. The toolchain in conan 2 doesn't appear to
        # set these correctly so you have to do it in the configure variables.
        cmake.configure(variables= {
            "BUILD_TESTING": "OFF",
            "CATCH_INSTALL_DOCS": "OFF",
            "CATCH_INSTALL_EXTRAS": "ON",
            }
        )
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

        os.mkdir(f'{self.package_folder}/licenses/')
        shutil.copy2(f'{self.recipe_folder}/LICENSE.txt', f'{self.package_folder}/licenses/')

    def package_info(self):
        lib_suffix = "d" if self.settings.build_type == "Debug" else ""

        self.cpp_info.set_property("cmake_file_name", "Catch2")
        self.cpp_info.set_property("cmake_target_name", "Catch2::Catch2WithMain")
        self.cpp_info.set_property("pkg_config_name", "catch2-with-main")

        # Catch2
        self.cpp_info.components["catch2base"].set_property("cmake_file_name", "Catch2::Catch2")
        self.cpp_info.components["catch2base"].set_property("pkg_config_name", "catch2")
        self.cpp_info.components["catch2base"].libs = ["Catch2" + lib_suffix]
        self.cpp_info.components["catch2base"].builddirs.append("lib/cmake/Catch2")

        # Catch2WithMain
        self.cpp_info.components["catch2main"].set_property("cmake_file_name", "Catch2::Catch2WithMain")
        self.cpp_info.components["catch2main"].set_property("cmake_target_name", "Catch2::Catch2WithMain")
        self.cpp_info.components["catch2main"].set_property("pkg_config_name", "catch2-with-main")
        self.cpp_info.components["catch2main"].libs = ["Catch2Main" + lib_suffix]
        self.cpp_info.components["catch2main"].requires = ["catch2base"]