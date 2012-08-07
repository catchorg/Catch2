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
#include "catch_interfaces_capture.h"
#include "catch_debugger.hpp"
#include "catch_context.h"
#include "catch_common.h"
#include <ostream>

namespace Catch {

struct TestFailureException{};

class ScopedInfo {
public:
    ScopedInfo() : m_oss() {
        getCurrentContext().getResultCapture().pushScopedInfo( this );
    }
    
    ~ScopedInfo() {
        getCurrentContext().getResultCapture().popScopedInfo( this );
    }
    
    template<typename T>
    ScopedInfo& operator << ( const T& value ) {
        m_oss << value;
        return *this; 
    }
    
    std::string getInfo () const {
        return m_oss.str();
    }
    
private:
    std::ostringstream m_oss;
};
        
// This is just here to avoid compiler warnings with macro constants
inline bool isTrue( bool value ){ return value; }
    
} // end namespace Catch

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ACCEPT_EXPR( expr, stopOnFailure, originalExpr ) \
    if( Catch::ResultAction::Value internal_catch_action = Catch::getCurrentContext().getResultCapture().acceptExpression( expr )  ) { \
        if( internal_catch_action & Catch::ResultAction::Debug ) BreakIntoDebugger(); \
        if( internal_catch_action & Catch::ResultAction::Abort ) throw Catch::TestFailureException(); \
        if( Catch::isTrue( stopOnFailure ) ) throw Catch::TestFailureException(); \
        if( Catch::isTrue( false ) ){ bool this_is_here_to_invoke_warnings = ( originalExpr ); Catch::isTrue( this_is_here_to_invoke_warnings ); } \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TEST( expr, isNot, stopOnFailure, macroName ) \
    do { try { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr, isNot )->*expr ), stopOnFailure, expr ); \
    } catch( Catch::TestFailureException& ) { \
        throw; \
    } catch( ... ) { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr ) << Catch::getRegistryHub().getExceptionTranslatorRegistry().translateActiveException() ).setResultType( Catch::ResultWas::ThrewException ), false, expr ); \
        throw; \
    } } while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_IF( expr, isNot, stopOnFailure, macroName ) \
    INTERNAL_CATCH_TEST( expr, isNot, stopOnFailure, macroName ); \
    if( Catch::getCurrentContext().getResultCapture().getLastResult()->ok() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ELSE( expr, isNot, stopOnFailure, macroName ) \
    INTERNAL_CATCH_TEST( expr, isNot, stopOnFailure, macroName ); \
    if( !Catch::getCurrentContext().getResultCapture().getLastResult()->ok() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_NO_THROW( expr, stopOnFailure, macroName ) \
    try { \
        expr; \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr ).setResultType( Catch::ResultWas::Ok ), stopOnFailure, false ); \
    } \
    catch( ... ) { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr ) << Catch::getRegistryHub().getExceptionTranslatorRegistry().translateActiveException() ).setResultType( Catch::ResultWas::ThrewException ), stopOnFailure, false ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS( expr, exceptionType, stopOnFailure, macroName ) \
    try { \
        if( Catch::getCurrentContext().getConfig()->allowThrows() ) { \
            expr; \
            INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr ).setResultType( Catch::ResultWas::DidntThrowException ), stopOnFailure, false ); \
        } \
    } \
    catch( Catch::TestFailureException& ) { \
        throw; \
    } \
    catch( exceptionType ) { \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr ).setResultType( Catch::ResultWas::Ok ), stopOnFailure, false ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_AS( expr, exceptionType, stopOnFailure, macroName ) \
    INTERNAL_CATCH_THROWS( expr, exceptionType, stopOnFailure, macroName ) \
    catch( ... ) { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr ) << Catch::getRegistryHub().getExceptionTranslatorRegistry() ).setResultType( Catch::ResultWas::ThrewException ), stopOnFailure, false ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_MSG( reason, resultType, stopOnFailure, macroName ) \
    Catch::getCurrentContext().getResultCapture().acceptExpression( ( Catch::ExpressionBuilder( CATCH_INTERNAL_LINEINFO, macroName ) << reason ).setResultType( resultType ) );

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_SCOPED_INFO( log ) \
    Catch::ScopedInfo INTERNAL_CATCH_UNIQUE_NAME( info ); \
    INTERNAL_CATCH_UNIQUE_NAME( info ) << log

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CHECK_THAT( arg, matcher, stopOnFailure, macroName ) \
    do { try { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionBuilder( CATCH_INTERNAL_LINEINFO, macroName, #arg " " #matcher, false ).acceptMatcher( ::Catch::Matchers::matcher, arg, #matcher ) ), stopOnFailure, false ); \
    } catch( Catch::TestFailureException& ) { \
        throw; \
    } catch( ... ) { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ExpressionBuilder( CATCH_INTERNAL_LINEINFO, macroName, #arg " " #matcher ) << Catch::getRegistryHub().getExceptionTranslatorRegistry().translateActiveException() ).setResultType( Catch::ResultWas::ThrewException ), false, false ); \
        throw; \
    }}while( Catch::isTrue( false ) )

#endif // TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
