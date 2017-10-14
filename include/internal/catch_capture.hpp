/*
 *  Created by Phil on 18/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED

#include "catch_result_builder.h"
#include "catch_message.h"
#include "catch_interfaces_capture.h"
#include "catch_debugger.h"
#include "catch_common.h"
#include "catch_tostring.h"
#include "catch_interfaces_runner.h"
#include "catch_compiler_capabilities.h"


#if !defined(CATCH_CONFIG_DISABLE_STRINGIFICATION)
# define CATCH_INTERNAL_STRINGIFY(expr) #expr
#else
# define CATCH_INTERNAL_STRINGIFY(expr) "Disabled by CATCH_CONFIG_DISABLE_STRINGIFICATION"
#endif

#if defined(CATCH_CONFIG_FAST_COMPILE)
///////////////////////////////////////////////////////////////////////////////
// We can speedup compilation significantly by breaking into debugger lower in
// the callstack, because then we don't have to expand CATCH_BREAK_INTO_DEBUGGER
// macro in each assertion
#define INTERNAL_CATCH_REACT( resultBuilder ) \
    resultBuilder.react();

///////////////////////////////////////////////////////////////////////////////
// Another way to speed-up compilation is to omit local try-catch for REQUIRE*
// macros.
// This can potentially cause false negative, if the test code catches
// the exception before it propagates back up to the runner.
#define INTERNAL_CATCH_TEST_NO_TRY( macroName, resultDisposition, expr ) \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(expr), resultDisposition ); \
        __catchResult.setExceptionGuard(); \
        CATCH_INTERNAL_SUPPRESS_PARENTHESES_WARNINGS \
        ( __catchResult <= expr ).endExpression(); \
        CATCH_INTERNAL_UNSUPPRESS_PARENTHESES_WARNINGS \
        __catchResult.unsetExceptionGuard(); \
        INTERNAL_CATCH_REACT( __catchResult ) \
    } while( Catch::isTrue( false && static_cast<bool>( !!(expr) ) ) ) // expr here is never evaluated at runtime but it forces the compiler to give it a look
// The double negation silences MSVC's C4800 warning, the static_cast forces short-circuit evaluation if the type has overloaded &&.

#define INTERNAL_CHECK_THAT_NO_TRY( macroName, matcher, resultDisposition, arg ) \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(arg) ", " CATCH_INTERNAL_STRINGIFY(matcher), resultDisposition ); \
        __catchResult.setExceptionGuard(); \
        __catchResult.captureMatch( arg, matcher, CATCH_INTERNAL_STRINGIFY(matcher) ); \
        __catchResult.unsetExceptionGuard(); \
        INTERNAL_CATCH_REACT( __catchResult ) \
    } while( Catch::alwaysFalse() )

#else
///////////////////////////////////////////////////////////////////////////////
// In the event of a failure works out if the debugger needs to be invoked
// and/or an exception thrown and takes appropriate action.
// This needs to be done as a macro so the debugger will stop in the user
// source code rather than in Catch library code
#define INTERNAL_CATCH_REACT( resultBuilder ) \
    if( resultBuilder.shouldDebugBreak() ) CATCH_BREAK_INTO_DEBUGGER(); \
    resultBuilder.react();
#endif


///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TEST( macroName, resultDisposition, expr ) \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(expr), resultDisposition ); \
        try { \
            CATCH_INTERNAL_SUPPRESS_PARENTHESES_WARNINGS \
            ( __catchResult <= expr ).endExpression(); \
            CATCH_INTERNAL_UNSUPPRESS_PARENTHESES_WARNINGS \
        } \
        catch( ... ) { \
            __catchResult.useActiveException( resultDisposition ); \
        } \
        INTERNAL_CATCH_REACT( __catchResult ) \
    } while( Catch::isTrue( false && static_cast<bool>( !!(expr) ) ) ) // expr here is never evaluated at runtime but it forces the compiler to give it a look
    // The double negation silences MSVC's C4800 warning, the static_cast forces short-circuit evaluation if the type has overloaded &&.

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_IF( macroName, resultDisposition, expr ) \
    INTERNAL_CATCH_TEST( macroName, resultDisposition, expr ); \
    if( Catch::getResultCapture().lastAssertionPassed() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ELSE( macroName, resultDisposition, expr ) \
    INTERNAL_CATCH_TEST( macroName, resultDisposition, expr ); \
    if( !Catch::getResultCapture().lastAssertionPassed() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_NO_THROW( macroName, resultDisposition, expr ) \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(expr), resultDisposition ); \
        try { \
            static_cast<void>(expr); \
            __catchResult.captureResult( Catch::ResultWas::Ok ); \
        } \
        catch( ... ) { \
            __catchResult.useActiveException( resultDisposition ); \
        } \
        INTERNAL_CATCH_REACT( __catchResult ) \
    } while( Catch::alwaysFalse() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS( macroName, resultDisposition, matcher, expr ) \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(expr), resultDisposition, CATCH_INTERNAL_STRINGIFY(matcher) ); \
        if( __catchResult.allowThrows() ) \
            try { \
                static_cast<void>(expr); \
                __catchResult.captureResult( Catch::ResultWas::DidntThrowException ); \
            } \
            catch( ... ) { \
                __catchResult.captureExpectedException( matcher ); \
            } \
        else \
            __catchResult.captureResult( Catch::ResultWas::Ok ); \
        INTERNAL_CATCH_REACT( __catchResult ) \
    } while( Catch::alwaysFalse() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_AS( macroName, exceptionType, resultDisposition, expr ) \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(expr) ", " CATCH_INTERNAL_STRINGIFY(exceptionType), resultDisposition ); \
        if( __catchResult.allowThrows() ) \
            try { \
                static_cast<void>(expr); \
                __catchResult.captureResult( Catch::ResultWas::DidntThrowException ); \
            } \
            catch( exceptionType ) { \
                __catchResult.captureResult( Catch::ResultWas::Ok ); \
            } \
            catch( ... ) { \
                __catchResult.useActiveException( resultDisposition ); \
            } \
        else \
            __catchResult.captureResult( Catch::ResultWas::Ok ); \
        INTERNAL_CATCH_REACT( __catchResult ) \
    } while( Catch::alwaysFalse() )


///////////////////////////////////////////////////////////////////////////////
#ifdef CATCH_CONFIG_VARIADIC_MACROS
    #define INTERNAL_CATCH_MSG( macroName, messageType, resultDisposition, ... ) \
        do { \
            Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, "", resultDisposition ); \
            __catchResult << __VA_ARGS__ + ::Catch::StreamEndStop(); \
            __catchResult.captureResult( messageType ); \
            INTERNAL_CATCH_REACT( __catchResult ) \
        } while( Catch::alwaysFalse() )
#else
    #define INTERNAL_CATCH_MSG( macroName, messageType, resultDisposition, log ) \
        do { \
            Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, "", resultDisposition ); \
            __catchResult << log + ::Catch::StreamEndStop(); \
            __catchResult.captureResult( messageType ); \
            INTERNAL_CATCH_REACT( __catchResult ) \
        } while( Catch::alwaysFalse() )
#endif

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_INFO( macroName, log ) \
    Catch::ScopedMessage INTERNAL_CATCH_UNIQUE_NAME( scopedMessage ) = Catch::MessageBuilder( macroName, CATCH_INTERNAL_LINEINFO, Catch::ResultWas::Info ) << log;

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CHECK_THAT( macroName, matcher, resultDisposition, arg ) \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, CATCH_INTERNAL_STRINGIFY(arg) ", " CATCH_INTERNAL_STRINGIFY(matcher), resultDisposition ); \
        try { \
            __catchResult.captureMatch( arg, matcher, CATCH_INTERNAL_STRINGIFY(matcher) ); \
        } catch( ... ) { \
            __catchResult.useActiveException( resultDisposition | Catch::ResultDisposition::ContinueOnFailure ); \
        } \
        INTERNAL_CATCH_REACT( __catchResult ) \
    } while( Catch::alwaysFalse() )

#endif // TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
