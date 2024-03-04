#!/usr/bin/env python
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy, rmdir
from conan.tools.build import check_min_cppstd
import os
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
    settings = "os", "compiler", "build_type", "arch"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
    }

    def set_version(self):
        pattern = re.compile(r"\w*VERSION (\d+\.\d+\.\d+) # CML version placeholder, don't delete")
        with open("CMakeLists.txt") as file:
            for line in file:
                result = pattern.search(line)
                if result:
                    self.version = result.group(1)

        self.output.info(f'Using version: {self.version}')

    def export(self):
        copy(self, "LICENSE.txt", src=self.recipe_folder, dst=self.export_folder)

    def export_sources(self):
        copy(self, "CMakeLists.txt", src=self.recipe_folder, dst=self.export_sources_folder)
        copy(self, "src/*", src=self.recipe_folder, dst=self.export_sources_folder)
        copy(self, "extras/*", src=self.recipe_folder, dst=self.export_sources_folder)
        copy(self, "CMake/*", src=self.recipe_folder, dst=self.export_sources_folder)

    def layout(self):
        cmake_layout(self)

    def validate(self):
        if self.settings.compiler.get_safe("cppstd"):
            check_min_cppstd(self, 17)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["BUILD_TESTING"] = False
        tc.cache_variables["CATCH_INSTALL_DOCS"] = False
        tc.cache_variables["CATCH_INSTALL_EXTRAS"] = True
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "LICENSE.txt", src=str(self.recipe_folder), dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "share"))
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))
        for cmake_file in ["ParseAndAddCatchTests.cmake", "Catch.cmake", "CatchAddTests.cmake"]:
            copy(
                self,
                cmake_file,
                src=os.path.join(self.export_sources_folder, "extras"),
                dst=os.path.join(self.package_folder, "lib", "cmake", "Catch2"),
            )

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
