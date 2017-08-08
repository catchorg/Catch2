/*
 *  Created by Phil on 18/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED

#include "catch_assertionhandler.h"
#include "catch_result_builder.h"
#include "catch_message.h"
#include "catch_interfaces_capture.h"
#include "catch_debugger.h"
#include "catch_common.h"
#include "catch_tostring.h"
#include "catch_interfaces_runner.h"
#include "catch_compiler_capabilities.h"


#if defined(CATCH_CONFIG_FAST_COMPILE)
///////////////////////////////////////////////////////////////////////////////
// We can speedup compilation significantly by breaking into debugger lower in
// the callstack, because then we don't have to expand CATCH_BREAK_INTO_DEBUGGER
// macro in each assertion
#define INTERNAL_CATCH_REACT( resultBuilder ) \
    resultBuilder.react();
#define INTERNAL_CATCH_REACT2( handler ) \
    handler.reactWithDebugBreak();

///////////////////////////////////////////////////////////////////////////////
// Another way to speed-up compilation is to omit local try-catch for REQUIRE*
// macros.
// This can potentially cause false negative, if the test code catches
// the exception before it propagates back up to the runner.
#define INTERNAL_CATCH_TRY( capturer ) capturer.setExceptionGuard();
#define INTERNAL_CATCH_CATCH( capturer, disposition ) capturer.unsetExceptionGuard();
#define INTERNAL_CATCH_CATCH2( capturer ) capturer.unsetExceptionGuard();

#else // CATCH_CONFIG_FAST_COMPILE

///////////////////////////////////////////////////////////////////////////////
// In the event of a failure works out if the debugger needs to be invoked
// and/or an exception thrown and takes appropriate action.
// This needs to be done as a macro so the debugger will stop in the user
// source code rather than in Catch library code
#define INTERNAL_CATCH_REACT( resultBuilder ) \
    if( resultBuilder.shouldDebugBreak() ) CATCH_BREAK_INTO_DEBUGGER(); \
    resultBuilder.react();
#define INTERNAL_CATCH_REACT2( handler ) \
    if( handler.shouldDebugBreak() ) CATCH_BREAK_INTO_DEBUGGER(); \
    handler.reactWithoutDebugBreak();

#define INTERNAL_CATCH_TRY( capturer ) try
#define INTERNAL_CATCH_CATCH( capturer, disposition ) catch(...) { capturer.useActiveException( disposition ); }
#define INTERNAL_CATCH_CATCH2( capturer ) catch(...) { capturer.useActiveException(); }

#endif

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TEST( macroName, resultDisposition, ... ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName, CATCH_INTERNAL_LINEINFO, #__VA_ARGS__, resultDisposition ); \
        INTERNAL_CATCH_TRY( catchAssertionHandler ) { \
            CATCH_INTERNAL_SUPPRESS_PARENTHESES_WARNINGS \
            catchAssertionHandler.handle( Catch::Decomposer() <= __VA_ARGS__ ); \
            CATCH_INTERNAL_UNSUPPRESS_PARENTHESES_WARNINGS \
        } INTERNAL_CATCH_CATCH2( catchAssertionHandler ) \
        INTERNAL_CATCH_REACT2( catchAssertionHandler ) \
    } while( Catch::isTrue( false && static_cast<bool>( !!(__VA_ARGS__) ) ) ) // the expression here is never evaluated at runtime but it forces the compiler to give it a look
    // The double negation silences MSVC's C4800 warning, the static_cast forces short-circuit evaluation if the type has overloaded &&.

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_IF( macroName, resultDisposition, ... ) \
    INTERNAL_CATCH_TEST( macroName, resultDisposition, __VA_ARGS__ ); \
    if( Catch::getResultCapture().lastAssertionPassed() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ELSE( macroName, resultDisposition, ... ) \
    INTERNAL_CATCH_TEST( macroName, resultDisposition, __VA_ARGS__ ); \
    if( !Catch::getResultCapture().lastAssertionPassed() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_NO_THROW( macroName, resultDisposition, ... ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName, CATCH_INTERNAL_LINEINFO, #__VA_ARGS__, resultDisposition ); \
        try { \
            static_cast<void>(__VA_ARGS__); \
            catchAssertionHandler.handle( Catch::ResultWas::Ok ); \
        } \
        catch( ... ) { \
            catchAssertionHandler.useActiveException(); \
        } \
        INTERNAL_CATCH_REACT2( catchAssertionHandler ) \
    } while( Catch::alwaysFalse() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS( macroName, resultDisposition, ... ) \
    do { \
        Catch::ResultBuilder __catchResult( macroName, CATCH_INTERNAL_LINEINFO, #__VA_ARGS__, resultDisposition); \
        if( __catchResult.allowThrows() ) \
            try { \
                static_cast<void>(__VA_ARGS__); \
                __catchResult.captureResult( Catch::ResultWas::DidntThrowException ); \
            } \
            catch( ... ) { \
                __catchResult.captureExpectedException( "" ); \
            } \
        else \
            __catchResult.captureResult( Catch::ResultWas::Ok ); \
        INTERNAL_CATCH_REACT( __catchResult ) \
    } while( Catch::alwaysFalse() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_AS( macroName, exceptionType, resultDisposition, expr ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName, CATCH_INTERNAL_LINEINFO, #expr ", " #exceptionType, resultDisposition ); \
        if( catchAssertionHandler.allowThrows() ) \
            try { \
                static_cast<void>(expr); \
                catchAssertionHandler.handle( Catch::ResultWas::DidntThrowException ); \
            } \
            catch( exceptionType const& ) { \
                catchAssertionHandler.handle( Catch::ResultWas::Ok ); \
            } \
            catch( ... ) { \
                catchAssertionHandler.useActiveException(); \
            } \
        else \
            catchAssertionHandler.handle( Catch::ResultWas::Ok ); \
        INTERNAL_CATCH_REACT2( catchAssertionHandler ) \
    } while( Catch::alwaysFalse() )


///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_MSG( macroName, messageType, resultDisposition, ... ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName, CATCH_INTERNAL_LINEINFO, "", resultDisposition ); \
        catchAssertionHandler.handle( messageType, ( Catch::MessageStream() << __VA_ARGS__ + ::Catch::StreamEndStop() ).m_stream.str().c_str() ); \
        INTERNAL_CATCH_REACT2( catchAssertionHandler ) \
    } while( Catch::alwaysFalse() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_INFO( macroName, log ) \
    Catch::ScopedMessage INTERNAL_CATCH_UNIQUE_NAME( scopedMessage ) = Catch::MessageBuilder( macroName, CATCH_INTERNAL_LINEINFO, Catch::ResultWas::Info ) << log;

#if !defined(CATCH_CONFIG_DISABLE_MATCHERS)
///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CHECK_THAT( macroName, matcher, resultDisposition, arg ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName, CATCH_INTERNAL_LINEINFO, #arg ", " #matcher, resultDisposition ); \
        INTERNAL_CATCH_TRY( catchAssertionHandler ) { \
            catchAssertionHandler.handle( Catch::makeMatchExpr( arg, matcher, #matcher ) ); \
        } INTERNAL_CATCH_CATCH2( catchAssertionHandler ) \
        INTERNAL_CATCH_REACT2( catchAssertionHandler ) \
    } while( Catch::alwaysFalse() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_STR_MATCHES( macroName, resultDisposition, matcher, ... ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName, CATCH_INTERNAL_LINEINFO, #__VA_ARGS__ ", " #matcher, resultDisposition ); \
        if( catchAssertionHandler.allowThrows() ) \
            try { \
                static_cast<void>(__VA_ARGS__); \
                catchAssertionHandler.handle( Catch::ResultWas::DidntThrowException ); \
            } \
            catch( ... ) { \
                handleExceptionMatchExpr( catchAssertionHandler, matcher, #matcher ); \
            } \
        else \
            catchAssertionHandler.handle( Catch::ResultWas::Ok ); \
        INTERNAL_CATCH_REACT2( catchAssertionHandler ) \
    } while( Catch::alwaysFalse() )


///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_MATCHES( macroName, exceptionType, resultDisposition, matcher, ... ) \
    do { \
        Catch::AssertionHandler catchAssertionHandler( macroName, CATCH_INTERNAL_LINEINFO, #__VA_ARGS__ ", " #exceptionType ", " #matcher, resultDisposition ); \
        if( catchAssertionHandler.allowThrows() ) \
            try { \
                static_cast<void>(__VA_ARGS__ ); \
                catchAssertionHandler.handle( Catch::ResultWas::DidntThrowException ); \
            } \
            catch( exceptionType const& ex ) { \
                catchAssertionHandler.handle( Catch::makeMatchExpr( ex, matcher, #matcher ) ); \
            } \
            catch( ... ) { \
                catchAssertionHandler.useActiveException(); \
            } \
        else \
            catchAssertionHandler.handle( Catch::ResultWas::Ok ); \
        INTERNAL_CATCH_REACT2( catchAssertionHandler ) \
    } while( Catch::alwaysFalse() )
#endif // CATCH_CONFIG_DISABLE_MATCHERS


#endif // TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
