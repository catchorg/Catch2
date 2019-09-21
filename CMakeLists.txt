cmake_minimum_required(VERSION 3.5)
project(Catch2 LANGUAGES CXX VERSION 2.9.2)

# Provide path for scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(GNUInstallDirs)
include(CTest)

find_program(CATCH_DPKG_BUILDPACKAGE_FOUND dpkg-buildpackage)

if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported!"
      "Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeFiles/"
      "and ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()

cmake_dependent_option(CATCH_BUILD_PACKAGE
  "Enable Catch2 package building" ON
  "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)
cmake_dependent_option(CATCH_BUILD_PACKAGE_DEB
  "Enable creating a Catch2 .deb" ON
  "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR;CATCH_DPKG_BUILDPACKAGE_FOUND" OFF)

cmake_dependent_option(CATCH_BUILD_TESTING
  "Build Catch2 SelfTest project" ON
  "BUILD_TESTING;CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)
cmake_dependent_option(CATCH_ENABLE_WERROR
  "Enable all warnings as errors" ON
  "BUILD_TESTING;CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)
cmake_dependent_option(CATCH_INSTALL_HELPERS
  "Install Catch2 contrib alonside library" ON
  "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

cmake_dependent_option(CATCH_INSTALL_DOCS
  "Install Catch2 documentation alongside library" ON
  "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

option(CATCH_USE_VALGRIND "Perform SelfTests with Valgrind" OFF)
option(CATCH_BUILD_EXAMPLES "Build documentation examples" OFF)
option(CATCH_BUILD_EXTRA_TESTS "Build extra tests" OFF)
option(CATCH_ENABLE_COVERAGE "Generate coverage for codecov.io" OFF)


# define some folders
set(CATCH_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SELF_TEST_DIR ${CATCH_DIR}/projects/SelfTest)
set(BENCHMARK_DIR ${CATCH_DIR}/projects/Benchmark)
set(HEADER_DIR ${CATCH_DIR}/include)

if(USE_WMAIN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:wmainCRTStartup")
endif()

if (BUILD_TESTING AND CATCH_BUILD_TESTING AND NOT_SUBPROJECT)
    find_package(PythonInterp)
    if (NOT PYTHONINTERP_FOUND)
        message(FATAL_ERROR "Python not found, but required for tests")
    endif()
    add_subdirectory(projects)
endif()

if(CATCH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(CATCH_BUILD_EXTRA_TESTS)
    add_subdirectory(projects/ExtraTests)
endif()

# add catch as a 'linkable' target
add_library(Catch2 INTERFACE)
# provide a namespaced alias for clients to 'link' against if catch is included
# as a sub-project
add_library(Catch2::Catch2 ALIAS Catch2)


# depend on some obvious c++11 features so the dependency is transitively
# added to dependents
target_compile_features(Catch2
  INTERFACE
    cxx_alignas
    cxx_alignof
    cxx_attributes
    cxx_auto_type
    cxx_constexpr
    cxx_defaulted_functions
    cxx_deleted_functions
    cxx_final
    cxx_lambdas
    cxx_noexcept
    cxx_override
    cxx_range_for
    cxx_rvalue_references
    cxx_static_assert
    cxx_strong_enums
    cxx_trailing_return_types
    cxx_unicode_literals
    cxx_user_literals
    cxx_variadic_macros
)

target_include_directories(Catch2
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/single_include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(Catch2 INTERFACE $<$<PLATFORM_ID:Android>:log>)

if (NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  return()
endif()

# Only perform the installation steps when Catch is not being used as
# a subproject via `add_subdirectory`, or the destinations will break,
# see https://github.com/catchorg/Catch2/issues/1373
set(CATCH_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Catch2")

configure_file(CMake/catch2.pc.in catch2.pc @ONLY)
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/CMake/Catch2Config.cmake.in
  ${PROJECT_BINARY_DIR}/Catch2Config.cmake
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  NO_SET_AND_CHECK_MACRO
  INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}")


# create and install an export set for catch target as Catch2::Catch
install(TARGETS Catch2 EXPORT Catch2Targets)
install(EXPORT Catch2Targets
  NAMESPACE Catch2::
  DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}")

# By default, FooConfigVersion is tied to architecture that it was
# generated on. Because Catch2 is header-only, it is arch-independent
# and thus Catch2ConfigVersion should not be tied to the architecture
# it was generated on.
#
# CMake does not provide a direct customization point for this in
# `write_basic_package_version_file`, but it can be accomplished
# indirectly by temporarily redefining `CMAKE_SIZEOF_VOID_P` to an
# empty string. Note that just undefining the variable could be
# insufficient in cases where the variable was already in CMake cache
# XXX: CMake 3.14 adds an ARCH_INDEPENDENT option that can replace this
set(CATCH2_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
set(CMAKE_SIZEOF_VOID_P "")
write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/Catch2ConfigVersion.cmake"
  COMPATIBILITY SameMajorVersion)
set(CMAKE_SIZEOF_VOID_P ${CATCH2_CMAKE_SIZEOF_VOID_P})

install(DIRECTORY "single_include/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(
  FILES
    "${PROJECT_BINARY_DIR}/Catch2Config.cmake"
    "${PROJECT_BINARY_DIR}/Catch2ConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}
)

# Install documentation
if(CATCH_INSTALL_DOCS)
  install(DIRECTORY docs/ DESTINATION "${CMAKE_INSTALL_DOCDIR}")
endif()

if(CATCH_INSTALL_HELPERS)
  # Install CMake scripts
  install(DIRECTORY contrib/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}
    FILES_MATCHING PATTERN "*.cmake")
  
  # Install debugger helpers
  install(DIRECTORY contrib/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/Catch2
    FILES_MATCHING PATTERN "*init")
endif()

install(FILES "${PROJECT_BINARY_DIR}/catch2.pc"
  DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgconfig)

if (NOT CATCH_BUILD_PACKAGE)
  return()
endif()

if (CATCH_BUILD_PACKAGE_DEB)
  list(APPEND binary-generators "DEB")
endif()

if (CMAKE_VERSION VERSION_LESS 3.12)
  set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
endif()

set(CPACK_GENERATOR ${binary-generators})
set(CPACK_PACKAGE_CONTACT "https://github.com/catchorg/Catch2/")

set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILENAME}")

list(APPEND CPACK_SOURCE_IGNORE_FILES /.git/ /build/ .gitignore [.]DS_Store)

include(CPack)
