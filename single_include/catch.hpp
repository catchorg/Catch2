/*
 *  Generated: 2012-09-20 08:36:21.042619
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
#pragma clang diagnostic ignored "-Wno-global-constructors"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
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
        {
            delete *it;
        }
    }
    template<typename AssociativeContainerT>
    inline void deleteAllValues( AssociativeContainerT& container ) {
        typename AssociativeContainerT::const_iterator it = container.begin();
        typename AssociativeContainerT::const_iterator itEnd = container.end();
        for(; it != itEnd; ++it )
        {
            delete it->second;
        }
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
        SourceLineInfo( const std::string& _function, const std::string& _file, std::size_t _line )
        :   function( _function ),
            file( _file ),
            line( _line )
        {}
        SourceLineInfo( const SourceLineInfo& other )
        :   file( other.file ),
            line( other.line )
        {}
        void swap( SourceLineInfo& other ){
            file.swap( other.file );
            std::swap( line, other.line );
        }

        std::string function;
        std::string file;
        std::size_t line;
    };

    inline std::ostream& operator << ( std::ostream& os, const SourceLineInfo& info ) {
#ifndef __GNUG__
        os << info.file << "(" << info.line << "): ";
#else
        os << info.file << ":" << info.line << ": ";
#endif
        return os;
    }

    CATCH_ATTRIBUTE_NORETURN
    inline void throwLogicError( const std::string& message, const SourceLineInfo& locationInfo ) {
        std::ostringstream oss;
        oss << "Internal Catch error: '" << message << "' at: " << locationInfo;
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

    class TestCaseInfo;
    struct IResultCapture;
    struct IRunner;
    struct IGeneratorsForTest;
    struct IConfig;

    class StreamBufBase : public std::streambuf {
    public:
        virtual ~StreamBufBase();
    };

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
    std::streambuf* createStreamBuf( const std::string& streamName );

}

// #included from: internal/catch_test_registry.hpp
#define TWOBLUECUBES_CATCH_REGISTRY_HPP_INCLUDED

// #included from: catch_interfaces_testcase.h
#define TWOBLUECUBES_CATCH_INTERFACES_TESTCASE_H_INCLUDED

// #included from: catch_ptr.hpp
#define TWOBLUECUBES_CATCH_PTR_HPP_INCLUDED

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
        Ptr( const Ptr& other ) : m_p( other.m_p ){
            if( m_p )
                m_p->addRef();
        }
        ~Ptr(){
            if( m_p )
                m_p->release();
        }
        Ptr& operator = ( T* p ){
            Ptr temp( p );
            swap( temp );
            return *this;
        }
        Ptr& operator = ( const Ptr& other ){
            Ptr temp( other );
            swap( temp );
            return *this;
        }
        void swap( Ptr& other ){
            std::swap( m_p, other.m_p );
        }

        T* get(){
            return m_p;
        }
        const T* get() const{
            return m_p;
        }

        T& operator*() const {
            return *m_p;
        }

        T* operator->() const {
            return m_p;
        }

        bool operator !() const {
            return m_p == NULL;
        }

    private:
        T* m_p;
    };

    struct IShared : NonCopyable {
        virtual ~IShared();
        virtual void addRef() = 0;
        virtual void release() = 0;
    };

    template<typename T>
    struct SharedImpl : T {

        SharedImpl() : m_rc( 0 ){}

        virtual void addRef(){
            ++m_rc;
        }
        virtual void release(){
            if( --m_rc == 0 )
                delete this;
        }

        int m_rc;
    };

} // end namespace Catch

#include <vector>

namespace Catch {

    class TestCaseFilters;

    struct ITestCase : IShared {
        virtual void invoke () const = 0;
    protected:
        virtual ~ITestCase();
    };

    class TestCaseInfo;

    struct ITestCaseRegistry {
        virtual ~ITestCaseRegistry();
        virtual const std::vector<TestCaseInfo>& getAllTests() const = 0;
        virtual std::vector<TestCaseInfo> getMatchingTestCases( const std::string& rawTestSpec ) const = 0;
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

struct AutoReg {

    AutoReg(    TestFunction function,
                const char* name,
                const char* description,
                const SourceLineInfo& lineInfo );

    template<typename C>
    AutoReg(    void (C::*method)(),
                const char* name,
                const char* description,
                const SourceLineInfo& lineInfo ) {
        registerTestCase( new MethodTestCase<C>( method ), name, description, lineInfo );
    }

    void registerTestCase(  ITestCase* testCase,
                            const char* name,
                            const char* description,
                            const SourceLineInfo& lineInfo );

    ~AutoReg();

private:
    AutoReg( const AutoReg& );
    void operator= ( const AutoReg& );
};

} // end namespace Catch

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TESTCASE( Name, Desc ) \
    static void INTERNAL_CATCH_UNIQUE_NAME( TestCaseFunction_catch_internal_ )(); \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ ), Name, Desc, CATCH_INTERNAL_LINEINFO ); }\
    static void INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ )()

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TESTCASE_NORETURN( Name, Desc ) \
    static void INTERNAL_CATCH_UNIQUE_NAME( TestCaseFunction_catch_internal_ )() CATCH_ATTRIBUTE_NORETURN; \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ ), Name, Desc, CATCH_INTERNAL_LINEINFO ); }\
    static void INTERNAL_CATCH_UNIQUE_NAME(  TestCaseFunction_catch_internal_ )()

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, Name, Desc ) \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &QualifiedMethod, Name, Desc, CATCH_INTERNAL_LINEINFO ); }

///////////////////////////////////////////////////////////////////////////////
#define TEST_CASE_METHOD( ClassName, TestName, Desc )\
    namespace{ \
        struct INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ ) : ClassName{ \
            void test(); \
        }; \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( &INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ )::test, TestName, Desc, CATCH_INTERNAL_LINEINFO ); \
    } \
    void INTERNAL_CATCH_UNIQUE_NAME( TestCaseMethod_catch_internal_ )::test()

// #included from: internal/catch_capture.hpp
#define TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED

// #included from: catch_expression_builder.hpp
#define TWOBLUECUBES_CATCH_EXPRESSION_BUILDER_HPP_INCLUDED

// #included from: catch_expression.hpp
#define TWOBLUECUBES_CATCH_EXPRESSION_HPP_INCLUDED

// #included from: catch_resultinfo_builder.h
#define TWOBLUECUBES_CATCH_RESULTINFO_BUILDER_H_INCLUDED

// #included from: catch_tostring.hpp
#define TWOBLUECUBES_CATCH_TOSTRING_HPP_INCLUDED

#include <sstream>

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

    // If the type does not have its own << overload for ostream then
    // this one will be used instead
    inline std::ostream& operator << ( std::ostream& ss, NonStreamable ){
        return ss << "{?}";
    }

    template<typename T>
    inline std::string makeString( const T& value ) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    template<typename T>
    inline std::string makeString( T* p ) {
        if( !p )
            return INTERNAL_CATCH_STRINGIFY( NULL );
        std::ostringstream oss;
        oss << p;
        return oss.str();
    }

    template<typename T>
    inline std::string makeString( const T* p ) {
        if( !p )
            return INTERNAL_CATCH_STRINGIFY( NULL );
        std::ostringstream oss;
        oss << p;
        return oss.str();
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
    return Detail::makeString( value );
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
    oss << value;
    return oss.str();
}

inline std::string toString( bool value ) {
    return value ? "true" : "false";
}

inline std::string toString( char value ) {
    return value < ' '
        ? toString( (unsigned int)value )
        : Detail::makeString( value );
}

inline std::string toString( signed char value ) {
    return toString( static_cast<char>( value ) );
}

#ifdef CATCH_CONFIG_CPP11_NULLPTR
inline std::string toString( std::nullptr_t ) {
    return "nullptr";
}
#endif

#ifdef __OBJC__
    inline std::string toString( NSString const * const& nsstring ) {
        return std::string( "@\"" ) + [nsstring UTF8String] + "\"";
    }
    inline std::string toString( NSString * CATCH_ARC_STRONG const& nsstring ) {
        return std::string( "@\"" ) + [nsstring UTF8String] + "\"";
    }
    inline std::string toString( NSObject* const& nsObject ) {
        return toString( [nsObject description] );
    }
#endif

} // end namespace Catch

// #included from: catch_resultinfo.h
#define TWOBLUECUBES_CATCH_RESULT_INFO_H_INCLUDED

#include <string>
// #included from: catch_result_type.h
#define TWOBLUECUBES_CATCH_RESULT_TYPE_H_INCLUDED

namespace Catch {

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

struct ResultAction { enum Value {
    None,
    Failed = 1, // Failure - but no debug break if Debug bit not set
    Debug = 2,  // If this bit is set, invoke the debugger
    Abort = 4   // Test run should abort
}; };

}


namespace Catch {

    class ResultInfo {
    public:
        ResultInfo();
        ResultInfo( const char* expr,
                    ResultWas::OfType result,
                    bool isNot,
                    const SourceLineInfo& lineInfo,
                    const char* macroName,
                   const char* message );
        ~ResultInfo();

        bool ok() const;
        ResultWas::OfType getResultType() const;
        bool hasExpression() const;
        bool hasMessage() const;
        std::string getExpression() const;
        bool hasExpandedExpression() const;
        std::string getExpandedExpression() const;
        std::string getMessage() const;
        std::string getFilename() const;
        std::size_t getLine() const;
        std::string getTestMacroName() const;

    protected:

        std::string getExpandedExpressionInternal() const;
        bool isNotExpression( const char* expr );

    protected:
        std::string m_macroName;
        SourceLineInfo m_lineInfo;
        std::string m_expr, m_lhs, m_rhs, m_op;
        std::string m_message;
        ResultWas::OfType m_result;
        bool m_isNot;
    };

} // end namespace Catch

// #included from: catch_evaluate.hpp
#define TWOBLUECUBES_CATCH_EVALUATE_HPP_INCLUDED

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
    template<Operator Op, typename T>
    bool compare( long lhs, const T* rhs ) {
        return Evaluator<const T*, const T*, Op>::evaluate( reinterpret_cast<const T*>( lhs ), rhs );
    }

    template<Operator Op, typename T>
    bool compare( long lhs, T* rhs ) {
        return Evaluator<T*, T*, Op>::evaluate( reinterpret_cast<T*>( lhs ), rhs );
    }

    template<Operator Op, typename T>
    bool compare( const T* lhs, long rhs ) {
        return Evaluator<const T*, const T*, Op>::evaluate( lhs, reinterpret_cast<const T*>( rhs ) );
    }

    template<Operator Op, typename T>
    bool compare( T* lhs, long rhs ) {
        return Evaluator<T*, T*, Op>::evaluate( lhs, reinterpret_cast<T*>( rhs ) );
    }

    // pointer to int (when comparing against NULL)
    template<Operator Op, typename T>
    bool compare( int lhs, const T* rhs ) {
        return Evaluator<const T*, const T*, Op>::evaluate( reinterpret_cast<const T*>( lhs ), rhs );
    }

    template<Operator Op, typename T>
    bool compare( int lhs, T* rhs ) {
        return Evaluator<T*, T*, Op>::evaluate( reinterpret_cast<T*>( lhs ), rhs );
    }

    template<Operator Op, typename T>
    bool compare( const T* lhs, int rhs ) {
        return Evaluator<const T*, const T*, Op>::evaluate( lhs, reinterpret_cast<const T*>( rhs ) );
    }

    template<Operator Op, typename T>
    bool compare( T* lhs, int rhs ) {
        return Evaluator<T*, T*, Op>::evaluate( lhs, reinterpret_cast<T*>( rhs ) );
    }

} // end of namespace Internal
} // end of namespace Catch

namespace Catch {

struct STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison;

class ResultInfoBuilder : public ResultInfo {

public:

    ResultInfoBuilder();

    ResultInfoBuilder(  const char* expr,
                        bool isNot,
                        const SourceLineInfo& lineInfo,
                        const char* macroName,
                        const char* message = "" );

    void setResultType( ResultWas::OfType result );
    void setMessage( const std::string& message );
    void setLineInfo( const SourceLineInfo& lineInfo );
    void setLhs( const std::string& lhs );
    void setRhs( const std::string& rhs );
    void setOp( const std::string& op );

    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator ||
    (
        const RhsT&
    );

    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator &&
    (
        const RhsT&
    );

private:
    friend class ExpressionBuilder;
    template<typename T> friend class Expression;

    template<typename T> friend class PtrExpression;

    ResultInfoBuilder& captureBoolExpression( bool result );

    template<Internal::Operator Op, typename T1, typename T2>
    ResultInfoBuilder& captureExpression( const T1& lhs, const T2& rhs ) {
        setResultType( Internal::compare<Op>( lhs, rhs ) ? ResultWas::Ok : ResultWas::ExpressionFailed );
        m_lhs = Catch::toString( lhs );
        m_rhs = Catch::toString( rhs );
        m_op = Internal::OperatorTraits<Op>::getName();
        return *this;
    }

    template<Internal::Operator Op, typename T>
    ResultInfoBuilder& captureExpression( const T* lhs, int rhs ) {
        return captureExpression<Op>( lhs, reinterpret_cast<const T*>( rhs ) );
    }
};

} // end namespace Catch

namespace Catch {

template<typename T>
class Expression {
	void operator = ( const Expression& );

public:
    Expression( ResultInfoBuilder& result, T lhs )
    :   m_result( result ),
        m_lhs( lhs )
    {}

    template<typename RhsT>
    ResultInfoBuilder& operator == ( const RhsT& rhs ) {
        return m_result.captureExpression<Internal::IsEqualTo>( m_lhs, rhs );
    }

    template<typename RhsT>
    ResultInfoBuilder& operator != ( const RhsT& rhs ) {
        return m_result.captureExpression<Internal::IsNotEqualTo>( m_lhs, rhs );
    }

    template<typename RhsT>
    ResultInfoBuilder& operator < ( const RhsT& rhs ) {
        return m_result.captureExpression<Internal::IsLessThan>( m_lhs, rhs );
    }

    template<typename RhsT>
    ResultInfoBuilder& operator > ( const RhsT& rhs ) {
        return m_result.captureExpression<Internal::IsGreaterThan>( m_lhs, rhs );
    }

    template<typename RhsT>
    ResultInfoBuilder& operator <= ( const RhsT& rhs ) {
        return m_result.captureExpression<Internal::IsLessThanOrEqualTo>( m_lhs, rhs );
    }

    template<typename RhsT>
    ResultInfoBuilder& operator >= ( const RhsT& rhs ) {
        return m_result.captureExpression<Internal::IsGreaterThanOrEqualTo>( m_lhs, rhs );
    }

    ResultInfoBuilder& operator == ( bool rhs ) {
        return m_result.captureExpression<Internal::IsEqualTo>( m_lhs, rhs );
    }

    ResultInfoBuilder& operator != ( bool rhs ) {
        return m_result.captureExpression<Internal::IsNotEqualTo>( m_lhs, rhs );
    }

    operator ResultInfoBuilder& () {
        return m_result.captureBoolExpression( m_lhs );
    }

    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator + ( const RhsT& );

    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator - ( const RhsT& );

private:
    ResultInfoBuilder& m_result;
    T m_lhs;
};

} // end namespace Catch

#include <sstream>

namespace Catch {

class ExpressionBuilder {
public:

    ExpressionBuilder(  const SourceLineInfo& lineInfo,
                        const char* macroName,
                        const char* expr = "",
                        bool isNot = false )
    : m_result( expr, isNot, lineInfo, macroName ),
      m_messageStream()
    {}

    template<typename T>
    Expression<const T&> operator->* ( const T & operand ) {
        Expression<const T&> expr( m_result, operand );
        return expr;
    }

    Expression<bool> operator->* ( bool value ) {
        Expression<bool> expr( m_result, value );
        return expr;
    }

    template<typename T>
    ExpressionBuilder& operator << ( const T & value ) {
        m_messageStream << Catch::toString( value );
        return *this;
    }

    template<typename MatcherT, typename ArgT>
    ExpressionBuilder& acceptMatcher(   const MatcherT& matcher,
                                        const ArgT& arg,
                                        const std::string& matcherCallAsString ) {
        std::string matcherAsString = Catch::toString( matcher );
        if( matcherAsString == "{?}" )
            matcherAsString = matcherCallAsString;
        m_result.setLhs( Catch::toString( arg ) );
        m_result.setRhs( matcherAsString );
        m_result.setOp( "matches" );
        m_result.setResultType( matcher( arg ) ? ResultWas::Ok : ResultWas::ExpressionFailed );
        return *this;
    }

    template<typename MatcherT, typename ArgT>
    ExpressionBuilder& acceptMatcher(   const MatcherT& matcher,
                                        ArgT* arg,
                                        const std::string& matcherCallAsString ) {
        std::string matcherAsString = Catch::toString( matcher );
        if( matcherAsString == "{?}" )
            matcherAsString = matcherCallAsString;
        m_result.setLhs( Catch::toString( arg ) );
        m_result.setRhs( matcherAsString );
        m_result.setOp( "matches" );
        m_result.setResultType( matcher( arg ) ? ResultWas::Ok : ResultWas::ExpressionFailed );
        return *this;
    }

    ExpressionBuilder& setResultType( ResultWas::OfType resultType ) {
        m_result.setResultType( resultType );
        return *this;
    }

    operator ResultInfoBuilder&() {
        m_result.setMessage( m_messageStream.str() );
        return m_result;
    }

private:
    ResultInfoBuilder m_result;
    std::ostringstream m_messageStream;
};

} // end namespace Catch

// #included from: catch_interfaces_capture.h
#define TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED

#include <string>
// #included from: catch_totals.hpp
#define TWOBLUECUBES_CATCH_TOTALS_HPP_INCLUDED

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

    class TestCaseInfo;
    class ScopedInfo;
    class ResultInfoBuilder;
    class ResultInfo;

    struct IResultCapture {

        virtual ~IResultCapture();

        virtual void testEnded( const ResultInfo& result ) = 0;
        virtual bool sectionStarted(    const std::string& name,
                                        const std::string& description,
                                        const SourceLineInfo& lineInfo,
                                        Counts& assertions ) = 0;
        virtual void sectionEnded( const std::string& name, const Counts& assertions ) = 0;
        virtual void pushScopedInfo( ScopedInfo* scopedInfo ) = 0;
        virtual void popScopedInfo( ScopedInfo* scopedInfo ) = 0;
        virtual bool shouldDebugBreak() const = 0;

        virtual ResultAction::Value acceptResult( bool result ) = 0;
        virtual ResultAction::Value acceptResult( ResultWas::OfType result ) = 0;
        virtual ResultAction::Value acceptExpression( const ResultInfoBuilder& resultInfo ) = 0;
        virtual void acceptMessage( const std::string& msg ) = 0;

        virtual std::string getCurrentTestName() const = 0;
        virtual const ResultInfo* getLastResult() const = 0;
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

// #included from: internal/catch_section.hpp
#define TWOBLUECUBES_CATCH_SECTION_HPP_INCLUDED

#include <string>

namespace Catch {

    class Section {
    public:
        Section(    const std::string& name,
                    const std::string& description,
                    const SourceLineInfo& lineInfo )
        :   m_name( name ),
            m_sectionIncluded( getCurrentContext().getResultCapture().sectionStarted( name, description, lineInfo, m_assertions ) )
        {}

        ~Section() {
            if( m_sectionIncluded )
                getCurrentContext().getResultCapture().sectionEnded( m_name, m_assertions );
        }

        // This indicates whether the section should be executed or not
        operator bool() {
            return m_sectionIncluded;
        }

    private:

        std::string m_name;
        Counts m_assertions;
        bool m_sectionIncluded;
    };

} // end namespace Catch

#define INTERNAL_CATCH_SECTION( name, desc ) \
    if( Catch::Section INTERNAL_CATCH_UNIQUE_NAME( catch_internal_Section ) = Catch::Section( name, desc, CATCH_INTERNAL_LINEINFO ) )

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
        return m_from+static_cast<T>( index );
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
#define TWOBLUECUBES_CATCH_INTERFACES_EXCEPTIONS_H_INCLUDED

#include <string>
// #included from: catch_interfaces_registry_hub.h
#define TWOBLUECUBES_CATCH_INTERFACES_REGISTRY_HUB_H_INCLUDED

// #included from: catch_interfaces_reporter.h
#define TWOBLUECUBES_CATCH_IREPORTERREGISTRY_INCLUDED

// #included from: catch_config.hpp
#define TWOBLUECUBES_CATCH_RUNNERCONFIG_HPP_INCLUDED

// #included from: catch_test_spec.h
#define TWOBLUECUBES_CATCH_TESTSPEC_H_INCLUDED

// #included from: catch_test_case_info.h
#define TWOBLUECUBES_CATCH_TESTCASEINFO_H_INCLUDED

#include <string>
#include <set>

namespace Catch {

    struct ITestCase;

    class TestCaseInfo {
    public:
        TestCaseInfo();

        TestCaseInfo(   ITestCase* testCase,
                        const char* name,
                        const char* description,
                        const SourceLineInfo& lineInfo );

        TestCaseInfo( const TestCaseInfo& other, const std::string& name );
        TestCaseInfo( const TestCaseInfo& other );

        void invoke() const;
        const std::string& getName() const;
        const std::string& getDescription() const;
        const SourceLineInfo& getLineInfo() const;
        bool isHidden() const;
        bool hasTag( const std::string& tag ) const;
        const std::set<std::string>& tags() const;

        void swap( TestCaseInfo& other );
        bool operator == ( const TestCaseInfo& other ) const;
        bool operator < ( const TestCaseInfo& other ) const;
        TestCaseInfo& operator = ( const TestCaseInfo& other );

    private:
        Ptr<ITestCase> m_test;
        std::string m_name;
        std::string m_description;
        std::set<std::string> m_tags;
        SourceLineInfo m_lineInfo;
        bool m_isHidden;
    };
}

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
        :   m_stringToMatch( testSpec ),
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

            if( m_stringToMatch[0] == '*' ) {
                m_stringToMatch = m_stringToMatch.substr( 1 );
                m_wildcardPosition = (WildcardPosition)( m_wildcardPosition | WildcardAtStart );
            }
            if( m_stringToMatch[m_stringToMatch.size()-1] == '*' ) {
                m_stringToMatch = m_stringToMatch.substr( 0, m_stringToMatch.size()-1 );
                m_wildcardPosition = (WildcardPosition)( m_wildcardPosition | WildcardAtEnd );
            }
        }

        IfFilterMatches::DoWhat getFilterType() const {
            return m_filterType;
        }

        bool shouldInclude( const TestCaseInfo& testCase ) const {
            return isMatch( testCase ) == (m_filterType == IfFilterMatches::IncludeTests);
        }
    private:

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#endif

        bool isMatch( const TestCaseInfo& testCase ) const {
            const std::string& name = testCase.getName();

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

        bool shouldInclude( const TestCaseInfo& testCase ) const {
            if( !m_inclusionFilters.empty() ) {
                std::vector<TestCaseFilter>::const_iterator it = m_inclusionFilters.begin();
                std::vector<TestCaseFilter>::const_iterator itEnd = m_inclusionFilters.end();
                for(; it != itEnd; ++it )
                    if( it->shouldInclude( testCase ) )
                        break;
                if( it == itEnd )
                    return false;
            }
            else if( m_exclusionFilters.empty() ) {
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

#include <memory>
#include <vector>
#include <string>
#include <iostream>

namespace Catch {

    struct Include { enum WhichResults {
        FailedOnly,
        SuccessfulResults
    }; };

    struct List{ enum What {
        None = 0,

        Reports = 1,
        Tests = 2,
        All = 3,

        TestNames = 6,

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
        :   m_streambuf( NULL ),
            m_os( std::cout.rdbuf() )
        {}

        Config( const ConfigData& data )
        :   m_data( data ),
            m_streambuf( NULL ),
            m_os( std::cout.rdbuf() )
        {}

        virtual ~Config() {
            m_os.rdbuf( std::cout.rdbuf() );
            delete m_streambuf;
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
            std::streambuf* newBuf = createStreamBuf( streamName );
            setStreamBuf( newBuf );
            delete m_streambuf;
            m_streambuf = newBuf;
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
        std::streambuf* m_streambuf;
        mutable std::ostream m_os;
    };

} // end namespace Catch

#include <string>
#include <ostream>
#include <map>

namespace Catch
{
    struct ReporterConfig
    {
        ReporterConfig( const std::string& _name,
                        std::ostream& _stream,
                        bool _includeSuccessfulResults,
                        const ConfigData& _fullConfig )
        :   name( _name ),
            stream( _stream ),
            includeSuccessfulResults( _includeSuccessfulResults ),
            fullConfig( _fullConfig )
        {}

        ReporterConfig( const ReporterConfig& other )
        :   name( other.name ),
            stream( other.stream ),
            includeSuccessfulResults( other.includeSuccessfulResults ),
            fullConfig( other.fullConfig )
        {}

        std::string name;
        std::ostream& stream;
        bool includeSuccessfulResults;
        ConfigData fullConfig;

    private:
        void operator=(const ReporterConfig&);
    };

    class TestCaseInfo;
    class ResultInfo;

    struct IReporter : IShared {
        virtual ~IReporter();
        virtual bool shouldRedirectStdout() const = 0;
        virtual void StartTesting() = 0;
        virtual void EndTesting( const Totals& totals ) = 0;
        virtual void StartGroup( const std::string& groupName ) = 0;
        virtual void EndGroup( const std::string& groupName, const Totals& totals ) = 0;
        virtual void StartSection( const std::string& sectionName, const std::string& description ) = 0;
        virtual void NoAssertionsInSection( const std::string& sectionName ) = 0;
        virtual void NoAssertionsInTestCase( const std::string& testName ) = 0;
        virtual void EndSection( const std::string& sectionName, const Counts& assertions ) = 0;
        virtual void StartTestCase( const TestCaseInfo& testInfo ) = 0;
        virtual void Aborted() = 0;
        virtual void EndTestCase( const TestCaseInfo& testInfo, const Totals& totals, const std::string& stdOut, const std::string& stdErr ) = 0;
        virtual void Result( const ResultInfo& result ) = 0;
    };

    struct IReporterFactory {
        virtual ~IReporterFactory();
        virtual IReporter* create( const ReporterConfig& config ) const = 0;
        virtual std::string getDescription() const = 0;
    };

    struct IReporterRegistry {
        typedef std::map<std::string, IReporterFactory*> FactoryMap;

        virtual ~IReporterRegistry();
        virtual IReporter* create( const std::string& name, const ReporterConfig& config ) const = 0;
        virtual const FactoryMap& getFactories() const = 0;
    };

    inline std::string trim( const std::string& str ) {
        std::string::size_type start = str.find_first_not_of( "\n\r\t " );
        std::string::size_type end = str.find_last_not_of( "\n\r\t " );

        return start != std::string::npos ? str.substr( start, 1+end-start ) : "";
    }
}

#include <vector>

namespace Catch {

    class TestCaseInfo;
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
        virtual void registerTest( const TestCaseInfo& testInfo ) = 0;
        virtual void registerTranslator( const IExceptionTranslator* translator ) = 0;
    };

    IRegistryHub& getRegistryHub();
    IMutableRegistryHub& getMutableRegistryHub();
    void cleanUp();
}


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
            oss << "Approx( " << m_value << ")";
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
    namespace StdString {

        struct Equals {
            Equals( const std::string& str ) : m_str( str ){}

            bool operator()( const std::string& str ) const
            {
                return str == m_str;
            }

            friend std::ostream& operator<<( std::ostream& os, const Equals& matcher )
            {
                os << "equals: \"" << matcher.m_str << "\"";
                return os;
            }
            std::string m_str;
        };

        struct Contains {
            Contains( const std::string& substr ) : m_substr( substr ){}

            bool operator()( const std::string& str ) const
            {
                return str.find( m_substr ) != std::string::npos;
            }

            friend std::ostream& operator<<( std::ostream& os, const Contains& matcher )
            {
                os << "contains: \"" << matcher.m_substr << "\"";
                return os;
            }
            std::string m_substr;
        };

        struct StartsWith {
            StartsWith( const std::string& substr ) : m_substr( substr ){}

            bool operator()( const std::string& str ) const
            {
                return str.find( m_substr ) == 0;
            }

            friend std::ostream& operator<<( std::ostream& os, const StartsWith& matcher )
            {
                os << "starts with: \"" << matcher.m_substr << "\"";
                return os;
            }
            std::string m_substr;
        };

        struct EndsWith {
            EndsWith( const std::string& substr ) : m_substr( substr ){}

            bool operator()( const std::string& str ) const
            {
                return str.find( m_substr ) == str.size() - m_substr.size();
            }

            friend std::ostream& operator<<( std::ostream& os, const EndsWith& matcher )
            {
                os << "ends with: \"" << matcher.m_substr << "\"";
                return os;
            }
            std::string m_substr;
        };
    } // namespace StdString
    } // namespace Impl

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
#define TWOBLUECUBES_INTERNAL_CATCH_INTERFACES_RUNNER_H_INCLUDED

#include <string>

namespace Catch {
    class TestCaseInfo;

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

                        getMutableRegistryHub().registerTest( TestCaseInfo( new OcMethod( cls, selector ), name.c_str(), desc.c_str(), SourceLineInfo() ) );
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

            struct StringHolder {
                StringHolder( NSString* substr ) : m_substr( [substr copy] ){}
                StringHolder() {
                    arcSafeRelease( m_substr );
                }

                NSString* m_substr;
            };

            struct Equals : StringHolder {
                Equals( NSString* substr ) : StringHolder( substr ){}

                bool operator()( NSString* str ) const {
                    return [str isEqualToString:m_substr];
                }

                friend std::ostream& operator<<( std::ostream& os, const Equals& matcher ) {
                    os << "equals string: " << Catch::toString( matcher.m_substr );
                    return os;
                }
            };

            struct Contains : StringHolder {
                Contains( NSString* substr ) : StringHolder( substr ){}

                bool operator()( NSString* str ) const {
                    return [str rangeOfString:m_substr].location != NSNotFound;
                }

                friend std::ostream& operator<<( std::ostream& os, const Contains& matcher ) {
                    os << "contains: " << Catch::toString( matcher.m_substr );
                    return os;
                }
            };

            struct StartsWith : StringHolder {
                StartsWith( NSString* substr ) : StringHolder( substr ){}

                bool operator()( NSString* str ) const {
                    return [str rangeOfString:m_substr].location == 0;
                }

                friend std::ostream& operator<<( std::ostream& os, const StartsWith& matcher ) {
                    os << "starts with: " << Catch::toString( matcher.m_substr );
                    return os;
                }
            };
            struct EndsWith : StringHolder {
                EndsWith( NSString* substr ) : StringHolder( substr ){}

                bool operator()( NSString* str ) const {
                    return [str rangeOfString:m_substr].location == [str length] - [m_substr length];
                }

                friend std::ostream& operator<<( std::ostream& os, const EndsWith& matcher ) {
                    os << "ends with: " << Catch::toString( matcher.m_substr );
                    return os;
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
            return m_argv[0];
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
        virtual std::string optionDescription() const { return ""; };

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
                return "[<option for help on>]";
            }
            virtual std::string optionSummary() const {
                return "Shows this usage summary, or help on a specific option, if supplied";
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
                    "inclusion spec included them. Subsequent inclusion specs will take precendence, however. "
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
                for( std::size_t i = 0; i < cmd.argsCount(); ++i )
                    filters.addFilter( TestCaseFilter( cmd[i] ) );
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
                return "[all | tests | reporters [xml]]";
            }
            virtual std::string optionSummary() const {
                return "Lists available tests or reporters";
            }

            virtual std::string optionDescription() const {
                return
                    "With no arguments this option will list all registered tests - one per line.\n"
                    "Supplying the xml argument formats the list as an xml document (which may be useful for "
                    "consumption by other tools).\n"
                    "Supplying the tests or reporters lists tests or reporters respectively - with descriptions.\n"
                    "\n"
                    "Examples:\n"
                    "\n"
                    "    -l\n"
                    "    -l tests\n"
                    "    -l reporters xml\n"
                    "    -l xml";
            }

            virtual void parseIntoConfig( const Command& cmd, ConfigData& config ) {
                config.listSpec = List::TestNames;
                if( cmd.argsCount() >= 1 ) {
                    if( cmd[0] == "all" )
                        config.listSpec = List::All;
                    else if( cmd[0] == "tests" )
                        config.listSpec = List::Tests;
                    else if( cmd[0] == "reporters" )
                        config.listSpec = List::Reports;
                    else
                        cmd.raiseError( "Expected [tests] or [reporters]" );
                }
                if( cmd.argsCount() >= 2 ) {
                    if( cmd[1] == "xml" )
                        config.listSpec = static_cast<List::What>( config.listSpec | List::AsXml );
                    else if( cmd[1] == "text" )
                        config.listSpec = static_cast<List::What>( config.listSpec | List::AsText );
                    else
                        cmd.raiseError( "Expected [xml] or [text]" );
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
                    "tests, and potentially summarises the results. By default a basic reporter "
                    "is used that writes IDE friendly results. CATCH comes bundled with some "
                    "alternative reporters, but more can be added in client code.\n"
                    "\n"
                    "The bundled reporters are:\n"
                    "    -r basic\n"
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
                    "sent to stdout (note that uses ofstdout and stderr from within test cases are "
                    "redirected and included in the report - so even stderr will effectively end up "
                    "on stdout). If the name begins with % it is interpretted as a stream. "
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
                    "added worked first time!). To see successul, as well as failing, test results "
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

#include <limits>

namespace Catch {
    inline bool matchesFilters( const std::vector<TestCaseFilters>& filters, const TestCaseInfo& testCase ) {
        std::vector<TestCaseFilters>::const_iterator it = filters.begin();
        std::vector<TestCaseFilters>::const_iterator itEnd = filters.end();
        for(; it != itEnd; ++it )
            if( !it->shouldInclude( testCase ) )
                return false;
        return true;
    }
    inline void List( const ConfigData& config ) {

        if( config.listSpec & List::Reports ) {
            std::cout << "Available reports:\n";
            IReporterRegistry::FactoryMap::const_iterator it = getRegistryHub().getReporterRegistry().getFactories().begin();
            IReporterRegistry::FactoryMap::const_iterator itEnd = getRegistryHub().getReporterRegistry().getFactories().end();
            for(; it != itEnd; ++it ) {
                // !TBD: consider listAs()
                std::cout << "\t" << it->first << "\n\t\t'" << it->second->getDescription() << "'\n";
            }
            std::cout << std::endl;
        }

        if( config.listSpec & List::Tests ) {
            if( config.filters.empty() )
                std::cout << "All available test cases:\n";
            else
                std::cout << "Matching test cases:\n";
            std::vector<TestCaseInfo>::const_iterator it = getRegistryHub().getTestCaseRegistry().getAllTests().begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = getRegistryHub().getTestCaseRegistry().getAllTests().end();
            std::size_t matchedTests = 0;
            for(; it != itEnd; ++it ) {
                if( matchesFilters( config.filters, *it ) ) {
                    matchedTests++;
                    // !TBD: consider listAs()
                    std::cout << "\t" << it->getName() << "\n";
                    if( ( config.listSpec & List::TestNames ) != List::TestNames )
                        std::cout << "\t\t '" << it->getDescription() << "'\n";
                }
            }
            if( config.filters.empty() )
                std::cout << pluralise( matchedTests, "test case" ) << std::endl;
            else
                std::cout << pluralise( matchedTests, "matching test case" ) << std::endl;
        }

        if( ( config.listSpec & List::All ) == 0 ) {
            std::ostringstream oss;
            oss << "Unknown list type";
            throw std::domain_error( oss.str() );
        }
    }

} // end namespace Catch

// #included from: internal/catch_runner_impl.hpp
#define TWOBLUECUBES_INTERNAL_CATCH_RUNNER_HPP_INCLUDED

// #included from: catch_running_test.hpp
#define TWOBLUECUBES_INTERNAL_CATCH_RUNNING_TEST_HPP_INCLUDED

// #included from: catch_section_info.hpp
#define TWOBLUECUBES_INTERNAL_CATCH_SECTION_INFO_HPP_INCLUDED

#include <map>
#include <string>

namespace Catch {

    class SectionInfo {
    public:

        enum Status {
            Root,
            Unknown,
            Branch,
            TestedBranch,
            TestedLeaf
        };

        SectionInfo( SectionInfo* parent )
        :   m_status( Unknown ),
            m_parent( parent )
        {}

        SectionInfo()
        :   m_status( Root ),
            m_parent( NULL )
        {}

        ~SectionInfo() {
            deleteAllValues( m_subSections );
        }

        bool shouldRun() const {
            return m_status < TestedBranch;
        }

        bool ran() {
            if( m_status < Branch ) {
                m_status = TestedLeaf;
                return true;
            }
            return false;
        }

        bool isBranch() const {
            return m_status == Branch;
        }

        void ranToCompletion() {
            if( m_status == Branch && !hasUntestedSections() )
                m_status = TestedBranch;
        }

        SectionInfo* findSubSection( const std::string& name ) {
            std::map<std::string, SectionInfo*>::const_iterator it = m_subSections.find( name );
            return it != m_subSections.end()
                        ? it->second
                        : NULL;
        }

        SectionInfo* addSubSection( const std::string& name ) {
            SectionInfo* subSection = new SectionInfo( this );
            m_subSections.insert( std::make_pair( name, subSection ) );
            m_status = Branch;
            return subSection;
        }

        SectionInfo* getParent() {
            return m_parent;
        }

        bool hasUntestedSections() const {
            if( m_status == Unknown )
                return true;

            std::map<std::string, SectionInfo*>::const_iterator it = m_subSections.begin();
            std::map<std::string, SectionInfo*>::const_iterator itEnd = m_subSections.end();
            for(; it != itEnd; ++it ) {
                if( it->second->hasUntestedSections() )
                    return true;
            }
            return false;
        }

    private:
        Status m_status;
        std::map<std::string, SectionInfo*> m_subSections;
        SectionInfo* m_parent;
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
        explicit RunningTest( const TestCaseInfo* info = NULL )
        :   m_info( info ),
            m_runStatus( RanAtLeastOneSection ),
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
            if( m_runStatus == RanAtLeastOneSection ||
                m_runStatus == EncounteredASection ) {
                m_runStatus = RanToCompletionWithSections;
                if( m_lastSectionToRun ) {
                    m_lastSectionToRun->ranToCompletion();
                    m_changed = true;
                }
            }
            else {
                m_runStatus = RanToCompletionWithNoSections;
            }
        }

        bool addSection( const std::string& name ) {
            if( m_runStatus == NothingRun )
                m_runStatus = EncounteredASection;

            SectionInfo* thisSection = m_currentSection->findSubSection( name );
            if( !thisSection ) {
                thisSection = m_currentSection->addSubSection( name );
                m_changed = true;
            }

            if( !wasSectionSeen() && thisSection->shouldRun() ) {
                m_currentSection = thisSection;
                m_lastSectionToRun = NULL;
                return true;
            }
            return false;
        }

        void endSection( const std::string& ) {
            if( m_currentSection->ran() ) {
                m_runStatus = RanAtLeastOneSection;
                m_changed = true;
            }
            else if( m_runStatus == EncounteredASection ) {
                m_runStatus = RanAtLeastOneSection;
                m_lastSectionToRun = m_currentSection;
            }
            m_currentSection = m_currentSection->getParent();
        }

        const TestCaseInfo& getTestCaseInfo() const {
            return *m_info;
        }

        bool hasUntestedSections() const {
            return  m_runStatus == RanAtLeastOneSection ||
                    ( m_rootSection.hasUntestedSections() && m_changed );
        }

    private:
        const TestCaseInfo* m_info;
        RunStatus m_runStatus;
        SectionInfo m_rootSection;
        SectionInfo* m_currentSection;
        SectionInfo* m_lastSectionToRun;
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

        explicit Runner( const Config& config, const Ptr<IReporter>& reporter )
        :   m_context( getCurrentMutableContext() ),
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
            m_reporter->StartTesting();
        }

        virtual ~Runner() {
            m_reporter->EndTesting( m_totals );
            m_context.setRunner( m_prevRunner );
            m_context.setConfig( NULL );
            m_context.setResultCapture( m_prevResultCapture );
            m_context.setConfig( m_prevConfig );
        }

        Totals runMatching( const std::string& testSpec ) {

            std::vector<TestCaseInfo> matchingTests = getRegistryHub().getTestCaseRegistry().getMatchingTestCases( testSpec );

            Totals totals;

            m_reporter->StartGroup( testSpec );

            std::vector<TestCaseInfo>::const_iterator it = matchingTests.begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = matchingTests.end();
            for(; it != itEnd; ++it )
                totals += runTest( *it );
            // !TBD use std::accumulate?

            m_reporter->EndGroup( testSpec, totals );
            return totals;
        }

        Totals runTest( const TestCaseInfo& testInfo ) {
            Totals prevTotals = m_totals;

            std::string redirectedCout;
            std::string redirectedCerr;

            m_reporter->StartTestCase( testInfo );

            m_runningTest = new RunningTest( &testInfo );

            do {
                do {
                    m_currentResult.setLineInfo( m_runningTest->getTestCaseInfo().getLineInfo() );
                    runCurrentTest( redirectedCout, redirectedCerr );
                }
                while( m_runningTest->hasUntestedSections() && !aborting() );
            }
            while( getCurrentContext().advanceGeneratorsForCurrentTest() && !aborting() );

            delete m_runningTest;
            m_runningTest = NULL;

            Totals deltaTotals = m_totals.delta( prevTotals );
            m_totals.testCases += deltaTotals.testCases;
            m_reporter->EndTestCase( testInfo, deltaTotals, redirectedCout, redirectedCerr );
            return deltaTotals;
        }

        const Config& config() const {
            return m_config;
        }

    private: // IResultCapture

        virtual ResultAction::Value acceptResult( bool result ) {
            return acceptResult( result ? ResultWas::Ok : ResultWas::ExpressionFailed );
        }

        virtual ResultAction::Value acceptResult( ResultWas::OfType result ) {
            m_currentResult.setResultType( result );
            return actOnCurrentResult();
        }

        virtual ResultAction::Value acceptExpression( const ResultInfoBuilder& resultInfo ) {
            m_currentResult = resultInfo;
            return actOnCurrentResult();
        }

        virtual void acceptMessage( const std::string& msg ) {
            m_currentResult.setMessage( msg );
        }

        virtual void testEnded( const ResultInfo& result ) {
            if( result.getResultType() == ResultWas::Ok ) {
                m_totals.assertions.passed++;
            }
            else if( !result.ok() ) {
                m_totals.assertions.failed++;

                std::vector<ResultInfo>::const_iterator it = m_info.begin();
                std::vector<ResultInfo>::const_iterator itEnd = m_info.end();
                for(; it != itEnd; ++it )
                    m_reporter->Result( *it );
                m_info.clear();
            }

            if( result.getResultType() == ResultWas::Info )
                m_info.push_back( result );
            else
                m_reporter->Result( result );
        }

        virtual bool sectionStarted (
            const std::string& name,
            const std::string& description,
            const SourceLineInfo& lineInfo,
            Counts& assertions
        )
        {
            std::ostringstream oss;
            oss << name << "@" << lineInfo;

            if( !m_runningTest->addSection( oss.str() ) )
                return false;

            m_currentResult.setLineInfo( lineInfo );
            m_reporter->StartSection( name, description );
            assertions = m_totals.assertions;

            return true;
        }

        virtual void sectionEnded( const std::string& name, const Counts& prevAssertions ) {
            Counts assertions = m_totals.assertions - prevAssertions;
            if( assertions.total() == 0  &&
               ( m_config.data().warnings & ConfigData::WarnAbout::NoAssertions ) &&
               !m_runningTest->isBranchSection() ) {
                m_reporter->NoAssertionsInSection( name );
                m_totals.assertions.failed++;
                assertions.failed++;
            }
            m_runningTest->endSection( name );
            m_reporter->EndSection( name, assertions );
        }

        virtual void pushScopedInfo( ScopedInfo* scopedInfo ) {
            m_scopedInfos.push_back( scopedInfo );
        }

        virtual void popScopedInfo( ScopedInfo* scopedInfo ) {
            if( m_scopedInfos.back() == scopedInfo )
                m_scopedInfos.pop_back();
        }

        virtual bool shouldDebugBreak() const {
            return m_config.shouldDebugBreak();
        }

        virtual std::string getCurrentTestName() const {
            return m_runningTest
                ? m_runningTest->getTestCaseInfo().getName()
                : "";
        }

        virtual const ResultInfo* getLastResult() const {
            return &m_lastResult;
        }

    public:
        // !TBD We need to do this another way!
        bool aborting() const {
            return m_totals.assertions.failed == static_cast<std::size_t>( m_config.getCutoff() );
        }

    private:

        ResultAction::Value actOnCurrentResult() {
            testEnded( m_currentResult );
            m_lastResult = m_currentResult;

            m_currentResult = ResultInfoBuilder();

            ResultAction::Value action = ResultAction::None;

            if( !m_lastResult.ok() ) {
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
                m_runningTest->reset();
                Counts prevAssertions = m_totals.assertions;
                if( m_reporter->shouldRedirectStdout() ) {
                    StreamRedirect coutRedir( std::cout, redirectedCout );
                    StreamRedirect cerrRedir( std::cerr, redirectedCerr );
                    m_runningTest->getTestCaseInfo().invoke();
                }
                else {
                    m_runningTest->getTestCaseInfo().invoke();
                }
                Counts assertions = m_totals.assertions - prevAssertions;
                if( assertions.total() == 0  &&
                   ( m_config.data().warnings & ConfigData::WarnAbout::NoAssertions ) &&
                   !m_runningTest->hasSections() ) {
                        m_totals.assertions.failed++;
                        m_reporter->NoAssertionsInTestCase( m_runningTest->getTestCaseInfo().getName() );
                }
                m_runningTest->ranToCompletion();
            }
            catch( TestFailureException& ) {
                // This just means the test was aborted due to failure
            }
            catch(...) {
                acceptMessage( getRegistryHub().getExceptionTranslatorRegistry().translateActiveException() );
                acceptResult( ResultWas::ThrewException );
            }
            m_info.clear();
        }

    private:
        IMutableContext& m_context;
        RunningTest* m_runningTest;
        ResultInfoBuilder m_currentResult;
        ResultInfo m_lastResult;

        const Config& m_config;
        Totals m_totals;
        Ptr<IReporter> m_reporter;
        std::vector<ScopedInfo*> m_scopedInfos;
        std::vector<ResultInfo> m_info;
        IRunner* m_prevRunner;
        IResultCapture* m_prevResultCapture;
        const IConfig* m_prevConfig;
    };

} // end namespace Catch

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
            resolveStream();
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

            std::vector<TestCaseFilters>::const_iterator it = filterGroups.begin();
            std::vector<TestCaseFilters>::const_iterator itEnd = filterGroups.end();
            for(; it != itEnd && !context.aborting(); ++it ) {
                m_reporter->StartGroup( it->getName() );
                totals += runTestsForGroup( context, *it );
                if( context.aborting() )
                    m_reporter->Aborted();
                m_reporter->EndGroup( it->getName(), totals );
            }
            return totals;
        }

        Totals runTestsForGroup( Runner& context, const TestCaseFilters& filterGroup ) {
            Totals totals;
            std::vector<TestCaseInfo>::const_iterator it = getRegistryHub().getTestCaseRegistry().getAllTests().begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = getRegistryHub().getTestCaseRegistry().getAllTests().end();
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
            if( testsRunForGroup == 0 )
                std::cerr << "\n[No test cases matched with: " << filterGroup.getName() << "]" << std::endl;
            return totals;

        }

    private:
        void resolveStream() {
            if( !m_config.stream.empty() ) {
                if( m_config.stream[0] == '%' )
                    m_configWrapper.useStream( m_config.stream.substr( 1 ) );
                else
                    m_configWrapper.setFilename( m_config.stream );
            }
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
            ? "basic"
            : m_config.reporter;

            ReporterConfig reporterConfig( m_config.name, m_configWrapper.stream(), m_config.includeWhichResults == Include::SuccessfulResults, m_config );

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
        Ptr<IReporter> m_reporter;
        std::set<TestCaseInfo> m_testsAlreadyRun;
    };

    inline int Main( Config& configWrapper ) {
        int result = 0;
        try
        {
            Runner2 runner( configWrapper );

            const ConfigData& config = configWrapper.data();

            // Handle list request
            if( config.listSpec != List::None ) {
                List( config );
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

    inline void addIndent( std::ostream& os, std::size_t indent ) {
        while( indent-- > 0 )
            os << ' ';
    }

    inline void recursivelyWrapLine( std::ostream& os, std::string paragraph, std::size_t columns, std::size_t indent ) {
        std::size_t width = columns-indent;
        std::size_t tab = 0;
        std::size_t wrapPoint = width;
        for( std::size_t pos = 0; pos < paragraph.size(); ++pos ) {
            if( pos == width ) {
                addIndent( os, indent );
                os << paragraph.substr( 0, wrapPoint ) << "\n";
                return recursivelyWrapLine( os, paragraph.substr( wrapPoint+1 ), columns, indent+tab );
            }
            if( paragraph[pos] == '\t' ) {
                    tab = pos;
                    paragraph = paragraph.substr( 0, tab ) + paragraph.substr( tab+1 );
                    pos--;
            }
            else if( paragraph[pos] == ' ' ) {
                wrapPoint = pos;
            }
        }
        addIndent( os, indent );
        os << paragraph << "\n";
    }

    inline std::string addLineBreaks( const std::string& str, std::size_t columns, std::size_t indent = 0 ) {
        std::ostringstream oss;
        std::string::size_type pos = 0;
        std::string::size_type newline = str.find_first_of( '\n' );
        while( newline != std::string::npos ) {
            std::string paragraph = str.substr( pos, newline-pos );
            recursivelyWrapLine( oss, paragraph, columns, indent );
            pos = newline+1;
            newline = str.find_first_of( '\n', pos );
        }
        if( pos != str.size() )
            recursivelyWrapLine( oss, str.substr( pos, str.size()-pos ), columns, indent );

        return oss.str();
    }

    inline void showHelp( const CommandParser& parser ) {
        std::string exeName = parser.exeName();
        std::string::size_type pos = exeName.find_last_of( "/\\" );
        if( pos != std::string::npos ) {
            exeName = exeName.substr( pos+1 );
        }

        AllOptions options;
        Options::HelpOptionParser helpOpt;
        bool displayedSpecificOption = false;
        for( AllOptions::const_iterator it = options.begin(); it != options.end(); ++it ) {
            OptionParser& opt = **it;
            if( opt.find( parser ) && opt.optionNames() != helpOpt.optionNames() ) {
                displayedSpecificOption = true;
                std::cout   << "\n" << opt.optionNames() << " " << opt.argsSynopsis() << "\n\n"
                            << opt.optionSummary() << "\n\n"

                << addLineBreaks( opt.optionDescription(), 80, 2 ) << "\n" << std::endl;
            }
        }

        if( !displayedSpecificOption ) {
            std::cout << exeName << " is a CATCH host application. Options are as follows:\n\n";
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

// #included from: catch_test_case_registry_impl.hpp

#include <vector>
#include <set>
#include <sstream>
#include <iostream>

namespace Catch {

    class TestRegistry : public ITestCaseRegistry {
    public:
        TestRegistry() : m_unnamedCount( 0 ) {}
        virtual ~TestRegistry();

        virtual void registerTest( const TestCaseInfo& testInfo ) {
            if( testInfo.getName() == "" ) {
                std::ostringstream oss;
                oss << testInfo.getName() << "unnamed/" << ++m_unnamedCount;
                return registerTest( TestCaseInfo( testInfo, oss.str() ) );
            }

            if( m_functions.find( testInfo ) == m_functions.end() ) {
                m_functions.insert( testInfo );
                m_functionsInOrder.push_back( testInfo );
                if( !testInfo.isHidden() )
                    m_nonHiddenFunctions.push_back( testInfo );
            }
            else {
                const TestCaseInfo& prev = *m_functions.find( testInfo );
                std::cerr   << "error: TEST_CASE( \"" << testInfo.getName() << "\" ) already defined.\n"
                            << "\tFirst seen at " << SourceLineInfo( prev.getLineInfo() ) << "\n"
                            << "\tRedefined at " << SourceLineInfo( testInfo.getLineInfo() ) << std::endl;
                exit(1);
            }
        }

        virtual const std::vector<TestCaseInfo>& getAllTests() const {
            return m_functionsInOrder;
        }

        virtual const std::vector<TestCaseInfo>& getAllNonHiddenTests() const {
            return m_nonHiddenFunctions;
        }

        // !TBD deprecated
        virtual std::vector<TestCaseInfo> getMatchingTestCases( const std::string& rawTestSpec ) const {
            std::vector<TestCaseInfo> matchingTests;
            getMatchingTestCases( rawTestSpec, matchingTests );
            return matchingTests;
        }

        // !TBD deprecated
        virtual void getMatchingTestCases( const std::string& rawTestSpec, std::vector<TestCaseInfo>& matchingTestsOut ) const {
            TestCaseFilter filter( rawTestSpec );

            std::vector<TestCaseInfo>::const_iterator it = m_functionsInOrder.begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = m_functionsInOrder.end();
            for(; it != itEnd; ++it ) {
                if( filter.shouldInclude( *it ) ) {
                    matchingTestsOut.push_back( *it );
                }
            }
        }
        virtual void getMatchingTestCases( const TestCaseFilters& filters, std::vector<TestCaseInfo>& matchingTestsOut ) const {
            std::vector<TestCaseInfo>::const_iterator it = m_functionsInOrder.begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = m_functionsInOrder.end();
            // !TBD: replace with algorithm
            for(; it != itEnd; ++it )
                if( filters.shouldInclude( *it ) )
                    matchingTestsOut.push_back( *it );
        }

    private:

        std::set<TestCaseInfo> m_functions;
        std::vector<TestCaseInfo> m_functionsInOrder;
        std::vector<TestCaseInfo> m_nonHiddenFunctions;
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

    ///////////////////////////////////////////////////////////////////////////

    AutoReg::AutoReg(   TestFunction function,
                        const char* name,
                        const char* description,
                        const SourceLineInfo& lineInfo ) {
        registerTestCase( new FreeFunctionTestCase( function ), name, description, lineInfo );
    }

    AutoReg::~AutoReg() {}

    void AutoReg::registerTestCase( ITestCase* testCase,
                                    const char* name,
                                    const char* description,
                                    const SourceLineInfo& lineInfo ) {
        getMutableRegistryHub().registerTest( TestCaseInfo( testCase, name, description, lineInfo ) );
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

        virtual IReporter* create( const std::string& name, const ReporterConfig& config ) const {
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
#define TWOBLUECUBES_CATCH_EXCEPTION_TRANSLATOR_HPP_INCLUDED

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
            virtual void registerTest( const TestCaseInfo& testInfo ) {
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

} // end namespace Catch
// #included from: catch_notimplemented_exception.hpp
#define TWOBLUECUBES_CATCH_NOTIMPLEMENTED_EXCEPTION_HPP_INCLUDED

#include <ostream>

namespace Catch {

    NotImplementedException::NotImplementedException( const SourceLineInfo& lineInfo )
    :   m_lineInfo( lineInfo ) {
        std::ostringstream oss;
        oss << lineInfo << "function ";
        if( !lineInfo.function.empty() )
            oss << lineInfo.function << " ";
        oss << "not implemented";
        m_what = oss.str();
    }

    const char* NotImplementedException::what() const throw() {
        return m_what.c_str();
    }

} // end namespace Catch

// #included from: catch_context_impl.hpp

// #included from: catch_stream.hpp
#define TWOBLUECUBES_CATCH_STREAM_HPP_INCLUDED

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
        int	overflow( int c ) {
            sync();

            if( c != EOF ) {
                if( pbase() == epptr() )
                    m_writer( std::string( 1, static_cast<char>( c ) ) );
                else
                    sputc( static_cast<char>( c ) );
            }
            return 0;
        }

        int	sync() {
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
}

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

    std::streambuf* createStreamBuf( const std::string& streamName ) {
        if( streamName == "stdout" ) return std::cout.rdbuf();
        if( streamName == "stderr" ) return std::cerr.rdbuf();
        if( streamName == "debug" ) return new StreamBufImpl<OutputDebugWriter>;

        throw std::domain_error( "Unknown stream: " + streamName );
    }

    void cleanUpContext() {
        delete currentContext;
        currentContext = NULL;
    }
}
// #included from: catch_console_colour_impl.hpp
#define TWOBLUECUBES_CATCH_CONSOLE_COLOUR_IMPL_HPP_INCLUDED

// #included from: catch_console_colour.hpp
#define TWOBLUECUBES_CATCH_CONSOLE_COLOUR_HPP_INCLUDED

namespace Catch {

    struct ConsoleColourImpl;

    class TextColour : NonCopyable {
    public:

        enum Colours {
            None,

            FileName,
            ResultError,
            ResultSuccess,

            Error,
            Success,

            OriginalExpression,
            ReconstructedExpression
        };

        TextColour( Colours colour = None );
        void set( Colours colour );
        ~TextColour();

    private:
        ConsoleColourImpl* m_impl;
    };

} // end namespace Catch

#ifdef CATCH_PLATFORM_WINDOWS

#include <windows.h>

namespace Catch {

    namespace {

        WORD mapConsoleColour( TextColour::Colours colour ) {
            switch( colour ) {
                case TextColour::FileName:
                    return FOREGROUND_INTENSITY;                    // greyed out
                case TextColour::ResultError:
                    return FOREGROUND_RED | FOREGROUND_INTENSITY;   // bright red
                case TextColour::ResultSuccess:
                    return FOREGROUND_GREEN | FOREGROUND_INTENSITY; // bright green
                case TextColour::Error:
                    return FOREGROUND_RED;                          // dark red
                case TextColour::Success:
                    return FOREGROUND_GREEN;                        // dark green
                case TextColour::OriginalExpression:
                    return FOREGROUND_BLUE | FOREGROUND_GREEN;      // turquoise
                case TextColour::ReconstructedExpression:
                    return FOREGROUND_RED | FOREGROUND_GREEN;       // greeny-yellow
                default: return 0;
            }
        }
    }

    struct ConsoleColourImpl {

        ConsoleColourImpl()
        :   hStdout( GetStdHandle(STD_OUTPUT_HANDLE) ),
            wOldColorAttrs( 0 )
        {
            GetConsoleScreenBufferInfo( hStdout, &csbiInfo );
            wOldColorAttrs = csbiInfo.wAttributes;
        }

        ~ConsoleColourImpl() {
            SetConsoleTextAttribute( hStdout, wOldColorAttrs );
        }

        void set( TextColour::Colours colour ) {
            WORD consoleColour = mapConsoleColour( colour );
            if( consoleColour > 0 )
                SetConsoleTextAttribute( hStdout, consoleColour );
        }

        HANDLE hStdout;
        CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
        WORD wOldColorAttrs;
    };

    TextColour::TextColour( Colours colour )
    : m_impl( new ConsoleColourImpl() )
    {
        if( colour )
            m_impl->set( colour );
    }

    TextColour::~TextColour() {
        delete m_impl;
    }

    void TextColour::set( Colours colour ) {
        m_impl->set( colour );
    }

} // end namespace Catch

#else

namespace Catch {
    TextColour::TextColour( Colours ){}
    TextColour::~TextColour(){}
    void TextColour::set( Colours ){}

} // end namespace Catch

#endif

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

// #included from: catch_resultinfo.hpp
#define TWOBLUECUBES_CATCH_RESULT_INFO_HPP_INCLUDED

namespace Catch {

    ResultInfo::ResultInfo()
    :   m_macroName(),
        m_expr(),
        m_lhs(),
        m_rhs(),
        m_op(),
        m_message(),
        m_result( ResultWas::Unknown ),
        m_isNot( false )
        {}

    ResultInfo::ResultInfo(const char* expr,
                           ResultWas::OfType result,
                           bool isNot,
                           const SourceLineInfo& lineInfo,
                           const char* macroName,
                           const char* message )
    :   m_macroName( macroName ),
        m_lineInfo( lineInfo ),
        m_expr( expr ),
        m_lhs(),
        m_rhs(),
        m_op( isNotExpression( expr ) ? "!" : "" ),
        m_message( message ),
        m_result( result ),
        m_isNot( isNot )
        {
            if( isNot )
                m_expr = "!" + m_expr;
        }

    ResultInfo::~ResultInfo() {}

    bool ResultInfo::ok() const {
        return ( m_result & ResultWas::FailureBit ) != ResultWas::FailureBit;
    }

    ResultWas::OfType ResultInfo::getResultType() const {
        return m_result;
    }

    bool ResultInfo::hasExpression() const {
        return !m_expr.empty();
    }

    bool ResultInfo::hasMessage() const {
        return !m_message.empty();
    }

    std::string ResultInfo::getExpression() const {
        return m_expr;
    }

    bool ResultInfo::hasExpandedExpression() const {
        return hasExpression() && getExpandedExpressionInternal() != m_expr;
    }

    std::string ResultInfo::getExpandedExpression() const {
        return hasExpression() ? getExpandedExpressionInternal() : "";
    }

    std::string ResultInfo::getMessage() const {
        return m_message;
    }

    std::string ResultInfo::getFilename() const {
        return m_lineInfo.file;
    }

    std::size_t ResultInfo::getLine() const {
        return m_lineInfo.line;
    }

    std::string ResultInfo::getTestMacroName() const {
        return m_macroName;
    }

    std::string ResultInfo::getExpandedExpressionInternal() const {
        if( m_op == "" || m_isNot )
            return m_lhs.empty() ? m_expr : m_op + m_lhs;
        else if( m_op == "matches" )
            return m_lhs + " " + m_rhs;
        else if( m_op != "!" )
        {
            if( m_lhs.size() + m_rhs.size() < 30 )
                return m_lhs + " " + m_op + " " + m_rhs;
            else if( m_lhs.size() < 70 && m_rhs.size() < 70 )
                return "\n\t" + m_lhs + "\n\t" + m_op + "\n\t" + m_rhs;
            else
                return "\n" + m_lhs + "\n" + m_op + "\n" + m_rhs + "\n\n";
        }
        else
            return "{can't expand - use " + m_macroName + "_FALSE( " + m_expr.substr(1) + " ) instead of " + m_macroName + "( " + m_expr + " ) for better diagnostics}";
    }

    bool ResultInfo::isNotExpression( const char* expr ) {
        return expr && expr[0] == '!';
    }

} // end namespace Catch

// #included from: catch_resultinfo_builder.hpp
#define TWOBLUECUBES_CATCH_RESULTINFO_BUILDER_HPP_INCLUDED

namespace Catch {

    ResultInfoBuilder::ResultInfoBuilder() {}

    ResultInfoBuilder::ResultInfoBuilder(   const char* expr,
                                            bool isNot,
                                            const SourceLineInfo& lineInfo,
                                            const char* macroName,
                                            const char* message )
    : ResultInfo( expr, ResultWas::Unknown, isNot, lineInfo, macroName, message )
    {}

    void ResultInfoBuilder::setResultType( ResultWas::OfType result ) {
        // Flip bool results if isNot is set
        if( m_isNot && result == ResultWas::Ok )
            m_result = ResultWas::ExpressionFailed;
        else if( m_isNot && result == ResultWas::ExpressionFailed )
            m_result = ResultWas::Ok;
        else
            m_result = result;
    }

    void ResultInfoBuilder::setMessage( const std::string& message ) {
        m_message = message;
    }

    void ResultInfoBuilder::setLineInfo( const SourceLineInfo& lineInfo ) {
        m_lineInfo = lineInfo;
    }

    void ResultInfoBuilder::setLhs( const std::string& lhs ) {
        m_lhs = lhs;
    }

    void ResultInfoBuilder::setRhs( const std::string& rhs ) {
        m_rhs = rhs;
    }

    void ResultInfoBuilder::setOp( const std::string& op ) {
        m_op = op;
    }

    ResultInfoBuilder& ResultInfoBuilder::captureBoolExpression( bool result ) {
        m_lhs = Catch::toString( result );
        m_op = m_isNot ? "!" : "";
        setResultType( result ? ResultWas::Ok : ResultWas::ExpressionFailed );
        return *this;
    }

} // end namespace Catch

// #included from: catch_test_case_info.hpp
#define TWOBLUECUBES_CATCH_TESTCASEINFO_HPP_INCLUDED

namespace Catch {

    namespace {
        void extractTags( std::string& str, std::set<std::string>& tags ) {
            std::string remainder;
            std::size_t last = 0;
            std::size_t begin = str.find_first_of( '[' );
            while( begin != std::string::npos ) {
                std::size_t end = str.find_first_of( ']', begin );
                if( end != std::string::npos ) {
                    std::string tag = str.substr( begin+1, end-begin-1 );
                    tags.insert( tag );
                    if( begin - last > 0 )
                        remainder += str.substr( last, begin-last );
                    last = end+1;
                }
                else if( begin != str.size()-1 ) {
                    end = begin+1;
                }
                else {
                    break;
                }
                begin = str.find_first_of( '[', end );
            }
            if( !tags.empty() ) {
                if( last < str.size() )
                    str = remainder + str.substr( last );
                else
                    str = remainder;
            }
        }
    }
    TestCaseInfo::TestCaseInfo( ITestCase* testCase,
                                const char* name,
                                const char* description,
                                const SourceLineInfo& lineInfo )
    :   m_test( testCase ),
        m_name( name ),
        m_description( description ),
        m_lineInfo( lineInfo ),
        m_isHidden( startsWith( name, "./" ) )
    {
        extractTags( m_description, m_tags );
        if( hasTag( "hide" ) )
            m_isHidden = true;
    }

    TestCaseInfo::TestCaseInfo()
    :   m_test( NULL ),
        m_name(),
        m_description(),
        m_isHidden( false )
    {}

    TestCaseInfo::TestCaseInfo( const TestCaseInfo& other, const std::string& name )
    :   m_test( other.m_test ),
        m_name( name ),
        m_description( other.m_description ),
        m_tags( other.m_tags ),
        m_lineInfo( other.m_lineInfo ),
        m_isHidden( other.m_isHidden )
    {}

    TestCaseInfo::TestCaseInfo( const TestCaseInfo& other )
    :   m_test( other.m_test ),
        m_name( other.m_name ),
        m_description( other.m_description ),
        m_tags( other.m_tags ),
        m_lineInfo( other.m_lineInfo ),
        m_isHidden( other.m_isHidden )
    {}

    void TestCaseInfo::invoke() const {
        m_test->invoke();
    }

    const std::string& TestCaseInfo::getName() const {
        return m_name;
    }

    const std::string& TestCaseInfo::getDescription() const {
        return m_description;
    }

    const SourceLineInfo& TestCaseInfo::getLineInfo() const {
        return m_lineInfo;
    }

    bool TestCaseInfo::isHidden() const {
        return m_isHidden;
    }

    bool TestCaseInfo::hasTag( const std::string& tag ) const {
        return m_tags.find( tag ) != m_tags.end();
    }
    const std::set<std::string>& TestCaseInfo::tags() const {
        return m_tags;
    }

    void TestCaseInfo::swap( TestCaseInfo& other ) {
        m_test.swap( other.m_test );
        m_name.swap( other.m_name );
        m_description.swap( other.m_description );
        m_lineInfo.swap( other.m_lineInfo );
    }

    bool TestCaseInfo::operator == ( const TestCaseInfo& other ) const {
        return m_test.get() == other.m_test.get() && m_name == other.m_name;
    }

    bool TestCaseInfo::operator < ( const TestCaseInfo& other ) const {
        return m_name < other.m_name;
    }
    TestCaseInfo& TestCaseInfo::operator = ( const TestCaseInfo& other ) {
        TestCaseInfo temp( other );
        swap( temp );
        return *this;
    }
}

// #included from: ../reporters/catch_reporter_basic.hpp
#define TWOBLUECUBES_CATCH_REPORTER_BASIC_HPP_INCLUDED

// #included from: ../internal/catch_reporter_registrars.hpp
#define TWOBLUECUBES_CATCH_REPORTER_REGISTRARS_HPP_INCLUDED

namespace Catch {

    template<typename T>
    class ReporterRegistrar {

        class ReporterFactory : public IReporterFactory {

            virtual IReporter* create( const ReporterConfig& config ) const {
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
                m_config.stream << counts.failed << " of " << counts.total() << " " << label << "s failed";
            else
                m_config.stream << ( counts.failed > 1 ? allPrefix : "" ) << pluralise( counts.failed, label ) << " failed";
        }

        void ReportCounts( const Totals& totals, const std::string& allPrefix = "All " ) {
            if( totals.assertions.total() == 0 ) {
                m_config.stream << "No tests ran";
            }
            else if( totals.assertions.failed ) {
                TextColour colour( TextColour::ResultError );
                ReportCounts( "test case", totals.testCases, allPrefix );
                if( totals.testCases.failed > 0 ) {
                    m_config.stream << " (";
                    ReportCounts( "assertion", totals.assertions, allPrefix );
                    m_config.stream << ")";
                }
            }
            else {
                TextColour colour( TextColour::ResultSuccess );
                m_config.stream   << allPrefix << "tests passed ("
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
                m_config.stream << "\n[Testing aborted. ";
                ReportCounts( totals, "The first " );
            }
            else {
                m_config.stream << "\n[Testing completed. ";
                ReportCounts( totals );
            }
            m_config.stream << "]\n" << std::endl;
        }

        virtual void StartGroup( const std::string& groupName ) {
            m_groupSpan = groupName;
        }

        virtual void EndGroup( const std::string& groupName, const Totals& totals ) {
            if( m_groupSpan.emitted && !groupName.empty() ) {
                m_config.stream << "[End of group: '" << groupName << "'. ";
                ReportCounts( totals );
                m_config.stream << "]\n" << std::endl;
                m_groupSpan = SpanInfo();
            }
        }

        virtual void StartTestCase( const TestCaseInfo& testInfo ) {
            m_testSpan = testInfo.getName();
        }

        virtual void StartSection( const std::string& sectionName, const std::string& ) {
            m_sectionSpans.push_back( SpanInfo( sectionName ) );
        }

        virtual void NoAssertionsInSection( const std::string& sectionName ) {
            startSpansLazily();
            TextColour colour( TextColour::ResultError );
            m_config.stream << "\nNo assertions in section, '" << sectionName << "'\n" << std::endl;
        }
        virtual void NoAssertionsInTestCase( const std::string& testName ) {
            startSpansLazily();
            TextColour colour( TextColour::ResultError );
            m_config.stream << "\nNo assertions in test case, '" << testName << "'\n" << std::endl;
        }

        virtual void EndSection( const std::string& sectionName, const Counts& assertions ) {

            SpanInfo& sectionSpan = m_sectionSpans.back();
            if( sectionSpan.emitted && !sectionSpan.name.empty() ) {
                m_config.stream << "[End of section: '" << sectionName << "' ";

                if( assertions.failed ) {
                    TextColour colour( TextColour::ResultError );
                    ReportCounts( "assertion", assertions);
                }
                else {
                    TextColour colour( TextColour::ResultSuccess );
                    m_config.stream   << ( assertions.passed > 1 ? "All " : "" )
                                        << pluralise( assertions.passed, "assertion" ) << " passed" ;
                }
                m_config.stream << "]\n" << std::endl;
            }
            m_sectionSpans.pop_back();
        }

        virtual void Result( const ResultInfo& resultInfo ) {
            if( !m_config.includeSuccessfulResults && resultInfo.getResultType() == ResultWas::Ok )
                return;

            startSpansLazily();

            if( !resultInfo.getFilename().empty() ) {
                TextColour colour( TextColour::FileName );
                m_config.stream << SourceLineInfo( resultInfo.getFilename(), resultInfo.getLine() );
            }

            if( resultInfo.hasExpression() ) {
                TextColour colour( TextColour::OriginalExpression );
                m_config.stream << resultInfo.getExpression();
                if( resultInfo.ok() ) {
                    TextColour successColour( TextColour::Success );
                    m_config.stream << " succeeded";
                }
                else {
                    TextColour errorColour( TextColour::Error );
                    m_config.stream << " failed";
                }
            }
            switch( resultInfo.getResultType() ) {
                case ResultWas::ThrewException:
                {
                    TextColour colour( TextColour::Error );
                    if( resultInfo.hasExpression() )
                        m_config.stream << " with unexpected";
                    else
                        m_config.stream << "Unexpected";
                    m_config.stream << " exception with message: '" << resultInfo.getMessage() << "'";
                }
                    break;
                case ResultWas::DidntThrowException:
                {
                    TextColour colour( TextColour::Error );
                    if( resultInfo.hasExpression() )
                        m_config.stream << " because no exception was thrown where one was expected";
                    else
                        m_config.stream << "No exception thrown where one was expected";
                }
                    break;
                case ResultWas::Info:
                    streamVariableLengthText( "info", resultInfo.getMessage() );
                    break;
                case ResultWas::Warning:
                    m_config.stream << "warning:\n'" << resultInfo.getMessage() << "'";
                    break;
                case ResultWas::ExplicitFailure:
                {
                    TextColour colour( TextColour::Error );
                    m_config.stream << "failed with message: '" << resultInfo.getMessage() << "'";
                }
                    break;
                case ResultWas::Unknown: // These cases are here to prevent compiler warnings
                case ResultWas::Ok:
                case ResultWas::FailureBit:
                case ResultWas::ExpressionFailed:
                case ResultWas::Exception:
                    if( !resultInfo.hasExpression() ) {
                        if( resultInfo.ok() ) {
                            TextColour colour( TextColour::Success );
                            m_config.stream << " succeeded";
                        }
                        else {
                            TextColour colour( TextColour::Error );
                            m_config.stream << " failed";
                        }
                    }
                    break;
            }

            if( resultInfo.hasExpandedExpression() ) {
                m_config.stream << " for: ";
                TextColour colour( TextColour::ReconstructedExpression );
                m_config.stream << resultInfo.getExpandedExpression();
            }
            m_config.stream << std::endl;
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
                m_config.stream << "[Finished: '" << testInfo.getName() << "' ";
                ReportCounts( totals );
                m_config.stream << "]" << std::endl;
            }
        }

    private: // helpers

        void startSpansLazily() {
            if( !m_testingSpan.emitted ) {
                if( m_config.name.empty() )
                    m_config.stream << "[Started testing]" << std::endl;
                else
                    m_config.stream << "[Started testing: " << m_config.name << "]" << std::endl;
                m_testingSpan.emitted = true;
            }

            if( !m_groupSpan.emitted && !m_groupSpan.name.empty() ) {
                m_config.stream << "[Started group: '" << m_groupSpan.name << "']" << std::endl;
                m_groupSpan.emitted = true;
            }

            if( !m_testSpan.emitted ) {
                m_config.stream << std::endl << "[Running: " << m_testSpan.name << "]" << std::endl;
                m_testSpan.emitted = true;
            }

            if( !m_sectionSpans.empty() ) {
                SpanInfo& sectionSpan = m_sectionSpans.back();
                if( !sectionSpan.emitted && !sectionSpan.name.empty() ) {
                    if( m_firstSectionInTestCase ) {
                        m_config.stream << "\n";
                        m_firstSectionInTestCase = false;
                    }
                    std::vector<SpanInfo>::iterator it = m_sectionSpans.begin();
                    std::vector<SpanInfo>::iterator itEnd = m_sectionSpans.end();
                    for(; it != itEnd; ++it ) {
                        SpanInfo& prevSpan = *it;
                        if( !prevSpan.emitted && !prevSpan.name.empty() ) {
                            m_config.stream << "[Started section: '" << prevSpan.name << "']" << std::endl;
                            prevSpan.emitted = true;
                        }
                    }
                }
            }
        }

        void streamVariableLengthText( const std::string& prefix, const std::string& text ) {
            std::string trimmed = trim( text );
            if( trimmed.find_first_of( "\r\n" ) == std::string::npos ) {
                m_config.stream << "[" << prefix << ": " << trimmed << "]\n";
            }
            else {
                m_config.stream << "\n[" << prefix << "] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" << trimmed
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

            ScopedElement& writeText( const std::string& text ) {
                m_writer->writeText( text );
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

        XmlWriter& writeText( const std::string& text ) {
            if( !text.empty() ){
                bool tagWasOpen = m_tagIsOpen;
                ensureTagClosed();
                if( tagWasOpen )
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
            m_xml = XmlWriter( m_config.stream );
            m_xml.startElement( "Catch" );
            if( !m_config.name.empty() )
                m_xml.writeAttribute( "name", m_config.name );
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
            m_xml.startElement( "TestCase" ).writeAttribute( "name", testInfo.getName() );
            m_currentTestSuccess = true;
        }

        virtual void Result( const Catch::ResultInfo& resultInfo ) {
            if( !m_config.includeSuccessfulResults && resultInfo.getResultType() == ResultWas::Ok )
                return;

            if( resultInfo.hasExpression() ) {
                m_xml.startElement( "Expression" )
                    .writeAttribute( "success", resultInfo.ok() )
                    .writeAttribute( "filename", resultInfo.getFilename() )
                    .writeAttribute( "line", resultInfo.getLine() );

                m_xml.scopedElement( "Original" )
                    .writeText( resultInfo.getExpression() );
                m_xml.scopedElement( "Expanded" )
                    .writeText( resultInfo.getExpandedExpression() );
                m_currentTestSuccess &= resultInfo.ok();
            }

            switch( resultInfo.getResultType() ) {
                case ResultWas::ThrewException:
                    m_xml.scopedElement( "Exception" )
                        .writeAttribute( "filename", resultInfo.getFilename() )
                        .writeAttribute( "line", resultInfo.getLine() )
                        .writeText( resultInfo.getMessage() );
                    m_currentTestSuccess = false;
                    break;
                case ResultWas::Info:
                    m_xml.scopedElement( "Info" )
                        .writeText( resultInfo.getMessage() );
                    break;
                case ResultWas::Warning:
                    m_xml.scopedElement( "Warning" )
                        .writeText( resultInfo.getMessage() );
                    break;
                case ResultWas::ExplicitFailure:
                    m_xml.scopedElement( "Failure" )
                        .writeText( resultInfo.getMessage() );
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
            if( resultInfo.hasExpression() )
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

namespace Catch {

    class JunitReporter : public SharedImpl<IReporter> {

        struct TestStats {
            std::string m_element;
            std::string m_resultType;
            std::string m_message;
            std::string m_content;
        };

        struct TestCaseStats {

            TestCaseStats( const std::string& name = std::string() ) :m_name( name ){}

            double      m_timeInSeconds;
            std::string m_status;
            std::string m_className;
            std::string m_name;
            std::vector<TestStats> m_testStats;
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
            m_currentStats->m_testCaseStats.push_back( TestCaseStats( testInfo.getName() ) );
        }

        virtual void Result( const Catch::ResultInfo& resultInfo ) {
            if( resultInfo.getResultType() != ResultWas::Ok || m_config.includeSuccessfulResults ) {
                TestCaseStats& testCaseStats = m_currentStats->m_testCaseStats.back();
                TestStats stats;
                std::ostringstream oss;
                if( !resultInfo.getMessage().empty() )
                    oss << resultInfo.getMessage() << " at ";
                oss << SourceLineInfo( resultInfo.getFilename(), resultInfo.getLine() );
                stats.m_content = oss.str();
                stats.m_message = resultInfo.getExpandedExpression();
                stats.m_resultType = resultInfo.getTestMacroName();

                switch( resultInfo.getResultType() ) {
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
                    case ResultWas::Unknown:
                    case ResultWas::FailureBit:
                    case ResultWas::Exception:
                    case ResultWas::DidntThrowException:
                        break;
                }
                testCaseStats.m_testStats.push_back( stats );
            }
        }

        virtual void EndTestCase( const Catch::TestCaseInfo&, const Totals&, const std::string& stdOut, const std::string& stdErr ) {
            if( !stdOut.empty() )
                m_stdOut << stdOut << "\n";
            if( !stdErr.empty() )
                m_stdErr << stdErr << "\n";
        }

        virtual void Aborted() {
            // !TBD
        }

        virtual void EndTesting( const Totals& ) {
            std::ostream& str = m_config.stream;
            {
                XmlWriter xml( str );

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

                xml.scopedElement( "system-out" ).writeText( trim( m_stdOut.str() ) );
                xml.scopedElement( "system-err" ).writeText( trim( m_stdErr.str() ) );
            }
        }

        void OutputTestCases( XmlWriter& xml, const Stats& stats ) {
            std::vector<TestCaseStats>::const_iterator it = stats.m_testCaseStats.begin();
            std::vector<TestCaseStats>::const_iterator itEnd = stats.m_testCaseStats.end();
            for(; it != itEnd; ++it ) {
                xml.writeBlankLine();
                xml.writeComment( "Test case" );

                XmlWriter::ScopedElement e = xml.scopedElement( "testcase" );
                xml.writeAttribute( "classname", it->m_className );
                xml.writeAttribute( "name", it->m_name );
                xml.writeAttribute( "time", "tbd" );

                OutputTestResult( xml, *it );
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
        bool m_currentTestSuccess;

        Stats m_testSuiteStats;
        Stats* m_currentStats;
        std::vector<Stats> m_statsForSuites;
        std::ostringstream m_stdOut;
        std::ostringstream m_stdErr;
    };

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
    BasicReporter::~BasicReporter() {}
    IRunner::~IRunner() {}
    IMutableContext::~IMutableContext() {}
    IConfig::~IConfig() {}
    XmlReporter::~XmlReporter() {}
    JunitReporter::~JunitReporter() {}
    TestRegistry::~TestRegistry() {}
    FreeFunctionTestCase::~FreeFunctionTestCase() {}
    IGeneratorInfo::~IGeneratorInfo() {}
    IGeneratorsForTest::~IGeneratorsForTest() {}

    void Config::dummy() {}

    INTERNAL_CATCH_REGISTER_REPORTER( "basic", BasicReporter )
    INTERNAL_CATCH_REGISTER_REPORTER( "xml", XmlReporter )
    INTERNAL_CATCH_REGISTER_REPORTER( "junit", JunitReporter )

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

#define CATCH_REQUIRE( expr ) INTERNAL_CATCH_TEST( expr, false, true, "CATCH_REQUIRE" )
#define CATCH_REQUIRE_FALSE( expr ) INTERNAL_CATCH_TEST( expr, true, true, "CATCH_REQUIRE_FALSE" )

#define CATCH_REQUIRE_THROWS( expr ) INTERNAL_CATCH_THROWS( expr, ..., true, "CATCH_REQUIRE_THROWS" )
#define CATCH_REQUIRE_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( expr, exceptionType, true, "CATCH_REQUIRE_THROWS_AS" )
#define CATCH_REQUIRE_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( expr, true, "CATCH_REQUIRE_NOTHROW" )

#define CATCH_CHECK( expr ) INTERNAL_CATCH_TEST( expr, false, false, "CATCH_CHECK" )
#define CATCH_CHECK_FALSE( expr ) INTERNAL_CATCH_TEST( expr, true, false, "CATCH_CHECK_FALSE" )
#define CATCH_CHECKED_IF( expr ) INTERNAL_CATCH_IF( expr, false, false, "CATCH_CHECKED_IF" )
#define CATCH_CHECKED_ELSE( expr ) INTERNAL_CATCH_ELSE( expr, false, false, "CATCH_CHECKED_ELSE" )

#define CATCH_CHECK_THROWS( expr )  INTERNAL_CATCH_THROWS( expr, ..., false, "CATCH_CHECK_THROWS" )
#define CATCH_CHECK_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( expr, exceptionType, false, "CATCH_CHECK_THROWS_AS" )
#define CATCH_CHECK_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( expr, false, "CATCH_CHECK_NOTHROW" )

#define CHECK_THAT( arg, matcher ) INTERNAL_CHECK_THAT( arg, matcher, false, "CATCH_CHECK_THAT" )
#define CATCH_REQUIRE_THAT( arg, matcher ) INTERNAL_CHECK_THAT( arg, matcher, true, "CATCH_REQUIRE_THAT" )

#define CATCH_INFO( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Info, false, "CATCH_INFO" )
#define CATCH_WARN( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Warning, false, "CATCH_WARN" )
#define CATCH_FAIL( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::ExplicitFailure, true, "CATCH_FAIL" )
#define CATCH_SUCCEED( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Ok, false, "CATCH_SUCCEED" )
#define CATCH_SCOPED_INFO( msg ) INTERNAL_CATCH_SCOPED_INFO( msg )
#define CATCH_CAPTURE( msg ) INTERNAL_CATCH_MSG( #msg " := " << msg, Catch::ResultWas::Info, false, "CATCH_CAPTURE" )

#define CATCH_SECTION( name, description ) INTERNAL_CATCH_SECTION( name, description )

#define CATCH_TEST_CASE( name, description ) INTERNAL_CATCH_TESTCASE( name, description )
#define CATCH_TEST_CASE_NORETURN( name, description ) INTERNAL_CATCH_TESTCASE_NORETURN( name, description )
#define CATCH_ANON_TEST_CASE() INTERNAL_CATCH_TESTCASE( "", "Anonymous test case" )
#define CATCH_METHOD_AS_TEST_CASE( method, name, description ) INTERNAL_CATCH_METHOD_AS_TEST_CASE( method, name, description )

#define CATCH_REGISTER_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType )

#define CATCH_GENERATE( expr) INTERNAL_CATCH_GENERATE( expr )

///////////////
// Still to be implemented
//#define CHECK_NOFAIL( expr ) // !TBD - reports violation, but doesn't fail Test

// If CATCH_CONFIG_PREFIX_ALL is not defined then the CATCH_ prefix is not required
#else

#define REQUIRE( expr ) INTERNAL_CATCH_TEST( expr, false, true, "REQUIRE" )
#define REQUIRE_FALSE( expr ) INTERNAL_CATCH_TEST( expr, true, true, "REQUIRE_FALSE" )

#define REQUIRE_THROWS( expr ) INTERNAL_CATCH_THROWS( expr, ..., true, "REQUIRE_THROWS" )
#define REQUIRE_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( expr, exceptionType, true, "REQUIRE_THROWS_AS" )
#define REQUIRE_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( expr, true, "REQUIRE_NOTHROW" )

#define CHECK( expr ) INTERNAL_CATCH_TEST( expr, false, false, "CHECK" )
#define CHECK_FALSE( expr ) INTERNAL_CATCH_TEST( expr, true, false, "CHECK_FALSE" )
#define CHECKED_IF( expr ) INTERNAL_CATCH_IF( expr, false, false, "CHECKED_IF" )
#define CHECKED_ELSE( expr ) INTERNAL_CATCH_ELSE( expr, false, false, "CHECKED_ELSE" )

#define CHECK_THROWS( expr )  INTERNAL_CATCH_THROWS( expr, ..., false, "CHECK_THROWS" )
#define CHECK_THROWS_AS( expr, exceptionType ) INTERNAL_CATCH_THROWS_AS( expr, exceptionType, false, "CHECK_THROWS_AS" )
#define CHECK_NOTHROW( expr ) INTERNAL_CATCH_NO_THROW( expr, false, "CHECK_NOTHROW" )

#define CHECK_THAT( arg, matcher ) INTERNAL_CHECK_THAT( arg, matcher, false, "CHECK_THAT" )
#define REQUIRE_THAT( arg, matcher ) INTERNAL_CHECK_THAT( arg, matcher, true, "REQUIRE_THAT" )

#define INFO( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Info, false, "INFO" )
#define WARN( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Warning, false, "WARN" )
#define FAIL( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::ExplicitFailure, true, "FAIL" )
#define SUCCEED( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Ok, false, "SUCCEED" )
#define SCOPED_INFO( msg ) INTERNAL_CATCH_SCOPED_INFO( msg )
#define CAPTURE( msg ) INTERNAL_CATCH_MSG( #msg " := " << msg, Catch::ResultWas::Info, false, "CAPTURE" )

#define SECTION( name, description ) INTERNAL_CATCH_SECTION( name, description )

#define TEST_CASE( name, description ) INTERNAL_CATCH_TESTCASE( name, description )
#define TEST_CASE_NORETURN( name, description ) INTERNAL_CATCH_TESTCASE_NORETURN( name, description )
#define ANON_TEST_CASE() INTERNAL_CATCH_TESTCASE( "", "Anonymous test case" )
#define METHOD_AS_TEST_CASE( method, name, description ) INTERNAL_CATCH_METHOD_AS_TEST_CASE( method, name, description )

#define REGISTER_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType )

#define GENERATE( expr) INTERNAL_CATCH_GENERATE( expr )

#endif

#define CATCH_TRANSLATE_EXCEPTION( signature ) INTERNAL_CATCH_TRANSLATE_EXCEPTION( signature )

using Catch::Detail::Approx;

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_SINGLE_INCLUDE_CATCH_HPP_INCLUDED

