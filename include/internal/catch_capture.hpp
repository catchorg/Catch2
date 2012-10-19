/*
 *  Created by Phil on 18/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED

#include "catch_expression_builder.hpp"
#include "catch_assertionresult_builder.h"
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
    AssertionResultBuilder assertionBuilderFromMatcher( const MatcherT& matcher,
                                                        const std::string& matcherCallAsString ) {
        std::string matcherAsString = matcher.toString();
        if( matcherAsString == "{?}" )
            matcherAsString = matcherCallAsString;
        return AssertionResultBuilder()
            .setRhs( matcherAsString )
            .setOp( "matches" );
    }

    template<typename MatcherT, typename ArgT>
    AssertionResultBuilder assertionBuilderFromMatcher( const MatcherT& matcher,
                                                        const ArgT& arg,
                                                        const std::string& matcherCallAsString ) {
        return assertionBuilderFromMatcher( matcher, matcherCallAsString )
            .setLhs( Catch::toString( arg ) )
            .setResultType( matcher.match( arg ) ? ResultWas::Ok : ResultWas::ExpressionFailed );
    }

    template<typename MatcherT, typename ArgT>
    AssertionResultBuilder assertionBuilderFromMatcher( const MatcherT& matcher,
                                                        ArgT* arg,
                                                        const std::string& matcherCallAsString ) {
        return assertionBuilderFromMatcher( matcher, matcherCallAsString )
            .setLhs( Catch::toString( arg ) )
            .setResultType( matcher.match( arg ) ? ResultWas::Ok : ResultWas::ExpressionFailed );
    }
    
struct TestFailureException{};

class ScopedInfo {
public:
    ScopedInfo() {
        m_resultBuilder
            .setResultType( ResultWas::Info )
            .setMacroName( "SCOPED_INFO" );

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

    AssertionResult getInfo () const {
        return m_resultBuilder.build();
    }
    
private:
    AssertionResultBuilder m_resultBuilder;
    std::ostringstream m_oss;
};
        
// This is just here to avoid compiler warnings with macro constants and boolean literals
inline bool isTrue( bool value ){ return value; }

} // end namespace Catch

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ACCEPT_EXPR( evaluatedExpr, stopOnFailure, originalExpr ) \
    if( Catch::ResultAction::Value internal_catch_action = Catch::getResultCapture().acceptExpression( evaluatedExpr )  ) { \
        if( internal_catch_action & Catch::ResultAction::Debug ) BreakIntoDebugger(); \
        if( internal_catch_action & Catch::ResultAction::Abort ) throw Catch::TestFailureException(); \
        if( Catch::isTrue( stopOnFailure ) ) throw Catch::TestFailureException(); \
        if( Catch::isTrue( false ) ){ bool this_is_here_to_invoke_warnings = ( originalExpr ); Catch::isTrue( this_is_here_to_invoke_warnings ); } \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TEST( expr, isFalse, stopOnFailure, macroName ) \
    do { try { \
        Catch::getResultCapture().acceptAssertionInfo( Catch::AssertionInfo( macroName, CATCH_INTERNAL_LINEINFO, #expr  ) ); \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionBuilder()->*expr ).setIsFalse( isFalse ), stopOnFailure, expr ); \
    } catch( Catch::TestFailureException& ) { \
        throw; \
    } catch( ... ) { \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::AssertionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException(), false, expr ); \
        throw; \
    } } while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_IF( expr, isFalse, stopOnFailure, macroName ) \
    INTERNAL_CATCH_TEST( expr, isFalse, stopOnFailure, macroName ); \
    if( Catch::getResultCapture().getLastResult()->ok() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ELSE( expr, isFalse, stopOnFailure, macroName ) \
    INTERNAL_CATCH_TEST( expr, isFalse, stopOnFailure, macroName ); \
    if( !Catch::getResultCapture().getLastResult()->ok() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_NO_THROW( expr, stopOnFailure, macroName ) \
    try { \
        Catch::getResultCapture().acceptAssertionInfo( Catch::AssertionInfo( macroName, CATCH_INTERNAL_LINEINFO, #expr  ) ); \
        expr; \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::AssertionResultBuilder( Catch::ResultWas::Ok ), stopOnFailure, false ); \
    } \
    catch( ... ) { \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::AssertionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException(), stopOnFailure, false ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS( expr, exceptionType, stopOnFailure, macroName ) \
    try { \
        Catch::getResultCapture().acceptAssertionInfo( Catch::AssertionInfo( macroName, CATCH_INTERNAL_LINEINFO, #expr  ) ); \
        if( Catch::getCurrentContext().getConfig()->allowThrows() ) { \
            expr; \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::AssertionResultBuilder( Catch::ResultWas::DidntThrowException ), stopOnFailure, false ); \
        } \
    } \
    catch( Catch::TestFailureException& ) { \
        throw; \
    } \
    catch( exceptionType ) { \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::AssertionResultBuilder( Catch::ResultWas::Ok ), stopOnFailure, false ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_AS( expr, exceptionType, stopOnFailure, macroName ) \
    INTERNAL_CATCH_THROWS( expr, exceptionType, stopOnFailure, macroName ) \
    catch( ... ) { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::AssertionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException() ), stopOnFailure, false ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_MSG( reason, resultType, stopOnFailure, macroName ) \
    INTERNAL_CATCH_ACCEPT_EXPR( Catch::AssertionResultBuilder( resultType ) << reason, stopOnFailure, true );

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_SCOPED_INFO( log ) \
    Catch::ScopedInfo INTERNAL_CATCH_UNIQUE_NAME( info ); \
    INTERNAL_CATCH_UNIQUE_NAME( info ) << log

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CHECK_THAT( arg, matcher, stopOnFailure, macroName ) \
    do { try { \
        Catch::getResultCapture().acceptAssertionInfo( Catch::AssertionInfo( macroName, CATCH_INTERNAL_LINEINFO, #arg " " #matcher  ) ); \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::assertionBuilderFromMatcher( ::Catch::Matchers::matcher, arg, #matcher ) ), stopOnFailure, false ); \
    } catch( Catch::TestFailureException& ) { \
        throw; \
    } catch( ... ) { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::AssertionResultBuilder( Catch::ResultWas::ThrewException ) << Catch::translateActiveException() ), false, false ); \
        throw; \
    }}while( Catch::isTrue( false ) )

#endif // TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
