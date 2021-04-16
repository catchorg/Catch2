cmake_minimum_required(VERSION 3.5)

# detect if Catch is being bundled,
# disable testsuite in that case
if(NOT DEFINED PROJECT_NAME)
    set(NOT_SUBPROJECT ON)
else()
    set(NOT_SUBPROJECT OFF)
endif()

# Catch2's build breaks if done in-tree. You probably should not build
# things in tree anyway, but we can allow projects that include Catch2
# as a subproject to build in-tree as long as it is not in our tree.
if (CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(FATAL_ERROR "Building in-source is not supported! Create a build dir and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif()


project(Catch2 LANGUAGES CXX VERSION 2.13.6)

# Provide path for scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

include(GNUInstallDirs)

option(CATCH_USE_VALGRIND "Perform SelfTests with Valgrind" OFF)
option(CATCH_BUILD_TESTING "Build SelfTest project" ON)
option(CATCH_BUILD_EXAMPLES "Build documentation examples" OFF)
option(CATCH_BUILD_EXTRA_TESTS "Build extra tests" OFF)
option(CATCH_BUILD_STATIC_LIBRARY "Builds static library from the main implementation. EXPERIMENTAL" OFF)
option(CATCH_ENABLE_COVERAGE "Generate coverage for codecov.io" OFF)
option(CATCH_ENABLE_WERROR "Enable all warnings as errors" ON)
option(CATCH_INSTALL_DOCS "Install documentation alongside library" ON)
option(CATCH_INSTALL_HELPERS "Install contrib alongside library" ON)

# define some folders
set(CATCH_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SELF_TEST_DIR ${CATCH_DIR}/projects/SelfTest)
set(BENCHMARK_DIR ${CATCH_DIR}/projects/Benchmark)
set(HEADER_DIR ${CATCH_DIR}/include)

if(USE_WMAIN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:wmainCRTStartup")
endif()

if(NOT_SUBPROJECT)
    include(CTest)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    if(BUILD_TESTING AND CATCH_BUILD_TESTING)
        find_package(PythonInterp)
        if (NOT PYTHONINTERP_FOUND)
            message(FATAL_ERROR "Python not found, but required for tests")
        endif()
        add_subdirectory(projects)
    endif()
endif()

if(CATCH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(CATCH_BUILD_EXTRA_TESTS)
    add_subdirectory(projects/ExtraTests)
endif()

# add catch as a 'linkable' target
add_library(Catch2 INTERFACE)



# depend on some obvious c++11 features so the dependency is transitively added dependents
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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/single_include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if (ANDROID)
    target_link_libraries(Catch2 INTERFACE log)
endif()

# provide a namespaced alias for clients to 'link' against if catch is included as a sub-project
add_library(Catch2::Catch2 ALIAS Catch2)

# Hacky support for compiling the impl into a static lib
if (CATCH_BUILD_STATIC_LIBRARY)
  add_library(Catch2WithMain ${CMAKE_CURRENT_LIST_DIR}/src/catch_with_main.cpp)
  target_link_libraries(Catch2WithMain PUBLIC Catch2)
  add_library(Catch2::Catch2WithMain ALIAS Catch2WithMain)

  # Make the build reproducible on versions of g++ and clang that supports -ffile-prefix-map
  if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 8) OR
     ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 10))
    target_compile_options(Catch2WithMain PRIVATE "-ffile-prefix-map=${CMAKE_SOURCE_DIR}=.")
  endif()
endif(CATCH_BUILD_STATIC_LIBRARY)

# Only perform the installation steps when Catch is not being used as
# a subproject via `add_subdirectory`, or the destinations will break,
# see https://github.com/catchorg/Catch2/issues/1373
if (NOT_SUBPROJECT)
    include(CMakePackageConfigHelpers)
    set(CATCH_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Catch2")

    configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/CMake/Catch2Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/Catch2Config.cmake
        INSTALL_DESTINATION
          ${CATCH_CMAKE_CONFIG_DESTINATION}
    )

    # Workaround lack of generator expressions in install(TARGETS
    set(InstallationTargets Catch2)
    if (TARGET Catch2WithMain)
      list(APPEND InstallationTargets Catch2WithMain)
    endif()


    # create and install an export set for catch target as Catch2::Catch
    install(
      TARGETS
        ${InstallationTargets}
      EXPORT
        Catch2Targets
      DESTINATION
        ${CMAKE_INSTALL_LIBDIR}
    )


    install(
      EXPORT
        Catch2Targets
      NAMESPACE
        Catch2::
      DESTINATION
        ${CATCH_CMAKE_CONFIG_DESTINATION}
    )

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
    set(CATCH2_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
    set(CMAKE_SIZEOF_VOID_P "")
    write_basic_package_version_file(
      "${CMAKE_CURRENT_BINARY_DIR}/Catch2ConfigVersion.cmake"
      COMPATIBILITY
        SameMajorVersion
    )
    set(CMAKE_SIZEOF_VOID_P ${CATCH2_CMAKE_SIZEOF_VOID_P})

    install(
      DIRECTORY
        "single_include/"
      DESTINATION
        "${CMAKE_INSTALL_INCLUDEDIR}"
    )

    install(
      FILES
        "${CMAKE_CURRENT_BINARY_DIR}/Catch2Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/Catch2ConfigVersion.cmake"
      DESTINATION
        ${CATCH_CMAKE_CONFIG_DESTINATION}
    )

    # Install documentation
    if(CATCH_INSTALL_DOCS)
      install(
        DIRECTORY
          docs/
        DESTINATION
          "${CMAKE_INSTALL_DOCDIR}"
      )
    endif()

    if(CATCH_INSTALL_HELPERS)
    # Install CMake scripts
    install(
      FILES
        "contrib/ParseAndAddCatchTests.cmake"
        "contrib/Catch.cmake"
        "contrib/CatchAddTests.cmake"
      DESTINATION
        ${CATCH_CMAKE_CONFIG_DESTINATION}
    )

    # Install debugger helpers
    install(
      FILES
        "contrib/gdbinit"
        "contrib/lldbinit"
      DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/Catch2
    )
    endif()

    ## Provide some pkg-config integration
    set(PKGCONFIG_INSTALL_DIR
        "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig"
        CACHE PATH "Path where catch2.pc is installed"
    )
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/CMake/catch2.pc.in
      ${CMAKE_CURRENT_BINARY_DIR}/catch2.pc
      @ONLY
    )
    install(
      FILES
        "${CMAKE_CURRENT_BINARY_DIR}/catch2.pc"
      DESTINATION
        ${PKGCONFIG_INSTALL_DIR}
    )

    # CPack/CMake started taking the package version from project version 3.12
    # So we need to set the version manually for older CMake versions
    if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
        set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
    endif()

    set(CPACK_PACKAGE_CONTACT "https://github.com/catchorg/Catch2/")


    include( CPack )

endif(NOT_SUBPROJECT)
