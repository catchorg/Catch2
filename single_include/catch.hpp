/*
 *  CATCH v0.9 build 33 (integration branch)
 *  Generated: 2013-04-11 16:33:19.541792
 *  ----------------------------------------------------------
 *  This file has been merged from multiple headers. Please don't edit it directly
 *  Copyright (c) 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED
#define TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED

#define TWOBLUECUBES_CATCH_HPP_INCLUDED

#ifdef __clang__
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wvariadic-macros"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

// Use variadic macros if the compiler supports them
#if ( defined _MSC_VER && _MSC_VER > 1400 && !defined __EDGE__) || \
    ( defined __WAVE__ && __WAVE_HAS_VARIADICS ) || \
    ( defined __GNUC__ && __GNUC__ >= 3 ) || \
    ( !defined __cplusplus && __STDC_VERSION__ >= 199901L || __cplusplus >= 201103L )
    #define CATCH_CONFIG_VARIADIC_MACROS
#endif

// #included from: internal/catch_notimplemented_exception.h
#define TWOBLUECUBES_CATCH_NOTIMPLEMENTED_EXCEPTION_H_INCLUDED

// #included from: catch_common.h
#define TWOBLUECUBES_CATCH_COMMON_H_INCLUDED

#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) name##line
#define INTERNAL_CATCH_UNIQUE_NAME_LINE( name, line ) INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line )
#define INTERNAL_CATCH_UNIQUE_NAME( name ) INTERNAL_CATCH_UNIQUE_NAME_LINE( name, __LINE__ )

#define INTERNAL_CATCH_STRINGIFY2( expr ) #expr
#define INTERNAL_CATCH_STRINGIFY( expr ) INTERNAL_CATCH_STRINGIFY2( expr )

#ifdef __GNUC__
#define CATCH_ATTRIBUTE_NORETURN __attribute__ ((noreturn))
#else
#define CATCH_ATTRIBUTE_NORETURN
#endif

#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace Catch {

    class NonCopyable {
        NonCopyable( const NonCopyable& );
        void operator = ( const NonCopyable& );
    protected:
        NonCopyable() {}
        virtual ~NonCopyable();
    };

    class SafeBool {
    public:
        typedef void (SafeBool::*type)() const;

        static type makeSafe( bool value ) {
            return value ? &SafeBool::trueValue : 0;
        }
    private:
        void trueValue() const {}
    };

    template<typename ContainerT>
    inline void deleteAll( ContainerT& container ) {
        typename ContainerT::const_iterator it = container.begin();
        typename ContainerT::const_iterator itEnd = container.end();
        for(; it != itEnd; ++it )
            delete *it;
    }
    template<typename AssociativeContainerT>
    inline void deleteAllValues( AssociativeContainerT& container ) {
        typename AssociativeContainerT::const_iterator it = container.begin();
        typename AssociativeContainerT::const_iterator itEnd = container.end();
        for(; it != itEnd; ++it )
            delete it->second;
    }

    template<typename ContainerT, typename Function>
    inline void forEach( ContainerT& container, Function function ) {
        std::for_each( container.begin(), container.end(), function );
    }

    template<typename ContainerT, typename Function>
    inline void forEach( const ContainerT& container, Function function ) {
        std::for_each( container.begin(), container.end(), function );
    }

    inline bool startsWith( const std::string& s, const std::string& prefix ) {
        return s.size() >= prefix.size() && s.substr( 0, prefix.size() ) == prefix;
    }
    inline bool endsWith( const std::string& s, const std::string& suffix ) {
        return s.size() >= suffix.size() && s.substr( s.size()-suffix.size(), suffix.size() ) == suffix;
    }
    inline bool contains( const std::string& s, const std::string& infix ) {
        return s.find( infix ) != std::string::npos;
    }
    inline void toLowerInPlace( std::string& s ) {
        std::transform( s.begin(), s.end(), s.begin(), ::tolower );
    }
    inline std::string toLower( std::string const& s ) {
        std::string lc = s;
        toLowerInPlace( lc );
        return lc;
    }

    struct pluralise {
        pluralise( std::size_t count, const std::string& label )
        :   m_count( count ),
            m_label( label )
        {}

        friend std::ostream& operator << ( std::ostream& os, const pluralise& pluraliser ) {
            os << pluraliser.m_count << " " << pluraliser.m_label;
            if( pluraliser.m_count != 1 )
                os << "s";
            return os;
        }

        std::size_t m_count;
        std::string m_label;
    };

    struct SourceLineInfo {

        SourceLineInfo() : line( 0 ){}
        SourceLineInfo( const std::string& _file, std::size_t _line )
        :   file( _file ),
            line( _line )
        {}
        SourceLineInfo( const SourceLineInfo& other )
        :   file( other.file ),
            line( other.line )
        {}
        bool empty() const {
            return file.empty();
        }

        std::string file;
        std::size_t line;
    };

    inline std::ostream& operator << ( std::ostream& os, const SourceLineInfo& info ) {
#ifndef __GNUG__
        os << info.file << "(" << info.line << ")";
#else
        os << info.file << ":" << info.line;
#endif
        return os;
    }

    CATCH_ATTRIBUTE_NORETURN
    inline void throwLogicError( const std::string& message, const SourceLineInfo& locationInfo ) {
        std::ostringstream oss;
        oss << locationInfo << ": Internal Catch error: '" << message << "'";
        throw std::logic_error( oss.str() );
    }
}

#define CATCH_INTERNAL_LINEINFO ::Catch::SourceLineInfo( __FILE__, static_cast<std::size_t>( __LINE__ ) )
#define CATCH_INTERNAL_ERROR( msg ) ::Catch::throwLogicError( msg, CATCH_INTERNAL_LINEINFO );

#include <ostream>

namespace Catch {

    class NotImplementedException : public std::exception
    {
    public:
        NotImplementedException( const SourceLineInfo& lineInfo );

        virtual ~NotImplementedException() throw() {}

        virtual const char* what() const throw();

    private:
        std::string m_what;
        SourceLineInfo m_lineInfo;
    };

} // end namespace Catch

///////////////////////////////////////////////////////////////////////////////
#define CATCH_NOT_IMPLEMENTED throw Catch::NotImplementedException( CATCH_INTERNAL_LINEINFO )

// #included from: internal/catch_context.h
#define TWOBLUECUBES_CATCH_CONTEXT_H_INCLUDED

// #included from: catch_interfaces_generators.h
#define TWOBLUECUBES_CATCH_INTERFACES_GENERATORS_H_INCLUDED

#include <string>

namespace Catch {

    struct IGeneratorInfo {
        virtual ~IGeneratorInfo();
        virtual bool moveNext() = 0;
        virtual std::size_t getCurrentIndex() const = 0;
    };

    struct IGeneratorsForTest {
        virtual ~IGeneratorsForTest();

        virtual IGeneratorInfo& getGeneratorInfo( const std::string& fileInfo, std::size_t size ) = 0;
        virtual bool moveNext() = 0;
    };

    IGeneratorsForTest* createGeneratorsForTest();

} // end namespace Catch

#include <memory>
#include <vector>
#include <stdlib.h>

namespace Catch {

    class TestCase;
    class Stream;
    struct IResultCapture;
    struct IRunner;
    struct IGeneratorsForTest;
    struct IConfig;

    struct IContext
    {
        virtual ~IContext();

        virtual IResultCapture& getResultCapture() = 0;
        virtual IRunner& getRunner() = 0;
        virtual size_t getGeneratorIndex( const std::string& fileInfo, size_t totalSize ) = 0;
        virtual bool advanceGeneratorsForCurrentTest() = 0;
        virtual const IConfig* getConfig() const = 0;
    };

    struct IMutableContext : IContext
    {
        virtual ~IMutableContext();
        virtual void setResultCapture( IResultCapture* resultCapture ) = 0;
        virtual void setRunner( IRunner* runner ) = 0;
        virtual void setConfig( const IConfig* config ) = 0;
    };

    IContext& getCurrentContext();
    IMutableContext& getCurrentMutableContext();
    void cleanUpContext();
    Stream createStream( const std::string& streamName );

}

// #included from: internal/catch_test_registry.hpp
#define TWOBLUECUBES_CATCH_TEST_REGISTRY_HPP_INCLUDED

// #included from: catch_interfaces_testcase.h
#define TWOBLUECUBES_CATCH_INTERFACES_TESTCASE_H_INCLUDED

// #included from: catch_ptr.hpp
#define TWOBLUECUBES_CATCH_PTR_HPP_INCLUDED

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

namespace Catch {

    // An intrusive reference counting smart pointer.
    // T must implement addRef() and release() methods
    // typically implementing the IShared interface
    template<typename T>
    class Ptr {
    public:
        Ptr() : m_p( NULL ){}
        Ptr( T* p ) : m_p( p ){
            if( m_p )
                m_p->addRef();
        }
        Ptr( Ptr const& other ) : m_p( other.m_p ){
            if( m_p )
                m_p->addRef();
        }
        ~Ptr(){
            if( m_p )
                m_p->release();
        }
        void reset() {
            if( m_p )
                m_p->release();
            m_p = NULL;
        }
        Ptr& operator = ( T* p ){
            Ptr temp( p );
            swap( temp );
            return *this;
        }
        Ptr& operator = ( Ptr const& other ){
            Ptr temp( other );
            swap( temp );
            return *this;
        }
        void swap( Ptr& other ) { std::swap( m_p, other.m_p ); }
        T* get() { return m_p; }
        const T* get() const{ return m_p; }
        T& operator*() const { return *m_p; }
        T* operator->() const { return m_p; }
        bool operator !() const { return m_p == NULL; }
        operator SafeBool::type() const { return SafeBool::makeSafe( m_p != NULL ); }

    private:
        T* m_p;
    };

    struct IShared : NonCopyable {
        virtual ~IShared();
        virtual void addRef() const = 0;
        virtual void release() const = 0;
    };

    template<typename T = IShared>
    struct SharedImpl : T {

        SharedImpl() : m_rc( 0 ){}

        virtual void addRef() const {
            ++m_rc;
        }
        virtual void release() const {
            if( --m_rc == 0 )
                delete this;
        }

        mutable unsigned int m_rc;
    };

} // end namespace Catch

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <vector>

namespace Catch {

    class TestCaseFilters;

    struct ITestCase : IShared {
        virtual void invoke () const = 0;
    protected:
        virtual ~ITestCase();
    };

    class TestCase;

    struct ITestCaseRegistry {
        virtual ~ITestCaseRegistry();
        virtual const std::vector<TestCase>& getAllTests() const = 0;
        virtual std::vector<TestCase> getMatchingTestCases( const std::string& rawTestSpec ) const = 0;
    };
}

namespace Catch {

template<typename C>
class MethodTestCase : public SharedImpl<ITestCase> {

public:
    MethodTestCase( void (C::*method)() ) : m_method( method ) {}

    virtual void invoke() const {
        C obj;
        (obj.*m_method)();
    }

private:
    virtual ~MethodTestCase() {}

    void (C::*m_method)();
};

typedef void(*TestFunction)();

struct NameAndDesc {
    NameAndDesc( const char* _name = "", const char* _description= "" )
    : name( _name ), description( _description )
    {}

    const char* name;
    const char* description;
};

struct AutoReg {

    AutoReg(    TestFunction function,
                SourceLineInfo const& lineInfo,
                NameAndDesc const& nameAndDesc );

    template<typename C>
    AutoReg(    void (C::*method)(),
                char const* className,
                NameAndDesc const& nameAndDesc,
                SourceLineInfo const& lineInfo ) {
        registerTestCase(   new MethodTestCase<C>( method ),
                            className,
                            nameAndDesc,
                            lineInfo );
    }

    void registerTestCase(  ITestCase* testCase,
                            char const* className,
                            NameAndDesc const& nameAndDesc,
                            SourceLineInfo const& lineInfo );

    ~AutoReg();

private:
    AutoReg( const AutoReg& );
    void operator= ( const AutoReg& );
};

} // end namespace Catch

#ifdef CATCH_CONFIG_VARIADIC_MACROS
    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TESTCASE( ... ) \
        static void INTERNAL_CATCH_UNIQUE_NAME( TestCaseFunction_catch_internal_ )(); \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ ), CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc( __VA_ARGS__ ) ); }\
        static void INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ )()

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TESTCASE_NORETURN( ... ) \
        static void INTERNAL_CATCH_UNIQUE_NAME( TestCaseFunction_catch_internal_ )() CATCH_ATTRIBUTE_NORETURN; \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ ), CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc( __VA_ARGS__ ) ); }\
        static void INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ )()

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, ... ) \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &QualifiedMethod, "&" #QualifiedMethod, Catch::NameAndDesc( __VA_ARGS__ ), CATCH_INTERNAL_LINEINFO ); }

    ///////////////////////////////////////////////////////////////////////////////
    #define TEST_CASE_METHOD( ClassName, ... )\
        namespace{ \
            struct INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ ) : ClassName{ \
                void test(); \
            }; \
            Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( &INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ )::test, #ClassName, Catch::NameAndDesc( __VA_ARGS__ ), CATCH_INTERNAL_LINEINFO ); \
        } \
        void INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ )::test()

#else
    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TESTCASE( Name, Desc ) \
        static void INTERNAL_CATCH_UNIQUE_NAME( TestCaseFunction_catch_internal_ )(); \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ ), CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc( Name, Desc ) ); }\
        static void INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ )()

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TESTCASE_NORETURN( Name, Desc ) \
        static void INTERNAL_CATCH_UNIQUE_NAME( TestCaseFunction_catch_internal_ )() CATCH_ATTRIBUTE_NORETURN; \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ ), CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc( Name, Desc ) ); }\
        static void INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ )()

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, Name, Desc ) \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &QualifiedMethod, "&" #QualifiedMethod, Catch::NameAndDesc( Name, Desc ), CATCH_INTERNAL_LINEINFO ); }

    ///////////////////////////////////////////////////////////////////////////////
    #define TEST_CASE_METHOD( ClassName, TestName, Desc )\
        namespace{ \
            struct INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ ) : ClassName{ \
                void test(); \
            }; \
            Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( &INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ )::test, #ClassName, Catch::NameAndDesc( TestName, Desc ), CATCH_INTERNAL_LINEINFO ); \
        } \
        void INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ )::test()

#endif

// #included from: internal/catch_capture.hpp
#define TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED

// #included from: catch_expression_decomposer.hpp
#define TWOBLUECUBES_CATCH_EXPRESSION_DECOMPOSER_HPP_INCLUDED

// #included from: catch_expression_lhs.hpp
#define TWOBLUECUBES_CATCH_EXPRESSION_LHS_HPP_INCLUDED

// #included from: catch_expressionresult_builder.h
#define TWOBLUECUBES_CATCH_ASSERTIONRESULT_BUILDER_H_INCLUDED

// #included from: catch_tostring.hpp
#define TWOBLUECUBES_CATCH_TOSTRING_HPP_INCLUDED

#include <sstream>
#include <iomanip>
#include <limits>

#ifdef __OBJC__
// #included from: catch_objc_arc.hpp
#define TWOBLUECUBES_CATCH_OBJC_ARC_HPP_INCLUDED

#import <Foundation/Foundation.h>

#ifdef __has_feature
#define CATCH_ARC_ENABLED __has_feature(objc_arc)
#else
#define CATCH_ARC_ENABLED 0
#endif

void arcSafeRelease( NSObject* obj );
id performOptionalSelector( id obj, SEL sel );

#if !CATCH_ARC_ENABLED
inline void arcSafeRelease( NSObject* obj ) {
    [obj release];
}
inline id performOptionalSelector( id obj, SEL sel ) {
    if( [obj respondsToSelector: sel] )
        return [obj performSelector: sel];
    return nil;
}
#define CATCH_UNSAFE_UNRETAINED
#define CATCH_ARC_STRONG
#else
inline void arcSafeRelease( NSObject* ){}
inline id performOptionalSelector( id obj, SEL sel ) {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
#endif
    if( [obj respondsToSelector: sel] )
        return [obj performSelector: sel];
#ifdef __clang__
#pragma clang diagnostic pop
#endif
    return nil;
}
#define CATCH_UNSAFE_UNRETAINED __unsafe_unretained
#define CATCH_ARC_STRONG __strong
#endif

#endif

namespace Catch {
namespace Detail {

    struct NonStreamable {
        template<typename T> NonStreamable( const T& ){}
    };

} // end namespace Detail

// If the type does not have its own << overload for ostream then
// this one will be used instead
inline std::ostream& operator << ( std::ostream& ss, Detail::NonStreamable ){
    return ss << "{?}";
}

template<typename T>
struct StringMaker {
    static std::string convert( T const& value ) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
};
template<typename T>
struct StringMaker<T*> {
    static std::string convert( T const* p ) {
        if( !p )
            return INTERNAL_CATCH_STRINGIFY( NULL );
        std::ostringstream oss;
        oss << p;
        return oss.str();
    }
};

template<typename T>
struct StringMaker<std::vector<T> > {
    static std::string convert( std::vector<T> const& v ) {
        std::ostringstream oss;
        oss << "{ ";
        for( std::size_t i = 0; i < v.size(); ++ i ) {
            oss << v[i];
            if( i < v.size() - 1 )
                oss << ", ";
        }
        oss << " }";
        return oss.str();
    }
};

namespace Detail {
    template<typename T>
    inline std::string makeString( const T& value ) {
        return StringMaker<T>::convert( value );
    }
} // end namespace Detail

/// \brief converts any type to a string
///
/// The default template forwards on to ostringstream - except when an
/// ostringstream overload does not exist - in which case it attempts to detect
/// that and writes {?}.
/// Overload (not specialise) this template for custom typs that you don't want
/// to provide an ostream overload for.
template<typename T>
std::string toString( const T& value ) {
    return StringMaker<T>::convert( value );
//    return Detail::makeString( value );
}

// Built in overloads

inline std::string toString( const std::string& value ) {
    return "\"" + value + "\"";
}

inline std::string toString( const std::wstring& value ) {
    std::ostringstream oss;
    oss << "\"";
    for(size_t i = 0; i < value.size(); ++i )
        oss << static_cast<char>( value[i] <= 0xff ? value[i] : '?');
    oss << "\"";
    return oss.str();
}

inline std::string toString( const char* const value ) {
    return value ? Catch::toString( std::string( value ) ) : std::string( "{null string}" );
}

inline std::string toString( char* const value ) {
    return Catch::toString( static_cast<const char*>( value ) );
}

inline std::string toString( int value ) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

inline std::string toString( unsigned long value ) {
    std::ostringstream oss;
    if( value > 8192 )
        oss << "0x" << std::hex << value;
    else
        oss << value;
    return oss.str();
}

inline std::string toString( unsigned int value ) {
    return toString( static_cast<unsigned long>( value ) );
}

inline std::string toString( const double value ) {
    std::ostringstream oss;
    oss << std::setprecision (std::numeric_limits<double>::digits10 + 1)
        << value;
    return oss.str();
}

inline std::string toString( bool value ) {
    return value ? "true" : "false";
}

inline std::string toString( char value ) {
    return value < ' '
        ? toString( static_cast<unsigned int>( value ) )
        : Detail::makeString( value );
}

inline std::string toString( signed char value ) {
    return toString( static_cast<char>( value ) );
}

inline std::string toString( unsigned char value ) {
    return toString( static_cast<char>( value ) );
}

#ifdef CATCH_CONFIG_CPP11_NULLPTR
inline std::string toString( std::nullptr_t ) {
    return "nullptr";
}
#endif

#ifdef __OBJC__
    inline std::string toString( NSString const * const& nsstring ) {
        if( !nsstring )
            return "nil";
        return std::string( "@\"" ) + [nsstring UTF8String] + "\"";
    }
    inline std::string toString( NSString * CATCH_ARC_STRONG const& nsstring ) {
        if( !nsstring )
            return "nil";
        return std::string( "@\"" ) + [nsstring UTF8String] + "\"";
    }
    inline std::string toString( NSObject* const& nsObject ) {
        return toString( [nsObject description] );
    }
#endif

} // end namespace Catch

// #included from: catch_assertionresult.h
#define TWOBLUECUBES_CATCH_ASSERTIONRESULT_H_INCLUDED

#include <string>
// #included from: catch_result_type.h
#define TWOBLUECUBES_CATCH_RESULT_TYPE_H_INCLUDED

namespace Catch {

    // ResultWas::OfType enum
    struct ResultWas { enum OfType {
        Unknown = -1,
        Ok = 0,
        Info = 1,
        Warning = 2,

        FailureBit = 0x10,

        ExpressionFailed = FailureBit | 1,
        ExplicitFailure = FailureBit | 2,

        Exception = 0x100 | FailureBit,

        ThrewException = Exception | 1,
        DidntThrowException = Exception | 2

    }; };

    inline bool isOk( ResultWas::OfType resultType ) {
        return ( resultType & ResultWas::FailureBit ) == 0;
    }
    inline bool isJustInfo( int flags ) {
        return flags == ResultWas::Info;
    }

    // ResultAction::Value enum
    struct ResultAction { enum Value {
        None,
        Failed = 1, // Failure - but no debug break if Debug bit not set
        Debug = 2,  // If this bit is set, invoke the debugger
        Abort = 4   // Test run should abort
    }; };

    // ResultDisposition::Flags enum
    struct ResultDisposition { enum Flags {
            Normal = 0x00,

            ContinueOnFailure = 0x01,   // Failures fail test, but execution continues
            NegateResult = 0x02,        // Prefix expressiom with !
            SuppressFail = 0x04         // Failures are reported but do not fail the test
    }; };

    inline ResultDisposition::Flags operator | ( ResultDisposition::Flags lhs, ResultDisposition::Flags rhs ) {
        return static_cast<ResultDisposition::Flags>( static_cast<int>( lhs ) | static_cast<int>( rhs ) );
    }

    inline bool shouldContinueOnFailure( int flags )    { return ( flags & ResultDisposition::ContinueOnFailure ) != 0; }
    inline bool shouldNegate( int flags )               { return ( flags & ResultDisposition::NegateResult ) != 0; }
    inline bool shouldSuppressFailure( int flags )      { return ( flags & ResultDisposition::SuppressFail ) != 0; }

} // end namespace Catch


namespace Catch {

    struct AssertionInfo
    {
        AssertionInfo() {}
        AssertionInfo(  const std::string& _macroName,
                        const SourceLineInfo& _lineInfo,
                        const std::string& _capturedExpression,
                        ResultDisposition::Flags _resultDisposition );

        std::string macroName;
        SourceLineInfo lineInfo;
        std::string capturedExpression;
        ResultDisposition::Flags resultDisposition;
    };

    struct AssertionResultData
    {
        AssertionResultData() : resultType( ResultWas::Unknown ) {}

        std::string reconstructedExpression;
        std::string message;
        ResultWas::OfType resultType;
    };

    class AssertionResult {
    public:
        AssertionResult();
        AssertionResult( const AssertionInfo& info, const AssertionResultData& data );
        ~AssertionResult();

        bool isOk() const;
        bool succeeded() const;
        ResultWas::OfType getResultType() const;
        bool hasExpression() const;
        bool hasMessage() const;
        std::string getExpression() const;
        bool hasExpandedExpression() const;
        std::string getExpandedExpression() const;
        std::string getMessage() const;
        SourceLineInfo getSourceInfo() const;
        std::string getTestMacroName() const;

    protected:
        AssertionInfo m_info;
        AssertionResultData m_resultData;
    };

} // end namespace Catch

// #included from: catch_evaluate.hpp
#define TWOBLUECUBES_CATCH_EVALUATE_HPP_INCLUDED

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4389) // '==' : signed/unsigned mismatch
#endif

namespace Catch {
namespace Internal {

    enum Operator {
        IsEqualTo,
        IsNotEqualTo,
        IsLessThan,
        IsGreaterThan,
        IsLessThanOrEqualTo,
        IsGreaterThanOrEqualTo
    };

    template<Operator Op> struct OperatorTraits             { static const char* getName(){ return "*error*"; } };
    template<> struct OperatorTraits<IsEqualTo>             { static const char* getName(){ return "=="; } };
    template<> struct OperatorTraits<IsNotEqualTo>          { static const char* getName(){ return "!="; } };
    template<> struct OperatorTraits<IsLessThan>            { static const char* getName(){ return "<"; } };
    template<> struct OperatorTraits<IsGreaterThan>         { static const char* getName(){ return ">"; } };
    template<> struct OperatorTraits<IsLessThanOrEqualTo>   { static const char* getName(){ return "<="; } };
    template<> struct OperatorTraits<IsGreaterThanOrEqualTo>{ static const char* getName(){ return ">="; } };

    // So the compare overloads can be operator agnostic we convey the operator as a template
    // enum, which is used to specialise an Evaluator for doing the comparison.
    template<typename T1, typename T2, Operator Op>
    class Evaluator{};

    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsEqualTo> {
        static bool evaluate( const T1& lhs, const T2& rhs) {
            return const_cast<T1&>( lhs ) ==  const_cast<T2&>( rhs );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsNotEqualTo> {
        static bool evaluate( const T1& lhs, const T2& rhs ) {
            return const_cast<T1&>( lhs ) != const_cast<T2&>( rhs );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsLessThan> {
        static bool evaluate( const T1& lhs, const T2& rhs ) {
            return const_cast<T1&>( lhs ) < const_cast<T2&>( rhs );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsGreaterThan> {
        static bool evaluate( const T1& lhs, const T2& rhs ) {
            return const_cast<T1&>( lhs ) > const_cast<T2&>( rhs );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsGreaterThanOrEqualTo> {
        static bool evaluate( const T1& lhs, const T2& rhs ) {
            return const_cast<T1&>( lhs ) >= const_cast<T2&>( rhs );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsLessThanOrEqualTo> {
        static bool evaluate( const T1& lhs, const T2& rhs ) {
            return const_cast<T1&>( lhs ) <= const_cast<T2&>( rhs );
        }
    };

    template<Operator Op, typename T1, typename T2>
    bool applyEvaluator( const T1& lhs, const T2& rhs ) {
        return Evaluator<T1, T2, Op>::evaluate( lhs, rhs );
    }

    // This level of indirection allows us to specialise for integer types
    // to avoid signed/ unsigned warnings

    // "base" overload
    template<Operator Op, typename T1, typename T2>
    bool compare( const T1& lhs, const T2& rhs ) {
        return Evaluator<T1, T2, Op>::evaluate( lhs, rhs );
    }

    // unsigned X to int
    template<Operator Op> bool compare( unsigned int lhs, int rhs ) {
        return applyEvaluator<Op>( lhs, static_cast<unsigned int>( rhs ) );
    }
    template<Operator Op> bool compare( unsigned long lhs, int rhs ) {
        return applyEvaluator<Op>( lhs, static_cast<unsigned int>( rhs ) );
    }
    template<Operator Op> bool compare( unsigned char lhs, int rhs ) {
        return applyEvaluator<Op>( lhs, static_cast<unsigned int>( rhs ) );
    }

    // unsigned X to long
    template<Operator Op> bool compare( unsigned int lhs, long rhs ) {
        return applyEvaluator<Op>( lhs, static_cast<unsigned long>( rhs ) );
    }
    template<Operator Op> bool compare( unsigned long lhs, long rhs ) {
        return applyEvaluator<Op>( lhs, static_cast<unsigned long>( rhs ) );
    }
    template<Operator Op> bool compare( unsigned char lhs, long rhs ) {
        return applyEvaluator<Op>( lhs, static_cast<unsigned long>( rhs ) );
    }

    // int to unsigned X
    template<Operator Op> bool compare( int lhs, unsigned int rhs ) {
        return applyEvaluator<Op>( static_cast<unsigned int>( lhs ), rhs );
    }
    template<Operator Op> bool compare( int lhs, unsigned long rhs ) {
        return applyEvaluator<Op>( static_cast<unsigned int>( lhs ), rhs );
    }
    template<Operator Op> bool compare( int lhs, unsigned char rhs ) {
        return applyEvaluator<Op>( static_cast<unsigned int>( lhs ), rhs );
    }

    // long to unsigned X
    template<Operator Op> bool compare( long lhs, unsigned int rhs ) {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs );
    }
    template<Operator Op> bool compare( long lhs, unsigned long rhs ) {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs );
    }
    template<Operator Op> bool compare( long lhs, unsigned char rhs ) {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ), rhs );
    }

    // pointer to long (when comparing against NULL)
    template<Operator Op, typename T> bool compare( long lhs, T* rhs ) {
        return Evaluator<T*, T*, Op>::evaluate( reinterpret_cast<T*>( lhs ), rhs );
    }
    template<Operator Op, typename T> bool compare( T* lhs, long rhs ) {
        return Evaluator<T*, T*, Op>::evaluate( lhs, reinterpret_cast<T*>( rhs ) );
    }

    // pointer to int (when comparing against NULL)
    template<Operator Op, typename T> bool compare( int lhs, T* rhs ) {
        return Evaluator<T*, T*, Op>::evaluate( reinterpret_cast<T*>( lhs ), rhs );
    }
    template<Operator Op, typename T> bool compare( T* lhs, int rhs ) {
        return Evaluator<T*, T*, Op>::evaluate( lhs, reinterpret_cast<T*>( rhs ) );
    }

} // end of namespace Internal
} // end of namespace Catch

#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace Catch {

// Wraps the (stringised versions of) the lhs, operator and rhs of an expression - as well as
// the result of evaluating it. This is used to build an AssertionResult object
class ExpressionResultBuilder {
public:

    ExpressionResultBuilder( ResultWas::OfType resultType = ResultWas::Unknown );
    ExpressionResultBuilder( const ExpressionResultBuilder& other );
    ExpressionResultBuilder& operator=(const ExpressionResultBuilder& other );

    ExpressionResultBuilder& setResultType( ResultWas::OfType result );
    ExpressionResultBuilder& setResultType( bool result );
    ExpressionResultBuilder& setLhs( const std::string& lhs );
    ExpressionResultBuilder& setRhs( const std::string& rhs );
    ExpressionResultBuilder& setOp( const std::string& op );

    ExpressionResultBuilder& endExpression( ResultDisposition::Flags resultDisposition );

    template<typename T>
    ExpressionResultBuilder& operator << ( const T& value ) {
        m_stream << value;
        return *this;
    }

    std::string reconstructExpression( const AssertionInfo& info ) const;

    AssertionResult buildResult( const AssertionInfo& info ) const;

private:
    AssertionResultData m_data;
    struct ExprComponents {
        ExprComponents() : shouldNegate( false ) {}
        bool shouldNegate;
        std::string lhs, rhs, op;
    } m_exprComponents;
    std::ostringstream m_stream;
};

} // end namespace Catch

namespace Catch {

struct STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison;

// Wraps the LHS of an expression and captures the operator and RHS (if any) - wrapping them all
// in an ExpressionResultBuilder object
template<typename T>
class ExpressionLhs {
    void operator = ( const ExpressionLhs& );

public:
    ExpressionLhs( T lhs ) : m_lhs( lhs ) {}

    template<typename RhsT>
    ExpressionResultBuilder& operator == ( const RhsT& rhs ) {
        return captureExpression<Internal::IsEqualTo>( rhs );
    }

    template<typename RhsT>
    ExpressionResultBuilder& operator != ( const RhsT& rhs ) {
        return captureExpression<Internal::IsNotEqualTo>( rhs );
    }

    template<typename RhsT>
    ExpressionResultBuilder& operator < ( const RhsT& rhs ) {
        return captureExpression<Internal::IsLessThan>( rhs );
    }

    template<typename RhsT>
    ExpressionResultBuilder& operator > ( const RhsT& rhs ) {
        return captureExpression<Internal::IsGreaterThan>( rhs );
    }

    template<typename RhsT>
    ExpressionResultBuilder& operator <= ( const RhsT& rhs ) {
        return captureExpression<Internal::IsLessThanOrEqualTo>( rhs );
    }

    template<typename RhsT>
    ExpressionResultBuilder& operator >= ( const RhsT& rhs ) {
        return captureExpression<Internal::IsGreaterThanOrEqualTo>( rhs );
    }

    ExpressionResultBuilder& operator == ( bool rhs ) {
        return captureExpression<Internal::IsEqualTo>( rhs );
    }

    ExpressionResultBuilder& operator != ( bool rhs ) {
        return captureExpression<Internal::IsNotEqualTo>( rhs );
    }

    ExpressionResultBuilder& endExpression( ResultDisposition::Flags resultDisposition ) {
        bool value = m_lhs ? true : false;
        return m_result
            .setLhs( Catch::toString( value ) )
            .setResultType( value )
            .endExpression( resultDisposition );
    }

    // Only simple binary expressions are allowed on the LHS.
    // If more complex compositions are required then place the sub expression in parentheses
    template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator + ( const RhsT& );
    template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator - ( const RhsT& );
    template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator / ( const RhsT& );
    template<typename RhsT> STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator * ( const RhsT& );

private:
    template<Internal::Operator Op, typename RhsT>
    ExpressionResultBuilder& captureExpression( const RhsT& rhs ) {
        return m_result
            .setResultType( Internal::compare<Op>( m_lhs, rhs ) )
            .setLhs( Catch::toString( m_lhs ) )
            .setRhs( Catch::toString( rhs ) )
            .setOp( Internal::OperatorTraits<Op>::getName() );
    }

private:
    ExpressionResultBuilder m_result;
    T m_lhs;
};

} // end namespace Catch

namespace Catch {

// Captures the LHS of the expression and wraps it in an Expression Lhs object
class ExpressionDecomposer {
public:

    template<typename T>
    ExpressionLhs<const T&> operator->* ( const T & operand ) {
        return ExpressionLhs<const T&>( operand );
    }

    ExpressionLhs<bool> operator->* ( bool value ) {
        return ExpressionLhs<bool>( value );
    }
};

} // end namespace Catch

// #included from: catch_message.h
#define TWOBLUECUBES_CATCH_MESSAGE_H_INCLUDED

#include <string>

namespace Catch {

    struct MessageInfo {
        MessageInfo(    std::string const& _macroName,
                        SourceLineInfo const& _lineInfo,
                        ResultWas::OfType _type );

        std::string macroName;
        SourceLineInfo lineInfo;
        ResultWas::OfType type;
        std::string message;
        unsigned int sequence;

        bool operator == ( MessageInfo const& other ) const {
            return sequence == other.sequence;
        }
        bool operator < ( MessageInfo const& other ) const {
            return sequence < other.sequence;
        }
    private:
        static unsigned int globalCount;
    };

    class MessageBuilder : public MessageInfo {
    public:
        MessageBuilder( std::string const& _macroName,
                       SourceLineInfo const& _lineInfo,
                       ResultWas::OfType _type );

        MessageInfo build() const;

        template<typename T>
        MessageBuilder& operator << ( T const& _value ) {
            stream << _value;
            return *this;
        }
    private:
        std::ostringstream stream;
    };

    class ScopedMessageBuilder : public MessageBuilder {
    public:
        ScopedMessageBuilder(   std::string const& _macroName,
                                SourceLineInfo const& _lineInfo,
                                ResultWas::OfType _type );
        ~ScopedMessageBuilder();
    };

} // end namespace Catch

// #included from: catch_interfaces_capture.h
#define TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED

#include <string>
// #included from: catch_totals.hpp
#define TWOBLUECUBES_CATCH_TOTALS_HPP_INCLUDED

#include <cstddef>

namespace Catch {

    struct Counts {
        Counts() : passed( 0 ), failed( 0 ) {}

        Counts operator - ( const Counts& other ) const {
            Counts diff;
            diff.passed = passed - other.passed;
            diff.failed = failed - other.failed;
            return diff;
        }
        Counts& operator += ( const Counts& other ) {
            passed += other.passed;
            failed += other.failed;
            return *this;
        }

        std::size_t total() const {
            return passed + failed;
        }

        std::size_t passed;
        std::size_t failed;
    };

    struct Totals {

        Totals operator - ( const Totals& other ) const {
            Totals diff;
            diff.assertions = assertions - other.assertions;
            diff.testCases = testCases - other.testCases;
            return diff;
        }

        Totals delta( const Totals& prevTotals ) const {
            Totals diff = *this - prevTotals;
            if( diff.assertions.failed > 0 )
                ++diff.testCases.failed;
            else
                ++diff.testCases.passed;
            return diff;
        }

        Totals& operator += ( const Totals& other ) {
            assertions += other.assertions;
            testCases += other.testCases;
            return *this;
        }

        Counts assertions;
        Counts testCases;
    };
}


namespace Catch {

    class TestCase;
    class ExpressionResultBuilder;
    class AssertionResult;
    struct AssertionInfo;
    struct SectionInfo;
    class MessageBuilder;
    class ScopedMessageBuilder;

    struct IResultCapture {

        virtual ~IResultCapture();

        virtual void assertionEnded( AssertionResult const& result ) = 0;
        virtual bool sectionStarted(    SectionInfo const& sectionInfo,
                                        Counts& assertions ) = 0;
        virtual void sectionEnded( SectionInfo const& name, Counts const& assertions ) = 0;
        virtual void pushScopedMessage( ScopedMessageBuilder const& _builder ) = 0;
        virtual void popScopedMessage( ScopedMessageBuilder const& _builder ) = 0;

        virtual bool shouldDebugBreak() const = 0;

        virtual void acceptMessage( const MessageBuilder& messageBuilder ) = 0;
        virtual ResultAction::Value acceptExpression( ExpressionResultBuilder const& assertionResult, AssertionInfo const& assertionInfo ) = 0;

        virtual std::string getCurrentTestName() const = 0;
        virtual const AssertionResult* getLastResult() const = 0;
    };
}

// #included from: catch_debugger.hpp
#define TWOBLUECUBES_CATCH_DEBUGGER_HPP_INCLUDED

#include <iostream>

#if defined(__MAC_OS_X_VERSION_MIN_REQUIRED)
#define CATCH_PLATFORM_MAC
#elif  defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#define CATCH_PLATFORM_IPHONE
#elif defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER)
#define CATCH_PLATFORM_WINDOWS
#endif

#ifdef CATCH_PLATFORM_MAC

    #include <assert.h>
    #include <stdbool.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <sys/sysctl.h>

    namespace Catch{

        // The following function is taken directly from the following technical note:
        // http://developer.apple.com/library/mac/#qa/qa2004/qa1361.html

        // Returns true if the current process is being debugged (either
        // running under the debugger or has a debugger attached post facto).
        inline bool isDebuggerActive(){

            int                 junk;
            int                 mib[4];
            struct kinfo_proc   info;
            size_t              size;

            // Initialize the flags so that, if sysctl fails for some bizarre
            // reason, we get a predictable result.

            info.kp_proc.p_flag = 0;

            // Initialize mib, which tells sysctl the info we want, in this case
            // we're looking for information about a specific process ID.

            mib[0] = CTL_KERN;
            mib[1] = KERN_PROC;
            mib[2] = KERN_PROC_PID;
            mib[3] = getpid();

            // Call sysctl.

            size = sizeof(info);
            junk = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);
            assert(junk == 0);

            // We're being debugged if the P_TRACED flag is set.

            return ( (info.kp_proc.p_flag & P_TRACED) != 0 );
        }
    }

    // The following code snippet taken from:
    // http://cocoawithlove.com/2008/03/break-into-debugger.html
    #ifdef DEBUG
        #if defined(__ppc64__) || defined(__ppc__)
            #define BreakIntoDebugger() \
            if( Catch::isDebuggerActive() ) { \
            __asm__("li r0, 20\nsc\nnop\nli r0, 37\nli r4, 2\nsc\nnop\n" \
            : : : "memory","r0","r3","r4" ); \
            }
        #else
            #define BreakIntoDebugger() if( Catch::isDebuggerActive() ) {__asm__("int $3\n" : : );}
        #endif
    #else
        inline void BreakIntoDebugger(){}
    #endif

#elif defined(_MSC_VER)
    extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
    #define BreakIntoDebugger() if (IsDebuggerPresent() ) { __debugbreak(); }
    inline bool isDebuggerActive() {
        return IsDebuggerPresent() != 0;
    }
#elif defined(__MINGW32__)
    extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
    extern "C" __declspec(dllimport) void __stdcall DebugBreak();
    #define BreakIntoDebugger() if (IsDebuggerPresent() ) { DebugBreak(); }
    inline bool isDebuggerActive() {
        return IsDebuggerPresent() != 0;
    }
#else
       inline void BreakIntoDebugger(){}
       inline bool isDebuggerActive() { return false; }
#endif

#ifdef CATCH_PLATFORM_WINDOWS
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA( const char* );
inline void writeToDebugConsole( const std::string& text ) {
    ::OutputDebugStringA( text.c_str() );
}
#else
inline void writeToDebugConsole( const std::string& text ) {
    // !TBD: Need a version for Mac/ XCode and other IDEs
    std::cout << text;
}
#endif // CATCH_PLATFORM_WINDOWS

// #included from: catch_interfaces_registry_hub.h
#define TWOBLUECUBES_CATCH_INTERFACES_REGISTRY_HUB_H_INCLUDED

// #included from: catch_interfaces_reporter.h
#define TWOBLUECUBES_CATCH_INTERFACES_REPORTER_H_INCLUDED

// #included from: catch_config.hpp
#define TWOBLUECUBES_CATCH_CONFIG_HPP_INCLUDED

// #included from: catch_test_spec.h
#define TWOBLUECUBES_CATCH_TEST_SPEC_H_INCLUDED

// #included from: catch_test_case_info.h
#define TWOBLUECUBES_CATCH_TEST_CASE_INFO_H_INCLUDED

#include <string>
#include <set>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

namespace Catch {

    struct ITestCase;

    struct TestCaseInfo {
        TestCaseInfo(   const std::string& _name,
                        const std::string& _className,
                        const std::string& _description,
                        const std::set<std::string>& _tags,
                        bool _isHidden,
                        const SourceLineInfo& _lineInfo );

        TestCaseInfo( const TestCaseInfo& other );

        std::string name;
        std::string className;
        std::string description;
        std::set<std::string> tags;
        std::string tagsAsString;
        SourceLineInfo lineInfo;
        bool isHidden;
    };

    class TestCase : protected TestCaseInfo {
    public:

        TestCase( ITestCase* testCase, const TestCaseInfo& info );
        TestCase( const TestCase& other );

        TestCase withName( const std::string& _newName ) const;

        void invoke() const;

        const TestCaseInfo& getTestCaseInfo() const;

        bool isHidden() const;
        bool hasTag( const std::string& tag ) const;
        bool matchesTags( const std::string& tagPattern ) const;
        const std::set<std::string>& getTags() const;

        void swap( TestCase& other );
        bool operator == ( const TestCase& other ) const;
        bool operator < ( const TestCase& other ) const;
        TestCase& operator = ( const TestCase& other );

    private:
        Ptr<ITestCase> test;
    };

    TestCase makeTestCase(  ITestCase* testCase,
                            const std::string& className,
                            const std::string& name,
                            const std::string& description,
                            const SourceLineInfo& lineInfo );
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

// #included from: catch_tags.hpp
#define TWOBLUECUBES_CATCH_TAGS_HPP_INCLUDED

#include <string>
#include <set>
#include <map>
#include <vector>

#ifdef __clang__
#pragma clang diagnostic ignored "-Wpadded"
#endif

namespace Catch {
    class TagParser {
    public:
        virtual ~TagParser();

        void parse( const std::string& str ) {
            std::size_t pos = 0;
            while( pos < str.size() ) {
                char c = str[pos];
                if( c == '[' ) {
                    std::size_t end = str.find_first_of( ']', pos );
                    if( end != std::string::npos ) {
                        acceptTag( str.substr( pos+1, end-pos-1 ) );
                        pos = end+1;
                    }
                    else {
                        acceptChar( c );
                        pos++;
                    }
                }
                else {
                    acceptChar( c );
                    pos++;
                }
            }
            endParse();
        }

    protected:
        virtual void acceptTag( const std::string& tag ) = 0;
        virtual void acceptChar( char c ) = 0;
        virtual void endParse() {}

    private:
    };

    class TagExtracter : public TagParser {
    public:

        TagExtracter( std::set<std::string>& tags )
        :   m_tags( tags )
        {}
        virtual ~TagExtracter();

        void parse( std::string& description ) {
            TagParser::parse( description );
            description = m_remainder;
        }

    private:
        virtual void acceptTag( const std::string& tag ) {
            m_tags.insert( toLower( tag ) );
        }
        virtual void acceptChar( char c ) {
            m_remainder += c;
        }

        TagExtracter& operator=(const TagExtracter&);

        std::set<std::string>& m_tags;
        std::string m_remainder;
    };

    class Tag {
    public:
        Tag()
        :   m_isNegated( false )
        {}

        Tag( const std::string& name, bool isNegated )
        :   m_name( name ),
            m_isNegated( isNegated )
        {}

        std::string getName() const {
            return m_name;
        }
        bool isNegated() const {
            return m_isNegated;
        }

        bool operator ! () const {
            return m_name.empty();
        }

    private:
        std::string m_name;
        bool m_isNegated;
    };

    class TagSet {
        typedef std::map<std::string, Tag> TagMap;
    public:
        void add( const Tag& tag ) {
            m_tags.insert( std::make_pair( toLower( tag.getName() ), tag ) );
        }

        bool empty() const {
            return m_tags.empty();
        }

        bool matches( const std::set<std::string>& tags ) const {
            TagMap::const_iterator it = m_tags.begin();
            TagMap::const_iterator itEnd = m_tags.end();
            for(; it != itEnd; ++it ) {
                bool found = tags.find( it->first ) != tags.end();
                if( found == it->second.isNegated() )
                    return false;
            }
            return true;
        }

    private:
        TagMap m_tags;
    };

    class TagExpression {
    public:
        bool matches( const std::set<std::string>& tags ) const {
            std::vector<TagSet>::const_iterator it = m_tagSets.begin();
            std::vector<TagSet>::const_iterator itEnd = m_tagSets.end();
            for(; it != itEnd; ++it )
                if( it->matches( tags ) )
                    return true;
            return false;
        }

    private:
        friend class TagExpressionParser;

        std::vector<TagSet> m_tagSets;
    };

    class TagExpressionParser : public TagParser {
    public:
        TagExpressionParser( TagExpression& exp )
        :   m_isNegated( false ),
            m_exp( exp )
        {}

        ~TagExpressionParser();

    private:
        virtual void acceptTag( const std::string& tag ) {
            m_currentTagSet.add( Tag( tag, m_isNegated ) );
            m_isNegated = false;
        }
        virtual void acceptChar( char c ) {
            switch( c ) {
                case '~':
                    m_isNegated = true;
                    break;
                case ',':
                    m_exp.m_tagSets.push_back( m_currentTagSet );
                    break;
            }
        }
        virtual void endParse() {
            if( !m_currentTagSet.empty() )
                m_exp.m_tagSets.push_back( m_currentTagSet );
        }

        TagExpressionParser& operator=(const TagExpressionParser&);

        bool m_isNegated;
        TagSet m_currentTagSet;
        TagExpression& m_exp;
    };

} // end namespace Catch

#include <string>
#include <vector>

namespace Catch {

    struct IfFilterMatches{ enum DoWhat {
        AutoDetectBehaviour,
        IncludeTests,
        ExcludeTests
    }; };

    class TestCaseFilter {
        enum WildcardPosition {
            NoWildcard = 0,
            WildcardAtStart = 1,
            WildcardAtEnd = 2,
            WildcardAtBothEnds = WildcardAtStart | WildcardAtEnd
        };

    public:
        TestCaseFilter( const std::string& testSpec, IfFilterMatches::DoWhat matchBehaviour = IfFilterMatches::AutoDetectBehaviour )
        :   m_stringToMatch( toLower( testSpec ) ),
            m_filterType( matchBehaviour ),
            m_wildcardPosition( NoWildcard )
        {
            if( m_filterType == IfFilterMatches::AutoDetectBehaviour ) {
                if( startsWith( m_stringToMatch, "exclude:" ) ) {
                    m_stringToMatch = m_stringToMatch.substr( 8 );
                    m_filterType = IfFilterMatches::ExcludeTests;
                }
                else if( startsWith( m_stringToMatch, "~" ) ) {
                    m_stringToMatch = m_stringToMatch.substr( 1 );
                    m_filterType = IfFilterMatches::ExcludeTests;
                }
                else {
                    m_filterType = IfFilterMatches::IncludeTests;
                }
            }

            if( startsWith( m_stringToMatch, "*" ) ) {
                m_stringToMatch = m_stringToMatch.substr( 1 );
                m_wildcardPosition = (WildcardPosition)( m_wildcardPosition | WildcardAtStart );
            }
            if( endsWith( m_stringToMatch, "*" ) ) {
                m_stringToMatch = m_stringToMatch.substr( 0, m_stringToMatch.size()-1 );
                m_wildcardPosition = (WildcardPosition)( m_wildcardPosition | WildcardAtEnd );
            }
        }

        IfFilterMatches::DoWhat getFilterType() const {
            return m_filterType;
        }

        bool shouldInclude( const TestCase& testCase ) const {
            return isMatch( testCase ) == (m_filterType == IfFilterMatches::IncludeTests);
        }
    private:

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#endif

        bool isMatch( const TestCase& testCase ) const {
            std::string name = testCase.getTestCaseInfo().name;
            toLowerInPlace( name );

            switch( m_wildcardPosition ) {
                case NoWildcard:
                    return m_stringToMatch == name;
                case WildcardAtStart:
                    return endsWith( name, m_stringToMatch );
                case WildcardAtEnd:
                    return startsWith( name, m_stringToMatch );
                case WildcardAtBothEnds:
                    return contains( name, m_stringToMatch );
            }
            throw std::logic_error( "Unhandled wildcard type" );
        }

#ifdef __clang__
#pragma clang diagnostic pop
#endif

        std::string m_stringToMatch;
        IfFilterMatches::DoWhat m_filterType;
        WildcardPosition m_wildcardPosition;
    };

    class TestCaseFilters {
    public:
        TestCaseFilters( const std::string& name ) : m_name( name ) {}

        std::string getName() const {
            return m_name;
        }

        void addFilter( const TestCaseFilter& filter ) {
            if( filter.getFilterType() == IfFilterMatches::ExcludeTests )
                m_exclusionFilters.push_back( filter );
            else
                m_inclusionFilters.push_back( filter );
        }

        void addTags( const std::string& tagPattern ) {
            TagExpression exp;
            TagExpressionParser( exp ).parse( tagPattern );

            m_tagExpressions.push_back( exp );
        }

        bool shouldInclude( const TestCase& testCase ) const {
            if( !m_tagExpressions.empty() ) {
                std::vector<TagExpression>::const_iterator it = m_tagExpressions.begin();
                std::vector<TagExpression>::const_iterator itEnd = m_tagExpressions.end();
                for(; it != itEnd; ++it )
                    if( it->matches( testCase.getTags() ) )
                        break;
                if( it == itEnd )
                    return false;
            }

            if( !m_inclusionFilters.empty() ) {
                std::vector<TestCaseFilter>::const_iterator it = m_inclusionFilters.begin();
                std::vector<TestCaseFilter>::const_iterator itEnd = m_inclusionFilters.end();
                for(; it != itEnd; ++it )
                    if( it->shouldInclude( testCase ) )
                        break;
                if( it == itEnd )
                    return false;
            }
            else if( m_exclusionFilters.empty() && m_tagExpressions.empty() ) {
                return !testCase.isHidden();
            }

            std::vector<TestCaseFilter>::const_iterator it = m_exclusionFilters.begin();
            std::vector<TestCaseFilter>::const_iterator itEnd = m_exclusionFilters.end();
            for(; it != itEnd; ++it )
                if( !it->shouldInclude( testCase ) )
                    return false;
            return true;
        }
    private:
        std::vector<TagExpression> m_tagExpressions;
        std::vector<TestCaseFilter> m_inclusionFilters;
        std::vector<TestCaseFilter> m_exclusionFilters;
        std::string m_name;
    };

}

// #included from: catch_interfaces_config.h
#define TWOBLUECUBES_CATCH_INTERFACES_CONFIG_H_INCLUDED

namespace Catch {

    struct IConfig {

        virtual ~IConfig();

        virtual bool allowThrows() const = 0;
    };
}

// #included from: catch_stream.hpp
#define TWOBLUECUBES_CATCH_STREAM_HPP_INCLUDED

// #included from: catch_streambuf.h
#define TWOBLUECUBES_CATCH_STREAMBUF_H_INCLUDED

#include <streambuf>

namespace Catch {

    class StreamBufBase : public std::streambuf {
    public:
        virtual ~StreamBufBase();
    };
}

#include <stdexcept>
#include <cstdio>

namespace Catch {

    template<typename WriterF, size_t bufferSize=256>
    class StreamBufImpl : public StreamBufBase {
        char data[bufferSize];
        WriterF m_writer;

    public:
        StreamBufImpl() {
            setp( data, data + sizeof(data) );
        }

        ~StreamBufImpl() {
            sync();
        }

    private:
        int overflow( int c ) {
            sync();

            if( c != EOF ) {
                if( pbase() == epptr() )
                    m_writer( std::string( 1, static_cast<char>( c ) ) );
                else
                    sputc( static_cast<char>( c ) );
            }
            return 0;
        }

        int sync() {
            if( pbase() != pptr() ) {
                m_writer( std::string( pbase(), static_cast<std::string::size_type>( pptr() - pbase() ) ) );
                setp( pbase(), epptr() );
            }
            return 0;
        }
    };

    ///////////////////////////////////////////////////////////////////////////

    struct OutputDebugWriter {

        void operator()( const std::string &str ) {
            writeToDebugConsole( str );
        }
    };

    class Stream {
    public:
        Stream()
        : streamBuf( NULL ), isOwned( false )
        {}

        Stream( std::streambuf* _streamBuf, bool _isOwned )
        : streamBuf( _streamBuf ), isOwned( _isOwned )
        {}

        void release() {
            if( isOwned ) {
                delete streamBuf;
                streamBuf = NULL;
                isOwned = false;
            }
        }

        std::streambuf* streamBuf;

    private:
        bool isOwned;
    };
}

#include <memory>
#include <vector>
#include <string>
#include <iostream>

#ifndef CATCH_CONFIG_CONSOLE_WIDTH
#define CATCH_CONFIG_CONSOLE_WIDTH 80
#endif

namespace Catch {

    struct Include { enum WhichResults {
        FailedOnly,
        SuccessfulResults
    }; };

    struct List{ enum What {
        None = 0,

        Reports = 1,
        Tests = 2,
        Tags = 4,
        All = Reports | Tests | Tags,

        WhatMask = 0xf,

        AsText = 0x10,
        AsXml = 0x20,

        AsMask = 0xf0
    }; };

    struct ConfigData {

        struct WarnAbout { enum What {
            Nothing = 0x00,
            NoAssertions = 0x01
        }; };

        ConfigData()
        :   listSpec( List::None ),
            shouldDebugBreak( false ),
            includeWhichResults( Include::FailedOnly ),
            cutoff( -1 ),
            allowThrows( true ),
            warnings( WarnAbout::Nothing )
        {}

        std::string reporter;
        std::string outputFilename;
        List::What listSpec;
        std::vector<TestCaseFilters> filters;
        bool shouldDebugBreak;
        std::string stream;
        Include::WhichResults includeWhichResults;
        std::string name;
        int cutoff;
        bool allowThrows;
        WarnAbout::What warnings;
    };

    class Config : public IConfig {
    private:
        Config( const Config& other );
        Config& operator = ( const Config& other );
        virtual void dummy();
    public:

        Config()
        :   m_os( std::cout.rdbuf() )
        {}

        Config( const ConfigData& data )
        :   m_data( data ),
            m_os( std::cout.rdbuf() )
        {}

        virtual ~Config() {
            m_os.rdbuf( std::cout.rdbuf() );
            m_stream.release();
        }

        void setFilename( const std::string& filename ) {
            m_data.outputFilename = filename;
        }

        List::What getListSpec( void ) const {
            return m_data.listSpec;
        }

        const std::string& getFilename() const {
            return m_data.outputFilename ;
        }

        List::What listWhat() const {
            return static_cast<List::What>( m_data.listSpec & List::WhatMask );
        }

        List::What listAs() const {
            return static_cast<List::What>( m_data.listSpec & List::AsMask );
        }

        std::string getName() const {
            return m_data.name;
        }

        bool shouldDebugBreak() const {
            return m_data.shouldDebugBreak;
        }

        virtual std::ostream& stream() const {
            return m_os;
        }

        void setStreamBuf( std::streambuf* buf ) {
            m_os.rdbuf( buf ? buf : std::cout.rdbuf() );
        }

        void useStream( const std::string& streamName ) {
            Stream stream = createStream( streamName );
            setStreamBuf( stream.streamBuf );
            m_stream.release();
            m_stream = stream;
        }

        void addTestSpec( const std::string& testSpec ) {
            TestCaseFilters filters( testSpec );
            filters.addFilter( TestCaseFilter( testSpec ) );
            m_data.filters.push_back( filters );
        }

        virtual bool includeSuccessfulResults() const {
            return m_data.includeWhichResults == Include::SuccessfulResults;
        }

        int getCutoff() const {
            return m_data.cutoff;
        }

        virtual bool allowThrows() const {
            return m_data.allowThrows;
        }

        const ConfigData& data() const {
            return m_data;
        }
        ConfigData& data() {
            return m_data;
        }

    private:
        ConfigData m_data;

        // !TBD Move these out of here
        Stream m_stream;
        mutable std::ostream m_os;
    };

} // end namespace Catch

// #included from: catch_option.hpp
#define TWOBLUECUBES_CATCH_OPTION_HPP_INCLUDED

namespace Catch {

    // An optional type
    template<typename T>
    class Option {
    public:
        Option() : nullableValue( NULL ) {}
        Option( T const& _value )
        : nullableValue( new( storage ) T( _value ) )
        {}
        Option( Option const& _other )
        : nullableValue( _other ? new( storage ) T( *_other ) : NULL )
        {}

        ~Option() {
            reset();
        }

        Option& operator= ( Option const& _other ) {
            reset();
            if( _other )
                nullableValue = new( storage ) T( *_other );
            return *this;
        }

        void reset() {
            if( nullableValue )
                nullableValue->~T();
            nullableValue = NULL;
        }
        T& operator*() { return *nullableValue; }
        const T& operator*() const { return *nullableValue; }
        T* operator->() { return nullableValue; }
        const T* operator->() const { return nullableValue; }

        bool some() const { return nullableValue != NULL; }
        bool none() const { return nullableValue == NULL; }

        bool operator !() const { return nullableValue == NULL; }
        operator SafeBool::type() const {
            return SafeBool::makeSafe( some() );
        }

    private:
        T* nullableValue;
        char storage[sizeof(T)];
    };

} // end namespace Catch

#include <string>
#include <ostream>
#include <map>

namespace Catch
{
    struct ReporterConfig {
        ReporterConfig( std::ostream& _stream, ConfigData const& _fullConfig )
        :   m_stream( &_stream ), m_fullConfig( _fullConfig ) {}

        std::ostream& stream() const            { return *m_stream; }
        std::string name() const                { return m_fullConfig.name; }
        bool includeSuccessfulResults() const   { return m_fullConfig.includeWhichResults == Include::SuccessfulResults; }
        bool warnAboutMissingAssertions() const { return m_fullConfig.warnings & ConfigData::WarnAbout::NoAssertions; }

    private:
        std::ostream* m_stream;
        ConfigData m_fullConfig;
    };

    struct ReporterPreferences {
        ReporterPreferences()
        : shouldRedirectStdOut( false )
        {}

        bool shouldRedirectStdOut;
    };

    struct TestRunInfo {
        TestRunInfo( std::string const& _name ) : name( _name ) {}
        std::string name;
    };
    struct GroupInfo {
        GroupInfo(  std::string const& _name,
                    std::size_t _groupIndex,
                    std::size_t _groupsCount )
        :   name( _name ),
            groupIndex( _groupIndex ),
            groupsCounts( _groupsCount )
        {}

        std::string name;
        std::size_t groupIndex;
        std::size_t groupsCounts;
    };

    struct SectionInfo {
        SectionInfo(    std::string const& _name,
                        std::string const& _description,
                        SourceLineInfo const& _lineInfo )
        :   name( _name ),
            description( _description ),
            lineInfo( _lineInfo )
        {}

        std::string name;
        std::string description;
        SourceLineInfo lineInfo;
    };

    struct ThreadedSectionInfo : SectionInfo, SharedImpl<> {
        ThreadedSectionInfo( SectionInfo const& _sectionInfo, ThreadedSectionInfo* _parent = NULL )
        :   SectionInfo( _sectionInfo ),
            parent( _parent )
        {}
        virtual ~ThreadedSectionInfo();

        std::vector<Ptr<ThreadedSectionInfo> > children;
        ThreadedSectionInfo* parent;
    };

    struct AssertionStats {
        AssertionStats( AssertionResult const& _assertionResult,
                        std::vector<MessageInfo> const& _infoMessages,
                        Totals const& _totals )
        :   assertionResult( _assertionResult ),
            infoMessages( _infoMessages ),
            totals( _totals )
        {
            if( assertionResult.hasMessage() ) {
                // Copy message into messages list.
                // !TBD This should have been done earlier, somewhere
                MessageBuilder builder( assertionResult.getTestMacroName(), assertionResult.getSourceInfo(), assertionResult.getResultType() );
                builder << assertionResult.getMessage();
                infoMessages.push_back( builder.build() );
            }
        }
        virtual ~AssertionStats();

        AssertionResult assertionResult;
        std::vector<MessageInfo> infoMessages;
        Totals totals;
    };

    struct SectionStats {
        SectionStats(   SectionInfo const& _sectionInfo,
                        Counts const& _assertions,
                        bool _missingAssertions )
        :   sectionInfo( _sectionInfo ),
            assertions( _assertions ),
            missingAssertions( _missingAssertions )
        {}
        virtual ~SectionStats();

        SectionInfo sectionInfo;
        Counts assertions;
        bool missingAssertions;
    };

    struct TestCaseStats {
        TestCaseStats(  TestCaseInfo const& _testInfo,
                        Totals const& _totals,
                        std::string const& _stdOut,
                        std::string const& _stdErr,
                        bool _missingAssertions,
                        bool _aborting )
        : testInfo( _testInfo ),
            totals( _totals ),
            stdOut( _stdOut ),
            stdErr( _stdErr ),
            missingAssertions( _missingAssertions ),
            aborting( _aborting )
        {}
        virtual ~TestCaseStats();

        TestCaseInfo testInfo;
        Totals totals;
        std::string stdOut;
        std::string stdErr;
        bool missingAssertions;
        bool aborting;
    };

    struct TestGroupStats {
        TestGroupStats( GroupInfo const& _groupInfo,
                        Totals const& _totals,
                        bool _aborting )
        :   groupInfo( _groupInfo ),
            totals( _totals ),
            aborting( _aborting )
        {}
        TestGroupStats( GroupInfo const& _groupInfo )
        :   groupInfo( _groupInfo ),
            aborting( false )
        {}
        virtual ~TestGroupStats();

        GroupInfo groupInfo;
        Totals totals;
        bool aborting;
    };

    struct TestRunStats {
        TestRunStats(   TestRunInfo const& _runInfo,
                        Totals const& _totals,
                        bool _aborting )
        :   runInfo( _runInfo ),
            totals( _totals ),
            aborting( _aborting )
        {}
        TestRunStats( TestRunStats const& _other )
        :   runInfo( _other.runInfo ),
            totals( _other.totals ),
            aborting( _other.aborting )
        {}
        virtual ~TestRunStats();

        TestRunInfo runInfo;
        Totals totals;
        bool aborting;
    };

    // !Work In progress
    struct IStreamingReporter : IShared {
        virtual ~IStreamingReporter();

        // Implementing class must also provide the following static method:
        // static std::string getDescription();

        virtual ReporterPreferences getPreferences() const = 0;

        virtual void noMatchingTestCases( std::string const& spec ) = 0;

        virtual void testRunStarting( TestRunInfo const& testRunInfo ) = 0;
        virtual void testGroupStarting( GroupInfo const& groupInfo ) = 0;

        virtual void testCaseStarting( TestCaseInfo const& testInfo ) = 0;
        virtual void sectionStarting( SectionInfo const& sectionInfo ) = 0;

        virtual void assertionStarting( AssertionInfo const& assertionInfo ) = 0;

        virtual void assertionEnded( AssertionStats const& assertionStats ) = 0;
        virtual void sectionEnded( SectionStats const& sectionStats ) = 0;
        virtual void testCaseEnded( TestCaseStats const& testCaseStats ) = 0;
        virtual void testGroupEnded( TestGroupStats const& testGroupStats ) = 0;
        virtual void testRunEnded( TestRunStats const& testRunStats ) = 0;
    };

    struct StreamingReporterBase : SharedImpl<IStreamingReporter> {

        StreamingReporterBase( ReporterConfig const& _config )
        :   m_config( _config ),
            stream( _config.stream() )
        {}

        virtual ~StreamingReporterBase();

        virtual void noMatchingTestCases( std::string const& ) {}

        virtual void testRunStarting( TestRunInfo const& _testRunInfo ) {
            testRunInfo = _testRunInfo;
        }
        virtual void testGroupStarting( GroupInfo const& _groupInfo ) {
            unusedGroupInfo = _groupInfo;
        }

        virtual void testCaseStarting( TestCaseInfo const& _testInfo ) {
            unusedTestCaseInfo = _testInfo;
        }
        virtual void sectionStarting( SectionInfo const& _sectionInfo ) {
            Ptr<ThreadedSectionInfo> sectionInfo = new ThreadedSectionInfo( _sectionInfo );
            if( !currentSectionInfo ) {
                currentSectionInfo = sectionInfo;
                m_rootSections.push_back( currentSectionInfo );
            }
            else {
                currentSectionInfo->children.push_back( sectionInfo );
                sectionInfo->parent = currentSectionInfo.get();
                currentSectionInfo = sectionInfo;
            }
        }

        virtual void sectionEnded( SectionStats const& /* _sectionStats */ ) {
            currentSectionInfo = currentSectionInfo->parent;
        }
        virtual void testCaseEnded( TestCaseStats const& /* _testCaseStats */ ) {
            unusedTestCaseInfo.reset();
        }
        virtual void testGroupEnded( TestGroupStats const& /* _testGroupStats */ ) {
            unusedGroupInfo.reset();
        }
        virtual void testRunEnded( TestRunStats const& /* _testRunStats */ ) {
            currentSectionInfo.reset();
            unusedTestCaseInfo.reset();
            unusedGroupInfo.reset();
            testRunInfo.reset();
        }

        ReporterConfig m_config;
        Option<TestRunInfo> testRunInfo;
        Option<GroupInfo> unusedGroupInfo;
        Option<TestCaseInfo> unusedTestCaseInfo;
        Ptr<ThreadedSectionInfo> currentSectionInfo;
        std::ostream& stream;

        // !TBD: This should really go in the TestCaseStats class
        std::vector<Ptr<ThreadedSectionInfo> > m_rootSections;
    };

    struct TestGroupNode : TestGroupStats {
        TestGroupNode( TestGroupStats const& _stats ) : TestGroupStats( _stats ) {}
//        TestGroupNode( GroupInfo const& _info ) : TestGroupStats( _stats ) {}
        ~TestGroupNode();

    };

    struct TestRunNode : TestRunStats {

        TestRunNode( TestRunStats const& _stats ) : TestRunStats( _stats ) {}
        ~TestRunNode();

        std::vector<TestGroupNode> groups;
    };

    // Deprecated
    struct IReporter : IShared {
        virtual ~IReporter();

        virtual bool shouldRedirectStdout() const = 0;

        virtual void StartTesting() = 0;
        virtual void EndTesting( Totals const& totals ) = 0;
        virtual void StartGroup( std::string const& groupName ) = 0;
        virtual void EndGroup( std::string const& groupName, Totals const& totals ) = 0;
        virtual void StartTestCase( TestCaseInfo const& testInfo ) = 0;
        virtual void EndTestCase( TestCaseInfo const& testInfo, Totals const& totals, std::string const& stdOut, std::string const& stdErr ) = 0;
        virtual void StartSection( std::string const& sectionName, std::string const& description ) = 0;
        virtual void EndSection( std::string const& sectionName, const Counts& assertions ) = 0;
        virtual void NoAssertionsInSection( std::string const& sectionName ) = 0;
        virtual void NoAssertionsInTestCase( std::string const& testName ) = 0;
        virtual void Aborted() = 0;
        virtual void Result( const AssertionResult& result ) = 0;
    };

    struct IReporterFactory {
        virtual ~IReporterFactory();
        virtual IStreamingReporter* create( ReporterConfig const& config ) const = 0;
        virtual std::string getDescription() const = 0;
    };

    struct IReporterRegistry {
        typedef std::map<std::string, IReporterFactory*> FactoryMap;

        virtual ~IReporterRegistry();
        virtual IStreamingReporter* create( std::string const& name, ReporterConfig const& config ) const = 0;
        virtual const FactoryMap& getFactories() const = 0;
    };

    inline std::string trim( std::string const& str ) {
        std::string::size_type start = str.find_first_not_of( "\n\r\t " );
        std::string::size_type end = str.find_last_not_of( "\n\r\t " );

        return start != std::string::npos ? str.substr( start, 1+end-start ) : "";
    }
}

#include <vector>

namespace Catch {

    class TestCase;
    struct ITestCaseRegistry;
    struct IExceptionTranslatorRegistry;
    struct IExceptionTranslator;

    struct IRegistryHub {
        virtual ~IRegistryHub();

        virtual const IReporterRegistry& getReporterRegistry() const = 0;
        virtual const ITestCaseRegistry& getTestCaseRegistry() const = 0;
        virtual IExceptionTranslatorRegistry& getExceptionTranslatorRegistry() = 0;
    };

    struct IMutableRegistryHub {
        virtual ~IMutableRegistryHub();
        virtual void registerReporter( const std::string& name, IReporterFactory* factory ) = 0;
        virtual void registerTest( const TestCase& testInfo ) = 0;
        virtual void registerTranslator( const IExceptionTranslator* translator ) = 0;
    };

    IRegistryHub& getRegistryHub();
    IMutableRegistryHub& getMutableRegistryHub();
    void cleanUp();
    std::string translateActiveException();

}

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
        Catch::isTrue( false && originalExpr ); \
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
#define INTERNAL_CATCH_INFO( log, macroName ) \
    do { \
        Catch::getResultCapture().acceptMessage( Catch::MessageBuilder( macroName, CATCH_INTERNAL_LINEINFO, Catch::ResultWas::Info ) << log ); \
    } while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_MSG( log, messageType, resultDisposition, macroName ) \
    do { \
        INTERNAL_CATCH_ACCEPT_INFO( "", macroName, resultDisposition ); \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::ExpressionResultBuilder( messageType ) << log, resultDisposition, true ) \
    } while( Catch::isTrue( false ) )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_SCOPED_INFO( log, macroName ) \
    Catch::ScopedMessageBuilder INTERNAL_CATCH_UNIQUE_NAME( scopedMessage )( macroName, CATCH_INTERNAL_LINEINFO, Catch::ResultWas::Info ); \
    INTERNAL_CATCH_UNIQUE_NAME( scopedMessage ) << log; \
    Catch::getResultCapture().pushScopedMessage( INTERNAL_CATCH_UNIQUE_NAME( scopedMessage ) )

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

// #included from: internal/catch_section.hpp
#define TWOBLUECUBES_CATCH_SECTION_HPP_INCLUDED

#include <string>

namespace Catch {

    class Section {
    public:
        Section(    const SourceLineInfo& lineInfo,
                    const std::string& name,
                    const std::string& description = "" )
        :   m_info( name, description, lineInfo ),
            m_sectionIncluded( getCurrentContext().getResultCapture().sectionStarted( m_info, m_assertions ) )
        {}

        ~Section() {
            if( m_sectionIncluded )
                getCurrentContext().getResultCapture().sectionEnded( m_info, m_assertions );
        }

        // This indicates whether the section should be executed or not
        operator bool() {
            return m_sectionIncluded;
        }

    private:
        SectionInfo m_info;

        std::string m_name;
        Counts m_assertions;
        bool m_sectionIncluded;
    };

} // end namespace Catch

#ifdef CATCH_CONFIG_VARIADIC_MACROS
    #define INTERNAL_CATCH_SECTION( ... ) \
        if( Catch::Section INTERNAL_CATCH_UNIQUE_NAME( catch_internal_Section ) = Catch::Section( CATCH_INTERNAL_LINEINFO, __VA_ARGS__ ) )
#else
    #define INTERNAL_CATCH_SECTION( name, desc ) \
        if( Catch::Section INTERNAL_CATCH_UNIQUE_NAME( catch_internal_Section ) = Catch::Section( CATCH_INTERNAL_LINEINFO, name, desc ) )
#endif

// #included from: internal/catch_generators.hpp
#define TWOBLUECUBES_CATCH_GENERATORS_HPP_INCLUDED

#include <iterator>
#include <vector>
#include <string>
#include <stdlib.h>

namespace Catch {

template<typename T>
struct IGenerator {
    virtual ~IGenerator() {}
    virtual T getValue( std::size_t index ) const = 0;
    virtual std::size_t size () const = 0;
};

template<typename T>
class BetweenGenerator : public IGenerator<T> {
public:
    BetweenGenerator( T from, T to ) : m_from( from ), m_to( to ){}

    virtual T getValue( std::size_t index ) const {
        return m_from+static_cast<int>( index );
    }

    virtual std::size_t size() const {
        return static_cast<std::size_t>( 1+m_to-m_from );
    }

private:

    T m_from;
    T m_to;
};

template<typename T>
class ValuesGenerator : public IGenerator<T> {
public:
    ValuesGenerator(){}

    void add( T value ) {
        m_values.push_back( value );
    }

    virtual T getValue( std::size_t index ) const {
        return m_values[index];
    }

    virtual std::size_t size() const {
        return m_values.size();
    }

private:
    std::vector<T> m_values;
};

template<typename T>
class CompositeGenerator {
public:
    CompositeGenerator() : m_totalSize( 0 ) {}

    // *** Move semantics, similar to auto_ptr ***
    CompositeGenerator( CompositeGenerator& other )
    :   m_fileInfo( other.m_fileInfo ),
        m_totalSize( 0 )
    {
        move( other );
    }

    CompositeGenerator& setFileInfo( const char* fileInfo ) {
        m_fileInfo = fileInfo;
        return *this;
    }

    ~CompositeGenerator() {
        deleteAll( m_composed );
    }

    operator T () const {
        size_t overallIndex = getCurrentContext().getGeneratorIndex( m_fileInfo, m_totalSize );

        typename std::vector<const IGenerator<T>*>::const_iterator it = m_composed.begin();
        typename std::vector<const IGenerator<T>*>::const_iterator itEnd = m_composed.end();
        for( size_t index = 0; it != itEnd; ++it )
        {
            const IGenerator<T>* generator = *it;
            if( overallIndex >= index && overallIndex < index + generator->size() )
            {
                return generator->getValue( overallIndex-index );
            }
            index += generator->size();
        }
        CATCH_INTERNAL_ERROR( "Indexed past end of generated range" );
        return T(); // Suppress spurious "not all control paths return a value" warning in Visual Studio - if you know how to fix this please do so
    }

    void add( const IGenerator<T>* generator ) {
        m_totalSize += generator->size();
        m_composed.push_back( generator );
    }

    CompositeGenerator& then( CompositeGenerator& other ) {
        move( other );
        return *this;
    }

    CompositeGenerator& then( T value ) {
        ValuesGenerator<T>* valuesGen = new ValuesGenerator<T>();
        valuesGen->add( value );
        add( valuesGen );
        return *this;
    }

private:

    void move( CompositeGenerator& other ) {
        std::copy( other.m_composed.begin(), other.m_composed.end(), std::back_inserter( m_composed ) );
        m_totalSize += other.m_totalSize;
        other.m_composed.clear();
    }

    std::vector<const IGenerator<T>*> m_composed;
    std::string m_fileInfo;
    size_t m_totalSize;
};

namespace Generators
{
    template<typename T>
    CompositeGenerator<T> between( T from, T to ) {
        CompositeGenerator<T> generators;
        generators.add( new BetweenGenerator<T>( from, to ) );
        return generators;
    }

    template<typename T>
    CompositeGenerator<T> values( T val1, T val2 ) {
        CompositeGenerator<T> generators;
        ValuesGenerator<T>* valuesGen = new ValuesGenerator<T>();
        valuesGen->add( val1 );
        valuesGen->add( val2 );
        generators.add( valuesGen );
        return generators;
    }

    template<typename T>
    CompositeGenerator<T> values( T val1, T val2, T val3 ){
        CompositeGenerator<T> generators;
        ValuesGenerator<T>* valuesGen = new ValuesGenerator<T>();
        valuesGen->add( val1 );
        valuesGen->add( val2 );
        valuesGen->add( val3 );
        generators.add( valuesGen );
        return generators;
    }

    template<typename T>
    CompositeGenerator<T> values( T val1, T val2, T val3, T val4 ) {
        CompositeGenerator<T> generators;
        ValuesGenerator<T>* valuesGen = new ValuesGenerator<T>();
        valuesGen->add( val1 );
        valuesGen->add( val2 );
        valuesGen->add( val3 );
        valuesGen->add( val4 );
        generators.add( valuesGen );
        return generators;
    }

} // end namespace Generators

using namespace Generators;

} // end namespace Catch

#define INTERNAL_CATCH_LINESTR2( line ) #line
#define INTERNAL_CATCH_LINESTR( line ) INTERNAL_CATCH_LINESTR2( line )

#define INTERNAL_CATCH_GENERATE( expr ) expr.setFileInfo( __FILE__ "(" INTERNAL_CATCH_LINESTR( __LINE__ ) ")" )

// #included from: internal/catch_interfaces_exception.h
#define TWOBLUECUBES_CATCH_INTERFACES_EXCEPTION_H_INCLUDED

#include <string>

namespace Catch {

    typedef std::string(*exceptionTranslateFunction)();

    struct IExceptionTranslator {
        virtual ~IExceptionTranslator();
        virtual std::string translate() const = 0;
    };

    struct IExceptionTranslatorRegistry {
        virtual ~IExceptionTranslatorRegistry();

        virtual std::string translateActiveException() const = 0;
    };

    class ExceptionTranslatorRegistrar {
        template<typename T>
        class ExceptionTranslator : public IExceptionTranslator {
        public:

            ExceptionTranslator( std::string(*translateFunction)( T& ) )
            : m_translateFunction( translateFunction )
            {}

            virtual std::string translate() const {
                try {
                    throw;
                }
                catch( T& ex ) {
                    return m_translateFunction( ex );
                }
            }

        protected:
            std::string(*m_translateFunction)( T& );
        };

    public:
        template<typename T>
        ExceptionTranslatorRegistrar( std::string(*translateFunction)( T& ) ) {
            getMutableRegistryHub().registerTranslator
                ( new ExceptionTranslator<T>( translateFunction ) );
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TRANSLATE_EXCEPTION( signature ) \
    static std::string INTERNAL_CATCH_UNIQUE_NAME( catch_internal_ExceptionTranslator )( signature ); \
    namespace{ Catch::ExceptionTranslatorRegistrar INTERNAL_CATCH_UNIQUE_NAME( catch_internal_ExceptionRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME( catch_internal_ExceptionTranslator ) ); }\
    static std::string INTERNAL_CATCH_UNIQUE_NAME(  catch_internal_ExceptionTranslator )( signature )

// #included from: internal/catch_approx.hpp
#define TWOBLUECUBES_CATCH_APPROX_HPP_INCLUDED

#include <cmath>
#include <limits>

namespace Catch {
namespace Detail {

    class Approx {
    public:
        explicit Approx ( double value )
        :   m_epsilon( std::numeric_limits<float>::epsilon()*100 ),
            m_scale( 1.0 ),
            m_value( value )
        {}

        Approx( const Approx& other )
        :   m_epsilon( other.m_epsilon ),
            m_scale( other.m_scale ),
            m_value( other.m_value )
        {}

        static Approx custom() {
            return Approx( 0 );
        }

        Approx operator()( double value ) {
            Approx approx( value );
            approx.epsilon( m_epsilon );
            approx.scale( m_scale );
            return approx;
        }

        friend bool operator == ( double lhs, const Approx& rhs ) {
            // Thanks to Richard Harris for his help refining this formula
            return fabs( lhs - rhs.m_value ) < rhs.m_epsilon * (rhs.m_scale + (std::max)( fabs(lhs), fabs(rhs.m_value) ) );
        }

        friend bool operator == ( const Approx& lhs, double rhs ) {
            return operator==( rhs, lhs );
        }

        friend bool operator != ( double lhs, const Approx& rhs ) {
            return !operator==( lhs, rhs );
        }

        friend bool operator != ( const Approx& lhs, double rhs ) {
            return !operator==( rhs, lhs );
        }

        Approx& epsilon( double newEpsilon ) {
            m_epsilon = newEpsilon;
            return *this;
        }

        Approx& scale( double newScale ) {
            m_scale = newScale;
            return *this;
        }

        std::string toString() const {
            std::ostringstream oss;
            oss << "Approx( " << m_value << " )";
            return oss.str();
        }

    private:
        double m_epsilon;
        double m_scale;
        double m_value;
    };
}

template<>
inline std::string toString<Detail::Approx>( const Detail::Approx& value ) {
    return value.toString();
}

} // end namespace Catch

// #included from: internal/catch_matchers.hpp
#define TWOBLUECUBES_CATCH_MATCHERS_HPP_INCLUDED

namespace Catch {
namespace Matchers {
    namespace Impl {

    template<typename ExpressionT>
    struct Matcher : SharedImpl<IShared>
    {
        typedef ExpressionT ExpressionType;

        virtual ~Matcher() {}
        virtual Ptr<Matcher> clone() const = 0;
        virtual bool match( ExpressionT const& expr ) const = 0;
        virtual std::string toString() const = 0;
    };

    template<typename DerivedT, typename ExpressionT>
    struct MatcherImpl : Matcher<ExpressionT> {

        virtual Ptr<Matcher<ExpressionT> > clone() const {
            return Ptr<Matcher<ExpressionT> >( new DerivedT( static_cast<const DerivedT&>( *this ) ) );
        }
    };

    namespace Generic {

        template<typename ExpressionT>
        class AllOf : public MatcherImpl<AllOf<ExpressionT>, ExpressionT> {
        public:

            AllOf() {}
            AllOf( const AllOf& other ) : m_matchers( other.m_matchers ) {}

            AllOf& add( const Matcher<ExpressionT>& matcher ) {
                m_matchers.push_back( matcher.clone() );
                return *this;
            }
            virtual bool match( const ExpressionT& expr ) const
            {
                for( std::size_t i = 0; i < m_matchers.size(); ++i )
                    if( !m_matchers[i]->match( expr ) )
                        return false;
                return true;
            }
            virtual std::string toString() const {
                std::ostringstream oss;
                oss << "( ";
                for( std::size_t i = 0; i < m_matchers.size(); ++i ) {
                    if( i != 0 )
                        oss << " and ";
                    oss << m_matchers[i]->toString();
                }
                oss << " )";
                return oss.str();
            }

        private:
            std::vector<Ptr<Matcher<ExpressionT> > > m_matchers;
        };

        template<typename ExpressionT>
        class AnyOf : public MatcherImpl<AnyOf<ExpressionT>, ExpressionT> {
        public:

            AnyOf() {}
            AnyOf( const AnyOf& other ) : m_matchers( other.m_matchers ) {}

            AnyOf& add( const Matcher<ExpressionT>& matcher ) {
                m_matchers.push_back( matcher.clone() );
                return *this;
            }
            virtual bool match( const ExpressionT& expr ) const
            {
                for( std::size_t i = 0; i < m_matchers.size(); ++i )
                    if( m_matchers[i]->match( expr ) )
                        return true;
                return false;
            }
            virtual std::string toString() const {
                std::ostringstream oss;
                oss << "( ";
                for( std::size_t i = 0; i < m_matchers.size(); ++i ) {
                    if( i != 0 )
                        oss << " or ";
                    oss << m_matchers[i]->toString();
                }
                oss << " )";
                return oss.str();
            }

        private:
            std::vector<Ptr<Matcher<ExpressionT> > > m_matchers;
        };

    }

    namespace StdString {

        struct Equals : MatcherImpl<Equals, std::string> {
            Equals( const std::string& str ) : m_str( str ){}
            Equals( const Equals& other ) : m_str( other.m_str ){}

            virtual ~Equals();

            virtual bool match( const std::string& expr ) const {
                return m_str == expr;
            }
            virtual std::string toString() const {
                return "equals: \"" + m_str + "\"";
            }

            std::string m_str;
        };

        struct Contains : MatcherImpl<Contains, std::string> {
            Contains( const std::string& substr ) : m_substr( substr ){}
            Contains( const Contains& other ) : m_substr( other.m_substr ){}

            virtual ~Contains();

            virtual bool match( const std::string& expr ) const {
                return expr.find( m_substr ) != std::string::npos;
            }
            virtual std::string toString() const {
                return "contains: \"" + m_substr + "\"";
            }

            std::string m_substr;
        };

        struct StartsWith : MatcherImpl<StartsWith, std::string> {
            StartsWith( const std::string& substr ) : m_substr( substr ){}
            StartsWith( const StartsWith& other ) : m_substr( other.m_substr ){}

            virtual ~StartsWith();

            virtual bool match( const std::string& expr ) const {
                return expr.find( m_substr ) == 0;
            }
            virtual std::string toString() const {
                return "starts with: \"" + m_substr + "\"";
            }

            std::string m_substr;
        };

        struct EndsWith : MatcherImpl<EndsWith, std::string> {
            EndsWith( const std::string& substr ) : m_substr( substr ){}
            EndsWith( const EndsWith& other ) : m_substr( other.m_substr ){}

            virtual ~EndsWith();

            virtual bool match( const std::string& expr ) const {
                return expr.find( m_substr ) == expr.size() - m_substr.size();
            }
            virtual std::string toString() const {
                return "ends with: \"" + m_substr + "\"";
            }

            std::string m_substr;
        };
    } // namespace StdString
    } // namespace Impl

    // The following functions create the actual matcher objects.
    // This allows the types to be inferred
    template<typename ExpressionT>
    inline Impl::Generic::AllOf<ExpressionT> AllOf( const Impl::Matcher<ExpressionT>& m1,
                                                    const Impl::Matcher<ExpressionT>& m2 ) {
        return Impl::Generic::AllOf<ExpressionT>().add( m1 ).add( m2 );
    }
    template<typename ExpressionT>
    inline Impl::Generic::AllOf<ExpressionT> AllOf( const Impl::Matcher<ExpressionT>& m1,
                                                    const Impl::Matcher<ExpressionT>& m2,
                                                    const Impl::Matcher<ExpressionT>& m3 ) {
        return Impl::Generic::AllOf<ExpressionT>().add( m1 ).add( m2 ).add( m3 );
    }
    template<typename ExpressionT>
    inline Impl::Generic::AnyOf<ExpressionT> AnyOf( const Impl::Matcher<ExpressionT>& m1,
                                                    const Impl::Matcher<ExpressionT>& m2 ) {
        return Impl::Generic::AnyOf<ExpressionT>().add( m1 ).add( m2 );
    }
    template<typename ExpressionT>
    inline Impl::Generic::AnyOf<ExpressionT> AnyOf( const Impl::Matcher<ExpressionT>& m1,
                                                    const Impl::Matcher<ExpressionT>& m2,
                                                    const Impl::Matcher<ExpressionT>& m3 ) {
        return Impl::Generic::AnyOf<ExpressionT>().add( m1 ).add( m2 ).add( m3 );
    }

    inline Impl::StdString::Equals      Equals( const std::string& str ){ return Impl::StdString::Equals( str ); }
    inline Impl::StdString::Contains    Contains( const std::string& substr ){ return Impl::StdString::Contains( substr ); }
    inline Impl::StdString::StartsWith  StartsWith( const std::string& substr ){ return Impl::StdString::StartsWith( substr ); }
    inline Impl::StdString::EndsWith    EndsWith( const std::string& substr ){ return Impl::StdString::EndsWith( substr ); }

} // namespace Matchers

using namespace Matchers;

} // namespace Catch

// These files are included here so the single_include script doesn't put them
// in the conditionally compiled sections
// #included from: internal/catch_interfaces_runner.h
#define TWOBLUECUBES_CATCH_INTERFACES_RUNNER_H_INCLUDED

#include <string>

namespace Catch {
    class TestCase;

    struct IRunner {
        virtual ~IRunner();
    };
}


#ifdef __OBJC__
// #included from: internal/catch_objc.hpp
#define TWOBLUECUBES_CATCH_OBJC_HPP_INCLUDED

#import <objc/runtime.h>

#include <string>

// NB. Any general catch headers included here must be included
// in catch.hpp first to make sure they are included by the single
// header for non obj-usage

///////////////////////////////////////////////////////////////////////////////
// This protocol is really only here for (self) documenting purposes, since
// all its methods are optional.
@protocol OcFixture

@optional

-(void) setUp;
-(void) tearDown;

@end

namespace Catch {

    class OcMethod : public SharedImpl<ITestCase> {

    public:
        OcMethod( Class cls, SEL sel ) : m_cls( cls ), m_sel( sel ) {}

        virtual void invoke() const {
            id obj = [[m_cls alloc] init];

            performOptionalSelector( obj, @selector(setUp)  );
            performOptionalSelector( obj, m_sel );
            performOptionalSelector( obj, @selector(tearDown)  );

            arcSafeRelease( obj );
        }
    private:
        virtual ~OcMethod() {}

        Class m_cls;
        SEL m_sel;
    };

    namespace Detail{

        inline bool startsWith( const std::string& str, const std::string& sub ) {
            return str.length() > sub.length() && str.substr( 0, sub.length() ) == sub;
        }

        inline std::string getAnnotation(   Class cls,
                                            const std::string& annotationName,
                                            const std::string& testCaseName ) {
            NSString* selStr = [[NSString alloc] initWithFormat:@"Catch_%s_%s", annotationName.c_str(), testCaseName.c_str()];
            SEL sel = NSSelectorFromString( selStr );
            arcSafeRelease( selStr );
            id value = performOptionalSelector( cls, sel );
            if( value )
                return [(NSString*)value UTF8String];
            return "";
        }
    }

    inline size_t registerTestMethods() {
        size_t noTestMethods = 0;
        int noClasses = objc_getClassList( NULL, 0 );

        Class* classes = (CATCH_UNSAFE_UNRETAINED Class *)malloc( sizeof(Class) * noClasses);
        objc_getClassList( classes, noClasses );

        for( int c = 0; c < noClasses; c++ ) {
            Class cls = classes[c];
            {
                u_int count;
                Method* methods = class_copyMethodList( cls, &count );
                for( u_int m = 0; m < count ; m++ ) {
                    SEL selector = method_getName(methods[m]);
                    std::string methodName = sel_getName(selector);
                    if( Detail::startsWith( methodName, "Catch_TestCase_" ) ) {
                        std::string testCaseName = methodName.substr( 15 );
                        std::string name = Detail::getAnnotation( cls, "Name", testCaseName );
                        std::string desc = Detail::getAnnotation( cls, "Description", testCaseName );
                        const char* className = class_getName( cls );

                        getMutableRegistryHub().registerTest( makeTestCase( new OcMethod( cls, selector ), className, name.c_str(), desc.c_str(), SourceLineInfo() ) );
                        noTestMethods++;
                    }
                }
                free(methods);
            }
        }
        return noTestMethods;
    }

    namespace Matchers {
        namespace Impl {
        namespace NSStringMatchers {

            template<typename MatcherT>
            struct StringHolder : MatcherImpl<MatcherT, NSString*>{
                StringHolder( NSString* substr ) : m_substr( [substr copy] ){}
                StringHolder( StringHolder const& other ) : m_substr( [other.m_substr copy] ){}
                StringHolder() {
                    arcSafeRelease( m_substr );
                }

                NSString* m_substr;
            };

            struct Equals : StringHolder<Equals> {
                Equals( NSString* substr ) : StringHolder( substr ){}

                virtual bool match( ExpressionType const& str ) const {
                    return [str isEqualToString:m_substr];
                }

                virtual std::string toString() const {
                    return "equals string: \"" + Catch::toString( m_substr ) + "\"";
                }
            };

            struct Contains : StringHolder<Contains> {
                Contains( NSString* substr ) : StringHolder( substr ){}

                virtual bool match( ExpressionType const& str ) const {
                    return [str rangeOfString:m_substr].location != NSNotFound;
                }

                virtual std::string toString() const {
                    return "contains string: \"" + Catch::toString( m_substr ) + "\"";
                }
            };

            struct StartsWith : StringHolder<StartsWith> {
                StartsWith( NSString* substr ) : StringHolder( substr ){}

                virtual bool match( ExpressionType const& str ) const {
                    return [str rangeOfString:m_substr].location == 0;
                }

                virtual std::string toString() const {
                    return "starts with: \"" + Catch::toString( m_substr ) + "\"";
                }
            };
            struct EndsWith : StringHolder<EndsWith> {
                EndsWith( NSString* substr ) : StringHolder( substr ){}

                virtual bool match( ExpressionType const& str ) const {
                    return [str rangeOfString:m_substr].location == [str length] - [m_substr length];
                }

                virtual std::string toString() const {
                    return "ends with: \"" + Catch::toString( m_substr ) + "\"";
                }
            };

        } // namespace NSStringMatchers
        } // namespace Impl

        inline Impl::NSStringMatchers::Equals
            Equals( NSString* substr ){ return Impl::NSStringMatchers::Equals( substr ); }

        inline Impl::NSStringMatchers::Contains
            Contains( NSString* substr ){ return Impl::NSStringMatchers::Contains( substr ); }

        inline Impl::NSStringMatchers::StartsWith
            StartsWith( NSString* substr ){ return Impl::NSStringMatchers::StartsWith( substr ); }

        inline Impl::NSStringMatchers::EndsWith
            EndsWith( NSString* substr ){ return Impl::NSStringMatchers::EndsWith( substr ); }

    } // namespace Matchers

    using namespace Matchers;

} // namespace Catch

///////////////////////////////////////////////////////////////////////////////
#define OC_TEST_CASE( name, desc )\
+(NSString*) INTERNAL_CATCH_UNIQUE_NAME( Catch_Name_test ) \
{\
return @ name; \
}\
+(NSString*) INTERNAL_CATCH_UNIQUE_NAME( Catch_Description_test ) \
{ \
return @ desc; \
} \
-(void) INTERNAL_CATCH_UNIQUE_NAME( Catch_TestCase_test )

#endif

#if defined( CATCH_CONFIG_MAIN ) || defined( CATCH_CONFIG_RUNNER )
// #included from: internal/catch_impl.hpp
#define TWOBLUECUBES_CATCH_IMPL_HPP_INCLUDED

// Collect all the implementation files together here
// These are the equivalent of what would usually be cpp files

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif

// #included from: catch_runner.hpp
#define TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED

// #included from: internal/catch_commandline.hpp
#define TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED

namespace Catch {

    class Command {
    public:
        Command(){}

        explicit Command( const std::string& name ) : m_name( name ) {
        }

        Command& operator += ( const std::string& arg ) {
            m_args.push_back( arg );
            return *this;
        }
        Command& operator += ( const Command& other ) {
            std::copy( other.m_args.begin(), other.m_args.end(), std::back_inserter( m_args ) );
            if( m_name.empty() )
                m_name = other.m_name;
            return *this;
        }
        Command operator + ( const Command& other ) {
            Command newCommand( *this );
            newCommand += other;
            return newCommand;
        }

        operator SafeBool::type() const {
            return SafeBool::makeSafe( !m_name.empty() || !m_args.empty() );
        }

        std::string name() const { return m_name; }
        std::string operator[]( std::size_t i ) const { return m_args[i]; }
        std::size_t argsCount() const { return m_args.size(); }

        CATCH_ATTRIBUTE_NORETURN
        void raiseError( const std::string& message ) const {
            std::ostringstream oss;
            if( m_name.empty() )
                oss << "Error while parsing " << m_name << ". " << message << ".";
            else
                oss << "Error while parsing arguments. " << message << ".";

            if( m_args.size() > 0 )
                oss << " Arguments were:";
            for( std::size_t i = 0; i < m_args.size(); ++i )
                oss << " " << m_args[i];
            throw std::domain_error( oss.str() );
        }

    private:

        std::string m_name;
        std::vector<std::string> m_args;
    };

    class CommandParser {
    public:
        CommandParser( int argc, char const * const * argv ) : m_argc( static_cast<std::size_t>( argc ) ), m_argv( argv ) {}

        std::string exeName() const {
            std::string exeName = m_argv[0];
            std::string::size_type pos = exeName.find_last_of( "/\\" );
            if( pos != std::string::npos )
                exeName = exeName.substr( pos+1 );
            return exeName;
        }
        Command find( const std::string& arg1,  const std::string& arg2, const std::string& arg3 ) const {
            return find( arg1 ) + find( arg2 ) + find( arg3 );
        }

        Command find( const std::string& shortArg, const std::string& longArg ) const {
            return find( shortArg ) + find( longArg );
        }
        Command find( const std::string& arg ) const {
            if( arg.empty() )
                return getArgs( "", 1 );
            else
                for( std::size_t i = 1; i < m_argc; ++i  )
                    if( m_argv[i] == arg )
                        return getArgs( m_argv[i], i+1 );
            return Command();
        }
        Command getDefaultArgs() const {
            return getArgs( "", 1 );
        }

    private:
        Command getArgs( const std::string& cmdName, std::size_t from ) const {
            Command command( cmdName );
            for( std::size_t i = from; i < m_argc && m_argv[i][0] != '-'; ++i  )
                command += m_argv[i];
            return command;
        }

        std::size_t m_argc;
        char const * const * m_argv;
    };

    class OptionParser : public SharedImpl<IShared> {
    public:
        OptionParser( int minArgs = 0, int maxArgs = 0 )
        : m_minArgs( minArgs ), m_maxArgs( maxArgs )
        {}

        virtual ~OptionParser() {}

        Command find( const CommandParser& parser ) const {
            Command cmd;
            for( std::vector<std::string>::const_iterator it = m_optionNames.begin();
                it != m_optionNames.end();
                ++it )
                cmd += parser.find( *it );
            return cmd;
        }

        void validateArgs( const Command& args ) const {
            if(  tooFewArgs( args ) || tooManyArgs( args ) ) {
                std::ostringstream oss;
                if( m_maxArgs == -1 )
                    oss <<"Expected at least " << pluralise( static_cast<std::size_t>( m_minArgs ), "argument" );
                else if( m_minArgs == m_maxArgs )
                    oss <<"Expected " << pluralise( static_cast<std::size_t>( m_minArgs ), "argument" );
                else
                    oss <<"Expected between " << m_minArgs << " and " << m_maxArgs << " argument";
                args.raiseError( oss.str() );
            }
        }

        void parseIntoConfig( const CommandParser& parser, ConfigData& config ) {
            if( Command cmd = find( parser ) ) {
                validateArgs( cmd );
                parseIntoConfig( cmd, config );
            }
        }

        virtual void parseIntoConfig( const Command& cmd, ConfigData& config ) = 0;
        virtual std::string argsSynopsis() const = 0;
        virtual std::string optionSummary() const = 0;
        virtual std::string optionDescription() const { return ""; }

        std::string optionNames() const {
            std::string names;
            for(    std::vector<std::string>::const_iterator it = m_optionNames.begin();
                    it != m_optionNames.end();
                    ++it ) {
                if( !it->empty() ) {
                    if( !names.empty() )
                        names += ", ";
                    names += *it;
                }
                else {
                    names = "[" + names;
                }
            }
            if( names[0] == '[' )
                names += "]";
            return names;
        }

    protected:

        bool tooFewArgs( const Command& args ) const {
            return args.argsCount() < static_cast<std::size_t>( m_minArgs );
        }
        bool tooManyArgs( const Command& args ) const {
            return m_maxArgs >= 0 && args.argsCount() > static_cast<std::size_t>( m_maxArgs );
        }
        std::vector<std::string> m_optionNames;
        int m_minArgs;
        int m_maxArgs;
    };

    namespace Options {

        class HelpOptionParser : public OptionParser {
        public:
            HelpOptionParser() {
                m_optionNames.push_back( "-?" );
                m_optionNames.push_back( "-h" );
                m_optionNames.push_back( "--help" );
            }
            virtual std::string argsSynopsis() const {
                return "[<option for help on> ...]";
            }
            virtual std::string optionSummary() const {
                return "Shows this usage summary, or help on a specific option, or options, if supplied";
            }
            virtual std::string optionDescription() const {
                return "";
            }

            virtual void parseIntoConfig( const Command&, ConfigData& ) {
                // Does not affect config
            }
        };

        class TestCaseOptionParser : public OptionParser {
        public:
            TestCaseOptionParser() : OptionParser( 1, -1 ) {
                m_optionNames.push_back( "-t" );
                m_optionNames.push_back( "--test" );
                m_optionNames.push_back( "" ); // default option
            }
            virtual std::string argsSynopsis() const {
                return "<testspec> [<testspec>...]";
            }
            virtual std::string optionSummary() const {
                return "Specifies which test case or cases to run";
            }

            // Lines are split at the nearest prior space char to the 80 char column.
            // Tab chars are removed from the output but their positions are used to align
            // subsequently wrapped lines
            virtual std::string optionDescription() const {
                return
                    "This option allows one ore more test specs to be supplied. Each spec either fully "
                    "specifies a test case or is a pattern containing wildcards to match a set of test "
                    "cases. If this option is not provided then all test cases, except those prefixed "
                    "by './' are run\n"
                    "\n"
                    "Specs must be enclosed in \"quotes\" if they contain spaces. If they do not "
                    "contain spaces the quotes are optional.\n"
                    "\n"
                    "Wildcards consist of the * character at the beginning, end, or both and can substitute for "
                    "any number of any characters (including none)\n"
                    "\n"
                    "If spec is prefixed with exclude: or the ~ character then the pattern matches an exclusion. "
                    "This means that tests matching the pattern are excluded from the set - even if a prior "
                    "inclusion spec included them. Subsequent inclusion specs will take precedence, however. "
                    "Inclusions and exclusions are evaluated in left-to-right order.\n"
                    "\n"
                    "Examples:\n"
                    "\n"
                    "    -t thisTestOnly        \tMatches the test case called, 'thisTestOnly'\n"
                    "    -t \"this test only\"    \tMatches the test case called, 'this test only'\n"
                    "    -t these/*             \tMatches all cases starting with 'these/'\n"
                    "    -t exclude:notThis     \tMatches all tests except, 'notThis'\n"
                    "    -t ~notThis            \tMatches all tests except, 'notThis'\n"
                    "    -t ~*private*          \tMatches all tests except those that contain 'private'\n"
                    "    -t a/* ~a/b/* a/b/c    \tMatches all tests that start with 'a/', except those "
                                                 "that start with 'a/b/', except 'a/b/c', which is included";
            }

            virtual void parseIntoConfig( const Command& cmd, ConfigData& config ) {
                std::string groupName;
                for( std::size_t i = 0; i < cmd.argsCount(); ++i ) {
                    if( i != 0 )
                        groupName += " ";
                    groupName += cmd[i];
                }
                TestCaseFilters filters( groupName );
                for( std::size_t i = 0; i < cmd.argsCount(); ++i ) {
                    if( startsWith( cmd[i], "[" ) || startsWith( cmd[i], "~[" ) )
                        filters.addTags( cmd[i] );
                    else
                        filters.addFilter( TestCaseFilter( cmd[i] ) );
                }
                config.filters.push_back( filters );
            }
        };

        class TagOptionParser : public OptionParser {
        public:
            TagOptionParser() : OptionParser( 1, -1 ) {
                m_optionNames.push_back( "-g" );
                m_optionNames.push_back( "--tag" );
            }
            virtual std::string argsSynopsis() const {
                return "<tagspec> [,<tagspec>...]";
            }
            virtual std::string optionSummary() const {
                return "Matches test cases against tags or tag patterns";
            }

            // Lines are split at the nearest prior space char to the 80 char column.
            // Tab chars are removed from the output but their positions are used to align
            // subsequently wrapped lines
            virtual std::string optionDescription() const {
                return
                "This option allows one or more tags or tag patterns to be specified.\n"
                "Each tag is enclosed in square brackets. A series of tags form an AND expression "
                "wheras a comma seperated sequence forms an OR expression. e.g.:\n\n"
                "    -g [one][two],[three]\n\n"
                "This matches all tests tagged [one] and [two], as well as all tests tagged [three].\n\n"
                "Tags can be negated with the ~ character. This removes matching tests from the set. e.g.:\n\n"
                "    -g [one]~[two]\n\n"
                "matches all tests tagged [one], except those also tagged [two]";
            }

            virtual void parseIntoConfig( const Command& cmd, ConfigData& config ) {
                std::string groupName;
                for( std::size_t i = 0; i < cmd.argsCount(); ++i ) {
                    if( i != 0 )
                        groupName += " ";
                    groupName += cmd[i];
                }
                TestCaseFilters filters( groupName );
                for( std::size_t i = 0; i < cmd.argsCount(); ++i )
                    filters.addTags( cmd[i] );
                config.filters.push_back( filters );
            }
        };

        class ListOptionParser : public OptionParser {
        public:
            ListOptionParser() : OptionParser( 0, 2 ) {
                m_optionNames.push_back( "-l" );
                m_optionNames.push_back( "--list" );
            }
            virtual std::string argsSynopsis() const {
//                return "[all | tests | reporters | tags [xml]]";
                return "[all | tests | reporters | tags]";
            }
            virtual std::string optionSummary() const {
                return "Lists available tests or reporters";
            }

            virtual std::string optionDescription() const {
                return
                    "With no arguments this option will list all registered tests - one per line.\n"
//                    "Supplying the xml argument formats the list as an xml document (which may be useful for "
//                    "consumption by other tools).\n"
                    "Supplying the tests or reporters lists tests or reporters respectively - with descriptions.\n"
                    "\n"
                    "Examples:\n"
                    "\n"
                    "    -l\n"
                    "    -l tests\n"
                    "    -l tags\n"
                    "    -l reporters xml\n"
                    ;//"    -l xml";
            }

            virtual void parseIntoConfig( const Command& cmd, ConfigData& config ) {
                config.listSpec = List::Tests;
                if( cmd.argsCount() >= 1 ) {
                    if( cmd[0] == "all" )
                        config.listSpec = List::All;
                    else if( cmd[0] == "tests" )
                        config.listSpec = List::Tests;
                    else if( cmd[0] == "tags" )
                        config.listSpec = List::Tags;
                    else if( cmd[0] == "reporters" )
                        config.listSpec = List::Reports;
                    else
                        cmd.raiseError( "Expected tests, reporters or tags" );
                }
                if( cmd.argsCount() >= 2 ) {
                    if( cmd[1] == "xml" )
                        config.listSpec = static_cast<List::What>( config.listSpec | List::AsXml );
                    else if( cmd[1] == "text" )
                        config.listSpec = static_cast<List::What>( config.listSpec | List::AsText );
                    else
                        cmd.raiseError( "Expected xml or text" );
                }
            }
        };

        class ReporterOptionParser : public OptionParser {
        public:
            ReporterOptionParser() : OptionParser( 1, 1 ) {
                m_optionNames.push_back( "-r" );
                m_optionNames.push_back( "--reporter" );
            }
            virtual std::string argsSynopsis() const {
                return "<reporter name>";
            }
            virtual std::string optionSummary() const {
                return "Specifies type of reporter";
            }

            virtual std::string optionDescription() const {
                return
                    "A reporter is an object that formats and structures the output of running "
                    "tests, and potentially summarises the results. By default the console reporter "
                    "is used which writes IDE friendly results. CATCH comes bundled with some "
                    "alternative reporters, but more can be added in client code.\n"
                    "\n"
                    "The bundled reporters are:\n"
                    "    -r console\n"
                    "    -r xml\n"
                    "    -r junit\n"
                    "\n"
                    "The JUnit reporter is an xml format that follows the structure of the JUnit "
                    "XML Report ANT task, as consumed by a number of third-party tools, "
                    "including Continuous Integration servers such as Jenkins.\n"
                    "If not otherwise needed, the standard XML reporter is preferred as this is "
                    "a streaming reporter, whereas the Junit reporter needs to hold all its "
                    "results until the end so it can write the overall results into attributes "
                    "of the root node.";
            }

            virtual void parseIntoConfig( const Command& cmd, ConfigData& config ) {
                config.reporter = cmd[0];
            }
        };

        class OutputOptionParser : public OptionParser {
        public:
            OutputOptionParser() : OptionParser( 1, 1 ) {
                m_optionNames.push_back( "-o" );
                m_optionNames.push_back( "--out" );
            }
            virtual std::string argsSynopsis() const {
                return "<file name>|<%stream name>";
            }
            virtual std::string optionSummary() const {
                return "Sends output to a file or stream";
            }
            virtual std::string optionDescription() const {
                return
                    "Use this option to send all output to a file or a stream. By default output is "
                    "sent to stdout (note that uses of stdout and stderr from within test cases are "
                    "redirected and included in the report - so even stderr will effectively end up "
                    "on stdout). If the name begins with % it is interpreted as a stream. "
                    "Otherwise it is treated as a filename.\n"
                    "\n"
                    "Examples are:\n"
                    "\n"
                    "    -o filename.txt\n"
                    "    -o \"long filename.txt\"\n"
                    "    -o %stdout\n"
                    "    -o %stderr\n"
                    "    -o %debug    \t(The IDE's debug output window - currently only Windows' "
                                        "OutputDebugString is supported).";
            }
            virtual void parseIntoConfig( const Command& cmd, ConfigData& config ) {
                if( cmd[0][0] == '%' )
                    config.stream = cmd[0].substr( 1 );
                else
                    config.outputFilename = cmd[0];
            }
        };

        class SuccessOptionParser : public OptionParser {
        public:
            SuccessOptionParser() {
                m_optionNames.push_back( "-s" );
                m_optionNames.push_back( "--success" );
            }
            virtual std::string argsSynopsis() const {
                return "";
            }
            virtual std::string optionSummary() const {
                return "Shows results for successful tests";
            }
            virtual std::string optionDescription() const {
                return
                    "Usually you only want to see reporting for failed tests. Sometimes it's useful "
                    "to see all the output (especially when you don't trust that that test you just "
                    "added worked first time!). To see successful, as well as failing, test results "
                    "just pass this option.";
            }
            virtual void parseIntoConfig( const Command&, ConfigData& config ) {
                config.includeWhichResults = Include::SuccessfulResults;
            }
        };

        class DebugBreakOptionParser : public OptionParser {
        public:
            DebugBreakOptionParser() {
                m_optionNames.push_back( "-b" );
                m_optionNames.push_back( "--break" );
            }
            virtual std::string argsSynopsis() const {
                return "";
            }
            virtual std::string optionSummary() const {
                return "Breaks into the debugger on failure";
            }
            virtual std::string optionDescription() const {
                return
                    "In some IDEs (currently XCode and Visual Studio) it is possible for CATCH to "
                    "break into the debugger on a test failure. This can be very helpful during "
                    "debug sessions - especially when there is more than one path through a "
                    "particular test. In addition to the command line option, ensure you have "
                    "built your code with the DEBUG preprocessor symbol";
            }

            virtual void parseIntoConfig( const Command&, ConfigData& config ) {
                config.shouldDebugBreak = true;
            }
        };

        class NameOptionParser : public OptionParser {
        public:
            NameOptionParser() : OptionParser( 1, 1 ) {
                m_optionNames.push_back( "-n" );
                m_optionNames.push_back( "--name" );
            }
            virtual std::string argsSynopsis() const {
                return "<name>";
            }
            virtual std::string optionSummary() const {
                return "Names a test run";
            }
            virtual std::string optionDescription() const {
                return
                    "If a name is supplied it will be used by the reporter to provide an overall "
                    "name for the test run. This can be useful if you are sending to a file, for "
                    "example, and need to distinguish different test runs - either from different "
                    "Catch executables or runs of the same executable with different options.\n"
                    "\n"
                    "Examples:\n"
                    "\n"
                    "    -n testRun\n"
                    "    -n \"tests of the widget component\"";
            }

            virtual void parseIntoConfig( const Command& cmd, ConfigData& config ) {
                config.name = cmd[0];
            }
        };

        class AbortOptionParser : public OptionParser {
        public:
            AbortOptionParser() : OptionParser( 0, 1 ) {
                m_optionNames.push_back( "-a" );
                m_optionNames.push_back( "--abort" );
            }
            virtual std::string argsSynopsis() const {
                return "[#]";
            }
            virtual std::string optionSummary() const {
                return "Aborts after a certain number of failures";
            }
            virtual std::string optionDescription() const {
                return
                    "If a REQUIRE assertion fails the test case aborts, but subsequent test cases "
                    "are still run. If a CHECK assertion fails even the current test case is not "
                    "aborted.\n"
                    "\n"
                    "Sometimes this results in a flood of failure messages and you'd rather just "
                    "see the first few. Specifying -a or --abort on its own will abort the whole "
                    "test run on the first failed assertion of any kind. Following it with a "
                    "number causes it to abort after that number of assertion failures.";
            }

            virtual void parseIntoConfig( const Command& cmd, ConfigData& config ) {
                int threshold = 1;
                if( cmd.argsCount() == 1 ) {
                    std::stringstream ss;
                    ss << cmd[0];
                    ss >> threshold;
                    if( ss.fail() || threshold <= 0 )
                        cmd.raiseError( "threshold must be a number greater than zero" );
                }
                config.cutoff = threshold;
            }
        };

        class NoThrowOptionParser : public OptionParser {
        public:
            NoThrowOptionParser() {
                m_optionNames.push_back( "-nt" );
                m_optionNames.push_back( "--nothrow" );
            }
            virtual std::string argsSynopsis() const {
                return "";
            }
            virtual std::string optionSummary() const {
                return "Elides assertions expected to throw";
            }
            virtual std::string optionDescription() const {
                return
                    "Skips all assertions that test that an exception is thrown, "
                    "e.g. REQUIRE_THROWS.\n"
                    "\n"
                    "These can be a nuisance in certain debugging environments that may break when "
                    "exceptions are thrown (while this is usually optional for handled exceptions, "
                    "it can be useful to have enabled if you are trying to track down something "
                    "unexpected).\n"
                    "\n"
                    "When running with this option the throw checking assertions are skipped so "
                    "as not to contribute additional noise.";
            }

            virtual void parseIntoConfig( const Command&, ConfigData& config ) {
                config.allowThrows = false;
            }
        };

        class WarningsOptionParser : public OptionParser {
        public:
            WarningsOptionParser() : OptionParser( 1, -1 ) {
                m_optionNames.push_back( "-w" );
                m_optionNames.push_back( "--warnings" );
            }
            virtual std::string argsSynopsis() const {
                return "<warning>";
            }
            virtual std::string optionSummary() const {
                return "Enable warnings";
            }
            virtual std::string optionDescription() const {
                return
                    "Enables the named warnings. If the warnings are violated the test case is "
                    "failed.\n"
                    "\n"
                    "At present only one warning has been provided: NoAssertions. If this warning "
                    "is enabled then any test case that completes without an assertions (CHECK, "
                    "REQUIRE etc) being encountered violates the warning.\n"
                    "\n"
                    "e.g.:\n"
                    "\n"
                    "    -w NoAssertions";
            }

            virtual void parseIntoConfig( const Command& cmd, ConfigData& config ) {
                for( std::size_t i = 0; i < cmd.argsCount(); ++i ) {
                    if( cmd[i] == "NoAssertions" )
                        config.warnings = (ConfigData::WarnAbout::What)( config.warnings | ConfigData::WarnAbout::NoAssertions );
                    else
                        cmd.raiseError( "Unrecognised warning: " + cmd[i] );
                }
            }
        };
    }

    class AllOptions
    {
    public:
        typedef std::vector<Ptr<OptionParser> > Parsers;
        typedef Parsers::const_iterator const_iterator;
        typedef Parsers::const_iterator iterator;

        AllOptions() {
            add<Options::TestCaseOptionParser>();   // Keep this one first

            add<Options::TagOptionParser>();
            add<Options::ListOptionParser>();
            add<Options::ReporterOptionParser>();
            add<Options::OutputOptionParser>();
            add<Options::SuccessOptionParser>();
            add<Options::DebugBreakOptionParser>();
            add<Options::NameOptionParser>();
            add<Options::AbortOptionParser>();
            add<Options::NoThrowOptionParser>();
            add<Options::WarningsOptionParser>();

            add<Options::HelpOptionParser>();       // Keep this one last
        }

        void parseIntoConfig( const CommandParser& parser, ConfigData& config ) {
            config.name = parser.exeName();
            if( endsWith( config.name, ".exe" ) )
               config.name = config.name.substr( 0, config.name.size()-4 );
            for( const_iterator it = m_parsers.begin(); it != m_parsers.end(); ++it )
                (*it)->parseIntoConfig( parser, config );
        }

        const_iterator begin() const {
            return m_parsers.begin();
        }
        const_iterator end() const {
            return m_parsers.end();
        }
    private:

        template<typename T>
        void add() {
            m_parsers.push_back( new T() );
        }
        Parsers m_parsers;

    };

} // end namespace Catch

// #included from: internal/catch_list.hpp
#define TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED

// #included from: catch_line_wrap.h
#define TWOBLUECUBES_CATCH_LINE_WRAP_H_INCLUDED

#include <string>
#include <vector>

namespace Catch {

    class LineWrapper {
    public:
        LineWrapper();

        LineWrapper& setIndent( std::size_t _indent );
        LineWrapper& setInitialIndent( std::size_t _initalIndent );
        LineWrapper& setRight( std::size_t _right );
        LineWrapper& setTabChar( char _tabChar );

        LineWrapper& wrap( std::string const& _str );

        std::string toString() const;

        typedef std::vector<std::string>::const_iterator const_iterator;

        const_iterator begin() const { return lines.begin(); }
        const_iterator end() const { return lines.end(); }
        std::string const& last() const { return lines.back(); }
        std::size_t size() const { return lines.size(); }
        std::string const& operator[]( std::size_t _index ) const { return lines[_index]; }

        friend std::ostream& operator << ( std::ostream& _stream, LineWrapper const& _lineWrapper );

    private:
        void wrapInternal( std::string const& _str );
        void addLine( const std::string& _line );
        bool isWrapPoint( char c );
        std::size_t getCurrentIndent() const;

        std::size_t right;
        std::size_t nextTab;
        std::size_t tab;
        std::size_t indent;
        std::size_t initialIndent;
        std::string wrappableChars;
        char tabChar;
        int recursionCount;
        std::vector<std::string> lines;
    };

} // end namespace Catch

// #included from: catch_console_colour.hpp
#define TWOBLUECUBES_CATCH_CONSOLE_COLOUR_HPP_INCLUDED

namespace Catch {

    namespace Detail {
        struct IColourImpl;
    }

    struct Colour {
        enum Code {
            None = 0,

            White,
            Red,
            Green,
            Blue,
            Cyan,
            Yellow,
            Grey,

            Bright = 0x10,

            BrightRed = Bright | Red,
            BrightGreen = Bright | Green,
            LightGrey = Bright | Grey,
            BrightWhite = Bright | White,

            // By intention
            FileName = LightGrey,
            ResultError = BrightRed,
            ResultSuccess = BrightGreen,

            Error = BrightRed,
            Success = Green,

            OriginalExpression = Cyan,
            ReconstructedExpression = Yellow,

            SecondaryText = LightGrey,
            Headers = White
        };

        // Use constructed object for RAII guard
        Colour( Code _colourCode );
        ~Colour();

        // Use static method for one-shot changes
        static void use( Code _colourCode );

    private:
        static Detail::IColourImpl* impl;
    };

} // end namespace Catch

#include <limits>
#include <algorithm>

namespace Catch {
    inline bool matchesFilters( const std::vector<TestCaseFilters>& filters, const TestCase& testCase ) {
        std::vector<TestCaseFilters>::const_iterator it = filters.begin();
        std::vector<TestCaseFilters>::const_iterator itEnd = filters.end();
        for(; it != itEnd; ++it )
            if( !it->shouldInclude( testCase ) )
                return false;
        return true;
    }

    inline void listTests( const ConfigData& config ) {
        if( config.filters.empty() )
            std::cout << "All available test cases:\n";
        else
            std::cout << "Matching test cases:\n";
        std::vector<TestCase> const& allTests = getRegistryHub().getTestCaseRegistry().getAllTests();
        std::vector<TestCase>::const_iterator it = allTests.begin(), itEnd = allTests.end();

        // First pass - get max tags
        std::size_t maxTagLen = 0;
        std::size_t maxNameLen = 0;
        for(; it != itEnd; ++it ) {
            if( matchesFilters( config.filters, *it ) ) {
                maxTagLen = (std::max)( it->getTestCaseInfo().tagsAsString.size(), maxTagLen );
                maxNameLen = (std::max)( it->getTestCaseInfo().name.size(), maxNameLen );
            }
        }

        // Try to fit everything in. If not shrink tag column first, down to 30
        // then shrink name column until it all fits (strings will be wrapped within column)
        while( maxTagLen + maxNameLen > CATCH_CONFIG_CONSOLE_WIDTH-5 ) {
            if( maxTagLen > 30 )
                --maxTagLen;
            else
                --maxNameLen;
        }

        std::size_t matchedTests = 0;
        for( it = allTests.begin(); it != itEnd; ++it ) {
            if( matchesFilters( config.filters, *it ) ) {
                matchedTests++;
                // !TBD: consider listAs()
                LineWrapper nameWrapper;
                nameWrapper.setRight( maxNameLen ).setIndent( 2 ).wrap( it->getTestCaseInfo().name );

                LineWrapper tagsWrapper;
                tagsWrapper.setRight( maxTagLen ).wrap( it->getTestCaseInfo().tagsAsString );

                for( std::size_t i = 0; i < std::max( nameWrapper.size(), tagsWrapper.size() ); ++i ) {
                    Colour::Code colour = Colour::None;
                    if( it->getTestCaseInfo().isHidden )
                        colour = Colour::SecondaryText;
                    std::string nameCol;
                    if( i < nameWrapper.size() ) {
                        nameCol = nameWrapper[i];
                    }
                    else {
                        nameCol = "    ...";
                        colour = Colour::SecondaryText;
                    }

                    {
                        Colour colourGuard( colour );
                        std::cout << nameCol;
                    }
                    if( i < tagsWrapper.size() && !tagsWrapper[i].empty() ) {
                        if( i == 0 ) {
                            Colour colourGuard( Colour::SecondaryText );
                            std::cout << "  " << std::string( maxNameLen - nameCol.size(), '.' ) << "  ";
                        }
                        else {
                            std::cout << std::string( maxNameLen - nameCol.size(), ' ' ) << "    ";
                        }
                        std::cout << tagsWrapper[i];
                    }
                    std::cout << "\n";
                }
            }
        }
        if( config.filters.empty() )
            std::cout << pluralise( matchedTests, "test case" ) << std::endl;
        else
            std::cout << pluralise( matchedTests, "matching test case" ) << std::endl;
    }

    inline void listTags( const ConfigData& config ) {
        if( config.filters.empty() )
            std::cout << "All available tags:\n";
        else
            std::cout << "Matching tags:\n";
        std::vector<TestCase> const& allTests = getRegistryHub().getTestCaseRegistry().getAllTests();
        std::vector<TestCase>::const_iterator it = allTests.begin(), itEnd = allTests.end();

        std::map<std::string, int> tagCounts;

        std::size_t maxTagLen = 0;

        for(; it != itEnd; ++it ) {
            if( matchesFilters( config.filters, *it ) ) {
                for( std::set<std::string>::const_iterator  tagIt = it->getTestCaseInfo().tags.begin(),
                                                            tagItEnd = it->getTestCaseInfo().tags.end();
                        tagIt != tagItEnd;
                        ++tagIt ) {
                    std::string tagName = *tagIt;
                    maxTagLen = (std::max)( maxTagLen, tagName.size() );
                    std::map<std::string, int>::iterator countIt = tagCounts.find( tagName );
                    if( countIt == tagCounts.end() )
                        tagCounts.insert( std::make_pair( tagName, 1 ) );
                    else
                        countIt->second++;
                }
            }
        }
        maxTagLen +=4;
        if( maxTagLen > CATCH_CONFIG_CONSOLE_WIDTH-10 )
            maxTagLen = CATCH_CONFIG_CONSOLE_WIDTH-10;

        for( std::map<std::string, int>::const_iterator countIt = tagCounts.begin(), countItEnd = tagCounts.end();
                countIt != countItEnd;
                ++countIt ) {
            LineWrapper wrapper;
            wrapper.setIndent(2).setRight( maxTagLen ).wrap( "[" + countIt->first + "]" );

            std::cout << wrapper;
            std::size_t dots = 2;
            if( maxTagLen > wrapper.last().size() )
                dots += maxTagLen - wrapper.last().size();
            {
                Colour colourGuard( Colour::SecondaryText );
                std::cout << std::string( dots, '.' );
            }
            std::cout   << countIt->second
                        << "\n";
        }
        std::cout << pluralise( tagCounts.size(), "tag" ) << std::endl;
    }

    inline void listReporters( const ConfigData& /*config*/ ) {
        std::cout << "Available reports:\n";
        IReporterRegistry::FactoryMap const& factories = getRegistryHub().getReporterRegistry().getFactories();
        IReporterRegistry::FactoryMap::const_iterator it = factories.begin(), itEnd = factories.end();
        for(; it != itEnd; ++it ) {
            // !TBD: consider listAs()
            std::cout << "\t" << it->first << "\n\t\t'" << it->second->getDescription() << "'\n";
        }
        std::cout << std::endl;
    }

    inline void list( const ConfigData& config ) {
        if( config.listSpec & List::Tests )
            listTests( config );
        if( config.listSpec & List::Tags )
            listTags( config );
        if( config.listSpec & List::Reports )
            listReporters( config );
        if( ( config.listSpec & List::All ) == 0 )
            throw std::logic_error( "Unknown list type" );
    }

} // end namespace Catch

// #included from: internal/catch_runner_impl.hpp
#define TWOBLUECUBES_CATCH_RUNNER_IMPL_HPP_INCLUDED

// #included from: catch_running_test.hpp
#define TWOBLUECUBES_CATCH_RUNNING_TEST_HPP_INCLUDED

// #included from: catch_section_info.hpp
#define TWOBLUECUBES_CATCH_SECTION_INFO_HPP_INCLUDED

#include <map>
#include <string>

namespace Catch {

    class RunningSection {
    public:

        typedef std::vector<RunningSection*> SubSections;

        enum State {
            Root,
            Unknown,
            Branch,
            TestedBranch,
            TestedLeaf
        };

        RunningSection( RunningSection* parent, const std::string& name )
        :   m_state( Unknown ),
            m_parent( parent ),
            m_name( name )
        {}

        RunningSection( const std::string& name )
        :   m_state( Root ),
            m_parent( NULL ),
            m_name( name )
        {}

        ~RunningSection() {
            deleteAll( m_subSections );
        }

        std::string getName() const {
            return m_name;
        }

        bool shouldRun() const {
            return m_state < TestedBranch;
        }

        bool isBranch() const {
            return m_state == Branch;
        }

        const RunningSection* getParent() const {
            return m_parent;
        }

        bool hasUntestedSections() const {
            if( m_state == Unknown )
                return true;
            for(    SubSections::const_iterator it = m_subSections.begin();
                    it != m_subSections.end();
                    ++it)
                if( (*it)->hasUntestedSections() )
                    return true;
            return false;
        }

        // Mutable methods:

        RunningSection* getParent() {
            return m_parent;
        }

        RunningSection* findOrAddSubSection( const std::string& name, bool& changed ) {
            for(    SubSections::const_iterator it = m_subSections.begin();
                    it != m_subSections.end();
                    ++it)
                if( (*it)->getName() == name )
                    return *it;
            RunningSection* subSection = new RunningSection( this, name );
            m_subSections.push_back( subSection );
            m_state = Branch;
            changed = true;
            return subSection;
        }

        bool ran() {
            if( m_state >= Branch )
                return false;
            m_state = TestedLeaf;
            return true;
        }

        void ranToCompletion() {
            if( m_state == Branch && !hasUntestedSections() )
                m_state = TestedBranch;
        }

    private:
        State m_state;
        RunningSection* m_parent;
        std::string m_name;
        SubSections m_subSections;
    };
}

namespace Catch {

    class RunningTest {

        enum RunStatus {
            NothingRun,
            EncounteredASection,
            RanAtLeastOneSection,
            RanToCompletionWithSections,
            RanToCompletionWithNoSections
        };

    public:
        explicit RunningTest( const TestCase& info )
        :   m_info( info ),
            m_runStatus( RanAtLeastOneSection ),
            m_rootSection( info.getTestCaseInfo().name ),
            m_currentSection( &m_rootSection ),
            m_changed( false )
        {}

        bool wasSectionSeen() const {
            return  m_runStatus == RanAtLeastOneSection ||
                    m_runStatus == RanToCompletionWithSections;
        }

        bool isBranchSection() const {
            return  m_currentSection &&
                    m_currentSection->isBranch();
        }

        bool hasSections() const {
            return  m_runStatus == RanAtLeastOneSection ||
                    m_runStatus == RanToCompletionWithSections ||
                    m_runStatus == EncounteredASection;
        }

        void reset() {
            m_runStatus = NothingRun;
            m_changed = false;
            m_lastSectionToRun = NULL;
        }

        void ranToCompletion() {
            if( m_runStatus != RanAtLeastOneSection && m_runStatus != EncounteredASection )
                m_runStatus = RanToCompletionWithNoSections;
            m_runStatus = RanToCompletionWithSections;
            if( m_lastSectionToRun ) {
                m_lastSectionToRun->ranToCompletion();
                m_changed = true;
            }
        }

        bool addSection( const std::string& name ) {
            if( m_runStatus == NothingRun )
                m_runStatus = EncounteredASection;

            RunningSection* thisSection = m_currentSection->findOrAddSubSection( name, m_changed );

            if( !wasSectionSeen() && thisSection->shouldRun() ) {
                m_currentSection = thisSection;
                m_lastSectionToRun = NULL;
                return true;
            }
            return false;
        }

        void endSection( const std::string&, bool stealth ) {
            if( m_currentSection->ran() ) {
                if( !stealth )
                    m_runStatus = RanAtLeastOneSection;
                m_changed = true;
            }
            else if( m_runStatus == EncounteredASection ) {
                if( !stealth )
                    m_runStatus = RanAtLeastOneSection;
                m_lastSectionToRun = m_currentSection;
            }
            m_currentSection = m_currentSection->getParent();
        }

        const TestCase& getTestCase() const {
            return m_info;
        }

        bool hasUntestedSections() const {
            return  m_runStatus == RanAtLeastOneSection ||
                    ( m_rootSection.hasUntestedSections() && m_changed );
        }

    private:
		RunningTest( RunningTest const& );
		void operator=( RunningTest const& );

        const TestCase& m_info;
        RunStatus m_runStatus;
        RunningSection m_rootSection;
        RunningSection* m_currentSection;
        RunningSection* m_lastSectionToRun;
        bool m_changed;
    };
}

#include <set>
#include <string>

namespace Catch {

    class StreamRedirect {

    public:
        StreamRedirect( std::ostream& stream, std::string& targetString )
        :   m_stream( stream ),
            m_prevBuf( stream.rdbuf() ),
            m_targetString( targetString )
        {
            stream.rdbuf( m_oss.rdbuf() );
        }

        ~StreamRedirect() {
            m_targetString += m_oss.str();
            m_stream.rdbuf( m_prevBuf );
        }

    private:
        std::ostream& m_stream;
        std::streambuf* m_prevBuf;
        std::ostringstream m_oss;
        std::string& m_targetString;
    };

    ///////////////////////////////////////////////////////////////////////////

    class Runner : public IResultCapture, public IRunner {

        Runner( const Runner& );
        void operator =( const Runner& );

    public:

        explicit Runner( const Config& config, const Ptr<IStreamingReporter>& reporter )
        :   m_runInfo( config.data().name ),
            m_context( getCurrentMutableContext() ),
            m_runningTest( NULL ),
            m_config( config ),
            m_reporter( reporter ),
            m_prevRunner( &m_context.getRunner() ),
            m_prevResultCapture( &m_context.getResultCapture() ),
            m_prevConfig( m_context.getConfig() )
        {
            m_context.setRunner( this );
            m_context.setConfig( &m_config );
            m_context.setResultCapture( this );
            m_reporter->testRunStarting( m_runInfo );
        }

        virtual ~Runner() {
            m_reporter->testRunEnded( TestRunStats( m_runInfo, m_totals, aborting() ) );
            m_context.setRunner( m_prevRunner );
            m_context.setConfig( NULL );
            m_context.setResultCapture( m_prevResultCapture );
            m_context.setConfig( m_prevConfig );
        }

        void testGroupStarting( std::string const& testSpec, std::size_t groupIndex, std::size_t groupsCount ) {
            m_reporter->testGroupStarting( GroupInfo( testSpec, groupIndex, groupsCount ) );
        }
        void testGroupEnded( std::string const& testSpec, Totals const& totals, std::size_t groupIndex, std::size_t groupsCount ) {
            m_reporter->testGroupEnded( TestGroupStats( GroupInfo( testSpec, groupIndex, groupsCount ), totals, aborting() ) );
        }

        Totals runMatching( const std::string& testSpec, std::size_t groupIndex, std::size_t groupsCount ) {

            std::vector<TestCase> matchingTests = getRegistryHub().getTestCaseRegistry().getMatchingTestCases( testSpec );

            Totals totals;

            testGroupStarting( testSpec, groupIndex, groupsCount );

            std::vector<TestCase>::const_iterator it = matchingTests.begin();
            std::vector<TestCase>::const_iterator itEnd = matchingTests.end();
            for(; it != itEnd; ++it )
                totals += runTest( *it );

            testGroupEnded( testSpec, totals, groupIndex, groupsCount );
            return totals;
        }

        Totals runTest( const TestCase& testCase ) {
            Totals prevTotals = m_totals;

            std::string redirectedCout;
            std::string redirectedCerr;

            TestCaseInfo testInfo = testCase.getTestCaseInfo();

            m_reporter->testCaseStarting( testInfo );

            m_runningTest = new RunningTest( testCase );

            do {
                do {
                    runCurrentTest( redirectedCout, redirectedCerr );
                }
                while( m_runningTest->hasUntestedSections() && !aborting() );
            }
            while( getCurrentContext().advanceGeneratorsForCurrentTest() && !aborting() );

            Totals deltaTotals = m_totals.delta( prevTotals );
            bool missingAssertions = false;
            if( deltaTotals.assertions.total() == 0  &&
               ( m_config.data().warnings & ConfigData::WarnAbout::NoAssertions ) ) {
                m_totals.assertions.failed++;
                deltaTotals = m_totals.delta( prevTotals );
                missingAssertions = true;
            }

            m_totals.testCases += deltaTotals.testCases;

            m_reporter->testCaseEnded( TestCaseStats(   testInfo,
                                                        deltaTotals,
                                                        redirectedCout,
                                                        redirectedCerr,
                                                        missingAssertions,
                                                        aborting() ) );

            delete m_runningTest;
            m_runningTest = NULL;

            return deltaTotals;
        }

        const Config& config() const {
            return m_config;
        }

    private: // IResultCapture

        virtual void acceptMessage( const MessageBuilder& messageBuilder ) {
            m_messages.push_back( messageBuilder.build() );
        }

        virtual ResultAction::Value acceptExpression( const ExpressionResultBuilder& assertionResult, const AssertionInfo& assertionInfo ) {
            m_lastAssertionInfo = assertionInfo;
            return actOnCurrentResult( assertionResult.buildResult( assertionInfo ) );
        }

        virtual void assertionEnded( const AssertionResult& result ) {
            if( result.getResultType() == ResultWas::Ok ) {
                m_totals.assertions.passed++;
            }
            else if( !result.isOk() ) {
                m_totals.assertions.failed++;
            }

            m_reporter->assertionEnded( AssertionStats( result, m_messages, m_totals ) );

            // Reset working state
            m_lastAssertionInfo = AssertionInfo( "", m_lastAssertionInfo.lineInfo, "{Unknown expression after the reported line}" , m_lastAssertionInfo.resultDisposition );
            m_messages.clear();
        }

        virtual bool sectionStarted (
            SectionInfo const& sectionInfo,
            Counts& assertions
        )
        {
            std::ostringstream oss;
            oss << sectionInfo.name << "@" << sectionInfo.lineInfo;

            if( !m_runningTest->addSection( oss.str() ) )
                return false;

            m_lastAssertionInfo.lineInfo = sectionInfo.lineInfo;

            m_reporter->sectionStarting( sectionInfo );

            assertions = m_totals.assertions;

            return true;
        }

        virtual void sectionEnded( SectionInfo const& info, Counts const& prevAssertions ) {
            if( std::uncaught_exception() ) {
                m_unfinishedSections.push_back( UnfinishedSections( info, prevAssertions ) );
                return;
            }

            Counts assertions = m_totals.assertions - prevAssertions;
            bool missingAssertions = false;
            if( assertions.total() == 0 &&
               ( m_config.data().warnings & ConfigData::WarnAbout::NoAssertions ) &&
               !m_runningTest->isBranchSection() ) {
                m_totals.assertions.failed++;
                assertions.failed++;
                missingAssertions = true;

            }
            m_runningTest->endSection( info.name, false );

            m_reporter->sectionEnded( SectionStats( info, assertions, missingAssertions ) );
            m_messages.clear();
        }

        virtual void pushScopedMessage( ScopedMessageBuilder const& _builder ) {
            m_messages.push_back( _builder.build() );
        }

        virtual void popScopedMessage( ScopedMessageBuilder const& _builder ) {
            m_messages.erase( std::remove( m_messages.begin(), m_messages.end(), _builder ), m_messages.end() );
        }

        virtual bool shouldDebugBreak() const {
            return m_config.shouldDebugBreak();
        }

        virtual std::string getCurrentTestName() const {
            return m_runningTest
                ? m_runningTest->getTestCase().getTestCaseInfo().name
                : "";
        }

        virtual const AssertionResult* getLastResult() const {
            return &m_lastResult;
        }

    public:
        // !TBD We need to do this another way!
        bool aborting() const {
            return m_totals.assertions.failed == static_cast<std::size_t>( m_config.getCutoff() );
        }

    private:

        ResultAction::Value actOnCurrentResult( const AssertionResult& result ) {
            m_lastResult = result;
            assertionEnded( m_lastResult );

            ResultAction::Value action = ResultAction::None;

            if( !m_lastResult.isOk() ) {
                action = ResultAction::Failed;
                if( shouldDebugBreak() )
                    action = (ResultAction::Value)( action | ResultAction::Debug );
                if( aborting() )
                    action = (ResultAction::Value)( action | ResultAction::Abort );
            }
            return action;
        }

        void runCurrentTest( std::string& redirectedCout, std::string& redirectedCerr ) {
            try {
                m_lastAssertionInfo = AssertionInfo( "TEST_CASE", m_runningTest->getTestCase().getTestCaseInfo().lineInfo, "", ResultDisposition::Normal );
                m_runningTest->reset();

                if( m_reporter->getPreferences().shouldRedirectStdOut ) {
                    StreamRedirect coutRedir( std::cout, redirectedCout );
                    StreamRedirect cerrRedir( std::cerr, redirectedCerr );
                    m_runningTest->getTestCase().invoke();
                }
                else {
                    m_runningTest->getTestCase().invoke();
                }
                m_runningTest->ranToCompletion();
            }
            catch( TestFailureException& ) {
                // This just means the test was aborted due to failure
            }
            catch(...) {
                ExpressionResultBuilder exResult( ResultWas::ThrewException );
                exResult << translateActiveException();
                actOnCurrentResult( exResult.buildResult( m_lastAssertionInfo )  );
            }
            for( std::vector<UnfinishedSections>::const_iterator it = m_unfinishedSections.begin(),
                        itEnd = m_unfinishedSections.end();
                    it != itEnd;
                    ++it )
                sectionEnded( it->info, it->prevAssertions );
            m_unfinishedSections.clear();
            m_messages.clear();
        }

    private:
        struct UnfinishedSections {
            UnfinishedSections( SectionInfo const& _info, Counts const& _prevAssertions )
            : info( _info ), prevAssertions( _prevAssertions )
            {}

            SectionInfo info;
            Counts prevAssertions;
        };

        TestRunInfo m_runInfo;
        IMutableContext& m_context;
        RunningTest* m_runningTest;
        AssertionResult m_lastResult;

        const Config& m_config;
        Totals m_totals;
        Ptr<IStreamingReporter> m_reporter;
        std::vector<MessageInfo> m_messages;
        IRunner* m_prevRunner;
        IResultCapture* m_prevResultCapture;
        const IConfig* m_prevConfig;
        AssertionInfo m_lastAssertionInfo;
        std::vector<UnfinishedSections> m_unfinishedSections;
    };

} // end namespace Catch

// #included from: internal/catch_version.h
#define TWOBLUECUBES_CATCH_VERSION_H_INCLUDED

namespace Catch {

    // Versioning information
    struct Version {
        Version(    unsigned int _majorVersion,
                    unsigned int _minorVersion,
                    unsigned int _buildNumber,
                    std::string const& _branchName )
        :   majorVersion( _majorVersion ),
            minorVersion( _minorVersion ),
            buildNumber( _buildNumber ),
            branchName( _branchName )
        {}

        const unsigned int majorVersion;
        const unsigned int minorVersion;
        const unsigned int buildNumber;
        const std::string branchName;

	private:
		void operator=( Version const& );
    };

    extern Version libraryVersion;
}

#include <fstream>
#include <stdlib.h>
#include <limits>

namespace Catch {

    class Runner2 { // This will become Runner when Runner becomes Context

    public:
        Runner2( Config& configWrapper )
        :   m_configWrapper( configWrapper ),
            m_config( configWrapper.data() )
        {
            openStream();
            makeReporter();
        }

        Totals runTests() {

            std::vector<TestCaseFilters> filterGroups = m_config.filters;
            if( filterGroups.empty() ) {
                TestCaseFilters filterGroup( "" );
                filterGroups.push_back( filterGroup );
            }

            Runner context( m_configWrapper, m_reporter ); // This Runner will be renamed Context
            Totals totals;

            for( std::size_t i=0; i < filterGroups.size() && !context.aborting(); ++i ) {
                context.testGroupStarting( filterGroups[i].getName(), i, filterGroups.size() );
                totals += runTestsForGroup( context, filterGroups[i] );
                context.testGroupEnded( filterGroups[i].getName(), totals, i, filterGroups.size() );
            }
            return totals;
        }

        Totals runTestsForGroup( Runner& context, const TestCaseFilters& filterGroup ) {
            Totals totals;
            std::vector<TestCase>::const_iterator it = getRegistryHub().getTestCaseRegistry().getAllTests().begin();
            std::vector<TestCase>::const_iterator itEnd = getRegistryHub().getTestCaseRegistry().getAllTests().end();
            int testsRunForGroup = 0;
            for(; it != itEnd; ++it ) {
                if( filterGroup.shouldInclude( *it ) ) {
                    testsRunForGroup++;
                    if( m_testsAlreadyRun.find( *it ) == m_testsAlreadyRun.end() ) {

                        if( context.aborting() )
                            break;

                        totals += context.runTest( *it );
                        m_testsAlreadyRun.insert( *it );
                    }
                }
            }
            if( testsRunForGroup == 0 && !filterGroup.getName().empty() )
                m_reporter->noMatchingTestCases( filterGroup.getName() );
            return totals;

        }

    private:
        void openStream() {
            if( !m_config.stream.empty() )
                m_configWrapper.useStream( m_config.stream );

            // Open output file, if specified
            if( !m_config.outputFilename.empty() ) {
                m_ofs.open( m_config.outputFilename.c_str() );
                if( m_ofs.fail() ) {
                    std::ostringstream oss;
                    oss << "Unable to open file: '" << m_config.outputFilename << "'";
                    throw std::domain_error( oss.str() );
                }
                m_configWrapper.setStreamBuf( m_ofs.rdbuf() );
            }
        }
        void makeReporter() {
            std::string reporterName = m_config.reporter.empty()
            ? "console"
            : m_config.reporter;

            ReporterConfig reporterConfig( m_configWrapper.stream(), m_config );

            m_reporter = getRegistryHub().getReporterRegistry().create( reporterName, reporterConfig );
            if( !m_reporter ) {
                std::ostringstream oss;
                oss << "No reporter registered with name: '" << reporterName << "'";
                throw std::domain_error( oss.str() );
            }
        }

    private:
        Config& m_configWrapper;
        const ConfigData& m_config;
        std::ofstream m_ofs;
        Ptr<IStreamingReporter> m_reporter;
        std::set<TestCase> m_testsAlreadyRun;
    };

    inline int Main( Config& configWrapper ) {
        int result = 0;
        try
        {
            Runner2 runner( configWrapper );

            const ConfigData& config = configWrapper.data();

            // Handle list request
            if( config.listSpec != List::None ) {
                list( config );
                Catch::cleanUp();
                return 0;
            }

            result = static_cast<int>( runner.runTests().assertions.failed );

        }
        catch( std::exception& ex ) {
            std::cerr << ex.what() << std::endl;
            result = (std::numeric_limits<int>::max)();
        }

        Catch::cleanUp();
        return result;
    }

    inline void showUsage( std::ostream& os ) {
        AllOptions options;

        for( AllOptions::const_iterator it = options.begin(); it != options.end(); ++it ) {
            OptionParser& opt = **it;
            os << "  " << opt.optionNames() << " " << opt.argsSynopsis() << "\n";
        }
        os << "\nFor more detail usage please see: https://github.com/philsquared/Catch/wiki/Command-line\n" << std::endl;
    }

    inline void showHelp( const CommandParser& parser ) {
        AllOptions options;
        Options::HelpOptionParser helpOpt;
        bool displayedSpecificOption = false;
        for( AllOptions::const_iterator it = options.begin(); it != options.end(); ++it ) {
            OptionParser& opt = **it;
            if( opt.find( parser ) && opt.optionNames() != helpOpt.optionNames() ) {
                displayedSpecificOption = true;
                std::cout   << "\n" << opt.optionNames() << " " << opt.argsSynopsis() << "\n\n"
                            << opt.optionSummary() << "\n\n"
                            << LineWrapper().setIndent( 2 ).wrap( opt.optionDescription() ) << "\n" << std::endl;
            }
        }

        if( !displayedSpecificOption ) {
            std::cout   << "\nCATCH v"  << libraryVersion.majorVersion << "."
                                        << libraryVersion.minorVersion << " build "
                                        << libraryVersion.buildNumber;
            if( libraryVersion.branchName != "master" )
                std::cout << " (" << libraryVersion.branchName << " branch)";

            std::cout << "\n\n" << parser.exeName() << " is a CATCH host application. Options are as follows:\n\n";
            showUsage( std::cout );
        }
    }

    inline int Main( int argc, char* const argv[], Config& config ) {

        try {
            CommandParser parser( argc, argv );

            if( Command cmd = Options::HelpOptionParser().find( parser ) ) {
                if( cmd.argsCount() != 0 )
                    cmd.raiseError( "Does not accept arguments" );

                showHelp( parser );
                Catch::cleanUp();
                return 0;
            }

            AllOptions options;

            options.parseIntoConfig( parser, config.data() );
        }
        catch( std::exception& ex ) {
            std::cerr << ex.what() << "\n\nUsage: ...\n\n";
            showUsage( std::cerr );
            Catch::cleanUp();
            return (std::numeric_limits<int>::max)();
        }

        return Main( config );
    }

    inline int Main( int argc, char* const argv[] ) {
        Config config;
// !TBD: This doesn't always work, for some reason
//        if( isDebuggerActive() )
//            config.useStream( "debug" );
        return Main( argc, argv, config );
    }

} // end namespace Catch

// #included from: catch_registry_hub.hpp
#define TWOBLUECUBES_CATCH_REGISTRY_HUB_HPP_INCLUDED

// #included from: catch_test_case_registry_impl.hpp
#define TWOBLUECUBES_CATCH_TEST_CASE_REGISTRY_IMPL_HPP_INCLUDED

#include <vector>
#include <set>
#include <sstream>
#include <iostream>

namespace Catch {

    class TestRegistry : public ITestCaseRegistry {
    public:
        TestRegistry() : m_unnamedCount( 0 ) {}
        virtual ~TestRegistry();

        virtual void registerTest( const TestCase& testCase ) {
            std::string name = testCase.getTestCaseInfo().name;
            if( name == "" ) {
                std::ostringstream oss;
                oss << "Anonymous test case " << ++m_unnamedCount;
                return registerTest( testCase.withName( oss.str() ) );
            }

            if( m_functions.find( testCase ) == m_functions.end() ) {
                m_functions.insert( testCase );
                m_functionsInOrder.push_back( testCase );
                if( !testCase.isHidden() )
                    m_nonHiddenFunctions.push_back( testCase );
            }
            else {
                const TestCase& prev = *m_functions.find( testCase );
                std::cerr   << "error: TEST_CASE( \"" << name << "\" ) already defined.\n"
                            << "\tFirst seen at " << SourceLineInfo( prev.getTestCaseInfo().lineInfo ) << "\n"
                            << "\tRedefined at " << SourceLineInfo( testCase.getTestCaseInfo().lineInfo ) << std::endl;
                exit(1);
            }
        }

        virtual const std::vector<TestCase>& getAllTests() const {
            return m_functionsInOrder;
        }

        virtual const std::vector<TestCase>& getAllNonHiddenTests() const {
            return m_nonHiddenFunctions;
        }

        // !TBD deprecated
        virtual std::vector<TestCase> getMatchingTestCases( const std::string& rawTestSpec ) const {
            std::vector<TestCase> matchingTests;
            getMatchingTestCases( rawTestSpec, matchingTests );
            return matchingTests;
        }

        // !TBD deprecated
        virtual void getMatchingTestCases( const std::string& rawTestSpec, std::vector<TestCase>& matchingTestsOut ) const {
            TestCaseFilter filter( rawTestSpec );

            std::vector<TestCase>::const_iterator it = m_functionsInOrder.begin();
            std::vector<TestCase>::const_iterator itEnd = m_functionsInOrder.end();
            for(; it != itEnd; ++it ) {
                if( filter.shouldInclude( *it ) ) {
                    matchingTestsOut.push_back( *it );
                }
            }
        }
        virtual void getMatchingTestCases( const TestCaseFilters& filters, std::vector<TestCase>& matchingTestsOut ) const {
            std::vector<TestCase>::const_iterator it = m_functionsInOrder.begin();
            std::vector<TestCase>::const_iterator itEnd = m_functionsInOrder.end();
            // !TBD: replace with algorithm
            for(; it != itEnd; ++it )
                if( filters.shouldInclude( *it ) )
                    matchingTestsOut.push_back( *it );
        }

    private:

        std::set<TestCase> m_functions;
        std::vector<TestCase> m_functionsInOrder;
        std::vector<TestCase> m_nonHiddenFunctions;
        size_t m_unnamedCount;
    };

    ///////////////////////////////////////////////////////////////////////////

    class FreeFunctionTestCase : public SharedImpl<ITestCase> {
    public:

        FreeFunctionTestCase( TestFunction fun ) : m_fun( fun ) {}

        virtual void invoke() const {
            m_fun();
        }

    private:
        virtual ~FreeFunctionTestCase();

        TestFunction m_fun;
    };

    inline std::string extractClassName( const std::string& classOrQualifiedMethodName ) {
        std::string className = classOrQualifiedMethodName;
        if( className[0] == '&' )
        {
            std::size_t lastColons = className.rfind( "::" );
            std::size_t penultimateColons = className.rfind( "::", lastColons-1 );
            if( penultimateColons == std::string::npos )
                penultimateColons = 1;
            className = className.substr( penultimateColons, lastColons-penultimateColons );
        }
        return className;
    }

    ///////////////////////////////////////////////////////////////////////////

    AutoReg::AutoReg(   TestFunction function,
                        SourceLineInfo const& lineInfo,
                        NameAndDesc const& nameAndDesc ) {
        registerTestCase( new FreeFunctionTestCase( function ), "global", nameAndDesc, lineInfo );
    }

    AutoReg::~AutoReg() {}

    void AutoReg::registerTestCase( ITestCase* testCase,
                                    char const* classOrQualifiedMethodName,
                                    NameAndDesc const& nameAndDesc,
                                    SourceLineInfo const& lineInfo ) {

        getMutableRegistryHub().registerTest
            ( makeTestCase( testCase,
                            extractClassName( classOrQualifiedMethodName ),
                            nameAndDesc.name,
                            nameAndDesc.description,
                            lineInfo ) );
    }

} // end namespace Catch

// #included from: catch_reporter_registry.hpp
#define TWOBLUECUBES_CATCH_REPORTER_REGISTRY_HPP_INCLUDED

#include <map>

namespace Catch {

    class ReporterRegistry : public IReporterRegistry {

    public:

        virtual ~ReporterRegistry() {
            deleteAllValues( m_factories );
        }

        virtual IStreamingReporter* create( const std::string& name, const ReporterConfig& config ) const {
            FactoryMap::const_iterator it =  m_factories.find( name );
            if( it == m_factories.end() )
                return NULL;
            return it->second->create( config );
        }

        void registerReporter( const std::string& name, IReporterFactory* factory ) {
            m_factories.insert( std::make_pair( name, factory ) );
        }

        const FactoryMap& getFactories() const {
            return m_factories;
        }

    private:
        FactoryMap m_factories;
    };
}

// #included from: catch_exception_translator_registry.hpp
#define TWOBLUECUBES_CATCH_EXCEPTION_TRANSLATOR_REGISTRY_HPP_INCLUDED

#ifdef __OBJC__
#import "Foundation/Foundation.h"
#endif

namespace Catch {

    class ExceptionTranslatorRegistry : public IExceptionTranslatorRegistry {
    public:
        ~ExceptionTranslatorRegistry() {
            deleteAll( m_translators );
        }

        virtual void registerTranslator( const IExceptionTranslator* translator ) {
            m_translators.push_back( translator );
        }

        virtual std::string translateActiveException() const {
            try {
#ifdef __OBJC__
                // In Objective-C try objective-c exceptions first
                @try {
                    throw;
                }
                @catch (NSException *exception) {
                    return toString( [exception description] );
                }
#else
                throw;
#endif
            }
            catch( std::exception& ex ) {
                return ex.what();
            }
            catch( std::string& msg ) {
                return msg;
            }
            catch( const char* msg ) {
                return msg;
            }
            catch(...) {
                return tryTranslators( m_translators.begin() );
            }
        }

        std::string tryTranslators( std::vector<const IExceptionTranslator*>::const_iterator it ) const {
            if( it == m_translators.end() )
                return "Unknown exception";

            try {
                return (*it)->translate();
            }
            catch(...) {
                return tryTranslators( it+1 );
            }
        }

    private:
        std::vector<const IExceptionTranslator*> m_translators;
    };
}

namespace Catch {

    namespace {

        class RegistryHub : public IRegistryHub, public IMutableRegistryHub {

            RegistryHub( const RegistryHub& );
            void operator=( const RegistryHub& );

        public: // IRegistryHub
            RegistryHub() {
            }
            virtual const IReporterRegistry& getReporterRegistry() const {
                return m_reporterRegistry;
            }
            virtual const ITestCaseRegistry& getTestCaseRegistry() const {
                return m_testCaseRegistry;
            }
            virtual IExceptionTranslatorRegistry& getExceptionTranslatorRegistry() {
                return m_exceptionTranslatorRegistry;
            }

        public: // IMutableRegistryHub
            virtual void registerReporter( const std::string& name, IReporterFactory* factory ) {
                m_reporterRegistry.registerReporter( name, factory );
            }
            virtual void registerTest( const TestCase& testInfo ) {
                m_testCaseRegistry.registerTest( testInfo );
            }
            virtual void registerTranslator( const IExceptionTranslator* translator ) {
                m_exceptionTranslatorRegistry.registerTranslator( translator );
            }

        private:
            TestRegistry m_testCaseRegistry;
            ReporterRegistry m_reporterRegistry;
            ExceptionTranslatorRegistry m_exceptionTranslatorRegistry;
        };

        // Single, global, instance
        inline RegistryHub*& getTheRegistryHub() {
            static RegistryHub* theRegistryHub = NULL;
            if( !theRegistryHub )
                theRegistryHub = new RegistryHub();
            return theRegistryHub;
        }
    }

    IRegistryHub& getRegistryHub() {
        return *getTheRegistryHub();
    }
    IMutableRegistryHub& getMutableRegistryHub() {
        return *getTheRegistryHub();
    }
    void cleanUp() {
        delete getTheRegistryHub();
        getTheRegistryHub() = NULL;
        cleanUpContext();
    }
    std::string translateActiveException() {
        return getRegistryHub().getExceptionTranslatorRegistry().translateActiveException();
    }

} // end namespace Catch

// #included from: catch_notimplemented_exception.hpp
#define TWOBLUECUBES_CATCH_NOTIMPLEMENTED_EXCEPTION_HPP_INCLUDED

#include <ostream>

namespace Catch {

    NotImplementedException::NotImplementedException( const SourceLineInfo& lineInfo )
    :   m_lineInfo( lineInfo ) {
        std::ostringstream oss;
        oss << lineInfo << ": function ";
        oss << "not implemented";
        m_what = oss.str();
    }

    const char* NotImplementedException::what() const throw() {
        return m_what.c_str();
    }

} // end namespace Catch

// #included from: catch_context_impl.hpp
#define TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED

namespace Catch {

    class Context : public IMutableContext {

        Context() : m_config( NULL ) {}
        Context( const Context& );
        void operator=( const Context& );

    public: // IContext
        virtual IResultCapture& getResultCapture() {
            return *m_resultCapture;
        }
        virtual IRunner& getRunner() {
            return *m_runner;
        }
        virtual size_t getGeneratorIndex( const std::string& fileInfo, size_t totalSize ) {
            return getGeneratorsForCurrentTest()
            .getGeneratorInfo( fileInfo, totalSize )
            .getCurrentIndex();
        }
        virtual bool advanceGeneratorsForCurrentTest() {
            IGeneratorsForTest* generators = findGeneratorsForCurrentTest();
            return generators && generators->moveNext();
        }

        virtual const IConfig* getConfig() const {
            return m_config;
        }

    public: // IMutableContext
        virtual void setResultCapture( IResultCapture* resultCapture ) {
            m_resultCapture = resultCapture;
        }
        virtual void setRunner( IRunner* runner ) {
            m_runner = runner;
        }
        virtual void setConfig( const IConfig* config ) {
            m_config = config;
        }

        friend IMutableContext& getCurrentMutableContext();

    private:
        IGeneratorsForTest* findGeneratorsForCurrentTest() {
            std::string testName = getResultCapture().getCurrentTestName();

            std::map<std::string, IGeneratorsForTest*>::const_iterator it =
            m_generatorsByTestName.find( testName );
            return it != m_generatorsByTestName.end()
            ? it->second
            : NULL;
        }

        IGeneratorsForTest& getGeneratorsForCurrentTest() {
            IGeneratorsForTest* generators = findGeneratorsForCurrentTest();
            if( !generators ) {
                std::string testName = getResultCapture().getCurrentTestName();
                generators = createGeneratorsForTest();
                m_generatorsByTestName.insert( std::make_pair( testName, generators ) );
            }
            return *generators;
        }

    private:
        IRunner* m_runner;
        IResultCapture* m_resultCapture;
        const IConfig* m_config;
        std::map<std::string, IGeneratorsForTest*> m_generatorsByTestName;
    };

    namespace {
        Context* currentContext = NULL;
    }
    IMutableContext& getCurrentMutableContext() {
        if( !currentContext )
            currentContext = new Context();
        return *currentContext;
    }
    IContext& getCurrentContext() {
        return getCurrentMutableContext();
    }

    Stream createStream( const std::string& streamName ) {
        if( streamName == "stdout" ) return Stream( std::cout.rdbuf(), false );
        if( streamName == "stderr" ) return Stream( std::cerr.rdbuf(), false );
        if( streamName == "debug" ) return Stream( new StreamBufImpl<OutputDebugWriter>, true );

        throw std::domain_error( "Unknown stream: " + streamName );
    }

    void cleanUpContext() {
        delete currentContext;
        currentContext = NULL;
    }
}

// #included from: catch_console_colour_impl.hpp
#define TWOBLUECUBES_CATCH_CONSOLE_COLOUR_IMPL_HPP_INCLUDED

namespace Catch { namespace Detail {
    struct IColourImpl {
        virtual ~IColourImpl() {}
        virtual void use( Colour::Code _colourCode ) = 0;
    };
}}

#if defined ( CATCH_PLATFORM_WINDOWS ) /////////////////////////////////////////

#include <windows.h>

namespace Catch {
namespace {

    class Win32ColourImpl : public Detail::IColourImpl {
    public:
        Win32ColourImpl() : stdoutHandle( GetStdHandle(STD_OUTPUT_HANDLE) )
        {
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
            GetConsoleScreenBufferInfo( stdoutHandle, &csbiInfo );
            originalAttributes = csbiInfo.wAttributes;
        }

        virtual void use( Colour::Code _colourCode ) {
            switch( _colourCode ) {
                case Colour::None:      return setTextAttribute( originalAttributes );
                case Colour::White:     return setTextAttribute( FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE );
                case Colour::Red:       return setTextAttribute( FOREGROUND_RED );
                case Colour::Green:     return setTextAttribute( FOREGROUND_GREEN );
                case Colour::Blue:      return setTextAttribute( FOREGROUND_BLUE );
                case Colour::Cyan:      return setTextAttribute( FOREGROUND_BLUE | FOREGROUND_GREEN );
                case Colour::Yellow:    return setTextAttribute( FOREGROUND_RED | FOREGROUND_GREEN );
                case Colour::Grey:      return setTextAttribute( 0 );

                case Colour::LightGrey:     return setTextAttribute( FOREGROUND_INTENSITY );
                case Colour::BrightRed:     return setTextAttribute( FOREGROUND_INTENSITY | FOREGROUND_RED );
                case Colour::BrightGreen:   return setTextAttribute( FOREGROUND_INTENSITY | FOREGROUND_GREEN );
                case Colour::BrightWhite:   return setTextAttribute( FOREGROUND_INTENSITY |  FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE );

                case Colour::Bright: throw std::logic_error( "not a colour" );
            }
        }

    private:
        void setTextAttribute( WORD _textAttribute ) {
            SetConsoleTextAttribute( stdoutHandle, _textAttribute );
        }
        HANDLE stdoutHandle;
        WORD originalAttributes;
    };

    inline bool shouldUseColourForPlatform() {
        return true;
    }

    Win32ColourImpl platformColourImpl;

} // end anon namespace
} // end namespace Catch

#else // Not Windows - assumed to be POSIX compatible //////////////////////////

#include <unistd.h>

namespace Catch {
namespace {

    // use POSIX/ ANSI console terminal codes
    // Thanks to Adam Strzelecki for original contribution
    // (http://github.com/nanoant)
    // https://github.com/philsquared/Catch/pull/131
    class PosixColourImpl : public Detail::IColourImpl {
    public:
        virtual void use( Colour::Code _colourCode ) {
            switch( _colourCode ) {
                case Colour::None:
                case Colour::White:     return setColour( "[0m" );
                case Colour::Red:       return setColour( "[0;31m" );
                case Colour::Green:     return setColour( "[0;32m" );
                case Colour::Blue:      return setColour( "[0:34m" );
                case Colour::Cyan:      return setColour( "[0;36m" );
                case Colour::Yellow:    return setColour( "[0;33m" );
                case Colour::Grey:      return setColour( "[1;30m" );

                case Colour::LightGrey:     return setColour( "[0;37m" );
                case Colour::BrightRed:     return setColour( "[1;31m" );
                case Colour::BrightGreen:   return setColour( "[1;32m" );
                case Colour::BrightWhite:   return setColour( "[1;37m" );

                case Colour::Bright: throw std::logic_error( "not a colour" );
            }
        }
    private:
        void setColour( const char* _escapeCode ) {
            std::cout << '\033' << _escapeCode;
        }
    };

    inline bool shouldUseColourForPlatform() {
        return isatty( fileno(stdout) );
    }

    PosixColourImpl platformColourImpl;

} // end anon namespace
} // end namespace Catch

#endif // not Windows

namespace Catch {

    namespace {
        struct NoColourImpl : Detail::IColourImpl {
            void use( Colour::Code ) {}
        };
        NoColourImpl noColourImpl;
        static const bool shouldUseColour = shouldUseColourForPlatform() &&
                                            !isDebuggerActive();
    }

    Colour::Colour( Code _colourCode ){ use( _colourCode ); }
    Colour::~Colour(){ use( None ); }
    void Colour::use( Code _colourCode ) {
        impl->use( _colourCode );
    }

    Detail::IColourImpl* Colour::impl = shouldUseColour
            ? static_cast<Detail::IColourImpl*>( &platformColourImpl )
            : static_cast<Detail::IColourImpl*>( &noColourImpl );

} // end namespace Catch

// #included from: catch_generators_impl.hpp
#define TWOBLUECUBES_CATCH_GENERATORS_IMPL_HPP_INCLUDED

#include <vector>
#include <string>
#include <map>

namespace Catch {

    struct GeneratorInfo : IGeneratorInfo {

        GeneratorInfo( std::size_t size )
        :   m_size( size ),
            m_currentIndex( 0 )
        {}

        bool moveNext() {
            if( ++m_currentIndex == m_size ) {
                m_currentIndex = 0;
                return false;
            }
            return true;
        }

        std::size_t getCurrentIndex() const {
            return m_currentIndex;
        }

        std::size_t m_size;
        std::size_t m_currentIndex;
    };

    ///////////////////////////////////////////////////////////////////////////

    class GeneratorsForTest : public IGeneratorsForTest {

    public:
        ~GeneratorsForTest() {
            deleteAll( m_generatorsInOrder );
        }

        IGeneratorInfo& getGeneratorInfo( const std::string& fileInfo, std::size_t size ) {
            std::map<std::string, IGeneratorInfo*>::const_iterator it = m_generatorsByName.find( fileInfo );
            if( it == m_generatorsByName.end() ) {
                IGeneratorInfo* info = new GeneratorInfo( size );
                m_generatorsByName.insert( std::make_pair( fileInfo, info ) );
                m_generatorsInOrder.push_back( info );
                return *info;
            }
            return *it->second;
        }

        bool moveNext() {
            std::vector<IGeneratorInfo*>::const_iterator it = m_generatorsInOrder.begin();
            std::vector<IGeneratorInfo*>::const_iterator itEnd = m_generatorsInOrder.end();
            for(; it != itEnd; ++it ) {
                if( (*it)->moveNext() )
                    return true;
            }
            return false;
        }

    private:
        std::map<std::string, IGeneratorInfo*> m_generatorsByName;
        std::vector<IGeneratorInfo*> m_generatorsInOrder;
    };

    IGeneratorsForTest* createGeneratorsForTest()
    {
        return new GeneratorsForTest();
    }

} // end namespace Catch

// #included from: catch_assertionresult.hpp
#define TWOBLUECUBES_CATCH_ASSERTIONRESULT_HPP_INCLUDED

namespace Catch {

    AssertionInfo::AssertionInfo(   const std::string& _macroName,
                                    const SourceLineInfo& _lineInfo,
                                    const std::string& _capturedExpression,
                                    ResultDisposition::Flags _resultDisposition )
    :   macroName( _macroName ),
        lineInfo( _lineInfo ),
        capturedExpression( _capturedExpression ),
        resultDisposition( _resultDisposition )
    {
        if( shouldNegate( resultDisposition ) )
            capturedExpression = "!" + _capturedExpression;
    }

    AssertionResult::AssertionResult() {}

    AssertionResult::AssertionResult( const AssertionInfo& info, const AssertionResultData& data )
    :   m_info( info ),
        m_resultData( data )
    {}

    AssertionResult::~AssertionResult() {}

    // Result was a success
    bool AssertionResult::succeeded() const {
        return Catch::isOk( m_resultData.resultType );
    }

    // Result was a success, or failure is suppressed
    bool AssertionResult::isOk() const {
        return Catch::isOk( m_resultData.resultType ) || shouldSuppressFailure( m_info.resultDisposition );
    }

    ResultWas::OfType AssertionResult::getResultType() const {
        return m_resultData.resultType;
    }

    bool AssertionResult::hasExpression() const {
        return !m_info.capturedExpression.empty();
    }

    bool AssertionResult::hasMessage() const {
        return !m_resultData.message.empty();
    }

    std::string AssertionResult::getExpression() const {
        return m_info.capturedExpression;
    }

    bool AssertionResult::hasExpandedExpression() const {
        return hasExpression() && getExpandedExpression() != getExpression();
    }

    std::string AssertionResult::getExpandedExpression() const {
        return m_resultData.reconstructedExpression;
    }

    std::string AssertionResult::getMessage() const {
        return m_resultData.message;
    }
    SourceLineInfo AssertionResult::getSourceInfo() const {
        return m_info.lineInfo;
    }

    std::string AssertionResult::getTestMacroName() const {
        return m_info.macroName;
    }

} // end namespace Catch

// #included from: catch_expressionresult_builder.hpp
#define TWOBLUECUBES_CATCH_EXPRESSIONRESULT_BUILDER_HPP_INCLUDED

#include <assert.h>

namespace Catch {

    ExpressionResultBuilder::ExpressionResultBuilder( ResultWas::OfType resultType ) {
        m_data.resultType = resultType;
    }
    ExpressionResultBuilder::ExpressionResultBuilder( const ExpressionResultBuilder& other )
    :   m_data( other.m_data ),
        m_exprComponents( other.m_exprComponents )
    {
        m_stream << other.m_stream.str();
    }
    ExpressionResultBuilder& ExpressionResultBuilder::operator=(const ExpressionResultBuilder& other ) {
        m_data = other.m_data;
        m_exprComponents = other.m_exprComponents;
        m_stream.str("");
        m_stream << other.m_stream.str();
        return *this;
    }
    ExpressionResultBuilder& ExpressionResultBuilder::setResultType( ResultWas::OfType result ) {
        m_data.resultType = result;
        return *this;
    }
    ExpressionResultBuilder& ExpressionResultBuilder::setResultType( bool result ) {
        m_data.resultType = result ? ResultWas::Ok : ResultWas::ExpressionFailed;
        return *this;
    }
    ExpressionResultBuilder& ExpressionResultBuilder::endExpression( ResultDisposition::Flags resultDisposition ) {
        m_exprComponents.shouldNegate = shouldNegate( resultDisposition );
        return *this;
    }
    ExpressionResultBuilder& ExpressionResultBuilder::setLhs( const std::string& lhs ) {
        m_exprComponents.lhs = lhs;
        return *this;
    }
    ExpressionResultBuilder& ExpressionResultBuilder::setRhs( const std::string& rhs ) {
        m_exprComponents.rhs = rhs;
        return *this;
    }
    ExpressionResultBuilder& ExpressionResultBuilder::setOp( const std::string& op ) {
        m_exprComponents.op = op;
        return *this;
    }
    AssertionResult ExpressionResultBuilder::buildResult( const AssertionInfo& info ) const
    {
        assert( m_data.resultType != ResultWas::Unknown );

        AssertionResultData data = m_data;

        // Flip bool results if shouldNegate is set
        if( m_exprComponents.shouldNegate && data.resultType == ResultWas::Ok )
            data.resultType = ResultWas::ExpressionFailed;
        else if( m_exprComponents.shouldNegate && data.resultType == ResultWas::ExpressionFailed )
            data.resultType = ResultWas::Ok;

        data.message = m_stream.str();
        data.reconstructedExpression = reconstructExpression( info );
        if( m_exprComponents.shouldNegate ) {
            if( m_exprComponents.op == "" )
                data.reconstructedExpression = "!" + data.reconstructedExpression;
            else
                data.reconstructedExpression = "!(" + data.reconstructedExpression + ")";
        }
        return AssertionResult( info, data );
    }
    std::string ExpressionResultBuilder::reconstructExpression( const AssertionInfo& info ) const {
        if( m_exprComponents.op == "" )
            return m_exprComponents.lhs.empty() ? info.capturedExpression : m_exprComponents.op + m_exprComponents.lhs;
        else if( m_exprComponents.op == "matches" )
            return m_exprComponents.lhs + " " + m_exprComponents.rhs;
        else if( m_exprComponents.op != "!" ) {
            if( m_exprComponents.lhs.size() + m_exprComponents.rhs.size() < 40 &&
                m_exprComponents.lhs.find("\n") == std::string::npos &&
                m_exprComponents.rhs.find("\n") == std::string::npos )
                return m_exprComponents.lhs + " " + m_exprComponents.op + " " + m_exprComponents.rhs;
            else
                return m_exprComponents.lhs + "\n" + m_exprComponents.op + "\n" + m_exprComponents.rhs;
        }
        else
            return "{can't expand - use " + info.macroName + "_FALSE( " + info.capturedExpression.substr(1) + " ) instead of " + info.macroName + "( " + info.capturedExpression + " ) for better diagnostics}";
    }

} // end namespace Catch

// #included from: catch_test_case_info.hpp
#define TWOBLUECUBES_CATCH_TEST_CASE_INFO_HPP_INCLUDED

namespace Catch {

    TestCase makeTestCase(  ITestCase* _testCase,
                            const std::string& _className,
                            const std::string& _name,
                            const std::string& _descOrTags,
                            const SourceLineInfo& _lineInfo )
    {
        std::string desc = _descOrTags;
        bool isHidden( startsWith( _name, "./" ) );
        std::set<std::string> tags;
        TagExtracter( tags ).parse( desc );
        if( tags.find( "hide" ) != tags.end() )
            isHidden = true;

        TestCaseInfo info( _name, _className, desc, tags, isHidden, _lineInfo );
        return TestCase( _testCase, info );
    }

    TestCaseInfo::TestCaseInfo( const std::string& _name,
                                const std::string& _className,
                                const std::string& _description,
                                const std::set<std::string>& _tags,
                                bool _isHidden,
                                const SourceLineInfo& _lineInfo )
    :   name( _name ),
        className( _className ),
        description( _description ),
        tags( _tags ),
        lineInfo( _lineInfo ),
        isHidden( _isHidden )
    {
        std::ostringstream oss;
        for( std::set<std::string>::const_iterator it = _tags.begin(), itEnd = _tags.end(); it != itEnd; ++it )
            oss << "[" << *it << "]";
        tagsAsString = oss.str();
    }

    TestCaseInfo::TestCaseInfo( const TestCaseInfo& other )
    :   name( other.name ),
        className( other.className ),
        description( other.description ),
        tags( other.tags ),
        tagsAsString( other.tagsAsString ),
        lineInfo( other.lineInfo ),
        isHidden( other.isHidden )
    {}

    TestCase::TestCase( ITestCase* testCase, const TestCaseInfo& info ) : TestCaseInfo( info ), test( testCase ) {}

    TestCase::TestCase( const TestCase& other )
    :   TestCaseInfo( other ),
        test( other.test )
    {}

    TestCase TestCase::withName( const std::string& _newName ) const {
        TestCase other( *this );
        other.name = _newName;
        return other;
    }

    void TestCase::invoke() const {
        test->invoke();
    }

    bool TestCase::isHidden() const {
        return TestCaseInfo::isHidden;
    }

    bool TestCase::hasTag( const std::string& tag ) const {
        return tags.find( toLower( tag ) ) != tags.end();
    }
    bool TestCase::matchesTags( const std::string& tagPattern ) const {
        TagExpression exp;
        TagExpressionParser( exp ).parse( tagPattern );
        return exp.matches( tags );
    }
    const std::set<std::string>& TestCase::getTags() const {
        return tags;
    }

    void TestCase::swap( TestCase& other ) {
        test.swap( other.test );
        className.swap( other.className );
        name.swap( other.name );
        description.swap( other.description );
        std::swap( lineInfo, other.lineInfo );
    }

    bool TestCase::operator == ( const TestCase& other ) const {
        return  test.get() == other.test.get() &&
                name == other.name &&
                className == other.className;
    }

    bool TestCase::operator < ( const TestCase& other ) const {
        return name < other.name;
    }
    TestCase& TestCase::operator = ( const TestCase& other ) {
        TestCase temp( other );
        swap( temp );
        return *this;
    }

    const TestCaseInfo& TestCase::getTestCaseInfo() const
    {
        return *this;
    }

} // end namespace Catch

// #included from: catch_version.hpp
#define TWOBLUECUBES_CATCH_VERSION_HPP_INCLUDED

namespace Catch {

    // These numbers are maintained by a script
    Version libraryVersion( 0, 9, 33, "integration" );
}

// #included from: catch_line_wrap.hpp
#define TWOBLUECUBES_CATCH_LINE_WRAP_HPP_INCLUDED

namespace Catch {

    LineWrapper::LineWrapper()
    :   right( CATCH_CONFIG_CONSOLE_WIDTH-1 ),
        nextTab( 0 ),
        tab( 0 ),
        indent( 0 ),
        initialIndent( (std::size_t)-1 ), // use indent by default
        wrappableChars( " [({.,/|\\" ),
        tabChar( '\t' ),
        recursionCount( 0 )
    {}

    LineWrapper& LineWrapper::setIndent( std::size_t _indent ) {
        indent = _indent;
        return *this;
    }
    LineWrapper& LineWrapper::setInitialIndent( std::size_t _initialIndent ) {
        initialIndent = _initialIndent;
        return *this;
    }
    LineWrapper& LineWrapper::setRight( std::size_t _right ) {
        right = _right;
        return *this;
    }
    LineWrapper& LineWrapper::wrap( std::string const& _str ) {
        nextTab = tab = 0;
        wrapInternal( _str );
        return *this;
    }
    LineWrapper& LineWrapper::setTabChar( char _tabChar ) {
        tabChar = _tabChar;
        return *this;
    }
    bool LineWrapper::isWrapPoint( char c ) {
        return wrappableChars.find( c ) != std::string::npos;
    }
    void LineWrapper::wrapInternal( std::string const& _str ) {
        assert( ++recursionCount < 100 );

        std::size_t width = right - getCurrentIndent();
        std::size_t wrapPoint = width-tab;
        for( std::size_t pos = 0; pos < _str.size(); ++pos ) {
            if( _str[pos] == '\n' )
            {
                addLine( _str.substr( 0, pos ) );
                nextTab = tab = 0;
                return wrapInternal( _str.substr( pos+1 ) );
            }
            if( pos == width-tab ) {
                if( _str[wrapPoint] == ' ' ) {
                    addLine( _str.substr( 0, wrapPoint ) );
                    while( _str[++wrapPoint] == ' ' );
                }
                else if( isWrapPoint( _str[wrapPoint] ) ) {
                    addLine( _str.substr( 0, wrapPoint ) );
                }
                else {
                    addLine( _str.substr( 0, --wrapPoint ) + '-' );
                }
                return wrapInternal( _str.substr( wrapPoint ) );
            }
            if( _str[pos] == tabChar ) {
                nextTab = pos;
                std::string withoutTab = _str.substr( 0, nextTab ) + _str.substr( nextTab+1 );
                return wrapInternal( withoutTab );
            }
            else if( pos > 0 && isWrapPoint( _str[pos] ) ) {
                wrapPoint = pos;
            }
        }
        addLine( _str );
    }

    std::ostream& operator << ( std::ostream& _stream, LineWrapper const& _lineWrapper ) {
        for( LineWrapper::const_iterator it = _lineWrapper.begin(), itEnd = _lineWrapper.end();
            it != itEnd; ++it ) {
            if( it != _lineWrapper.begin() )
                _stream << "\n";
            _stream << *it;
        }
        return _stream;
    }
    std::string LineWrapper::toString() const {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }

    void LineWrapper::addLine( const std::string& _line ) {
        lines.push_back( std::string( tab + getCurrentIndent(), ' ' ) + _line );
        if( nextTab > 0 )
            tab = nextTab;
    }

    std::size_t LineWrapper::getCurrentIndent() const
    {
        return (initialIndent != (std::size_t)-1 && lines.empty() )
            ? initialIndent
            : indent;
    }

} // end namespace Catch

// #included from: catch_message.hpp
#define TWOBLUECUBES_CATCH_MESSAGE_HPP_INCLUDED

namespace Catch {

    MessageInfo::MessageInfo(   std::string const& _macroName,
                                SourceLineInfo const& _lineInfo,
                                ResultWas::OfType _type )
    :   macroName( _macroName ),
        lineInfo( _lineInfo ),
        type( _type ),
        sequence( ++globalCount )
    {}

    // This may need protecting if threading support is added
    unsigned int MessageInfo::globalCount = 0;

    ////////////////////////////////////////////////////////////////////////////

    MessageBuilder::MessageBuilder( std::string const& _macroName,
                                    SourceLineInfo const& _lineInfo,
                                    ResultWas::OfType _type )
    :   MessageInfo( _macroName, _lineInfo, _type )
    {}

    MessageInfo MessageBuilder::build() const {
        MessageInfo message = *this;
        message.message = stream.str();
        return message;
    }

    ////////////////////////////////////////////////////////////////////////////

    ScopedMessageBuilder::ScopedMessageBuilder
        (   std::string const& _macroName,
            SourceLineInfo const& _lineInfo,
            ResultWas::OfType _type )
    : MessageBuilder( _macroName, _lineInfo, _type )
    {}

    ScopedMessageBuilder::~ScopedMessageBuilder() {
        getResultCapture().popScopedMessage( *this );
    }

} // end namespace Catch

// #included from: catch_legacy_reporter_adapter.hpp
#define TWOBLUECUBES_CATCH_LEGACY_REPORTER_ADAPTER_HPP_INCLUDED

// #included from: catch_legacy_reporter_adapter.h
#define TWOBLUECUBES_CATCH_LEGACY_REPORTER_ADAPTER_H_INCLUDED

namespace Catch
{
    class LegacyReporterAdapter : public SharedImpl<IStreamingReporter>
    {
    public:
        LegacyReporterAdapter( Ptr<IReporter> const& legacyReporter, ReporterConfig const& config );
        virtual ~LegacyReporterAdapter();

        virtual ReporterPreferences getPreferences() const;
        virtual void noMatchingTestCases( std::string const& );
        virtual void testRunStarting( TestRunInfo const& );
        virtual void testGroupStarting( GroupInfo const& groupInfo );
        virtual void testCaseStarting( TestCaseInfo const& testInfo );
        virtual void sectionStarting( SectionInfo const& sectionInfo );
        virtual void assertionStarting( AssertionInfo const& );
        virtual void assertionEnded( AssertionStats const& assertionStats );
        virtual void sectionEnded( SectionStats const& sectionStats );
        virtual void testCaseEnded( TestCaseStats const& testCaseStats );
        virtual void testGroupEnded( TestGroupStats const& testGroupStats );
        virtual void testRunEnded( TestRunStats const& testRunStats );

    private:
        Ptr<IReporter> m_legacyReporter;
        ReporterConfig m_config;
    };
}

namespace Catch
{
    LegacyReporterAdapter::LegacyReporterAdapter( Ptr<IReporter> const& legacyReporter, ReporterConfig const& config )
    :   m_legacyReporter( legacyReporter ),
        m_config( config )
    {}
    LegacyReporterAdapter::~LegacyReporterAdapter() {}

    ReporterPreferences LegacyReporterAdapter::getPreferences() const {
        ReporterPreferences prefs;
        prefs.shouldRedirectStdOut = m_legacyReporter->shouldRedirectStdout();
        return prefs;
    }

    void LegacyReporterAdapter::noMatchingTestCases( std::string const& ) {}
    void LegacyReporterAdapter::testRunStarting( TestRunInfo const& ) {
        m_legacyReporter->StartTesting();
    }
    void LegacyReporterAdapter::testGroupStarting( GroupInfo const& groupInfo ) {
        m_legacyReporter->StartGroup( groupInfo.name );
    }
    void LegacyReporterAdapter::testCaseStarting( TestCaseInfo const& testInfo ) {
        m_legacyReporter->StartTestCase( testInfo );
    }
    void LegacyReporterAdapter::sectionStarting( SectionInfo const& sectionInfo ) {
        m_legacyReporter->StartSection( sectionInfo.name, sectionInfo.description );
    }
    void LegacyReporterAdapter::assertionStarting( AssertionInfo const& ) {
        // Not on legacy interface
    }

    void LegacyReporterAdapter::assertionEnded( AssertionStats const& assertionStats ) {
        if( assertionStats.assertionResult.getResultType() != ResultWas::Ok ) {
            for( std::vector<MessageInfo>::const_iterator it = assertionStats.infoMessages.begin(), itEnd = assertionStats.infoMessages.end();
                    it != itEnd;
                    ++it ) {
                if( it->type == ResultWas::Info ) {
                    ExpressionResultBuilder expressionBuilder( it->type );
                        expressionBuilder << it->message;
                    AssertionInfo info( it->macroName, it->lineInfo, "", ResultDisposition::Normal );
                    AssertionResult result = expressionBuilder.buildResult( info );
                    m_legacyReporter->Result( result );
                }
            }
        }
        m_legacyReporter->Result( assertionStats.assertionResult );
    }
    void LegacyReporterAdapter::sectionEnded( SectionStats const& sectionStats ) {
        if( sectionStats.missingAssertions )
            m_legacyReporter->NoAssertionsInSection( sectionStats.sectionInfo.name );
        m_legacyReporter->EndSection( sectionStats.sectionInfo.name, sectionStats.assertions );
    }
    void LegacyReporterAdapter::testCaseEnded( TestCaseStats const& testCaseStats ) {
        if( testCaseStats.missingAssertions )
            m_legacyReporter->NoAssertionsInTestCase( testCaseStats.testInfo.name );
        m_legacyReporter->EndTestCase
            (   testCaseStats.testInfo,
                testCaseStats.totals,
                testCaseStats.stdOut,
                testCaseStats.stdErr );
    }
    void LegacyReporterAdapter::testGroupEnded( TestGroupStats const& testGroupStats ) {
        if( testGroupStats.aborting )
            m_legacyReporter->Aborted();
        m_legacyReporter->EndGroup( testGroupStats.groupInfo.name, testGroupStats.totals );
    }
    void LegacyReporterAdapter::testRunEnded( TestRunStats const& testRunStats ) {
        m_legacyReporter->EndTesting( testRunStats.totals );
    }
}

// #included from: ../reporters/catch_reporter_basic.hpp
#define TWOBLUECUBES_CATCH_REPORTER_BASIC_HPP_INCLUDED

// #included from: ../internal/catch_reporter_registrars.hpp
#define TWOBLUECUBES_CATCH_REPORTER_REGISTRARS_HPP_INCLUDED

namespace Catch {

    template<typename T>
    class LegacyReporterRegistrar {

        class ReporterFactory : public IReporterFactory {

            virtual IStreamingReporter* create( const ReporterConfig& config ) const {
                return new LegacyReporterAdapter( new T( config ), config );
            }

            virtual std::string getDescription() const {
                return T::getDescription();
            }
        };

    public:

        LegacyReporterRegistrar( const std::string& name ) {
            getMutableRegistryHub().registerReporter( name, new ReporterFactory() );
        }
    };

    template<typename T>
    class ReporterRegistrar {

        class ReporterFactory : public IReporterFactory {

            // *** Please Note ***:
            // - If you end up here looking at a compiler error because it's trying to register
            // your custom reporter class be aware that the native reporter interface has changed
            // to IStreamingReporter. The "legacy" interface, IReporter, is still supported via
            // an adapter. Just use REGISTER_LEGACY_REPORTER to take advantage of the adapter.
            // However please consider updating to the new interface as the old one is now
            // deprecated and will probably be removed quite soon!
            // Please contact me via github if you have any questions at all about this.
            // In fact, ideally, please contact me anyway to let me know you've hit this - as I have
            // no idea who is actually using custom reporters at all (possibly no-one!).
            // The new interface is designed to minimise exposure to interface changes in the future.
            virtual IStreamingReporter* create( const ReporterConfig& config ) const {
                return new T( config );
            }

            virtual std::string getDescription() const {
                return T::getDescription();
            }
        };

    public:

        ReporterRegistrar( const std::string& name ) {
            getMutableRegistryHub().registerReporter( name, new ReporterFactory() );
        }
    };
}

#define INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( name, reporterType ) \
    Catch::LegacyReporterRegistrar<reporterType> catch_internal_RegistrarFor##reporterType( name );
#define INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType ) \
    Catch::ReporterRegistrar<reporterType> catch_internal_RegistrarFor##reporterType( name );

namespace Catch {

    class BasicReporter : public SharedImpl<IReporter> {

        struct SpanInfo {

            SpanInfo()
            :   emitted( false )
            {}

            SpanInfo( const std::string& spanName )
            :   name( spanName ),
                emitted( false )
            {}

            SpanInfo( const SpanInfo& other )
            :   name( other.name ),
                emitted( other.emitted )
            {}

            std::string name;
            bool emitted;
        };

    public:
        BasicReporter( const ReporterConfig& config )
        :   m_config( config ),
            m_firstSectionInTestCase( true ),
            m_aborted( false )
        {}

        virtual ~BasicReporter();

        static std::string getDescription() {
            return "Reports test results as lines of text";
        }

    private:

        void ReportCounts( const std::string& label, const Counts& counts, const std::string& allPrefix = "All " ) {
            if( counts.passed )
                m_config.stream() << counts.failed << " of " << counts.total() << " " << label << "s failed";
            else
                m_config.stream() << ( counts.failed > 1 ? allPrefix : "" ) << pluralise( counts.failed, label ) << " failed";
        }

        void ReportCounts( const Totals& totals, const std::string& allPrefix = "All " ) {
            if( totals.assertions.total() == 0 ) {
                m_config.stream() << "No tests ran";
            }
            else if( totals.assertions.failed ) {
                Colour colour( Colour::ResultError );
                ReportCounts( "test case", totals.testCases, allPrefix );
                if( totals.testCases.failed > 0 ) {
                    m_config.stream() << " (";
                    ReportCounts( "assertion", totals.assertions, allPrefix );
                    m_config.stream() << ")";
                }
            }
            else {
                Colour colour( Colour::ResultSuccess );
                m_config.stream()   << allPrefix << "tests passed ("
                                    << pluralise( totals.assertions.passed, "assertion" ) << " in "
                                    << pluralise( totals.testCases.passed, "test case" ) << ")";
            }
        }

    private: // IReporter

        virtual bool shouldRedirectStdout() const {
            return false;
        }

        virtual void StartTesting() {
            m_testingSpan = SpanInfo();
        }

        virtual void Aborted() {
            m_aborted = true;
        }

        virtual void EndTesting( const Totals& totals ) {
            // Output the overall test results even if "Started Testing" was not emitted
            if( m_aborted ) {
                m_config.stream() << "\n[Testing aborted. ";
                ReportCounts( totals, "The first " );
            }
            else {
                m_config.stream() << "\n[Testing completed. ";
                ReportCounts( totals );
            }
            m_config.stream() << "]\n" << std::endl;
        }

        virtual void StartGroup( const std::string& groupName ) {
            m_groupSpan = groupName;
        }

        virtual void EndGroup( const std::string& groupName, const Totals& totals ) {
            if( m_groupSpan.emitted && !groupName.empty() ) {
                m_config.stream() << "[End of group: '" << groupName << "'. ";
                ReportCounts( totals );
                m_config.stream() << "]\n" << std::endl;
                m_groupSpan = SpanInfo();
            }
        }

        virtual void StartTestCase( const TestCaseInfo& testInfo ) {
            m_testSpan = testInfo.name;
        }

        virtual void StartSection( const std::string& sectionName, const std::string& ) {
            m_sectionSpans.push_back( SpanInfo( sectionName ) );
        }

        virtual void NoAssertionsInSection( const std::string& sectionName ) {
            startSpansLazily();
            Colour colour( Colour::ResultError );
            m_config.stream() << "\nNo assertions in section, '" << sectionName << "'\n" << std::endl;
        }
        virtual void NoAssertionsInTestCase( const std::string& testName ) {
            startSpansLazily();
            Colour colour( Colour::ResultError );
            m_config.stream() << "\nNo assertions in test case, '" << testName << "'\n" << std::endl;
        }

        virtual void EndSection( const std::string& sectionName, const Counts& assertions ) {

            SpanInfo& sectionSpan = m_sectionSpans.back();
            if( sectionSpan.emitted && !sectionSpan.name.empty() ) {
                m_config.stream() << "[End of section: '" << sectionName << "' ";

                if( assertions.failed ) {
                    Colour colour( Colour::ResultError );
                    ReportCounts( "assertion", assertions);
                }
                else {
                    Colour colour( Colour::ResultSuccess );
                    m_config.stream()   << ( assertions.passed > 1 ? "All " : "" )
                                        << pluralise( assertions.passed, "assertion" ) << " passed" ;
                }
                m_config.stream() << "]\n" << std::endl;
            }
            m_sectionSpans.pop_back();
        }

        virtual void Result( const AssertionResult& assertionResult ) {
            if( !m_config.includeSuccessfulResults() && assertionResult.getResultType() == ResultWas::Ok )
                return;

            startSpansLazily();

            if( !assertionResult.getSourceInfo().empty() ) {
                Colour colour( Colour::FileName );
                m_config.stream() << assertionResult.getSourceInfo() << ": ";
            }

            if( assertionResult.hasExpression() ) {
                Colour colour( Colour::OriginalExpression );
                m_config.stream() << assertionResult.getExpression();
                if( assertionResult.succeeded() ) {
                    Colour successColour( Colour::Success );
                    m_config.stream() << " succeeded";
                }
                else {
                    Colour errorColour( Colour::Error );
                    m_config.stream() << " failed";
                    if( assertionResult.isOk() ) {
                        Colour okAnywayColour( Colour::Success );
                        m_config.stream() << " - but was ok";
                    }
                }
            }
            switch( assertionResult.getResultType() ) {
                case ResultWas::ThrewException:
                    {
                        Colour colour( Colour::Error );
                        if( assertionResult.hasExpression() )
                            m_config.stream() << " with unexpected";
                        else
                            m_config.stream() << "Unexpected";
                        m_config.stream() << " exception with message: '" << assertionResult.getMessage() << "'";
                    }
                    break;
                case ResultWas::DidntThrowException:
                    {
                        Colour colour( Colour::Error );
                        if( assertionResult.hasExpression() )
                            m_config.stream() << " because no exception was thrown where one was expected";
                        else
                            m_config.stream() << "No exception thrown where one was expected";
                    }
                    break;
                case ResultWas::Info:
                    {
                        Colour colour( Colour::ReconstructedExpression );
                        streamVariableLengthText( "info", assertionResult.getMessage() );
                    }
                    break;
                case ResultWas::Warning:
                    {
                        Colour colour( Colour::ReconstructedExpression );
                        streamVariableLengthText( "warning", assertionResult.getMessage() );
                    }
                    break;
                case ResultWas::ExplicitFailure:
                    {
                        Colour colour( Colour::Error );
                        m_config.stream() << "failed with message: '" << assertionResult.getMessage() << "'";
                    }
                    break;
                case ResultWas::Unknown: // These cases are here to prevent compiler warnings
                case ResultWas::Ok:
                case ResultWas::FailureBit:
                case ResultWas::ExpressionFailed:
                case ResultWas::Exception:
                    if( !assertionResult.hasExpression() ) {
                        if( assertionResult.succeeded() ) {
                            Colour colour( Colour::Success );
                            m_config.stream() << " succeeded";
                        }
                        else {
                            Colour colour( Colour::Error );
                            m_config.stream() << " failed";
                            if( assertionResult.isOk() ) {
                                Colour okAnywayColour( Colour::Success );
                                m_config.stream() << " - but was ok";
                            }
                        }
                    }
                    if( assertionResult.hasMessage() ) {
                        m_config.stream() << "\n";
                        Colour colour( Colour::ReconstructedExpression );
                        streamVariableLengthText( "with message", assertionResult.getMessage() );
                    }
                    break;
            }

            if( assertionResult.hasExpandedExpression() ) {
                m_config.stream() << " for: ";
                if( assertionResult.getExpandedExpression().size() > 40 ) {
                    m_config.stream() << "\n";
                    if( assertionResult.getExpandedExpression().size() < 70 )
                        m_config.stream() << "\t";
                }
                Colour colour( Colour::ReconstructedExpression );
                m_config.stream() << assertionResult.getExpandedExpression();
            }
            m_config.stream() << std::endl;
        }

        virtual void EndTestCase(   const TestCaseInfo& testInfo,
                                    const Totals& totals,
                                    const std::string& stdOut,
                                    const std::string& stdErr ) {
            if( !stdOut.empty() ) {
                startSpansLazily();
                streamVariableLengthText( "stdout", stdOut );
            }

            if( !stdErr.empty() ) {
                startSpansLazily();
                streamVariableLengthText( "stderr", stdErr );
            }

            if( m_testSpan.emitted ) {
                m_config.stream() << "[Finished: '" << testInfo.name << "' ";
                ReportCounts( totals );
                m_config.stream() << "]" << std::endl;
            }
        }

    private: // helpers

        void startSpansLazily() {
            if( !m_testingSpan.emitted ) {
                if( m_config.name().empty() )
                    m_config.stream() << "[Started testing]" << std::endl;
                else
                    m_config.stream() << "[Started testing: " << m_config.name() << "]" << std::endl;
                m_testingSpan.emitted = true;
            }

            if( !m_groupSpan.emitted && !m_groupSpan.name.empty() ) {
                m_config.stream() << "[Started group: '" << m_groupSpan.name << "']" << std::endl;
                m_groupSpan.emitted = true;
            }

            if( !m_testSpan.emitted ) {
                m_config.stream() << std::endl << "[Running: " << m_testSpan.name << "]" << std::endl;
                m_testSpan.emitted = true;
            }

            if( !m_sectionSpans.empty() ) {
                SpanInfo& sectionSpan = m_sectionSpans.back();
                if( !sectionSpan.emitted && !sectionSpan.name.empty() ) {
                    if( m_firstSectionInTestCase ) {
                        m_config.stream() << "\n";
                        m_firstSectionInTestCase = false;
                    }
                    std::vector<SpanInfo>::iterator it = m_sectionSpans.begin();
                    std::vector<SpanInfo>::iterator itEnd = m_sectionSpans.end();
                    for(; it != itEnd; ++it ) {
                        SpanInfo& prevSpan = *it;
                        if( !prevSpan.emitted && !prevSpan.name.empty() ) {
                            m_config.stream() << "[Started section: '" << prevSpan.name << "']" << std::endl;
                            prevSpan.emitted = true;
                        }
                    }
                }
            }
        }

        void streamVariableLengthText( const std::string& prefix, const std::string& text ) {
            std::string trimmed = trim( text );
            if( trimmed.find_first_of( "\r\n" ) == std::string::npos ) {
                m_config.stream() << "[" << prefix << ": " << trimmed << "]";
            }
            else {
                m_config.stream() << "\n[" << prefix << "] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" << trimmed
                << "\n[end of " << prefix << "] <<<<<<<<<<<<<<<<<<<<<<<<\n";
            }
        }

    private:
        ReporterConfig m_config;
        bool m_firstSectionInTestCase;

        SpanInfo m_testingSpan;
        SpanInfo m_groupSpan;
        SpanInfo m_testSpan;
        std::vector<SpanInfo> m_sectionSpans;
        bool m_aborted;
    };

} // end namespace Catch

// #included from: ../reporters/catch_reporter_xml.hpp
#define TWOBLUECUBES_CATCH_REPORTER_XML_HPP_INCLUDED

// #included from: ../internal/catch_xmlwriter.hpp
#define TWOBLUECUBES_CATCH_XMLWRITER_HPP_INCLUDED

#include <sstream>
#include <iostream>
#include <string>
#include <vector>

namespace Catch {

    class XmlWriter {
    public:

        class ScopedElement {
        public:
            ScopedElement( XmlWriter* writer )
            :   m_writer( writer )
            {}

            ScopedElement( const ScopedElement& other )
            :   m_writer( other.m_writer ){
                other.m_writer = NULL;
            }

            ~ScopedElement() {
                if( m_writer )
                    m_writer->endElement();
            }

            ScopedElement& writeText( const std::string& text, bool indent = true ) {
                m_writer->writeText( text, indent );
                return *this;
            }

            template<typename T>
            ScopedElement& writeAttribute( const std::string& name, const T& attribute ) {
                m_writer->writeAttribute( name, attribute );
                return *this;
            }

        private:
            mutable XmlWriter* m_writer;
        };

        XmlWriter()
        :   m_tagIsOpen( false ),
            m_needsNewline( false ),
            m_os( &std::cout )
        {}

        XmlWriter( std::ostream& os )
        :   m_tagIsOpen( false ),
            m_needsNewline( false ),
            m_os( &os )
        {}

        ~XmlWriter() {
            while( !m_tags.empty() )
                endElement();
        }

        XmlWriter& operator = ( const XmlWriter& other ) {
            XmlWriter temp( other );
            swap( temp );
            return *this;
        }

        void swap( XmlWriter& other ) {
            std::swap( m_tagIsOpen, other.m_tagIsOpen );
            std::swap( m_needsNewline, other.m_needsNewline );
            std::swap( m_tags, other.m_tags );
            std::swap( m_indent, other.m_indent );
            std::swap( m_os, other.m_os );
        }

        XmlWriter& startElement( const std::string& name ) {
            ensureTagClosed();
            newlineIfNecessary();
            stream() << m_indent << "<" << name;
            m_tags.push_back( name );
            m_indent += "  ";
            m_tagIsOpen = true;
            return *this;
        }

        ScopedElement scopedElement( const std::string& name ) {
            ScopedElement scoped( this );
            startElement( name );
            return scoped;
        }

        XmlWriter& endElement() {
            newlineIfNecessary();
            m_indent = m_indent.substr( 0, m_indent.size()-2 );
            if( m_tagIsOpen ) {
                stream() << "/>\n";
                m_tagIsOpen = false;
            }
            else {
                stream() << m_indent << "</" << m_tags.back() << ">\n";
            }
            m_tags.pop_back();
            return *this;
        }

        XmlWriter& writeAttribute( const std::string& name, const std::string& attribute ) {
            if( !name.empty() && !attribute.empty() ) {
                stream() << " " << name << "=\"";
                writeEncodedText( attribute );
                stream() << "\"";
            }
            return *this;
        }

        XmlWriter& writeAttribute( const std::string& name, bool attribute ) {
            stream() << " " << name << "=\"" << ( attribute ? "true" : "false" ) << "\"";
            return *this;
        }

        template<typename T>
        XmlWriter& writeAttribute( const std::string& name, const T& attribute ) {
            if( !name.empty() )
                stream() << " " << name << "=\"" << attribute << "\"";
            return *this;
        }

        XmlWriter& writeText( const std::string& text, bool indent = true ) {
            if( !text.empty() ){
                bool tagWasOpen = m_tagIsOpen;
                ensureTagClosed();
                if( tagWasOpen && indent )
                    stream() << m_indent;
                writeEncodedText( text );
                m_needsNewline = true;
            }
            return *this;
        }

        XmlWriter& writeComment( const std::string& text ) {
            ensureTagClosed();
            stream() << m_indent << "<!--" << text << "-->";
            m_needsNewline = true;
            return *this;
        }

        XmlWriter& writeBlankLine() {
            ensureTagClosed();
            stream() << "\n";
            return *this;
        }

    private:

        std::ostream& stream() {
            return *m_os;
        }

        void ensureTagClosed() {
            if( m_tagIsOpen ) {
                stream() << ">\n";
                m_tagIsOpen = false;
            }
        }

        void newlineIfNecessary() {
            if( m_needsNewline ) {
                stream() << "\n";
                m_needsNewline = false;
            }
        }

        void writeEncodedText( const std::string& text ) {
            static const char* charsToEncode = "<&\"";
            std::string mtext = text;
            std::string::size_type pos = mtext.find_first_of( charsToEncode );
            while( pos != std::string::npos ) {
                stream() << mtext.substr( 0, pos );

                switch( mtext[pos] ) {
                    case '<':
                        stream() << "&lt;";
                        break;
                    case '&':
                        stream() << "&amp;";
                        break;
                    case '\"':
                        stream() << "&quot;";
                        break;
                }
                mtext = mtext.substr( pos+1 );
                pos = mtext.find_first_of( charsToEncode );
            }
            stream() << mtext;
        }

        bool m_tagIsOpen;
        bool m_needsNewline;
        std::vector<std::string> m_tags;
        std::string m_indent;
        std::ostream* m_os;
    };

}
namespace Catch {
    class XmlReporter : public SharedImpl<IReporter> {
    public:
        XmlReporter( const ReporterConfig& config ) : m_config( config ) {}

        static std::string getDescription() {
            return "Reports test results as an XML document";
        }
        virtual ~XmlReporter();

    private: // IReporter

        virtual bool shouldRedirectStdout() const {
            return true;
        }

        virtual void StartTesting() {
            m_xml = XmlWriter( m_config.stream() );
            m_xml.startElement( "Catch" );
            if( !m_config.name().empty() )
                m_xml.writeAttribute( "name", m_config.name() );
        }

        virtual void EndTesting( const Totals& totals ) {
            m_xml.scopedElement( "OverallResults" )
                .writeAttribute( "successes", totals.assertions.passed )
                .writeAttribute( "failures", totals.assertions.failed );
            m_xml.endElement();
        }

        virtual void StartGroup( const std::string& groupName ) {
            m_xml.startElement( "Group" )
                .writeAttribute( "name", groupName );
        }

        virtual void EndGroup( const std::string&, const Totals& totals ) {
            m_xml.scopedElement( "OverallResults" )
                .writeAttribute( "successes", totals.assertions.passed )
                .writeAttribute( "failures", totals.assertions.failed );
            m_xml.endElement();
        }

        virtual void StartSection( const std::string& sectionName, const std::string& description ) {
            m_xml.startElement( "Section" )
                .writeAttribute( "name", sectionName )
                .writeAttribute( "description", description );
        }
        virtual void NoAssertionsInSection( const std::string& ) {}
        virtual void NoAssertionsInTestCase( const std::string& ) {}

        virtual void EndSection( const std::string& /*sectionName*/, const Counts& assertions ) {
            m_xml.scopedElement( "OverallResults" )
                .writeAttribute( "successes", assertions.passed )
                .writeAttribute( "failures", assertions.failed );
            m_xml.endElement();
        }

        virtual void StartTestCase( const Catch::TestCaseInfo& testInfo ) {
            m_xml.startElement( "TestCase" ).writeAttribute( "name", testInfo.name );
            m_currentTestSuccess = true;
        }

        virtual void Result( const Catch::AssertionResult& assertionResult ) {
            if( !m_config.includeSuccessfulResults() && assertionResult.getResultType() == ResultWas::Ok )
                return;

            if( assertionResult.hasExpression() ) {
                m_xml.startElement( "Expression" )
                    .writeAttribute( "success", assertionResult.succeeded() )
                    .writeAttribute( "filename", assertionResult.getSourceInfo().file )
                    .writeAttribute( "line", assertionResult.getSourceInfo().line );

                m_xml.scopedElement( "Original" )
                    .writeText( assertionResult.getExpression() );
                m_xml.scopedElement( "Expanded" )
                    .writeText( assertionResult.getExpandedExpression() );
                m_currentTestSuccess &= assertionResult.succeeded();
            }

            switch( assertionResult.getResultType() ) {
                case ResultWas::ThrewException:
                    m_xml.scopedElement( "Exception" )
                        .writeAttribute( "filename", assertionResult.getSourceInfo().file )
                        .writeAttribute( "line", assertionResult.getSourceInfo().line )
                        .writeText( assertionResult.getMessage() );
                    m_currentTestSuccess = false;
                    break;
                case ResultWas::Info:
                    m_xml.scopedElement( "Info" )
                        .writeText( assertionResult.getMessage() );
                    break;
                case ResultWas::Warning:
                    m_xml.scopedElement( "Warning" )
                        .writeText( assertionResult.getMessage() );
                    break;
                case ResultWas::ExplicitFailure:
                    m_xml.scopedElement( "Failure" )
                        .writeText( assertionResult.getMessage() );
                    m_currentTestSuccess = false;
                    break;
                case ResultWas::Unknown:
                case ResultWas::Ok:
                case ResultWas::FailureBit:
                case ResultWas::ExpressionFailed:
                case ResultWas::Exception:
                case ResultWas::DidntThrowException:
                    break;
            }
            if( assertionResult.hasExpression() )
                m_xml.endElement();
        }

        virtual void Aborted() {
            // !TBD
        }

        virtual void EndTestCase( const Catch::TestCaseInfo&, const Totals&, const std::string&, const std::string& ) {
            m_xml.scopedElement( "OverallResult" ).writeAttribute( "success", m_currentTestSuccess );
            m_xml.endElement();
        }

    private:
        ReporterConfig m_config;
        bool m_currentTestSuccess;
        XmlWriter m_xml;
    };

} // end namespace Catch

// #included from: ../reporters/catch_reporter_junit.hpp
#define TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED

#include <assert.h>

namespace Catch {

    class JunitReporter : public SharedImpl<IReporter> {

        struct TestStats {
            std::string m_element;
            std::string m_resultType;
            std::string m_message;
            std::string m_content;
        };

        struct TestCaseStats {

            TestCaseStats( const std::string& className, const std::string& name )
            :   m_className( className ),
                m_name( name )
            {}

            double      m_timeInSeconds;
            std::string m_status;
            std::string m_className;
            std::string m_name;
            std::string m_stdOut;
            std::string m_stdErr;
            std::vector<TestStats> m_testStats;
            std::vector<TestCaseStats> m_sections;
        };

        struct Stats {

            Stats( const std::string& name = std::string() )
            :   m_testsCount( 0 ),
                m_failuresCount( 0 ),
                m_disabledCount( 0 ),
                m_errorsCount( 0 ),
                m_timeInSeconds( 0 ),
                m_name( name )
            {}

            std::size_t m_testsCount;
            std::size_t m_failuresCount;
            std::size_t m_disabledCount;
            std::size_t m_errorsCount;
            double      m_timeInSeconds;
            std::string m_name;

            std::vector<TestCaseStats> m_testCaseStats;
        };

    public:
        JunitReporter( const ReporterConfig& config )
        :   m_config( config ),
            m_testSuiteStats( "AllTests" ),
            m_currentStats( &m_testSuiteStats )
        {}
        virtual ~JunitReporter();

        static std::string getDescription() {
            return "Reports test results in an XML format that looks like Ant's junitreport target";
        }

    private: // IReporter

        virtual bool shouldRedirectStdout() const {
            return true;
        }

        virtual void StartTesting(){}

        virtual void StartGroup( const std::string& groupName ) {
            if( groupName.empty() )
                m_statsForSuites.push_back( Stats( m_config.name() ) );
            else
                m_statsForSuites.push_back( Stats( groupName ) );
            m_currentStats = &m_statsForSuites.back();
        }

        virtual void EndGroup( const std::string&, const Totals& totals ) {
            m_currentStats->m_testsCount = totals.assertions.total();
            m_currentStats = &m_testSuiteStats;
        }

        virtual void StartSection( const std::string&, const std::string& ){}

        virtual void NoAssertionsInSection( const std::string& ) {}
        virtual void NoAssertionsInTestCase( const std::string& ) {}

        virtual void EndSection( const std::string&, const Counts& ) {}

        virtual void StartTestCase( const Catch::TestCaseInfo& testInfo ) {
            m_currentStats->m_testCaseStats.push_back( TestCaseStats( testInfo.className, testInfo.name ) );
            m_currentTestCaseStats.push_back( &m_currentStats->m_testCaseStats.back() );
        }

        virtual void Result( const Catch::AssertionResult& assertionResult ) {
            if( assertionResult.getResultType() != ResultWas::Ok || m_config.includeSuccessfulResults() ) {
                TestCaseStats& testCaseStats = m_currentStats->m_testCaseStats.back();
                TestStats stats;
                std::ostringstream oss;
                if( !assertionResult.getMessage().empty() )
                    oss << assertionResult.getMessage() << " at ";
                oss << assertionResult.getSourceInfo();
                stats.m_content = oss.str();
                stats.m_message = assertionResult.getExpandedExpression();
                stats.m_resultType = assertionResult.getTestMacroName();

                switch( assertionResult.getResultType() ) {
                    case ResultWas::ThrewException:
                        stats.m_element = "error";
                        m_currentStats->m_errorsCount++;
                        break;
                    case ResultWas::Info:
                        stats.m_element = "info"; // !TBD ?
                        break;
                    case ResultWas::Warning:
                        stats.m_element = "warning"; // !TBD ?
                        break;
                    case ResultWas::ExplicitFailure:
                        stats.m_element = "failure";
                        m_currentStats->m_failuresCount++;
                        break;
                    case ResultWas::ExpressionFailed:
                        stats.m_element = "failure";
                        m_currentStats->m_failuresCount++;
                        break;
                    case ResultWas::Ok:
                        stats.m_element = "success";
                        break;
                    case ResultWas::DidntThrowException:
                        stats.m_element = "failure";
                        m_currentStats->m_failuresCount++;
                        break;
                    case ResultWas::Unknown:
                    case ResultWas::FailureBit:
                    case ResultWas::Exception:
                        stats.m_element = "* internal error *";
                        break;
                }
                testCaseStats.m_testStats.push_back( stats );
            }
        }

        virtual void EndTestCase( const Catch::TestCaseInfo&, const Totals&, const std::string& stdOut, const std::string& stdErr ) {
            m_currentTestCaseStats.pop_back();
            assert( m_currentTestCaseStats.empty() );
            TestCaseStats& testCaseStats = m_currentStats->m_testCaseStats.back();
            testCaseStats.m_stdOut = stdOut;
            testCaseStats.m_stdErr = stdErr;
            if( !stdOut.empty() )
                m_stdOut << stdOut << "\n";
            if( !stdErr.empty() )
                m_stdErr << stdErr << "\n";
        }

        virtual void Aborted() {
            // !TBD
        }

        virtual void EndTesting( const Totals& ) {
            XmlWriter xml( m_config.stream() );

            if( m_statsForSuites.size() > 0 )
                xml.startElement( "testsuites" );

            std::vector<Stats>::const_iterator it = m_statsForSuites.begin();
            std::vector<Stats>::const_iterator itEnd = m_statsForSuites.end();

            for(; it != itEnd; ++it ) {
                XmlWriter::ScopedElement e = xml.scopedElement( "testsuite" );
                xml.writeAttribute( "name", it->m_name );
                xml.writeAttribute( "errors", it->m_errorsCount );
                xml.writeAttribute( "failures", it->m_failuresCount );
                xml.writeAttribute( "tests", it->m_testsCount );
                xml.writeAttribute( "hostname", "tbd" );
                xml.writeAttribute( "time", "tbd" );
                xml.writeAttribute( "timestamp", "tbd" );

                OutputTestCases( xml, *it );
            }

            xml.scopedElement( "system-out" ).writeText( trim( m_stdOut.str() ), false );
            xml.scopedElement( "system-err" ).writeText( trim( m_stdErr.str() ), false );
        }

        void OutputTestCases( XmlWriter& xml, const Stats& stats ) {
            std::vector<TestCaseStats>::const_iterator it = stats.m_testCaseStats.begin();
            std::vector<TestCaseStats>::const_iterator itEnd = stats.m_testCaseStats.end();
            for(; it != itEnd; ++it ) {

                XmlWriter::ScopedElement e = xml.scopedElement( "testcase" );
                xml.writeAttribute( "classname", it->m_className );
                xml.writeAttribute( "name", it->m_name );
                xml.writeAttribute( "time", "tbd" );

                OutputTestResult( xml, *it );

                std::string stdOut = trim( it->m_stdOut );
                if( !stdOut.empty() )
                    xml.scopedElement( "system-out" ).writeText( stdOut, false );
                std::string stdErr = trim( it->m_stdErr );
                if( !stdErr.empty() )
                    xml.scopedElement( "system-err" ).writeText( stdErr, false );
            }
        }

        void OutputTestResult( XmlWriter& xml, const TestCaseStats& stats ) {
            std::vector<TestStats>::const_iterator it = stats.m_testStats.begin();
            std::vector<TestStats>::const_iterator itEnd = stats.m_testStats.end();
            for(; it != itEnd; ++it ) {
                if( it->m_element != "success" ) {
                    XmlWriter::ScopedElement e = xml.scopedElement( it->m_element );

                    xml.writeAttribute( "message", it->m_message );
                    xml.writeAttribute( "type", it->m_resultType );
                    if( !it->m_content.empty() )
                        xml.writeText( it->m_content );
                }
            }
        }

    private:
        ReporterConfig m_config;
//        bool m_currentTestSuccess;

        Stats m_testSuiteStats;
        Stats* m_currentStats;
        std::vector<Stats> m_statsForSuites;
        std::vector<const TestCaseStats*> m_currentTestCaseStats;
        std::ostringstream m_stdOut;
        std::ostringstream m_stdErr;
    };

} // end namespace Catch

// #included from: ../reporters/catch_reporter_console.hpp
#define TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED

namespace Catch {

    struct ConsoleReporter : StreamingReporterBase {
        ConsoleReporter( ReporterConfig const& _config )
        :   StreamingReporterBase( _config ),
            m_headerPrinted( false ),
            m_atLeastOneTestCasePrinted( false )
        {}

        virtual ~ConsoleReporter();
        static std::string getDescription() {
            return "Reports test results as plain lines of text";
        }
        virtual ReporterPreferences getPreferences() const {
            ReporterPreferences prefs;
            prefs.shouldRedirectStdOut = false;
            return prefs;

        }

        virtual void noMatchingTestCases( std::string const& spec ) {
            stream << "No test cases matched '" << spec << "'" << std::endl;
        }

        virtual void assertionStarting( AssertionInfo const& ) {
        }

        virtual void assertionEnded( AssertionStats const& _assertionStats ) {
            AssertionResult const& result = _assertionStats.assertionResult;

            // Drop out if result was successful and we're not printing those
            if( !m_config.includeSuccessfulResults() && result.isOk() )
                return;

            lazyPrint();

            AssertionPrinter printer( stream, _assertionStats );
            printer.print();
            stream << std::endl;
        }

        virtual void sectionStarting( SectionInfo const& _sectionInfo ) {
            m_headerPrinted = false;
            StreamingReporterBase::sectionStarting( _sectionInfo );
        }
        virtual void sectionEnded( SectionStats const& _sectionStats ) {
            if( _sectionStats.missingAssertions ) {
                lazyPrint();
                Colour colour( Colour::ResultError );
                stream << "\nNo assertions in section, '" << _sectionStats.sectionInfo.name << "'\n" << std::endl;
            }
            m_headerPrinted = false;
            StreamingReporterBase::sectionEnded( _sectionStats );
        }

        virtual void testCaseEnded( TestCaseStats const& _testCaseStats ) {

            if( _testCaseStats.missingAssertions ) {
                lazyPrint();
                Colour colour( Colour::ResultError );
                stream << "\nNo assertions in test case, '" << _testCaseStats.testInfo.name << "'\n" << std::endl;
            }
            StreamingReporterBase::testCaseEnded( _testCaseStats );
            m_headerPrinted = false;
        }
        virtual void testGroupEnded( TestGroupStats const& _testGroupStats ) {
            if( !unusedGroupInfo ) {
                printSummaryDivider();
                stream << "Summary for group '" << _testGroupStats.groupInfo.name << "':\n";
                printTotals( _testGroupStats.totals );
                stream << "\n" << std::endl;
            }
            StreamingReporterBase::testGroupEnded( _testGroupStats );
        }
        virtual void testRunEnded( TestRunStats const& _testRunStats ) {
            if( m_atLeastOneTestCasePrinted )
                printTotalsDivider();
            printTotals( _testRunStats.totals );
            stream << "\n" << std::endl;
            StreamingReporterBase::testRunEnded( _testRunStats );
        }

    private:

        class AssertionPrinter {
			void operator= ( AssertionPrinter const& );
        public:
            AssertionPrinter( std::ostream& _stream, AssertionStats const& _stats )
            :   stream( _stream ),
                stats( _stats ),
                result( _stats.assertionResult ),
                colour( Colour::None ),
                message( result.getMessage() ),
                messages( _stats.infoMessages )
            {
                switch( result.getResultType() ) {
                    case ResultWas::Ok:
                        colour = Colour::Success;
                        passOrFail = "PASSED";
                        //if( result.hasMessage() )
                        if( _stats.infoMessages.size() == 1 )
                            messageLabel = "with message";
                        if( _stats.infoMessages.size() > 1 )
                            messageLabel = "with messages";
                        break;
                    case ResultWas::ExpressionFailed:
                        if( result.isOk() ) {
                            colour = Colour::Success;
                            passOrFail = "FAILED - but was ok";
                        }
                        else {
                            colour = Colour::Error;
                            passOrFail = "FAILED";
                        }
                        if( _stats.infoMessages.size() == 1 )
                            messageLabel = "with message";
                        if( _stats.infoMessages.size() > 1 )
                            messageLabel = "with messages";
                        break;
                    case ResultWas::ThrewException:
                        colour = Colour::Error;
                        passOrFail = "FAILED";
                        messageLabel = "due to unexpected exception with message";
                        break;
                    case ResultWas::DidntThrowException:
                        colour = Colour::Error;
                        passOrFail = "FAILED";
                        messageLabel = "because no exception was thrown where one was expected";
                        break;
                    case ResultWas::Info:
                        messageLabel = "info";
                        break;
                    case ResultWas::Warning:
                        messageLabel = "warning";
                        break;
                    case ResultWas::ExplicitFailure:
                        passOrFail = "FAILED";
                        colour = Colour::Error;
                        if( _stats.infoMessages.size() == 1 )
                            messageLabel = "explicitly with message";
                        if( _stats.infoMessages.size() > 1 )
                            messageLabel = "explicitly with messages";
                        break;
                    // These cases are here to prevent compiler warnings
                    case ResultWas::Unknown:
                    case ResultWas::FailureBit:
                    case ResultWas::Exception:
                        passOrFail = "** internal error **";
                        colour = Colour::Error;
                        break;
                }
            }

            void print() const {
                printSourceInfo();
                if( stats.totals.assertions.total() > 0 ) {
                    if( result.isOk() )
                        stream << "\n";
                    printResultType();
                    printOriginalExpression();
                    printReconstructedExpression();
                }
                else {
                    stream << "\n";
                }
                printMessage();
            }

        private:
            void printResultType() const {
                if( !passOrFail.empty() ) {
                    Colour colourGuard( colour );
                    stream << passOrFail << ":\n";
                }
            }
            void printOriginalExpression() const {
                if( result.hasExpression() ) {
                    Colour colourGuard( Colour::OriginalExpression );
                    stream  << "  ";
                    if( !result.getTestMacroName().empty() )
                        stream << result.getTestMacroName() << "( ";
                    stream << result.getExpression();
                    if( !result.getTestMacroName().empty() )
                        stream << " )";
                    stream << "\n";
                }
            }
            void printReconstructedExpression() const {
                if( result.hasExpandedExpression() ) {
                    stream << "with expansion:\n";
                    Colour colourGuard( Colour::ReconstructedExpression );
                    stream << LineWrapper().setIndent(2).wrap( result.getExpandedExpression() ) << "\n";
                }
            }
            void printMessage() const {
                if( !messageLabel.empty() )
                    stream << messageLabel << ":" << "\n";
                for( std::vector<MessageInfo>::const_iterator it = messages.begin(), itEnd = messages.end();
                        it != itEnd;
                        ++it ) {
                    stream << LineWrapper().setIndent(2).wrap( it->message ) << "\n";
                }
            }
            void printSourceInfo() const {
                Colour colourGuard( Colour::FileName );
                stream << result.getSourceInfo() << ": ";
            }

            std::ostream& stream;
            AssertionStats const& stats;
            AssertionResult const& result;
            Colour::Code colour;
            std::string passOrFail;
            std::string messageLabel;
            std::string message;
            std::vector<MessageInfo> messages;
        };

        void lazyPrint() {

            if( testRunInfo )
                lazyPrintRunInfo();
            if( unusedGroupInfo )
                lazyPrintGroupInfo();

            if( !m_headerPrinted ) {
                printTestCaseAndSectionHeader();
                m_headerPrinted = true;
            }
            m_atLeastOneTestCasePrinted = true;
        }
        void lazyPrintRunInfo() {
            stream  << "\n" << getTildes() << "\n";
            Colour colour( Colour::SecondaryText );
            stream  << testRunInfo->name
                    << " is a CATCH v"  << libraryVersion.majorVersion << "."
                    << libraryVersion.minorVersion << " b"
                    << libraryVersion.buildNumber;
            if( libraryVersion.branchName != "master" )
                stream << " (" << libraryVersion.branchName << ")";
            stream  << " host application.\n"
                    << "Run with -? for options\n\n";

            testRunInfo.reset();
        }
        void lazyPrintGroupInfo() {
            if( !unusedGroupInfo->name.empty() && unusedGroupInfo->groupsCounts > 1 ) {
                printClosedHeader( "Group: " + unusedGroupInfo->name );
                unusedGroupInfo.reset();
            }
        }
        void printTestCaseAndSectionHeader() {
            printOpenHeader(    unusedTestCaseInfo->name,
                                currentSectionInfo
                                    ? currentSectionInfo->lineInfo
                                    : unusedTestCaseInfo->lineInfo );
            if( currentSectionInfo ) {
                Colour colourGuard( Colour::Headers );
                std::vector<ThreadedSectionInfo*> sections;
                for(    ThreadedSectionInfo* section = currentSectionInfo.get();
                        section;
                        section = section->parent )
                    sections.push_back( section );

                // Sections
                if( !sections.empty() ) {
                    typedef std::vector<ThreadedSectionInfo*>::const_reverse_iterator It;
                    for( It it = sections.rbegin(), itEnd = sections.rend(); it != itEnd; ++it )
                        printUserString( (*it)->name, 2 );

                }
            }
            stream << getDots() << "\n" << std::endl;
        }

        void printClosedHeader( std::string const& _name ) {
            printOpenHeader( _name );
            stream << getDots() << "\n";
        }
        void printOpenHeader( std::string const& _name, SourceLineInfo const& _lineInfo = SourceLineInfo() ) {
            stream  << getDashes() << "\n";
            if( !_lineInfo.empty() ){
                Colour colourGuard( Colour::FileName );
                stream << _lineInfo << "\n\n";
            }
            {
                Colour colourGuard( Colour::Headers );
                printUserString( _name );
            }
        }

        // if string has a : in first line will set indent to follow it on
        // subsequent lines
        void printUserString( std::string const& _string, std::size_t indent = 0 ) {
            std::size_t i = _string.find( ": " );
            if( i != std::string::npos )
                i+=2;
            else
                i = 0;
            stream << LineWrapper()
                        .setIndent( indent+i)
                        .setInitialIndent( indent )
                        .wrap( _string ) << "\n";
        }

        void printTotals( const Totals& totals ) {
            if( totals.assertions.total() == 0 ) {
                stream << "No tests ran";
            }
            else if( totals.assertions.failed ) {
                Colour colour( Colour::ResultError );
                printCounts( "test case", totals.testCases );
                if( totals.testCases.failed > 0 ) {
                    stream << " (";
                    printCounts( "assertion", totals.assertions );
                    stream << ")";
                }
            }
            else {
                Colour colour( Colour::ResultSuccess );
                stream << "All tests passed ("
                        << pluralise( totals.assertions.passed, "assertion" ) << " in "
                        << pluralise( totals.testCases.passed, "test case" ) << ")";
            }
        }
        void printCounts( std::string const& label, Counts const& counts ) {
            if( counts.total() == 1 ) {
                stream << "1 " << label << " - ";
                if( counts.failed )
                    stream << "failed";
                else
                    stream << "passed";
            }
            else {
                stream << counts.total() << " " << label << "s ";
                if( counts.passed ) {
                    if( counts.failed )
                        stream << "- " << counts.failed << " failed";
                    else if( counts.passed == 2 )
                        stream << "- both passed";
                    else
                        stream << "- all passed";
                }
                else {
                    if( counts.failed == 2 )
                        stream << "- both failed";
                    else
                        stream << "- all failed";
                }
            }
        }

        void printTotalsDivider() {
            stream << getDoubleDashes() << "\n";
        }
        void printSummaryDivider() {
            stream << getDashes() << "\n";
        }
        static std::string const& getDashes() {
            static const std::string dashes( CATCH_CONFIG_CONSOLE_WIDTH-1, '-' );
            return dashes;
        }
        static std::string const& getDots() {
            static const std::string dots( CATCH_CONFIG_CONSOLE_WIDTH-1, '.' );
            return dots;
        }
        static std::string const& getDoubleDashes() {
            static const std::string doubleDashes( CATCH_CONFIG_CONSOLE_WIDTH-1, '=' );
            return doubleDashes;
        }
        static std::string const& getTildes() {
            static const std::string dots( CATCH_CONFIG_CONSOLE_WIDTH-1, '~' );
            return dots;
        }

    private:
        bool m_headerPrinted;
        bool m_atLeastOneTestCasePrinted;
    };

    INTERNAL_CATCH_REGISTER_REPORTER( "console", ConsoleReporter )

} // end namespace Catch

namespace Catch {
    NonCopyable::~NonCopyable() {}
    IShared::~IShared() {}
    StreamBufBase::~StreamBufBase() {}
    IContext::~IContext() {}
    IResultCapture::~IResultCapture() {}
    ITestCase::~ITestCase() {}
    ITestCaseRegistry::~ITestCaseRegistry() {}
    IRegistryHub::~IRegistryHub() {}
    IMutableRegistryHub::~IMutableRegistryHub() {}
    IExceptionTranslator::~IExceptionTranslator() {}
    IExceptionTranslatorRegistry::~IExceptionTranslatorRegistry() {}
    IReporter::~IReporter() {}
    IReporterFactory::~IReporterFactory() {}
    IReporterRegistry::~IReporterRegistry() {}
    IStreamingReporter::~IStreamingReporter() {}
    AssertionStats::~AssertionStats() {}
    SectionStats::~SectionStats() {}
    TestCaseStats::~TestCaseStats() {}
    TestGroupStats::~TestGroupStats() {}
    TestRunStats::~TestRunStats() {}
    ThreadedSectionInfo::~ThreadedSectionInfo() {}
    TestGroupNode::~TestGroupNode() {}
    TestRunNode::~TestRunNode() {}

    BasicReporter::~BasicReporter() {}
    StreamingReporterBase::~StreamingReporterBase() {}
    ConsoleReporter::~ConsoleReporter() {}
    IRunner::~IRunner() {}
    IMutableContext::~IMutableContext() {}
    IConfig::~IConfig() {}
    XmlReporter::~XmlReporter() {}
    JunitReporter::~JunitReporter() {}
    TestRegistry::~TestRegistry() {}
    FreeFunctionTestCase::~FreeFunctionTestCase() {}
    IGeneratorInfo::~IGeneratorInfo() {}
    IGeneratorsForTest::~IGeneratorsForTest() {}
    TagParser::~TagParser() {}
    TagExtracter::~TagExtracter() {}
    TagExpressionParser::~TagExpressionParser() {}

    Matchers::Impl::StdString::Equals::~Equals() {}
    Matchers::Impl::StdString::Contains::~Contains() {}
    Matchers::Impl::StdString::StartsWith::~StartsWith() {}
    Matchers::Impl::StdString::EndsWith::~EndsWith() {}

    void Config::dummy() {}

    INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( "basic", BasicReporter )
    INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( "xml", XmlReporter )
    INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( "junit", JunitReporter )

}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif

#ifdef CATCH_CONFIG_MAIN
// #included from: internal/catch_default_main.hpp
#define TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED

#ifndef __OBJC__

// Standard C/C++ main entry point
int main (int argc, char * const argv[]) {
    return Catch::Main( argc, argv );
}

#else // __OBJC__

// Objective-C entry point
int main (int argc, char * const argv[]) {
#if !CATCH_ARC_ENABLED
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
#endif

    Catch::registerTestMethods();
    int result = Catch::Main( argc, (char* const*)argv );

#if !CATCH_ARC_ENABLED
    [pool drain];
#endif

    return result;
}

#endif // __OBJC__

#endif

//////

// If this config identifier is defined then all CATCH macros are prefixed with CATCH_
#ifdef CATCH_CONFIG_PREFIX_ALL

#define CATCH_REQUIRE( expr ) INTERNAL_CATCH_TEST( expr, Catch::ResultDisposition::Normal, "CATCH_REQUIRE" )
#define CATCH_REQUIRE_FALSE( expr ) INTERNAL_CATCH_TEST( expr, Catch::ResultDisposition::Normal | Catch::ResultDisposition::NegateResult, "CATCH_REQUIRE_FALSE" )

#define CATCH_REQUIRE_THROWS( expr ) INTERNAL_CATCH_THROWS( expr, ..., Catch::ResultDisposition::Normal, "CATCH_REQUIRE_THROWS" )
#define CATCH_REQUIRE_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( expr, exceptionType, Catch::ResultDisposition::Normal, "CATCH_REQUIRE_THROWS_AS" )
#define CATCH_REQUIRE_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( expr, Catch::ResultDisposition::Normal, "CATCH_REQUIRE_NOTHROW" )

#define CATCH_CHECK( expr ) INTERNAL_CATCH_TEST( expr, Catch::ResultDisposition::ContinueOnFailure, "CATCH_CHECK" )
#define CATCH_CHECK_FALSE( expr ) INTERNAL_CATCH_TEST( expr, Catch::ResultDisposition::ContinueOnFailure | Catch::ResultDisposition::NegateResult, "CATCH_CHECK_FALSE" )
#define CATCH_CHECKED_IF( expr ) INTERNAL_CATCH_IF( expr, Catch::ResultDisposition::ContinueOnFailure, "CATCH_CHECKED_IF" )
#define CATCH_CHECKED_ELSE( expr ) INTERNAL_CATCH_ELSE( expr, Catch::ResultDisposition::ContinueOnFailure, "CATCH_CHECKED_ELSE" )
#define CATCH_CHECK_NOFAIL( expr ) INTERNAL_CATCH_TEST( expr, Catch::ResultDisposition::ContinueOnFailure | Catch::ResultDisposition::SuppressFail, "CATCH_CHECK_NOFAIL" )

#define CATCH_CHECK_THROWS( expr )  INTERNAL_CATCH_THROWS( expr, ..., Catch::ResultDisposition::ContinueOnFailure, "CATCH_CHECK_THROWS" )
#define CATCH_CHECK_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( expr, exceptionType, Catch::ResultDisposition::ContinueOnFailure, "CATCH_CHECK_THROWS_AS" )
#define CATCH_CHECK_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( expr, Catch::ResultDisposition::ContinueOnFailure, "CATCH_CHECK_NOTHROW" )

#define CHECK_THAT( arg, matcher ) INTERNAL_CHECK_THAT( arg, matcher, Catch::ResultDisposition::ContinueOnFailure, "CATCH_CHECK_THAT" )
#define CATCH_REQUIRE_THAT( arg, matcher ) INTERNAL_CHECK_THAT( arg, matcher, Catch::ResultDisposition::Normal, "CATCH_REQUIRE_THAT" )

#define CATCH_INFO( msg ) INTERNAL_CATCH_INFO( msg, "CATCH_INFO" )
#define CATCH_WARN( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Warning, Catch::ResultDisposition::ContinueOnFailure, "CATCH_WARN" )
#define CATCH_FAIL( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::ExplicitFailure, Catch::ResultDisposition::Normal, "CATCH_FAIL" )
#define CATCH_SUCCEED( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Ok, Catch::ResultDisposition::ContinueOnFailure, "CATCH_SUCCEED" )
#define CATCH_SCOPED_INFO( msg ) INTERNAL_CATCH_SCOPED_INFO( msg, "CATCH_SCOPED_INFO" )
#define CATCH_CAPTURE( msg ) INTERNAL_CATCH_INFO( #msg " := " << msg, "CATCH_CAPTURE" )
#define CATCH_SCOPED_CAPTURE( msg ) INTERNAL_CATCH_SCOPED_INFO( #msg " := " << msg, "CATCH_SCOPED_CAPTURE" )

#ifdef CATCH_CONFIG_VARIADIC_MACROS
    #define CATCH_TEST_CASE( ... ) INTERNAL_CATCH_TESTCASE( __VA_ARGS__ )
    #define CATCH_TEST_CASE_NORETURN( ... ) INTERNAL_CATCH_TESTCASE_NORETURN( __VA_ARGS__ )
    #define CATCH_METHOD_AS_TEST_CASE( method, ... ) INTERNAL_CATCH_METHOD_AS_TEST_CASE( method, __VA_ARGS__ )
    #define CATCH_SECTION( ... ) INTERNAL_CATCH_SECTION( __VA_ARGS__ )
#else
    #define CATCH_TEST_CASE( name, description ) INTERNAL_CATCH_TESTCASE( name, description )
    #define CATCH_TEST_CASE_NORETURN( name, description ) INTERNAL_CATCH_TESTCASE_NORETURN( name, description )
    #define CATCH_METHOD_AS_TEST_CASE( method, name, description ) INTERNAL_CATCH_METHOD_AS_TEST_CASE( method, name, description )
    #define CATCH_SECTION( name, description ) INTERNAL_CATCH_SECTION( name, description )
#endif
#define CATCH_ANON_TEST_CASE() INTERNAL_CATCH_TESTCASE( "", "" )

#define CATCH_REGISTER_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType )
#define CATCH_REGISTER_LEGACY_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( name, reporterType )

#define CATCH_GENERATE( expr) INTERNAL_CATCH_GENERATE( expr )

// "BDD-style" convenience wrappers
#ifdef CATCH_CONFIG_VARIADIC_MACROS
#define CATCH_SCENARIO( ... ) CATCH_TEST_CASE( "Scenario: " __VA_ARGS__ )
#else
#define CATCH_SCENARIO( name, tags ) CATCH_TEST_CASE( "Scenario: " name, tags )
#endif
#define CATCH_GIVEN( desc )    CATCH_SECTION( "Given: " desc, "" )
#define CATCH_WHEN( desc )     CATCH_SECTION( " When: " desc, "" )
#define CATCH_AND_WHEN( desc ) CATCH_SECTION( "  And: " desc, "" )
#define CATCH_THEN( desc )     CATCH_SECTION( " Then: " desc, "" )
#define CATCH_AND_THEN( desc ) CATCH_SECTION( "  And: " desc, "" )

// If CATCH_CONFIG_PREFIX_ALL is not defined then the CATCH_ prefix is not required
#else

#define REQUIRE( expr ) INTERNAL_CATCH_TEST( expr, Catch::ResultDisposition::Normal, "REQUIRE" )
#define REQUIRE_FALSE( expr ) INTERNAL_CATCH_TEST( expr, Catch::ResultDisposition::Normal | Catch::ResultDisposition::NegateResult, "REQUIRE_FALSE" )

#define REQUIRE_THROWS( expr ) INTERNAL_CATCH_THROWS( expr, ..., Catch::ResultDisposition::Normal, "REQUIRE_THROWS" )
#define REQUIRE_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( expr, exceptionType, Catch::ResultDisposition::Normal, "REQUIRE_THROWS_AS" )
#define REQUIRE_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( expr, Catch::ResultDisposition::Normal, "REQUIRE_NOTHROW" )

#define CHECK( expr ) INTERNAL_CATCH_TEST( expr, Catch::ResultDisposition::ContinueOnFailure, "CHECK" )
#define CHECK_FALSE( expr ) INTERNAL_CATCH_TEST( expr, Catch::ResultDisposition::ContinueOnFailure | Catch::ResultDisposition::NegateResult, "CHECK_FALSE" )
#define CHECKED_IF( expr ) INTERNAL_CATCH_IF( expr, Catch::ResultDisposition::ContinueOnFailure, "CHECKED_IF" )
#define CHECKED_ELSE( expr ) INTERNAL_CATCH_ELSE( expr, Catch::ResultDisposition::ContinueOnFailure, "CHECKED_ELSE" )
#define CHECK_NOFAIL( expr ) INTERNAL_CATCH_TEST( expr, Catch::ResultDisposition::ContinueOnFailure | Catch::ResultDisposition::SuppressFail, "CHECK_NOFAIL" )

#define CHECK_THROWS( expr )  INTERNAL_CATCH_THROWS( expr, ..., Catch::ResultDisposition::ContinueOnFailure, "CHECK_THROWS" )
#define CHECK_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( expr, exceptionType, Catch::ResultDisposition::ContinueOnFailure, "CHECK_THROWS_AS" )
#define CHECK_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( expr, Catch::ResultDisposition::ContinueOnFailure, "CHECK_NOTHROW" )

#define CHECK_THAT( arg, matcher ) INTERNAL_CHECK_THAT( arg, matcher, Catch::ResultDisposition::ContinueOnFailure, "CHECK_THAT" )
#define REQUIRE_THAT( arg, matcher ) INTERNAL_CHECK_THAT( arg, matcher, Catch::ResultDisposition::Normal, "REQUIRE_THAT" )

#define INFO( msg ) INTERNAL_CATCH_INFO( msg, "INFO" )
#define WARN( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Warning, Catch::ResultDisposition::ContinueOnFailure, "WARN" )
#define FAIL( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::ExplicitFailure, Catch::ResultDisposition::Normal, "FAIL" )
#define SUCCEED( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Ok, Catch::ResultDisposition::ContinueOnFailure, "SUCCEED" )
#define SCOPED_INFO( msg ) INTERNAL_CATCH_SCOPED_INFO( msg, "SCOPED_INFO" )
#define CAPTURE( msg ) INTERNAL_CATCH_INFO( #msg " := " << msg, "CAPTURE" )
#define SCOPED_CAPTURE( msg ) INTERNAL_CATCH_SCOPED_INFO( #msg " := " << msg, "SCOPED_CAPTURE" )

#ifdef CATCH_CONFIG_VARIADIC_MACROS
    #define TEST_CASE( ... ) INTERNAL_CATCH_TESTCASE( __VA_ARGS__ )
    #define TEST_CASE_NORETURN( ... ) INTERNAL_CATCH_TESTCASE_NORETURN( __VA_ARGS__ )
    #define METHOD_AS_TEST_CASE( method, ... ) INTERNAL_CATCH_METHOD_AS_TEST_CASE( method, __VA_ARGS__ )
    #define SECTION( ... ) INTERNAL_CATCH_SECTION( __VA_ARGS__ )
#else
    #define TEST_CASE( name, description ) INTERNAL_CATCH_TESTCASE( name, description )
    #define TEST_CASE_NORETURN( name, description ) INTERNAL_CATCH_TESTCASE_NORETURN( name, description )
    #define METHOD_AS_TEST_CASE( method, name, description ) INTERNAL_CATCH_METHOD_AS_TEST_CASE( method, name, description )
    #define SECTION( name, description ) INTERNAL_CATCH_SECTION( name, description )
#endif
#define ANON_TEST_CASE() INTERNAL_CATCH_TESTCASE( "", "" )

#define REGISTER_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType )
#define REGISTER_LEGACY_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_LEGACY_REPORTER( name, reporterType )

#define GENERATE( expr) INTERNAL_CATCH_GENERATE( expr )

#endif

#define CATCH_TRANSLATE_EXCEPTION( signature ) INTERNAL_CATCH_TRANSLATE_EXCEPTION( signature )

// "BDD-style" convenience wrappers
#ifdef CATCH_CONFIG_VARIADIC_MACROS
#define SCENARIO( ... ) TEST_CASE( "Scenario: " __VA_ARGS__ )
#else
#define SCENARIO( name, tags ) TEST_CASE( "Scenario: " name, tags )
#endif
#define GIVEN( desc )    SECTION( "   Given: " desc, "" )
#define WHEN( desc )     SECTION( "    When: " desc, "" )
#define AND_WHEN( desc ) SECTION( "And when: " desc, "" )
#define THEN( desc )     SECTION( "    Then: " desc, "" )
#define AND_THEN( desc ) SECTION( "     And: " desc, "" )

using Catch::Detail::Approx;

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED

