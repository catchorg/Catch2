/*
 *  Created by Phil on 18/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED

#include "catch_resultinfo_builder.hpp"
#include "catch_tostring.hpp"
#include "catch_resultinfo.hpp"
#include "catch_result_type.h"
#include "catch_interfaces_capture.h"
#include "catch_debugger.hpp"
#include "catch_evaluate.hpp"
#include "catch_context.h"
#include "catch_common.h"
#include <sstream>

namespace Catch
{
    
struct TestFailureException{};
struct DummyExceptionType_DontUse{};

template<typename T>
class Expression
{
	void operator = ( const Expression& );

public:
    ///////////////////////////////////////////////////////////////////////////
    Expression
    (
        ResultInfoBuilder& result, 
        T lhs 
    )
    :   m_result( result ),
        m_lhs( lhs )
    {
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    ResultInfoBuilder& operator == 
    (
        const RhsT& rhs
    )
    {
        return m_result.captureExpression<Internal::IsEqualTo>( m_lhs, rhs );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    ResultInfoBuilder& operator != 
    (
        const RhsT& rhs
    )
    {
        return m_result.captureExpression<Internal::IsNotEqualTo>( m_lhs, rhs );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    ResultInfoBuilder& operator <
    (
        const RhsT& rhs
    )
    {
        return m_result.captureExpression<Internal::IsLessThan>( m_lhs, rhs );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    ResultInfoBuilder& operator >
    (
        const RhsT& rhs
    )
    {
        return m_result.captureExpression<Internal::IsGreaterThan>( m_lhs, rhs );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    ResultInfoBuilder& operator <= 
    (
        const RhsT& rhs
    )
    {
        return m_result.captureExpression<Internal::IsLessThanOrEqualTo>( m_lhs, rhs );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    ResultInfoBuilder& operator >= 
    (
        const RhsT& rhs
    )
    {
        return m_result.captureExpression<Internal::IsGreaterThanOrEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    ResultInfoBuilder& operator == 
    (
        bool rhs
    )
    {
        return m_result.captureExpression<Internal::IsEqualTo>( m_lhs, rhs );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    ResultInfoBuilder& operator != 
    (
        bool rhs
    )
    {
        return m_result.captureExpression<Internal::IsNotEqualTo>( m_lhs, rhs );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    operator ResultInfoBuilder&
    ()
    {
        return m_result.captureBoolExpression( m_lhs );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator +
    (
        const RhsT&
    );
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator -
    (
        const RhsT&
    );

private:
    ResultInfoBuilder& m_result;
    T m_lhs;
};
    
template<typename LhsT> 
class PtrExpression
{
public:

    ///////////////////////////////////////////////////////////////////////////
    PtrExpression
    (
        ResultInfoBuilder& result, 
        const LhsT* lhs 
    )
    :   m_result( &result ),
        m_lhs( lhs )
    {}
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    ResultInfoBuilder& operator == 
    (
        const RhsT* rhs
    )
    {
        return m_result->captureExpression<Internal::IsEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    // This catches NULL
    ResultInfoBuilder& operator == 
    (
        LhsT* rhs
    )
    {
        return m_result->captureExpression<Internal::IsEqualTo>( m_lhs, rhs );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    ResultInfoBuilder& operator != 
    (
        const RhsT* rhs
    )
    {
        return m_result->captureExpression<Internal::IsNotEqualTo>( m_lhs, rhs );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // This catches NULL
    ResultInfoBuilder& operator != 
    (
        LhsT* rhs
    )
    {
        return m_result->captureExpression<Internal::IsNotEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    operator ResultInfoBuilder&
    ()
    {
        return m_result->captureBoolExpression( m_lhs );
    }
    
    
private:
    ResultInfoBuilder* m_result;
    const LhsT* m_lhs;
};

    
class ResultBuilder
{
public:

    ///////////////////////////////////////////////////////////////////////////
    ResultBuilder
    (
        const SourceLineInfo& lineInfo,
        const char* macroName,
        const char* expr = "",
        bool isNot = false
    )
    : m_result( expr, isNot, lineInfo, macroName ),
      m_messageStream()
    {}
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename T>
    Expression<const T&> operator->*
    (
        const T & operand
    )
    {
        Expression<const T&> expr( m_result, operand );
        
        return expr;
    }

    ///////////////////////////////////////////////////////////////////////////
    Expression<const char*> operator->*
    (
        const char* const& operand
     )
    {
        Expression<const char*> expr( m_result, operand );
        
        return expr;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename T>
    PtrExpression<T> operator->*
    (
        const T* operand
    )
    {
        PtrExpression<T> expr( m_result, operand );
        
        return expr;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename T>
    PtrExpression<T> operator->*
    (
        T* operand
    )
    {
        PtrExpression<T> expr( m_result, operand );
        
        return expr;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    Expression<bool> operator->*
    (
        bool value
    )
    {
        Expression<bool> expr( m_result, value );
        return expr;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename T>
    ResultBuilder& operator <<
    (
        const T & value
    )
    {
        m_messageStream << Catch::toString( value );        
        return *this;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename MatcherT, typename ArgT>
    ResultBuilder& acceptMatcher
    (
        const MatcherT& matcher,
        const ArgT& arg,
        const std::string& matcherCallAsString
    )
    {
        std::string matcherAsString = Catch::toString( matcher );
        if( matcherAsString == "{?}" )
            matcherAsString = matcherCallAsString;
        m_result.setLhs( Catch::toString( arg ) );
        m_result.setRhs( matcherAsString );
        m_result.setOp( "matches" );
        m_result.setResultType( matcher( arg ) ? ResultWas::Ok : ResultWas::ExpressionFailed );
        return *this;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename MatcherT, typename ArgT>
    ResultBuilder& acceptMatcher
    (
        const MatcherT& matcher,
        ArgT* arg,
        const std::string& matcherCallAsString
    )
    {
        std::string matcherAsString = Catch::toString( matcher );
        if( matcherAsString == "{?}" )
            matcherAsString = matcherCallAsString;
        m_result.setLhs( Catch::toString( arg ) );
        m_result.setRhs( matcherAsString );
        m_result.setOp( "matches" );
        m_result.setResultType( matcher( arg ) ? ResultWas::Ok : ResultWas::ExpressionFailed );
        return *this;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    ResultBuilder& setResultType
    (
        ResultWas::OfType resultType
    )
    {
        m_result.setResultType( resultType );
        return *this;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    operator ResultInfoBuilder&
    ()
    {
        m_result.setMessage( m_messageStream.str() );
        return m_result;
    }
    
private:
    ResultInfoBuilder m_result;
    std::ostringstream m_messageStream;
    
};

class ScopedInfo
{
public:
    ScopedInfo() : m_oss() {
        Context::getResultCapture().pushScopedInfo( this );
    }
    
    ~ScopedInfo() {
        Context::getResultCapture().popScopedInfo( this );
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
    if( Catch::ResultAction::Value internal_catch_action = Catch::Context::getResultCapture().acceptExpression( expr )  ) \
    { \
        if( internal_catch_action == Catch::ResultAction::DebugFailed ) BreakIntoDebugger(); \
        if( Catch::isTrue( stopOnFailure ) ) throw Catch::TestFailureException(); \
        if( Catch::isTrue( false ) ){ bool this_is_here_to_invoke_warnings = ( originalExpr ); Catch::isTrue( this_is_here_to_invoke_warnings ); } \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TEST( expr, isNot, stopOnFailure, macroName ) \
    do{ try{ \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ResultBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr, isNot )->*expr ), stopOnFailure, expr ); \
    }catch( Catch::TestFailureException& ){ \
        throw; \
    } catch( ... ){ \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ResultBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr ) << Catch::Context::getExceptionTranslatorRegistry().translateActiveException() ).setResultType( Catch::ResultWas::ThrewException ), false, expr ); \
        throw; \
    }}while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_IF( expr, isNot, stopOnFailure, macroName ) \
    INTERNAL_CATCH_TEST( expr, isNot, stopOnFailure, macroName ); \
    if( Catch::Context::getResultCapture().getLastResult()->ok() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ELSE( expr, isNot, stopOnFailure, macroName ) \
    INTERNAL_CATCH_TEST( expr, isNot, stopOnFailure, macroName ); \
    if( !Catch::Context::getResultCapture().getLastResult()->ok() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_NO_THROW( expr, stopOnFailure, macroName ) \
    try \
    { \
        expr; \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::ResultBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr ).setResultType( Catch::ResultWas::Ok ), stopOnFailure, false ); \
    } \
    catch( ... ) \
    { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ResultBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr ) << Catch::Context::getExceptionTranslatorRegistry().translateActiveException() ).setResultType( Catch::ResultWas::ThrewException ), stopOnFailure, false ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS( expr, exceptionType, stopOnFailure, macroName ) \
    try \
    { \
        expr; \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::ResultBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr ).setResultType( Catch::ResultWas::DidntThrowException ), stopOnFailure, false ); \
    } \
    catch( Catch::TestFailureException& ) \
    { \
        throw; \
    } \
    catch( exceptionType ) \
    { \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::ResultBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr ).setResultType( Catch::ResultWas::Ok ), stopOnFailure, false ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_AS( expr, exceptionType, stopOnFailure, macroName ) \
    INTERNAL_CATCH_THROWS( expr, exceptionType, stopOnFailure, macroName ) \
    catch( ... ) \
    { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ResultBuilder( CATCH_INTERNAL_LINEINFO, macroName, #expr ) << Catch::Context::getExceptionTranslatorRegistry().translateActiveException() ).setResultType( Catch::ResultWas::ThrewException ), stopOnFailure, false ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_MSG( reason, resultType, stopOnFailure, macroName ) \
    Catch::Context::getResultCapture().acceptExpression( ( Catch::ResultBuilder( CATCH_INTERNAL_LINEINFO, macroName ) << reason ).setResultType( resultType ) );

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_SCOPED_INFO( log ) \
    Catch::ScopedInfo INTERNAL_CATCH_UNIQUE_NAME( info ); \
    INTERNAL_CATCH_UNIQUE_NAME( info ) << log

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CHECK_THAT( arg, matcher, stopOnFailure, macroName ) \
    do{ try{ \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ResultBuilder( CATCH_INTERNAL_LINEINFO, macroName, #arg " " #matcher, false ).acceptMatcher( matcher, arg, #matcher ) ), stopOnFailure, false ); \
    }catch( Catch::TestFailureException& ){ \
        throw; \
    } catch( ... ){ \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ResultBuilder( CATCH_INTERNAL_LINEINFO, macroName, #arg " " #matcher ) << Catch::Context::getExceptionTranslatorRegistry().translateActiveException() ).setResultType( Catch::ResultWas::ThrewException ), false, false ); \
        throw; \
    }}while( Catch::isTrue( false ) )

#endif // TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
