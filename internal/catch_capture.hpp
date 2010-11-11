/*
 *  catch_capture.hpp
 *  Catch
 *
 *  Created by Phil on 18/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED

#include "catch_resultinfo.hpp"
#include <sstream>
#include <cmath>

namespace Catch
{

template<typename T>
std::string toString( const T& value )
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

class TestFailureException
{
};
class DummyExceptionType_DontUse
{
};
    
class MutableResultInfo : public ResultInfo
{
public:
    
    MutableResultInfo()
    {}
    
    MutableResultInfo( const std::string& expr, bool isNot, const std::string& filename, std::size_t line, const std::string& macroName )
    : ResultInfo( ( isNot ? "!" : "" ) + expr, ResultWas::Unknown, isNot, filename, line, macroName )
    {
    }
    void setResultType( ResultWas::OfType result )
    {
        // Flip bool results if isNot is set
        if( m_isNot && result == ResultWas::Ok )
            m_result = ResultWas::ExpressionFailed;
        else if( m_isNot && result == ResultWas::ExpressionFailed )
            m_result = ResultWas::Ok;
        else
            m_result = result;        
    }
    void setMessage( const std::string& message )
    {
        m_message = message;
    }

    template<typename RhsT>
    MutableResultInfo& operator ||( const RhsT& rhs )
    {
        m_expressionIncomplete = true;
        return *this;
    }
        
private:
    friend class ResultBuilder;
    void setLhs( const std::string& lhs )
    {
        m_lhs = lhs;
    }    
    MutableResultInfo& setRhs( const std::string& op, const std::string& rhs )
    {
        m_op = op;
        m_rhs = rhs;
        return *this;
    }    
};
    
class ResultBuilder
{
public:
    ResultBuilder( const char* expr, bool isNot, const std::string& filename, std::size_t line, const std::string& macroName )
    : m_result( expr, isNot, filename, line, macroName )
    {}
    
    template<typename T>
    ResultBuilder& operator->*(const T & operand)
    {
        m_result.setLhs( toString( operand ) );
        return *this;
    }
    
    template<typename RhsT>
    MutableResultInfo& operator == ( const RhsT& rhs )
    {
        return m_result.setRhs( "==", toString( rhs ) );
    }    
    template<typename RhsT>
    MutableResultInfo& operator != ( const RhsT& rhs )
    {
        return m_result.setRhs( "!=", toString( rhs ) );
    }    
    template<typename RhsT>
    MutableResultInfo& operator < ( const RhsT& rhs )
    {
        return m_result.setRhs( "<", toString( rhs ) );
    }    
    template<typename RhsT>
    MutableResultInfo& operator > ( const RhsT& rhs )
    {
        return m_result.setRhs( ">", toString( rhs ) );
    }    
    template<typename RhsT>
    MutableResultInfo& operator <= ( const RhsT& rhs )
    {
        return m_result.setRhs( "<=", toString( rhs ) );
    }    
    template<typename RhsT>
    MutableResultInfo& operator >= ( const RhsT& rhs )
    {
        return m_result.setRhs( ">=", toString( rhs ) );
    }    

    operator MutableResultInfo&()
    {
        return m_result;
    }
    
private:
    MutableResultInfo m_result;
    
};

class TestCaseInfo;
    
struct IResultListener
{
    virtual ~IResultListener(){}
    virtual void testEnded( const ResultInfo& result ) = 0;
};
    
class ResultsCapture
{
private:
    ResultsCapture()
    : m_listener( 0 )
    {
    }

    static ResultsCapture& instance()
    {
        static ResultsCapture instance;
        return instance;
    }
    
public:
    
    static IResultListener* setListener( IResultListener* listener )
    {
        IResultListener* prevListener = instance().m_listener;
        instance().m_listener = listener;
        return prevListener;
    }

    static void acceptExpression( const MutableResultInfo& resultInfo )
    {
        instance().currentResult = resultInfo;
    }

    static void acceptResult( bool result, bool stopOnFail )
    {
        acceptResult( result ? ResultWas::Ok : ResultWas::ExpressionFailed, stopOnFail );
    }
    
    static void acceptResult( ResultWas::OfType result, bool stopOnFail )
    {
        if( !acceptResult( result ) && stopOnFail )
        {
            throw TestFailureException();
        }
    }
    
    static bool acceptResult( ResultWas::OfType result )
    {
        MutableResultInfo& currentResult = instance().currentResult;
        currentResult.setResultType( result );
        
        if( instance().m_listener )
        {
            instance().m_listener->testEnded( currentResult );
        }
        bool ok = currentResult.ok();
        instance().currentResult = MutableResultInfo();
        return ok;
    }

    static bool acceptResult( bool expressionResult )
    {
        return acceptResult( expressionResult ? ResultWas::Ok : ResultWas::ExpressionFailed );
    }
    
    static void acceptMessage( const std::string& msg )
    {
        instance().currentResult.setMessage( msg );
    }
    
private:
    MutableResultInfo currentResult;
    IResultListener* m_listener;
};

// !TBD Need to clean this all up
#define CATCH_absTol 1e-10
#define CATCH_relTol 1e-10

class Approx
{
public:
    // !TBD more generic
    Approx( double d )
    : m_d( d )
    {
    }
    template<typename T>
    friend bool operator == ( const T& lhs, const Approx& rhs )
    {
        // !TBD Use proper tolerance
        // From: http://realtimecollisiondetection.net/blog/?p=89
        // see also: http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
        return fabs( lhs - rhs.m_d ) <= std::max( CATCH_absTol, CATCH_relTol * std::max( fabs(lhs), fabs(rhs.m_d) ) );
    }
    
    template<typename T>
    friend bool operator != ( const T& lhs, const Approx& rhs )
    {
        return ! operator==( lhs, rhs );
    }
    
    double m_d;
};

template<>
inline std::string toString<Approx>( const Approx& value )
{
    std::ostringstream oss;
    oss << "Approx( " << value.m_d << ")";
    return oss.str();
}

    
} // end namespace Catch

#define INTERNAL_CATCH_TEST( expr, isNot, stopOnFailure, macroName ) \
    Catch::ResultsCapture::acceptExpression( Catch::ResultBuilder( #expr, isNot, __FILE__, __LINE__, macroName )->*expr ); \
    Catch::ResultsCapture::acceptResult( expr, stopOnFailure );

#define INTERNAL_CATCH_THROWS( expr, exceptionType, nothrow, stopOnFailure, macroName ) \
    Catch::ResultsCapture::acceptExpression( Catch::ResultBuilder( #expr, false, __FILE__, __LINE__, macroName ) ); \
    try \
    { \
        expr; \
        Catch::ResultsCapture::acceptResult( nothrow, stopOnFailure ); \
    } \
    catch( exceptionType ) \
    { \
        Catch::ResultsCapture::acceptResult( !(nothrow), stopOnFailure ); \
    }

#define INTERNAL_CATCH_THROWS_AS( expr, exceptionType, nothrow, stopOnFailure, macroName ) \
INTERNAL_CATCH_THROWS( expr, exceptionType, nothrow, stopOnFailure, macroName ) \
catch( ... ) \
{ \
    Catch::ResultsCapture::acceptResult( false, stopOnFailure ); \
}

#define INTERNAL_CATCH_MSG( reason, resultType, stopOnFailure, macroName ) \
    Catch::ResultsCapture::acceptExpression( Catch::MutableResultInfo( "", false, __FILE__, __LINE__, macroName ) ); \
    Catch::ResultsCapture::acceptMessage( reason ); \
    Catch::ResultsCapture::acceptResult( resultType, stopOnFailure );


#endif // TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
