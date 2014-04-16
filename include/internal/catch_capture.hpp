/*
 *  Created by Phil on 18/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED

#include "catch_expression_decomposer.hpp"
#include "catch_expressionresult_builder.h"
#include "catch_message.h"
#include "catch_interfaces_capture.h"
#include "catch_debugger.h"
#include "catch_context.h"
#include "catch_common.h"
#include "catch_tostring.hpp"
#include "catch_interfaces_registry_hub.h"
#include "catch_interfaces_config.h"
#include "internal/catch_compiler_capabilities.h"

#include <ostream>

namespace Catch {

    inline IResultCapture& getResultCapture() {
        return getCurrentContext().getResultCapture();
    }

    template<typename MatcherT>
    ExpressionResultBuilder expressionResultBuilderFromMatcher( MatcherT const& matcher,
                                                                std::string const& matcherCallAsString ) {
        std::string matcherAsString = matcher.toString();
        if( matcherAsString == "{?}" )
            matcherAsString = matcherCallAsString;
        return ExpressionResultBuilder()
            .setRhs( matcherAsString )
            .setOp( "matches" );
    }

    template<typename MatcherT, typename ArgT>
    ExpressionResultBuilder expressionResultBuilderFromMatcher( MatcherT const& matcher,
                                                                ArgT const& arg,
                                                                std::string const& matcherCallAsString ) {
        return expressionResultBuilderFromMatcher( matcher, matcherCallAsString )
            .setLhs( Catch::toString( arg ) )
            .setResultType( matcher.match( arg ) );
    }

    template<typename MatcherT, typename ArgT>
    ExpressionResultBuilder expressionResultBuilderFromMatcher( MatcherT const& matcher,
                                                                ArgT* arg,
                                                                std::string const& matcherCallAsString ) {
        return expressionResultBuilderFromMatcher( matcher, matcherCallAsString )
            .setLhs( Catch::toString( arg ) )
            .setResultType( matcher.match( arg ) );
    }

struct TestFailureException{};

} // end namespace Catch


///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ACCEPT_EXPR( evaluatedExpr, resultDisposition, originalExpr ) \
    if( Catch::ResultAction::Value internal_catch_action = Catch::getResultCapture().acceptExpression( evaluatedExpr, __assertionInfo )  ) { \
        if( internal_catch_action & Catch::ResultAction::Debug ) CATCH_BREAK_INTO_DEBUGGER(); \
        if( internal_catch_action & Catch::ResultAction::Abort ) throw Catch::TestFailureException(); \
        if( !Catch::shouldContinueOnFailure( resultDisposition ) ) throw Catch::TestFailureException(); \
        Catch::isTrue( false && originalExpr ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TEST( expr, resultDisposition, macroName ) \
    do { \
        Catch::AssertionInfo __assertionInfo( macroName, CATCH_INTERNAL_LINEINFO, #expr, resultDisposition ); \
        try { \
            INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionDecomposer()->*expr ).endExpression( resultDisposition ), resultDisposition, expr ); \
        } catch( Catch::TestFailureException& ) { \
            throw; \
        } catch( ... ) { \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException(), \
                Catch::ResultDisposition::Normal, expr ); \
        } \
    } while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_IF( expr, resultDisposition, macroName ) \
    INTERNAL_CATCH_TEST( expr, resultDisposition, macroName ); \
    if( Catch::getResultCapture().getLastResult()->succeeded() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ELSE( expr, resultDisposition, macroName ) \
    INTERNAL_CATCH_TEST( expr, resultDisposition, macroName ); \
    if( !Catch::getResultCapture().getLastResult()->succeeded() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_NO_THROW( expr, resultDisposition, macroName ) \
    do { \
        Catch::AssertionInfo __assertionInfo( macroName, CATCH_INTERNAL_LINEINFO, #expr, resultDisposition ); \
        try { \
            expr; \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( Catch::ResultWas::Ok ), resultDisposition, false ); \
        } \
        catch( ... ) { \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException(), resultDisposition, false ); \
        } \
} while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_IMPL( expr, exceptionType, resultDisposition ) \
    try { \
        if( Catch::getCurrentContext().getConfig()->allowThrows() ) { \
            expr; \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( Catch::ResultWas::DidntThrowException ), resultDisposition, false ); \
        } \
    } \
    catch( Catch::TestFailureException& ) { \
        throw; \
    } \
    catch( exceptionType ) { \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( Catch::ResultWas::Ok ), resultDisposition, false ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS( expr, exceptionType, resultDisposition, macroName ) \
    do { \
        Catch::AssertionInfo __assertionInfo( macroName, CATCH_INTERNAL_LINEINFO, #expr, resultDisposition ); \
        INTERNAL_CATCH_THROWS_IMPL( expr, exceptionType, resultDisposition ) \
    } while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_AS( expr, exceptionType, resultDisposition, macroName ) \
    do { \
        Catch::AssertionInfo __assertionInfo( macroName, CATCH_INTERNAL_LINEINFO, #expr, resultDisposition ); \
        INTERNAL_CATCH_THROWS_IMPL( expr, exceptionType, resultDisposition ) \
        catch( ... ) { \
            INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException() ), \
                resultDisposition | Catch::ResultDisposition::ContinueOnFailure, false ); \
        } \
    } while( Catch::isTrue( false ) )


///////////////////////////////////////////////////////////////////////////////
#ifdef CATCH_CONFIG_VARIADIC_MACROS
    #define INTERNAL_CATCH_MSG( messageType, resultDisposition, macroName, ... ) \
        do { \
            Catch::AssertionInfo __assertionInfo( macroName, CATCH_INTERNAL_LINEINFO, "", resultDisposition ); \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( messageType ) << __VA_ARGS__ +::Catch::StreamEndStop(), resultDisposition, true ) \
        } while( Catch::isTrue( false ) )
#else
    #define INTERNAL_CATCH_MSG( messageType, resultDisposition, macroName, log ) \
        do { \
            Catch::AssertionInfo __assertionInfo( macroName, CATCH_INTERNAL_LINEINFO, "", resultDisposition ); \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( messageType ) << log, resultDisposition, true ) \
        } while( Catch::isTrue( false ) )
#endif

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_INFO( log, macroName ) \
    Catch::ScopedMessage INTERNAL_CATCH_UNIQUE_NAME( scopedMessage ) = Catch::MessageBuilder( macroName, CATCH_INTERNAL_LINEINFO, Catch::ResultWas::Info ) << log;


///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CHECK_THAT( arg, matcher, resultDisposition, macroName ) \
    do { \
        Catch::AssertionInfo __assertionInfo( macroName, CATCH_INTERNAL_LINEINFO, #arg " " #matcher, resultDisposition ); \
        try { \
            INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::expressionResultBuilderFromMatcher( ::Catch::Matchers::matcher, arg, #matcher ) ), resultDisposition, false ); \
        } catch( Catch::TestFailureException& ) { \
            throw; \
        } catch( ... ) { \
            INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException() ), \
                resultDisposition | Catch::ResultDisposition::ContinueOnFailure, false ); \
        } \
    } while( Catch::isTrue( false ) )

#endif // TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
