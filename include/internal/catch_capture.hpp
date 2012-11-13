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
#define INTERNAL_CATCH_ACCEPT_EXPR( evaluatedExpr, resultDisposition, originalExpr ) \
    if( Catch::ResultAction::Value internal_catch_action = Catch::getResultCapture().acceptExpression( evaluatedExpr, INTERNAL_CATCH_ASSERTIONINFO_NAME )  ) { \
        if( internal_catch_action & Catch::ResultAction::Debug ) BreakIntoDebugger(); \
        if( internal_catch_action & Catch::ResultAction::Abort ) throw Catch::TestFailureException(); \
        if( !Catch::shouldContinueOnFailure( resultDisposition ) ) throw Catch::TestFailureException(); \
        if( Catch::isTrue( false ) ){ bool this_is_here_to_invoke_warnings = ( originalExpr ); Catch::isTrue( this_is_here_to_invoke_warnings ); } \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ACCEPT_INFO( expr, macroName, resultDisposition ) \
    Catch::AssertionInfo INTERNAL_CATCH_ASSERTIONINFO_NAME( macroName, CATCH_INTERNAL_LINEINFO, expr, resultDisposition );

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TEST( expr, resultDisposition, macroName ) \
    do { \
        INTERNAL_CATCH_ACCEPT_INFO( #expr, macroName, resultDisposition ); \
        try { \
            INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionDecomposer()->*expr ).endExpression( resultDisposition ), resultDisposition, expr ); \
        } catch( Catch::TestFailureException& ) { \
            throw; \
        } catch( ... ) { \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException(), \
                resultDisposition | Catch::ResultDisposition::ContinueOnFailure, expr ); \
            throw; \
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
        INTERNAL_CATCH_ACCEPT_INFO( #expr, macroName, resultDisposition ); \
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
        INTERNAL_CATCH_ACCEPT_INFO( #expr, macroName, resultDisposition ); \
        INTERNAL_CATCH_THROWS_IMPL( expr, exceptionType, resultDisposition ) \
    } while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_AS( expr, exceptionType, resultDisposition, macroName ) \
    do { \
        INTERNAL_CATCH_ACCEPT_INFO( #expr, macroName, resultDisposition ); \
        INTERNAL_CATCH_THROWS_IMPL( expr, exceptionType, resultDisposition ) \
        catch( ... ) { \
            INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException() ), \
                resultDisposition | Catch::ResultDisposition::ContinueOnFailure, false ); \
        } \
    } while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_MSG( reason, resultType, resultDisposition, macroName ) \
    do { \
        INTERNAL_CATCH_ACCEPT_INFO( "", macroName, resultDisposition ); \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( resultType ) << reason, resultDisposition, true ) \
    } while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_SCOPED_INFO( log, macroName ) \
    INTERNAL_CATCH_ACCEPT_INFO( "", macroName, Catch::ResultDisposition::Normal ); \
    Catch::ScopedInfo INTERNAL_CATCH_UNIQUE_NAME( info ); \
    INTERNAL_CATCH_UNIQUE_NAME( info ) << log

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CHECK_THAT( arg, matcher, resultDisposition, macroName ) \
    do { \
        INTERNAL_CATCH_ACCEPT_INFO( #arg " " #matcher, macroName, resultDisposition ); \
        try { \
            INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::expressionResultBuilderFromMatcher( ::Catch::Matchers::matcher, arg, #matcher ) ), resultDisposition, false ); \
        } catch( Catch::TestFailureException& ) { \
            throw; \
        } catch( ... ) { \
            INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException() ), \
                resultDisposition | Catch::ResultDisposition::ContinueOnFailure, false ); \
            throw; \
        } \
    } while( Catch::isTrue( false ) )

#endif // TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
