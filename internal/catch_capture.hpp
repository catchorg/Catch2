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
#include "catch_result_type.h"
#include "catch_interfaces_capture.h"
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
        ///////////////////////////////////////////////////////////////////////
        static std::string apply
        (
            const T& value
        )
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
        ///////////////////////////////////////////////////////////////////////
        static std::string apply
        (
            const T&
        )
        {
            return "{?}";
        }
    };

}// end namespace Detail

///////////////////////////////////////////////////////////////////////////////
template<typename T>
std::string toString
(
    const T& value
)
{
    return Detail::StringMaker<T, Detail::IsStreamable<T>::value>::apply( value );
}
    
// Shortcut overloads

///////////////////////////////////////////////////////////////////////////////
inline std::string toString
(
    const std::string& value
)
{
    return value;
}

///////////////////////////////////////////////////////////////////////////////
inline std::string toString
(
    const char* value
)
{
    return value;
}    

///////////////////////////////////////////////////////////////////////////////
inline std::string toString
(
    int value
)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
inline std::string toString
(
    const double value 
)
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
    
    ///////////////////////////////////////////////////////////////////////////
    MutableResultInfo
    ()
    {}
    
    ///////////////////////////////////////////////////////////////////////////
    MutableResultInfo
    (
        const char* expr, 
        bool isNot, 
        const char* filename, 
        std::size_t line, 
        const char* macroName 
    )
    : ResultInfo( expr, ResultWas::Unknown, isNot, filename, line, macroName )
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    void setResultType
    (
        ResultWas::OfType result
    )
    {
        // Flip bool results if isNot is set
        if( m_isNot && result == ResultWas::Ok )
            m_result = ResultWas::ExpressionFailed;
        else if( m_isNot && result == ResultWas::ExpressionFailed )
            m_result = ResultWas::Ok;
        else
            m_result = result;        
    }

    ///////////////////////////////////////////////////////////////////////////
    void setMessage
    (
        const std::string& message
    )
    {
        m_message = message;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator ||
    (
        const RhsT&
    )
    {
        m_expressionIncomplete = true;
        return *this;
    }
        
private:
    friend class ResultBuilder;

    ///////////////////////////////////////////////////////////////////////////
    void setLhs
    (
        const std::string& lhs
    )
    {
        m_lhs = lhs;
    }    

    ///////////////////////////////////////////////////////////////////////////
    MutableResultInfo& setRhs
    (
        const std::string& op, 
        const std::string& rhs 
    )
    {
        m_op = op;
        m_rhs = rhs;
        return *this;
    }    
};
    
class ResultBuilder
{
public:

    ///////////////////////////////////////////////////////////////////////////
    ResultBuilder
    (
        const char* expr, 
        bool isNot, 
        const char* filename, 
        std::size_t line, 
        const char* macroName
    )
    : m_result( expr, isNot, filename, line, macroName )
    {}
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename T>
    ResultBuilder& operator->*
    (
        const T & operand
    )
    {
        m_result.setLhs( toString( operand ) );
        return *this;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator == 
    (
        const RhsT& rhs
    )
    {
        return m_result.setRhs( "==", toString( rhs ) );
    }    

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator != 
    (
        const RhsT& rhs
    )
    {
        return m_result.setRhs( "!=", toString( rhs ) );
    }    

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator < 
    (
        const RhsT& rhs
    )
    {
        return m_result.setRhs( "<", toString( rhs ) );
    }    

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator > 
    (
        const RhsT& rhs 
    )
    {
        return m_result.setRhs( ">", toString( rhs ) );
    }    

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator <= 
    (
        const RhsT& rhs
    )
    {
        return m_result.setRhs( "<=", toString( rhs ) );
    }    

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator >= 
    (
        const RhsT& rhs
    )
    {
        return m_result.setRhs( ">=", toString( rhs ) );
    }    

    ///////////////////////////////////////////////////////////////////////////
    operator MutableResultInfo&
    ()
    {
        return m_result;
    }
    
private:
    MutableResultInfo m_result;
    
};

class ScopedInfo
{
public:
    ///////////////////////////////////////////////////////////////////////////
    ScopedInfo
    ()
    {
        Hub::getResultCapture().pushScopedInfo( this );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    ~ScopedInfo
    ()
    {
        Hub::getResultCapture().popScopedInfo( this );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    ScopedInfo& operator << 
    (
        const char* str
    )
    {
        m_oss << str;
        return *this; 
    }
    
    ///////////////////////////////////////////////////////////////////////////
    std::string getInfo
    ()
    const
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
    ///////////////////////////////////////////////////////////////////////////
    // !TBD more generic
    Approx
    (
        double d
    )
    : m_d( d )
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename T>
    friend bool operator == 
    (
        const T& lhs, 
        const Approx& rhs
    )
    {
        // !TBD Use proper tolerance
        // From: http://realtimecollisiondetection.net/blog/?p=89
        // see also: http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
        return fabs( lhs - rhs.m_d ) <= catch_max( CATCH_absTol, CATCH_relTol * catch_max( fabs(lhs), fabs(rhs.m_d) ) );
    }
    
    ///////////////////////////////////////////////////////////////////////////
    template<typename T>
    friend bool operator != 
    (
        const T& lhs, 
        const Approx& rhs
    )
    {
        return ! operator==( lhs, rhs );
    }
    
    double m_d;
};

///////////////////////////////////////////////////////////////////////////////
template<>
inline std::string toString<Approx>
(
    const Approx& value
)
{
    std::ostringstream oss;
    oss << "Approx( " << value.m_d << ")";
    return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
// This is just here to avoid compiler warnings with macro constants
inline bool isTrue
(
    bool value
)
{
    return value;
}
    
} // end namespace Catch

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ACCEPT_RESULT( result, stopOnFailure ) \
    if( Catch::ResultAction::Value action = Catch::Hub::getResultCapture().acceptResult( result )  ) \
    { \
        if( action == Catch::ResultAction::DebugFailed ) DebugBreak(); \
        if( Catch::isTrue( stopOnFailure ) ) throw Catch::TestFailureException(); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TEST( expr, isNot, stopOnFailure, macroName ) \
    { \
        Catch::Hub::getResultCapture().acceptExpression( Catch::ResultBuilder( #expr, isNot, __FILE__, __LINE__, macroName )->*expr ); \
        INTERNAL_CATCH_ACCEPT_RESULT( expr, stopOnFailure ) \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS( expr, exceptionType, nothrow, stopOnFailure, macroName ) \
    Catch::Hub::getResultCapture().acceptExpression( Catch::ResultBuilder( #expr, false, __FILE__, __LINE__, macroName ) ); \
    try \
    { \
        expr; \
        INTERNAL_CATCH_ACCEPT_RESULT( nothrow, stopOnFailure ) \
    } \
    catch( exceptionType ) \
    { \
        INTERNAL_CATCH_ACCEPT_RESULT( !(nothrow), stopOnFailure ) \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_AS( expr, exceptionType, nothrow, stopOnFailure, macroName ) \
INTERNAL_CATCH_THROWS( expr, exceptionType, nothrow, stopOnFailure, macroName ) \
catch( ... ) \
{ \
    INTERNAL_CATCH_ACCEPT_RESULT( false, stopOnFailure ) \
}

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_MSG( reason, resultType, stopOnFailure, macroName ) \
    { \
        std::ostringstream INTERNAL_CATCH_UNIQUE_NAME( strm ); \
        INTERNAL_CATCH_UNIQUE_NAME( strm ) << reason; \
        Catch::Hub::getResultCapture().acceptExpression( Catch::MutableResultInfo( "", false, __FILE__, __LINE__, macroName ) ); \
        Catch::Hub::getResultCapture().acceptMessage( INTERNAL_CATCH_UNIQUE_NAME( strm ).str() ); \
        INTERNAL_CATCH_ACCEPT_RESULT( resultType, stopOnFailure ) \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_SCOPED_INFO( log ) Catch::ScopedInfo INTERNAL_CATCH_UNIQUE_NAME( info ); INTERNAL_CATCH_UNIQUE_NAME( info ) << log

#endif // TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED
