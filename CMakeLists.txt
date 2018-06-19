cmake_minimum_required(VERSION 3.5)

# detect if Catch is being bundled,
# disable testsuite in that case
if(NOT DEFINED PROJECT_NAME)
  set(NOT_SUBPROJECT ON)
endif()

project(Catch2 LANGUAGES CXX VERSION 2.2.3)

# Provide path for scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

option(CATCH_USE_VALGRIND "Perform SelfTests with Valgrind" OFF)
option(CATCH_BUILD_EXAMPLES "Build documentation examples" OFF)
option(CATCH_ENABLE_COVERAGE "Generate coverage for codecov.io" OFF)
option(CATCH_ENABLE_WERROR "Enable all warnings as errors" ON)
option(CATCH_INSTALL_DOCS "Install documentation alongside library" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# define some folders
set(CATCH_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SELF_TEST_DIR ${CATCH_DIR}/projects/SelfTest)
set(BENCHMARK_DIR ${CATCH_DIR}/projects/Benchmark)
set(HEADER_DIR ${CATCH_DIR}/include)

if(USE_WMAIN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:wmainCRTStartup")
endif()

if (BUILD_TESTING AND NOT_SUBPROJECT)
    add_subdirectory(projects)
endif()

if(CATCH_BUILD_EXAMPLES)
    add_subdirectory(examples)
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

# provide a namespaced alias for clients to 'link' against if catch is included as a sub-project
add_library(Catch2::Catch2 ALIAS Catch2)

set(CATCH_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Catch2")

# create and install an export set for catch target as Catch2::Catch
install(
  TARGETS
    Catch2
  EXPORT
    Catch2Config
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  EXPORT
    Catch2Config
  NAMESPACE
    Catch2::
  DESTINATION
    ${CATCH_CMAKE_CONFIG_DESTINATION}
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/Catch2ConfigVersion.cmake"
  COMPATIBILITY
    SameMajorVersion
)

install(
  DIRECTORY
    "single_include/"
  DESTINATION
    "${CMAKE_INSTALL_INCLUDEDIR}"
)
    
install(
  FILES 
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
