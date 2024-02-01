#!/usr/bin/env python
from conan import ConanFile, tools
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools import files
import os
import shutil


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
#         # We need this workaround until the toolchains feature
#         # to inject stuff like MD/MT
#         line_to_replace = 'list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")'
#         replacement_text = '''{}
# include("{}/conanbuildinfo.cmake")
# conan_basic_setup()'''.format(line_to_replace, self.package_folder.replace("\\", "/"))

#         files.replace_in_file(self, f"{self.source_folder}/CMakeLists.txt", line_to_replace, replacement_text)

        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

        os.mkdir(f'{self.package_folder}/licenses/')
        shutil.copy2(f'{self.recipe_folder}/LICENSE.txt', f'{self.package_folder}/licenses/')

    def package_info(self):
        lib_suffix = "d" if self.settings.build_type == "Debug" else ""

        self.cpp_info.names["cmake_find_package"] = "Catch2"
        self.cpp_info.names["cmake_find_package_multi"] = "Catch2"
        # Catch2
        self.cpp_info.components["catch2base"].names["cmake_find_package"] = "Catch2"
        self.cpp_info.components["catch2base"].names["cmake_find_package_multi"] = "Catch2"
        self.cpp_info.components["catch2base"].names["pkg_config"] = "Catch2"
        self.cpp_info.components["catch2base"].libs = ["Catch2" + lib_suffix]
        self.cpp_info.components["catch2base"].builddirs.append("lib/cmake/Catch2")
        # Catch2WithMain
        self.cpp_info.components["catch2main"].names["cmake_find_package"] = "Catch2WithMain"
        self.cpp_info.components["catch2main"].names["cmake_find_package_multi"] = "Catch2WithMain"
        self.cpp_info.components["catch2main"].names["pkg_config"] = "Catch2WithMain"
        self.cpp_info.components["catch2main"].libs = ["Catch2Main" + lib_suffix]
        self.cpp_info.components["catch2main"].requires = ["catch2base"]
