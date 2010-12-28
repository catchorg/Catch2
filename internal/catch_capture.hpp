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
#include "catch_debugger.hpp"
#include <sstream>
#include <cmath>

namespace Catch
{
namespace Detail
{
    // The following code, contributed by Sam Partington, allows us to choose an implementation
    // of toString() depending on whether a << overload is available
    
    struct NonStreamable
    {
        // allow construction from anything...
        template<typename Anything> 
        NonStreamable(Anything)
        {}
    };
    
    // a local operator<<  which may be called if there isn't a better one elsewhere...
    inline NonStreamable operator << ( std::ostream&, const NonStreamable& ns )
    {
        return ns;
    }

    template<typename T>
    struct IsStreamable
    {
        static NoType Deduce( const NonStreamable& );
        static YesType Deduce( std::ostream& );

        enum
        {
            value = sizeof( Deduce( Synth<std::ostream&>() << Synth<const T&>() ) ) 
                        == sizeof( YesType )
        };
    };
    
    // << is available, so use it with ostringstream to make the string
    template<typename T, bool streamable>
    struct StringMaker
    {
        static std::string apply( const T& value )
        {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }
    };
      
    // << not available - use a default string
    template<typename T>
    struct StringMaker<T, false>
    {
        static std::string apply( const T& )
        {
            return "{?}";
        }
    };

}// end namespace Detail

template<typename T>
std::string toString( const T& value )
{
    return Detail::StringMaker<T, Detail::IsStreamable<T>::value>::apply( value );
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
    MutableResultInfo& operator ||( const RhsT& )
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
class ScopedInfo;
    
struct IResultListener
{
    virtual ~IResultListener(){}
    virtual void testEnded( const ResultInfo& result ) = 0;
    virtual bool sectionStarted( const std::string& name, const std::string& description, std::size_t& successes, std::size_t& failures ) = 0;
    virtual void sectionEnded( const std::string& name, std::size_t successes, std::size_t failures ) = 0;
    virtual void pushScopedInfo( ScopedInfo* scopedInfo ) = 0;
    virtual void popScopedInfo( ScopedInfo* scopedInfo ) = 0;
    virtual bool shouldDebugBreak() const = 0;
};
  
    struct ResultAction
    {
        enum Value
        {
            None,
            Failed = 1,     // Failure - but no debug break if Debug bit not set
            DebugFailed = 3 // Indicates that the debugger should break, if possible
        };
        
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

    static ResultAction::Value acceptResult( bool result )
    {
        return acceptResult( result ? ResultWas::Ok : ResultWas::ExpressionFailed );
    }
    
    static ResultAction::Value acceptResult( ResultWas::OfType result )
    {
        MutableResultInfo& currentResult = instance().currentResult;
        currentResult.setResultType( result );
        
        if( instance().m_listener )
        {
            instance().m_listener->testEnded( currentResult );
        }
        bool ok = currentResult.ok();
        instance().currentResult = MutableResultInfo();
        if( ok )
            return ResultAction::None;
        else if( instance().m_listener->shouldDebugBreak() )
            return ResultAction::DebugFailed;
        else
            return ResultAction::Failed;
    }

    static void acceptMessage( const std::string& msg )
    {
        instance().currentResult.setMessage( msg );
    }

    static bool acceptSectionStart( const std::string& name, const std::string& description, std::size_t& successes, std::size_t& failures )
    {
        return instance().m_listener->sectionStarted( name, description, successes, failures );
    }
    
    static void acceptSectionEnd( const std::string& name, std::size_t successes, std::size_t failures )
    {
        instance().m_listener->sectionEnded( name, successes, failures );
    }
    
    static void pushScopedInfo( ScopedInfo* scopedInfo )
    {
        instance().m_listener->pushScopedInfo( scopedInfo );
    }
    static void popScopedInfo( ScopedInfo* scopedInfo )
    {
        instance().m_listener->popScopedInfo( scopedInfo );
    }
    
private:
    MutableResultInfo currentResult;
    IResultListener* m_listener;

};

class ScopedInfo
{
public:
    ScopedInfo()
    {
        ResultsCapture::pushScopedInfo( this );
    }
    
    ~ScopedInfo()
    {
        ResultsCapture::popScopedInfo( this );
    }
    
    ScopedInfo& operator << ( const char* str )
    {
        m_oss << str;
        return *this; 
    }
    
    std::string getInfo() const
    {
        return m_oss.str();
    }
    
private:
    std::ostringstream m_oss;
};
    
    
// !TBD Need to clean this all up
#define CATCH_absTol 1e-10
#define CATCH_relTol 1e-10

inline double catch_max( double x, double y )
{
    return x > y ? x : y;
}
    
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
        return fabs( lhs - rhs.m_d ) <= catch_max( CATCH_absTol, CATCH_relTol * catch_max( fabs(lhs), fabs(rhs.m_d) ) );
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

#define INTERNAL_CATCH_ACCEPT_RESULT( result, stopOnFailure ) \
    if( Catch::ResultAction::Value action = Catch::ResultsCapture::acceptResult( result )  ) \
    { \
        if( action == Catch::ResultAction::DebugFailed ) DebugBreak(); \
        if( stopOnFailure ) throw Catch::TestFailureException(); \
    }

#define INTERNAL_CATCH_TEST( expr, isNot, stopOnFailure, macroName ) \
    Catch::ResultsCapture::acceptExpression( Catch::ResultBuilder( #expr, isNot, __FILE__, __LINE__, macroName )->*expr ); \
    INTERNAL_CATCH_ACCEPT_RESULT( expr, stopOnFailure )

#define INTERNAL_CATCH_THROWS( expr, exceptionType, nothrow, stopOnFailure, macroName ) \
    Catch::ResultsCapture::acceptExpression( Catch::ResultBuilder( #expr, false, __FILE__, __LINE__, macroName ) ); \
    try \
    { \
        expr; \
        INTERNAL_CATCH_ACCEPT_RESULT( nothrow, stopOnFailure ) \
    } \
    catch( exceptionType ) \
    { \
        INTERNAL_CATCH_ACCEPT_RESULT( !(nothrow), stopOnFailure ) \
    }

#define INTERNAL_CATCH_THROWS_AS( expr, exceptionType, nothrow, stopOnFailure, macroName ) \
INTERNAL_CATCH_THROWS( expr, exceptionType, nothrow, stopOnFailure, macroName ) \
catch( ... ) \
{ \
    INTERNAL_CATCH_ACCEPT_RESULT( false, stopOnFailure ) \
}

#define INTERNAL_CATCH_MSG( reason, resultType, stopOnFailure, macroName ) \
    std::ostringstream INTERNAL_CATCH_UNIQUE_NAME( strm ); \
    INTERNAL_CATCH_UNIQUE_NAME( strm ) << reason; \
    Catch::ResultsCapture::acceptExpression( Catch::MutableResultInfo( "", false, __FILE__, __LINE__, macroName ) ); \
    Catch::ResultsCapture::acceptMessage( INTERNAL_CATCH_UNIQUE_NAME( strm ).str() ); \
    INTERNAL_CATCH_ACCEPT_RESULT( resultType, stopOnFailure ) \

#define INTERNAL_CATCH_SCOPED_INFO( log ) Catch::ScopedInfo INTERNAL_CATCH_UNIQUE_NAME( info ); INTERNAL_CATCH_UNIQUE_NAME( info ) << log

#endif // TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
