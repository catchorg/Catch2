/*
 *  catch.hpp
 *  Catch
 *
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

/* TBD:
 
 Next:

 Later:
    Finish command line parser (list as xml)
    Tags?
    Finish macros, listed here, later (just CHECK_NOFAIL now)
 */
#ifndef TWOBLUECUBES_CATCH_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_HPP_INCLUDED
#include "internal/catch_hub.h"
#include "internal/catch_test_registry.hpp"
#include "internal/catch_capture.hpp"
#include "internal/catch_section.hpp"
#include "internal/catch_generators.hpp"
#include "internal/catch_interfaces_exception.h"
#include "internal/catch_approx.hpp"
#include "internal/catch_test_case_info.hpp"
#include "internal/catch_matchers.hpp"

#ifdef __OBJC__
#include "internal/catch_objc.hpp"
#endif

#if defined( CATCH_CONFIG_MAIN ) || defined( CATCH_CONFIG_RUNNER )
#include "catch_runner.hpp"
#endif

#ifdef CATCH_CONFIG_MAIN
#include "internal/catch_default_main.hpp"
#endif

//////

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
#define SCOPED_INFO( msg ) INTERNAL_CATCH_SCOPED_INFO( msg )
#define CAPTURE( msg ) INTERNAL_CATCH_MSG( #msg " := " << msg, Catch::ResultWas::Info, false, "CAPTURE" )

#define SECTION( name, description ) INTERNAL_CATCH_SECTION( name, description )

#define TEST_CASE( name, description ) INTERNAL_CATCH_TESTCASE( name, description )
#define TEST_CASE_NORETURN( name, description ) INTERNAL_CATCH_TESTCASE_NORETURN( name, description )
#define ANON_TEST_CASE() INTERNAL_CATCH_TESTCASE( "", "Anonymous test case" )
#define METHOD_AS_TEST_CASE( method, name, description ) CATCH_METHOD_AS_TEST_CASE( method, name, description )

#define REGISTER_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType )
#define CATCH_TRANSLATE_EXCEPTION( signature ) INTERNAL_CATCH_TRANSLATE_EXCEPTION( signature )

#define GENERATE( expr) INTERNAL_CATCH_GENERATE( expr )

///////////////
// Still to be implemented
#define CHECK_NOFAIL( expr ) // !TBD - reports violation, but doesn't fail Test

using Catch::Detail::Approx;

#endif // TWOBLUECUBES_CATCH_HPP_INCLUDED
