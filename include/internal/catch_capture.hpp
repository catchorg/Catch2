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
#include "catch_interfaces_capture.h"
#include "catch_debugger.hpp"
#include "catch_context.h"
#include "catch_common.h"
#include "catch_interfaces_registry_hub.h"
#include <ostream>

namespace Catch {

    inline IResultCapture& getResultCapture() {
        return getCurrentContext().getResultCapture();
    }
    
    template<typename MatcherT>
    ExpressionResultBuilder expressionResultBuilderFromMatcher( const MatcherT& matcher,
                                                                const std::string& matcherCallAsString ) {
        std::string matcherAsString = matcher.toString();
        if( matcherAsString == "{?}" )
            matcherAsString = matcherCallAsString;
        return ExpressionResultBuilder()
            .setRhs( matcherAsString )
            .setOp( "matches" );
    }

    template<typename MatcherT, typename ArgT>
    ExpressionResultBuilder expressionResultBuilderFromMatcher( const MatcherT& matcher,
                                                                const ArgT& arg,
                                                                const std::string& matcherCallAsString ) {
        return expressionResultBuilderFromMatcher( matcher, matcherCallAsString )
            .setLhs( Catch::toString( arg ) )
            .setResultType( matcher.match( arg ) );
    }

    template<typename MatcherT, typename ArgT>
    ExpressionResultBuilder expressionResultBuilderFromMatcher( const MatcherT& matcher,
                                                                ArgT* arg,
                                                                const std::string& matcherCallAsString ) {
        return expressionResultBuilderFromMatcher( matcher, matcherCallAsString )
            .setLhs( Catch::toString( arg ) )
            .setResultType( matcher.match( arg ) );
    }
    
struct TestFailureException{};

class ScopedInfo {
public:
    ScopedInfo() : m_resultBuilder( ResultWas::Info ) {
        getResultCapture().pushScopedInfo( this );
    }
    ~ScopedInfo() {
        getResultCapture().popScopedInfo( this );
    }
    template<typename T>
    ScopedInfo& operator << ( const T& value ) {
        m_resultBuilder << value;
        return *this; 
    }
    AssertionResult buildResult( const AssertionInfo& assertionInfo ) const {
        return m_resultBuilder.buildResult( assertionInfo );
    }
    
private:
    ExpressionResultBuilder m_resultBuilder;
};
        
// This is just here to avoid compiler warnings with macro constants and boolean literals
inline bool isTrue( bool value ){ return value; }

} // end namespace Catch

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ASSERTIONINFO_NAME INTERNAL_CATCH_UNIQUE_NAME( __assertionInfo )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ACCEPT_EXPR( evaluatedExpr, stopOnFailure, originalExpr ) \
    if( Catch::ResultAction::Value internal_catch_action = Catch::getResultCapture().acceptExpression( evaluatedExpr, INTERNAL_CATCH_ASSERTIONINFO_NAME )  ) { \
        if( internal_catch_action & Catch::ResultAction::Debug ) BreakIntoDebugger(); \
        if( internal_catch_action & Catch::ResultAction::Abort ) throw Catch::TestFailureException(); \
        if( Catch::isTrue( stopOnFailure ) ) throw Catch::TestFailureException(); \
        if( Catch::isTrue( false ) ){ bool this_is_here_to_invoke_warnings = ( originalExpr ); Catch::isTrue( this_is_here_to_invoke_warnings ); } \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ACCEPT_INFO( expr, macroName, shouldNegate ) \
    Catch::AssertionInfo INTERNAL_CATCH_ASSERTIONINFO_NAME( macroName, CATCH_INTERNAL_LINEINFO, expr, shouldNegate );
// !TBD    Catch::getResultCapture().acceptAssertionInfo( INTERNAL_CATCH_ASSERTIONINFO_NAME )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TEST( expr, shouldNegate, stopOnFailure, macroName ) \
    do { \
        INTERNAL_CATCH_ACCEPT_INFO( #expr, macroName, shouldNegate ); \
        try { \
            INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionDecomposer()->*expr ).negate( shouldNegate ), stopOnFailure, expr ); \
        } catch( Catch::TestFailureException& ) { \
            throw; \
        } catch( ... ) { \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException(), false, expr ); \
            throw; \
        } \
    } while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_IF( expr, shouldNegate, stopOnFailure, macroName ) \
    INTERNAL_CATCH_TEST( expr, shouldNegate, stopOnFailure, macroName ); \
    if( Catch::getResultCapture().getLastResult()->ok() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ELSE( expr, shouldNegate, stopOnFailure, macroName ) \
    INTERNAL_CATCH_TEST( expr, shouldNegate, stopOnFailure, macroName ); \
    if( !Catch::getResultCapture().getLastResult()->ok() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_NO_THROW( expr, stopOnFailure, macroName ) \
    do { \
        INTERNAL_CATCH_ACCEPT_INFO( #expr, macroName, false ); \
        try { \
            expr; \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( Catch::ResultWas::Ok ), stopOnFailure, false ); \
        } \
        catch( ... ) { \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException(), stopOnFailure, false ); \
        } \
} while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_IMPL( expr, exceptionType, stopOnFailure ) \
    try { \
        if( Catch::getCurrentContext().getConfig()->allowThrows() ) { \
            expr; \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( Catch::ResultWas::DidntThrowException ), stopOnFailure, false ); \
        } \
    } \
    catch( Catch::TestFailureException& ) { \
        throw; \
    } \
    catch( exceptionType ) { \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( Catch::ResultWas::Ok ), stopOnFailure, false ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS( expr, exceptionType, stopOnFailure, macroName ) \
    do { \
        INTERNAL_CATCH_ACCEPT_INFO( #expr, macroName, false ); \
        INTERNAL_CATCH_THROWS_IMPL( expr, exceptionType, stopOnFailure ) \
    } while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_AS( expr, exceptionType, stopOnFailure, macroName ) \
    do { \
        INTERNAL_CATCH_ACCEPT_INFO( #expr, macroName, false ); \
        INTERNAL_CATCH_THROWS_IMPL( expr, exceptionType, stopOnFailure ) \
        catch( ... ) { \
            INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException() ), stopOnFailure, false ); \
        } \
    } while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_MSG( reason, resultType, stopOnFailure, macroName ) \
    INTERNAL_CATCH_ACCEPT_INFO( "", macroName, false ); \
    INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( resultType ) << reason, stopOnFailure, true );

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_SCOPED_INFO( log, macroName ) \
    INTERNAL_CATCH_ACCEPT_INFO( "", macroName, false ); \
    Catch::ScopedInfo INTERNAL_CATCH_UNIQUE_NAME( info ); \
    INTERNAL_CATCH_UNIQUE_NAME( info ) << log

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CHECK_THAT( arg, matcher, stopOnFailure, macroName ) \
    do { \
        INTERNAL_CATCH_ACCEPT_INFO( #arg " " #matcher, macroName, false ); \
        try { \
            INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::expressionResultBuilderFromMatcher( ::Catch::Matchers::matcher, arg, #matcher ) ), stopOnFailure, false ); \
        } catch( Catch::TestFailureException& ) { \
            throw; \
        } catch( ... ) { \
            INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException() ), false, false ); \
            throw; \
        } \
    } while( Catch::isTrue( false ) )

#endif // TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
