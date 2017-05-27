/*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef TWOBLUECUBES_CATCH_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_HPP_INCLUDED

#ifdef __clang__
#    pragma clang system_header
#elif defined __GNUC__
#    pragma GCC system_header
#endif

#include "internal/catch_suppress_warnings.h"

#if defined(CATCH_CONFIG_MAIN) || defined(CATCH_CONFIG_RUNNER)
#  define CATCH_IMPL
#endif

#ifdef CATCH_IMPL
#  ifndef CLARA_CONFIG_MAIN
#    define CLARA_CONFIG_MAIN_NOT_DEFINED
#    define CLARA_CONFIG_MAIN
#  endif
#endif

#include "internal/catch_notimplemented_exception.h"
#include "internal/catch_context.h"
#include "internal/catch_test_registry.hpp"
#include "internal/catch_capture.hpp"
#include "internal/catch_section.h"
#include "internal/catch_generators.hpp"
#include "internal/catch_interfaces_exception.h"
#include "internal/catch_approx.hpp"
#include "internal/catch_matchers_string.h"
#include "internal/catch_matchers_vector.h"
#include "internal/catch_compiler_capabilities.h"
#include "internal/catch_interfaces_tag_alias_registry.h"

// These files are included here so the single_include script doesn't put them
// in the conditionally compiled sections
#include "internal/catch_test_case_info.h"
#include "internal/catch_interfaces_runner.h"

#ifdef __OBJC__
#include "internal/catch_objc.hpp"
#endif

#ifdef CATCH_IMPL

// !TBD: Move the leak detector code into a separate header
#ifdef CATCH_CONFIG_WINDOWS_CRTDBG
#include <crtdbg.h>
class LeakDetector {
public:
    LeakDetector() {
        int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        flag |= _CRTDBG_LEAK_CHECK_DF;
        flag |= _CRTDBG_ALLOC_MEM_DF;
        _CrtSetDbgFlag(flag);
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
        // Change this to leaking allocation's number to break there
        _CrtSetBreakAlloc(-1);
    }
};
#else
class LeakDetector {};
#endif

LeakDetector leakDetector;

#include "internal/catch_impl.hpp"
#endif

#ifdef CATCH_CONFIG_MAIN
#include "internal/catch_default_main.hpp"
#endif


#ifdef CLARA_CONFIG_MAIN_NOT_DEFINED
#  undef CLARA_CONFIG_MAIN
#endif

//////

// If this config identifier is defined then all CATCH macros are prefixed with CATCH_
#ifdef CATCH_CONFIG_PREFIX_ALL

#if defined(CATCH_CONFIG_FAST_COMPILE)
#define CATCH_REQUIRE( expr ) INTERNAL_CATCH_TEST_NO_TRY( "CATCH_REQUIRE", Catch::ResultDisposition::Normal, expr )
#define CATCH_REQUIRE_FALSE( expr ) INTERNAL_CATCH_TEST_NO_TRY( "CATCH_REQUIRE_FALSE", Catch::ResultDisposition::Normal | Catch::ResultDisposition::FalseTest, expr )
#else
#define CATCH_REQUIRE( expr ) INTERNAL_CATCH_TEST( "CATCH_REQUIRE", Catch::ResultDisposition::Normal, expr )
#define CATCH_REQUIRE_FALSE( expr ) INTERNAL_CATCH_TEST( "CATCH_REQUIRE_FALSE", Catch::ResultDisposition::Normal | Catch::ResultDisposition::FalseTest, expr  )
#endif

#define CATCH_REQUIRE_THROWS( expr ) INTERNAL_CATCH_THROWS( "CATCH_REQUIRE_THROWS", Catch::ResultDisposition::Normal, "", expr )
#define CATCH_REQUIRE_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( "CATCH_REQUIRE_THROWS_AS", exceptionType, Catch::ResultDisposition::Normal, expr )
#define CATCH_REQUIRE_THROWS_WITH( expr, matcher ) INTERNAL_CATCH_THROWS( "CATCH_REQUIRE_THROWS_WITH", Catch::ResultDisposition::Normal, matcher, expr )
#define CATCH_REQUIRE_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( "CATCH_REQUIRE_NOTHROW", Catch::ResultDisposition::Normal, expr )

#define CATCH_CHECK( expr ) INTERNAL_CATCH_TEST( "CATCH_CHECK", Catch::ResultDisposition::ContinueOnFailure, expr )
#define CATCH_CHECK_FALSE( expr ) INTERNAL_CATCH_TEST( "CATCH_CHECK_FALSE", Catch::ResultDisposition::ContinueOnFailure | Catch::ResultDisposition::FalseTest, expr )
#define CATCH_CHECKED_IF( expr ) INTERNAL_CATCH_IF( "CATCH_CHECKED_IF", Catch::ResultDisposition::ContinueOnFailure, expr )
#define CATCH_CHECKED_ELSE( expr ) INTERNAL_CATCH_ELSE( "CATCH_CHECKED_ELSE", Catch::ResultDisposition::ContinueOnFailure, expr )
#define CATCH_CHECK_NOFAIL( expr ) INTERNAL_CATCH_TEST( "CATCH_CHECK_NOFAIL", Catch::ResultDisposition::ContinueOnFailure | Catch::ResultDisposition::SuppressFail, expr )

#define CATCH_CHECK_THROWS( expr )  INTERNAL_CATCH_THROWS( "CATCH_CHECK_THROWS", Catch::ResultDisposition::ContinueOnFailure, "", expr )
#define CATCH_CHECK_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( "CATCH_CHECK_THROWS_AS", exceptionType, Catch::ResultDisposition::ContinueOnFailure, expr )
#define CATCH_CHECK_THROWS_WITH( expr, matcher ) INTERNAL_CATCH_THROWS( "CATCH_CHECK_THROWS_WITH", Catch::ResultDisposition::ContinueOnFailure, matcher, expr )
#define CATCH_CHECK_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( "CATCH_CHECK_NOTHROW", Catch::ResultDisposition::ContinueOnFailure, expr )

#define CATCH_CHECK_THAT( arg, matcher ) INTERNAL_CHECK_THAT( "CATCH_CHECK_THAT", matcher, Catch::ResultDisposition::ContinueOnFailure, arg )

#if defined(CATCH_CONFIG_FAST_COMPILE)
#define CATCH_REQUIRE_THAT( arg, matcher ) INTERNAL_CHECK_THAT_NO_TRY( "CATCH_REQUIRE_THAT", matcher, Catch::ResultDisposition::Normal, arg )
#else
#define CATCH_REQUIRE_THAT( arg, matcher ) INTERNAL_CHECK_THAT( "CATCH_REQUIRE_THAT", matcher, Catch::ResultDisposition::Normal, arg )
#endif


#define CATCH_INFO( msg ) INTERNAL_CATCH_INFO( "CATCH_INFO", msg )
#define CATCH_WARN( msg ) INTERNAL_CATCH_MSG( "CATCH_WARN", Catch::ResultWas::Warning, Catch::ResultDisposition::ContinueOnFailure, msg )
#define CATCH_SCOPED_INFO( msg ) INTERNAL_CATCH_INFO( "CATCH_INFO", msg )
#define CATCH_CAPTURE( msg ) INTERNAL_CATCH_INFO( "CATCH_CAPTURE", #msg " := " << Catch::toString(msg) )
#define CATCH_SCOPED_CAPTURE( msg ) INTERNAL_CATCH_INFO( "CATCH_CAPTURE", #msg " := " << Catch::toString(msg) )

#ifdef CATCH_CONFIG_VARIADIC_MACROS
    #define CATCH_TEST_CASE( ... ) INTERNAL_CATCH_TESTCASE( __VA_ARGS__ )
    #define CATCH_TEST_CASE_METHOD( className, ... ) INTERNAL_CATCH_TEST_CASE_METHOD( className, __VA_ARGS__ )
    #define CATCH_METHOD_AS_TEST_CASE( method, ... ) INTERNAL_CATCH_METHOD_AS_TEST_CASE( method, __VA_ARGS__ )
    #define CATCH_REGISTER_TEST_CASE( Function, ... ) INTERNAL_CATCH_REGISTER_TESTCASE( Function, __VA_ARGS__ )
    #define CATCH_SECTION( ... ) INTERNAL_CATCH_SECTION( __VA_ARGS__ )
    #define CATCH_FAIL( ... ) INTERNAL_CATCH_MSG( "CATCH_FAIL", Catch::ResultWas::ExplicitFailure, Catch::ResultDisposition::Normal, __VA_ARGS__ )
    #define CATCH_FAIL_CHECK( ... ) INTERNAL_CATCH_MSG( "CATCH_FAIL_CHECK", Catch::ResultWas::ExplicitFailure, Catch::ResultDisposition::ContinueOnFailure, __VA_ARGS__ )
    #define CATCH_SUCCEED( ... ) INTERNAL_CATCH_MSG( "CATCH_SUCCEED", Catch::ResultWas::Ok, Catch::ResultDisposition::ContinueOnFailure, __VA_ARGS__ )
#else
    #define CATCH_TEST_CASE( name, description ) INTERNAL_CATCH_TESTCASE( name, description )
    #define CATCH_TEST_CASE_METHOD( className, name, description ) INTERNAL_CATCH_TEST_CASE_METHOD( className, name, description )
    #define CATCH_METHOD_AS_TEST_CASE( method, name, description ) INTERNAL_CATCH_METHOD_AS_TEST_CASE( method, name, description )
    #define CATCH_REGISTER_TEST_CASE( function, name, description ) INTERNAL_CATCH_REGISTER_TESTCASE( function, name, description )
    #define CATCH_SECTION( name, description ) INTERNAL_CATCH_SECTION( name, description )
    #define CATCH_FAIL( msg ) INTERNAL_CATCH_MSG( "CATCH_FAIL", Catch::ResultWas::ExplicitFailure, Catch::ResultDisposition::Normal, msg )
    #define CATCH_FAIL_CHECK( msg ) INTERNAL_CATCH_MSG( "CATCH_FAIL_CHECK", Catch::ResultWas::ExplicitFailure, Catch::ResultDisposition::ContinueOnFailure, msg )
    #define CATCH_SUCCEED( msg ) INTERNAL_CATCH_MSG( "CATCH_SUCCEED", Catch::ResultWas::Ok, Catch::ResultDisposition::ContinueOnFailure, msg )
#endif
#define CATCH_ANON_TEST_CASE() INTERNAL_CATCH_TESTCASE( "", "" )

#define CATCH_REGISTER_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType )
#define CATCH_REGISTER_LEGACY_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( name, reporterType )

#define CATCH_GENERATE( expr) INTERNAL_CATCH_GENERATE( expr )

// "BDD-style" convenience wrappers
#ifdef CATCH_CONFIG_VARIADIC_MACROS
#define CATCH_SCENARIO( ... ) CATCH_TEST_CASE( "Scenario: " __VA_ARGS__ )
#define CATCH_SCENARIO_METHOD( className, ... ) INTERNAL_CATCH_TEST_CASE_METHOD( className, "Scenario: " __VA_ARGS__ )
#else
#define CATCH_SCENARIO( name, tags ) CATCH_TEST_CASE( "Scenario: " name, tags )
#define CATCH_SCENARIO_METHOD( className, name, tags ) INTERNAL_CATCH_TEST_CASE_METHOD( className, "Scenario: " name, tags )
#endif
#define CATCH_GIVEN( desc )    CATCH_SECTION( std::string( "Given: ") + desc, "" )
#define CATCH_WHEN( desc )     CATCH_SECTION( std::string( " When: ") + desc, "" )
#define CATCH_AND_WHEN( desc ) CATCH_SECTION( std::string( "  And: ") + desc, "" )
#define CATCH_THEN( desc )     CATCH_SECTION( std::string( " Then: ") + desc, "" )
#define CATCH_AND_THEN( desc ) CATCH_SECTION( std::string( "  And: ") + desc, "" )

// If CATCH_CONFIG_PREFIX_ALL is not defined then the CATCH_ prefix is not required
#else

#if defined(CATCH_CONFIG_FAST_COMPILE)
#define REQUIRE( expr ) INTERNAL_CATCH_TEST_NO_TRY( "REQUIRE", Catch::ResultDisposition::Normal, expr )
#define REQUIRE_FALSE( expr ) INTERNAL_CATCH_TEST_NO_TRY( "REQUIRE_FALSE", Catch::ResultDisposition::Normal | Catch::ResultDisposition::FalseTest, expr )

#else
#define REQUIRE( expr ) INTERNAL_CATCH_TEST( "REQUIRE", Catch::ResultDisposition::Normal, expr  )
#define REQUIRE_FALSE( expr ) INTERNAL_CATCH_TEST( "REQUIRE_FALSE", Catch::ResultDisposition::Normal | Catch::ResultDisposition::FalseTest, expr )
#endif

#define REQUIRE_THROWS( expr ) INTERNAL_CATCH_THROWS( "REQUIRE_THROWS", Catch::ResultDisposition::Normal, "", expr )
#define REQUIRE_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( "REQUIRE_THROWS_AS", exceptionType, Catch::ResultDisposition::Normal, expr )
#define REQUIRE_THROWS_WITH( expr, matcher ) INTERNAL_CATCH_THROWS( "REQUIRE_THROWS_WITH", Catch::ResultDisposition::Normal, matcher, expr )
#define REQUIRE_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( "REQUIRE_NOTHROW", Catch::ResultDisposition::Normal, expr )

#define CHECK( expr ) INTERNAL_CATCH_TEST( "CHECK", Catch::ResultDisposition::ContinueOnFailure, expr )
#define CHECK_FALSE( expr ) INTERNAL_CATCH_TEST( "CHECK_FALSE", Catch::ResultDisposition::ContinueOnFailure | Catch::ResultDisposition::FalseTest, expr )
#define CHECKED_IF( expr ) INTERNAL_CATCH_IF( "CHECKED_IF", Catch::ResultDisposition::ContinueOnFailure, expr )
#define CHECKED_ELSE( expr ) INTERNAL_CATCH_ELSE( "CHECKED_ELSE", Catch::ResultDisposition::ContinueOnFailure, expr )
#define CHECK_NOFAIL( expr ) INTERNAL_CATCH_TEST( "CHECK_NOFAIL", Catch::ResultDisposition::ContinueOnFailure | Catch::ResultDisposition::SuppressFail, expr )

#define CHECK_THROWS( expr )  INTERNAL_CATCH_THROWS( "CHECK_THROWS", Catch::ResultDisposition::ContinueOnFailure, "", expr )
#define CHECK_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( "CHECK_THROWS_AS", exceptionType, Catch::ResultDisposition::ContinueOnFailure, expr )
#define CHECK_THROWS_WITH( expr, matcher ) INTERNAL_CATCH_THROWS( "CHECK_THROWS_WITH", Catch::ResultDisposition::ContinueOnFailure, matcher, expr )
#define CHECK_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( "CHECK_NOTHROW", Catch::ResultDisposition::ContinueOnFailure, expr )

#define CHECK_THAT( arg, matcher ) INTERNAL_CHECK_THAT( "CHECK_THAT", matcher, Catch::ResultDisposition::ContinueOnFailure, arg )

#if defined(CATCH_CONFIG_FAST_COMPILE)
#define REQUIRE_THAT( arg, matcher ) INTERNAL_CHECK_THAT_NO_TRY( "REQUIRE_THAT", matcher, Catch::ResultDisposition::Normal, arg )
#else
#define REQUIRE_THAT( arg, matcher ) INTERNAL_CHECK_THAT( "REQUIRE_THAT", matcher, Catch::ResultDisposition::Normal, arg )
#endif


#define INFO( msg ) INTERNAL_CATCH_INFO( "INFO", msg )
#define WARN( msg ) INTERNAL_CATCH_MSG( "WARN", Catch::ResultWas::Warning, Catch::ResultDisposition::ContinueOnFailure, msg )
#define SCOPED_INFO( msg ) INTERNAL_CATCH_INFO( "INFO", msg )
#define CAPTURE( msg ) INTERNAL_CATCH_INFO( "CAPTURE", #msg " := " << Catch::toString(msg) )
#define SCOPED_CAPTURE( msg ) INTERNAL_CATCH_INFO( "CAPTURE", #msg " := " << Catch::toString(msg) )

#ifdef CATCH_CONFIG_VARIADIC_MACROS
#define TEST_CASE( ... ) INTERNAL_CATCH_TESTCASE( __VA_ARGS__ )
#define TEST_CASE_METHOD( className, ... ) INTERNAL_CATCH_TEST_CASE_METHOD( className, __VA_ARGS__ )
#define METHOD_AS_TEST_CASE( method, ... ) INTERNAL_CATCH_METHOD_AS_TEST_CASE( method, __VA_ARGS__ )
#define REGISTER_TEST_CASE( Function, ... ) INTERNAL_CATCH_REGISTER_TESTCASE( Function, __VA_ARGS__ )
#define SECTION( ... ) INTERNAL_CATCH_SECTION( __VA_ARGS__ )
#define FAIL( ... ) INTERNAL_CATCH_MSG( "FAIL", Catch::ResultWas::ExplicitFailure, Catch::ResultDisposition::Normal, __VA_ARGS__ )
#define FAIL_CHECK( ... ) INTERNAL_CATCH_MSG( "FAIL_CHECK", Catch::ResultWas::ExplicitFailure, Catch::ResultDisposition::ContinueOnFailure, __VA_ARGS__ )
#define SUCCEED( ... ) INTERNAL_CATCH_MSG( "SUCCEED", Catch::ResultWas::Ok, Catch::ResultDisposition::ContinueOnFailure, __VA_ARGS__ )
#else
#define TEST_CASE( name, description ) INTERNAL_CATCH_TESTCASE( name, description )
    #define TEST_CASE_METHOD( className, name, description ) INTERNAL_CATCH_TEST_CASE_METHOD( className, name, description )
    #define METHOD_AS_TEST_CASE( method, name, description ) INTERNAL_CATCH_METHOD_AS_TEST_CASE( method, name, description )
    #define REGISTER_TEST_CASE( method, name, description ) INTERNAL_CATCH_REGISTER_TESTCASE( method, name, description )
    #define SECTION( name, description ) INTERNAL_CATCH_SECTION( name, description )
    #define FAIL( msg ) INTERNAL_CATCH_MSG( "FAIL", Catch::ResultWas::ExplicitFailure, Catch::ResultDisposition::Normal, msg )
    #define FAIL_CHECK( msg ) INTERNAL_CATCH_MSG( "FAIL_CHECK", Catch::ResultWas::ExplicitFailure, Catch::ResultDisposition::ContinueOnFailure, msg )
    #define SUCCEED( msg ) INTERNAL_CATCH_MSG( "SUCCEED", Catch::ResultWas::Ok, Catch::ResultDisposition::ContinueOnFailure, msg )
#endif
#define ANON_TEST_CASE() INTERNAL_CATCH_TESTCASE( "", "" )

#define REGISTER_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType )
#define REGISTER_LEGACY_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( name, reporterType )

#define GENERATE( expr) INTERNAL_CATCH_GENERATE( expr )

#endif

#define CATCH_TRANSLATE_EXCEPTION( signature ) INTERNAL_CATCH_TRANSLATE_EXCEPTION( signature )

// "BDD-style" convenience wrappers
#ifdef CATCH_CONFIG_VARIADIC_MACROS
#define SCENARIO( ... ) TEST_CASE( "Scenario: " __VA_ARGS__ )
#define SCENARIO_METHOD( className, ... ) INTERNAL_CATCH_TEST_CASE_METHOD( className, "Scenario: " __VA_ARGS__ )
#else
#define SCENARIO( name, tags ) TEST_CASE( "Scenario: " name, tags )
#define SCENARIO_METHOD( className, name, tags ) INTERNAL_CATCH_TEST_CASE_METHOD( className, "Scenario: " name, tags )
#endif
#define GIVEN( desc )    SECTION( std::string("   Given: ") + desc, "" )
#define WHEN( desc )     SECTION( std::string("    When: ") + desc, "" )
#define AND_WHEN( desc ) SECTION( std::string("And when: ") + desc, "" )
#define THEN( desc )     SECTION( std::string("    Then: ") + desc, "" )
#define AND_THEN( desc ) SECTION( std::string("     And: ") + desc, "" )

using Catch::Detail::Approx;

#include "internal/catch_reenable_warnings.h"

#endif // TWOBLUECUBES_CATCH_HPP_INCLUDED
