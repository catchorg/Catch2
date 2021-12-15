
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/catch_session.hpp>
#include <catch2/internal/catch_compiler_capabilities.hpp>
#include <catch2/internal/catch_config_wchar.hpp>
#include <catch2/internal/catch_leak_detector.hpp>
#include <catch2/internal/catch_platform.hpp>

namespace Catch {
    CATCH_INTERNAL_START_WARNINGS_SUPPRESSION
    CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS
    LeakDetector leakDetector;
    CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION
}

// Allow users of amalgamated .cpp file to remove our main and provide their own.
#if !defined(CATCH_AMALGAMATED_CUSTOM_MAIN)

#if defined(CATCH_CONFIG_WCHAR) && defined(CATCH_PLATFORM_WINDOWS) && defined(_UNICODE) && !defined(DO_NOT_USE_WMAIN)
// Standard C/C++ Win32 Unicode wmain entry point
extern "C" int __cdecl wmain (int argc, wchar_t * argv[], wchar_t * []) {
#else
// Standard C/C++ main entry point
int main (int argc, char * argv[]) {
#endif

    // We want to force the linker not to discard the global variable
    // and its constructor, as it (optionally) registers leak detector
    (void)&Catch::leakDetector;

    Catch::Session session;

#if defined(CATCH_CONFIG_MAIN_BAZEL_JUNIT_XML_OUTPUT_FILE)
    // Bazel expects test output to be written to this file in JUnit format.
    auto bazelOutputFile = std::getenv("XML_OUTPUT_FILE");
    if (bazelOutputFile != nullptr) {
        session.configData().reporterName = "junit";
        session.configData().outputFilename = bazelOutputFile;
    }
#endif

    return session.run( argc, argv );
}

#endif // !defined(CATCH_AMALGAMATED_CUSTOM_MAIN
