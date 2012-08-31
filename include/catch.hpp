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
#pragma clang diagnostic ignored "-Wno-global-constructors"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

#include "internal/catch_notimplemented_exception.h"
#include "internal/catch_context.h"
#include "internal/catch_test_registry.hpp"
#include "internal/catch_capture.hpp"
#include "internal/catch_section.hpp"
#include "internal/catch_generators.hpp"
#include "internal/catch_interfaces_exception.h"
#include "internal/catch_approx.hpp"
#include "internal/catch_matchers.hpp"

// These files are included here so the single_include script doesn't put them
// in the conditionally compiled sections
#include "internal/catch_test_case_info.h"
#include "internal/catch_interfaces_runner.h"

#ifdef __OBJC__
#include "internal/catch_objc.hpp"
#endif

#if defined( CATCH_CONFIG_MAIN ) || defined( CATCH_CONFIG_RUNNER )
#include "internal/catch_impl.hpp"
#endif

#ifdef CATCH_CONFIG_MAIN
#include "internal/catch_default_main.hpp"
#endif

//////

// If this config identifier is defined then all CATCH macros are prefixed with CATCH_
#ifdef CATCH_CONFIG_PREFIX_ALL

#define CATCH_REQUIRE( expr ) INTERNAL_CATCH_TEST( expr, false, true, "CATCH_REQUIRE" )
#define CATCH_REQUIRE_FALSE( expr ) INTERNAL_CATCH_TEST( expr, true, true, "CATCH_REQUIRE_FALSE" )

#define CATCH_REQUIRE_THROWS( expr ) INTERNAL_CATCH_THROWS( expr, ..., true, "CATCH_REQUIRE_THROWS" )
#define CATCH_REQUIRE_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( expr, exceptionType, true, "CATCH_REQUIRE_THROWS_AS" )
#define CATCH_REQUIRE_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( expr, true, "CATCH_REQUIRE_NOTHROW" )

#define CATCH_CHECK( expr ) INTERNAL_CATCH_TEST( expr, false, false, "CATCH_CHECK" )
#define CATCH_CHECK_FALSE( expr ) INTERNAL_CATCH_TEST( expr, true, false, "CATCH_CHECK_FALSE" )
#define CATCH_CHECKED_IF( expr ) INTERNAL_CATCH_IF( expr, false, false, "CATCH_CHECKED_IF" )
#define CATCH_CHECKED_ELSE( expr ) INTERNAL_CATCH_ELSE( expr, false, false, "CATCH_CHECKED_ELSE" )

#define CATCH_CHECK_THROWS( expr )  INTERNAL_CATCH_THROWS( expr, ..., false, "CATCH_CHECK_THROWS" )
#define CATCH_CHECK_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( expr, exceptionType, false, "CATCH_CHECK_THROWS_AS" )
#define CATCH_CHECK_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( expr, false, "CATCH_CHECK_NOTHROW" )

#define CHECK_THAT( arg, matcher ) INTERNAL_CHECK_THAT( arg, matcher, false, "CATCH_CHECK_THAT" )
#define CATCH_REQUIRE_THAT( arg, matcher ) INTERNAL_CHECK_THAT( arg, matcher, true, "CATCH_REQUIRE_THAT" )

#define CATCH_INFO( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Info, false, "CATCH_INFO" )
#define CATCH_WARN( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Warning, false, "CATCH_WARN" )
#define CATCH_FAIL( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::ExplicitFailure, true, "CATCH_FAIL" )
#define CATCH_SUCCEED( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Ok, false, "CATCH_SUCCEED" )
#define CATCH_SCOPED_INFO( msg ) INTERNAL_CATCH_SCOPED_INFO( msg )
#define CATCH_CAPTURE( msg ) INTERNAL_CATCH_MSG( #msg " := " << msg, Catch::ResultWas::Info, false, "CATCH_CAPTURE" )

#define CATCH_SECTION( name, description ) INTERNAL_CATCH_SECTION( name, description )

#define CATCH_TEST_CASE( name, description ) INTERNAL_CATCH_TESTCASE( name, description )
#define CATCH_TEST_CASE_NORETURN( name, description ) INTERNAL_CATCH_TESTCASE_NORETURN( name, description )
#define CATCH_ANON_TEST_CASE() INTERNAL_CATCH_TESTCASE( "", "Anonymous test case" )
#define CATCH_METHOD_AS_TEST_CASE( method, name, description ) INTERNAL_CATCH_METHOD_AS_TEST_CASE( method, name, description )

#define CATCH_REGISTER_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType )

#define CATCH_GENERATE( expr) INTERNAL_CATCH_GENERATE( expr )

///////////////
// Still to be implemented
//#define CHECK_NOFAIL( expr ) // !TBD - reports violation, but doesn't fail Test

// If CATCH_CONFIG_PREFIX_ALL is not defined then the CATCH_ prefix is not required
#else

#define REQUIRE( expr ) INTERNAL_CATCH_TEST( expr, false, true, "REQUIRE" )
#define REQUIRE_FALSE( expr ) INTERNAL_CATCH_TEST( expr, true, true, "REQUIRE_FALSE" )

#define REQUIRE_THROWS( expr ) INTERNAL_CATCH_THROWS( expr, ..., true, "REQUIRE_THROWS" )
#define REQUIRE_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( expr, exceptionType, true, "REQUIRE_THROWS_AS" )
#define REQUIRE_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( expr, true, "REQUIRE_NOTHROW" )

#define CHECK( expr ) INTERNAL_CATCH_TEST( expr, false, false, "CHECK" )
#define CHECK_FALSE( expr ) INTERNAL_CATCH_TEST( expr, true, false, "CHECK_FALSE" )
#define CHECKED_IF( expr ) INTERNAL_CATCH_IF( expr, false, false, "CHECKED_IF" )
#define CHECKED_ELSE( expr ) INTERNAL_CATCH_ELSE( expr, false, false, "CHECKED_ELSE" )

#define CHECK_THROWS( expr )  INTERNAL_CATCH_THROWS( expr, ..., false, "CHECK_THROWS" )
#define CHECK_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( expr, exceptionType, false, "CHECK_THROWS_AS" )
#define CHECK_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( expr, false, "CHECK_NOTHROW" )

#define CHECK_THAT( arg, matcher ) INTERNAL_CHECK_THAT( arg, matcher, false, "CHECK_THAT" )
#define REQUIRE_THAT( arg, matcher ) INTERNAL_CHECK_THAT( arg, matcher, true, "REQUIRE_THAT" )

#define INFO( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Info, false, "INFO" )
#define WARN( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Warning, false, "WARN" )
#define FAIL( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::ExplicitFailure, true, "FAIL" )
#define SUCCEED( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Ok, false, "SUCCEED" )
#define SCOPED_INFO( msg ) INTERNAL_CATCH_SCOPED_INFO( msg )
#define CAPTURE( msg ) INTERNAL_CATCH_MSG( #msg " := " << msg, Catch::ResultWas::Info, false, "CAPTURE" )

#define SECTION( name, description ) INTERNAL_CATCH_SECTION( name, description )

#define TEST_CASE( name, description ) INTERNAL_CATCH_TESTCASE( name, description )
#define TEST_CASE_NORETURN( name, description ) INTERNAL_CATCH_TESTCASE_NORETURN( name, description )
#define ANON_TEST_CASE() INTERNAL_CATCH_TESTCASE( "", "Anonymous test case" )
#define METHOD_AS_TEST_CASE( method, name, description ) INTERNAL_CATCH_METHOD_AS_TEST_CASE( method, name, description )

#define REGISTER_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType )

#define GENERATE( expr) INTERNAL_CATCH_GENERATE( expr )

#endif

#define CATCH_TRANSLATE_EXCEPTION( signature ) INTERNAL_CATCH_TRANSLATE_EXCEPTION( signature )

using Catch::Detail::Approx;

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_HPP_INCLUDED
