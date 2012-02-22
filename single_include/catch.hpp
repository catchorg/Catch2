// This file has been merged from multiple headers. Please don't edit it directly

#ifndef TWOBLUECUBES_CATCH_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_HPP_INCLUDED
/*
 *  catch.hpp
 *  Catch
 *
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

/* TBD:

 Next:

 Later:
    Finish command line parser (list as xml)
    Tags?
    Finish macros, listed here, later (just CHECK_NOFAIL now)
 */
#define TWOBLUECUBES_CATCH_HPP_INCLUDED
// #included from: internal/catch_hub.h

/*
 *  catch_hub.h
 *  Catch
 *
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_CATCH_HUB_H_INCLUDED

// #included from: catch_interfaces_reporter.h

/*
 *  catch_interfaces_reporter.h
 *  Test
 *
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_IREPORTERREGISTRY_INCLUDED

// #included from: catch_common.h

/*
 *  catch_common.h
 *  Catch
 *
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_COMMON_H_INCLUDED

#define INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line ) name##line
#define INTERNAL_CATCH_UNIQUE_NAME_LINE( name, line ) INTERNAL_CATCH_UNIQUE_NAME_LINE2( name, line )
#define INTERNAL_CATCH_UNIQUE_NAME( name ) INTERNAL_CATCH_UNIQUE_NAME_LINE( name, __LINE__ )

#define INTERNAL_CATCH_STRINGIFY2( expr ) #expr
#define INTERNAL_CATCH_STRINGIFY( expr ) INTERNAL_CATCH_STRINGIFY2( expr )

#ifdef __GNUC__
#define ATTRIBUTE_NORETURN __attribute__ ((noreturn))
#else
#define ATTRIBUTE_NORETURN
#endif

#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace Catch
{
	class NonCopyable
	{
		NonCopyable( const NonCopyable& );
		void operator = ( const NonCopyable& );
	protected:
		NonCopyable(){}
		virtual ~NonCopyable() {}
	};

    typedef char NoType;
    typedef int YesType;

    // create a T for use in sizeof expressions
    template<typename T> T Synth();

    template<typename ContainerT>
    inline void deleteAll( ContainerT& container )
    {
        typename ContainerT::const_iterator it = container.begin();
        typename ContainerT::const_iterator itEnd = container.end();
        for(; it != itEnd; ++it )
        {
            delete *it;
        }
    }
    template<typename AssociativeContainerT>
    inline void deleteAllValues( AssociativeContainerT& container )
    {
        typename AssociativeContainerT::const_iterator it = container.begin();
        typename AssociativeContainerT::const_iterator itEnd = container.end();
        for(; it != itEnd; ++it )
        {
            delete it->second;
        }
    }

    template<typename ContainerT, typename Function>
    inline void forEach( ContainerT& container, Function function )
    {
        std::for_each( container.begin(), container.end(), function );
    }

    template<typename ContainerT, typename Function>
    inline void forEach( const ContainerT& container, Function function )
    {
        std::for_each( container.begin(), container.end(), function );
    }

    struct SourceLineInfo
    {
        SourceLineInfo
        (
            const std::string& file,
            std::size_t line
        )
        :   file( file ),
            line( line )
        {}

        std::string file;
        std::size_t line;
    };

    inline std::ostream& operator << ( std::ostream& os, const SourceLineInfo& info )
    {
#ifndef __GNUG__
        os << info.file << "(" << info.line << "): ";
#else
        os << info.file << ":" << info.line << ": ";
#endif
        return os;
    }

    ATTRIBUTE_NORETURN
    inline void throwLogicError( const std::string& message, const std::string& file, long line )
    {
        std::ostringstream oss;
        oss << "Internal Catch error: '" << message << "' at: " << SourceLineInfo( file, line );
        throw std::logic_error( oss.str() );
    }
}

#define CATCH_INTERNAL_ERROR( msg ) throwLogicError( msg, __FILE__, __LINE__ );



#include <string>
#include <ostream>
#include <map>

namespace Catch
{
    ///////////////////////////////////////////////////////////////////////////
    struct IReporterConfig
    {
        virtual ~IReporterConfig
        ()
        {}

        virtual std::ostream& stream
            () const = 0;

        virtual bool includeSuccessfulResults
            () const = 0;

        virtual std::string getName
            () const = 0;
    };

    class TestCaseInfo;
    class ResultInfo;

    ///////////////////////////////////////////////////////////////////////////
    struct IReporter : NonCopyable
    {
        virtual ~IReporter
            (){}

        virtual bool shouldRedirectStdout
            () const = 0;

        virtual void StartTesting
            () = 0;

        virtual void EndTesting
            (   std::size_t succeeded,
                std::size_t failed
            ) = 0;

        virtual void StartGroup
            (   const std::string& groupName
            ) = 0;

        virtual void EndGroup
            (   const std::string& groupName,
                std::size_t succeeded,
                std::size_t failed
            ) = 0;

        virtual void StartSection
            (   const std::string& sectionName,
                const std::string description
            ) = 0;

        virtual void EndSection
            (   const std::string& sectionName,
                std::size_t succeeded,
                std::size_t failed
            ) = 0;

        virtual void StartTestCase
            (   const TestCaseInfo& testInfo
            ) = 0;

        virtual void EndTestCase
            (   const TestCaseInfo& testInfo,
                std::size_t succeeded,
                std::size_t failed,
                const std::string& stdOut,
                const std::string& stdErr
            ) = 0;

        virtual void Result
            (   const ResultInfo& result
            ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////
    struct IReporterFactory
    {
        virtual ~IReporterFactory
            (){}

        virtual IReporter* create
            (   const IReporterConfig& config
            ) const = 0;

        virtual std::string getDescription
            () const = 0;
    };

    ///////////////////////////////////////////////////////////////////////////
    struct IReporterRegistry
    {
        typedef std::map<std::string, IReporterFactory*> FactoryMap;

        virtual ~IReporterRegistry
            (){}

        virtual IReporter* create
            (   const std::string& name,
                const IReporterConfig& config
            ) const = 0;

        virtual void registerReporter
            (   const std::string& name,
                IReporterFactory* factory
            ) = 0;

        virtual const FactoryMap& getFactories
            () const = 0;

    };

    ///////////////////////////////////////////////////////////////////////////
    inline std::string trim( const std::string& str )
    {
        std::string::size_type start = str.find_first_not_of( "\n\r\t " );
        std::string::size_type end = str.find_last_not_of( "\n\r\t " );

        return start < end ? str.substr( start, 1+end-start ) : "";
    }


}


#include <memory>
#include <vector>
#include <stdlib.h>

namespace Catch
{
    class TestCaseInfo;
    struct IResultCapture;
    struct ITestCaseRegistry;
    struct IRunner;
    struct IExceptionTranslatorRegistry;
    class GeneratorsForTest;

    class StreamBufBase : public std::streambuf
    {
    };

    class Hub
    {
        Hub();

        static Hub& me();

        Hub( const Hub& );
        void operator=( const Hub& );

    public:

        static void setRunner
            ( IRunner* runner
            );

        static void setResultCapture
            ( IResultCapture* resultCapture
            );

        static IResultCapture& getResultCapture
            ();

        static IReporterRegistry& getReporterRegistry
            ();

        static ITestCaseRegistry& getTestCaseRegistry
            ();

        static IExceptionTranslatorRegistry& getExceptionTranslatorRegistry
            ();

        static std::streambuf* createStreamBuf
            ( const std::string& streamName
            );

        static IRunner& getRunner
            ();

        static size_t getGeneratorIndex
            (   const std::string& fileInfo,
                size_t totalSize
            );

        static bool advanceGeneratorsForCurrentTest
            ();

        static void cleanUp
            ();

    private:

        static Hub*& singleInstance();

        GeneratorsForTest* findGeneratorsForCurrentTest
            ();

        GeneratorsForTest& getGeneratorsForCurrentTest
            ();

        std::auto_ptr<IReporterRegistry> m_reporterRegistry;
        std::auto_ptr<ITestCaseRegistry> m_testCaseRegistry;
        std::auto_ptr<IExceptionTranslatorRegistry> m_exceptionTranslatorRegistry;
        IRunner* m_runner;
        IResultCapture* m_resultCapture;
        std::map<std::string, GeneratorsForTest*> m_generatorsByTestName;
    };
}

// #included from: internal/catch_test_registry.hpp

/*
 *  catch_test_registry.hpp
 *  Catch
 *
 *  Created by Phil on 18/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_CATCH_REGISTRY_HPP_INCLUDED

// #included from: catch_interfaces_testcase.h

/*
 *  catch_interfaces_testcase.h
 *  Catch
 *
 *  Created by Phil on 07/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_INTERFACES_TESTCASE_H_INCLUDED

#include <vector>

namespace Catch
{
    struct ITestCase
    {
        virtual ~ITestCase
        ()
        {}

        virtual void invoke
            () const = 0;

        virtual ITestCase* clone
            () const = 0;

        virtual bool operator ==
            ( const ITestCase& other
            ) const = 0;

        virtual bool operator <
            ( const ITestCase& other
            ) const = 0;
    };

    class TestCaseInfo;

    struct ITestCaseRegistry
    {
        virtual ~ITestCaseRegistry
        ()
        {}

        virtual void registerTest
            ( const TestCaseInfo& testInfo
            ) = 0;

        virtual const std::vector<TestCaseInfo>& getAllTests
            () const = 0;

        virtual std::vector<TestCaseInfo> getMatchingTestCases
            ( const std::string& rawTestSpec
            ) = 0;
    };
}




namespace Catch
{

template<typename C>
struct MethodTestCase : ITestCase
{
    ///////////////////////////////////////////////////////////////////////////
    MethodTestCase
    (
        void (C::*method)()
    )
    : m_method( method )
    {}

    ///////////////////////////////////////////////////////////////////////////
    virtual void invoke
    ()
    const
    {
        C obj;
        (obj.*m_method)();
    }

    ///////////////////////////////////////////////////////////////////////////
    virtual ITestCase* clone
    ()
    const
    {
        return new MethodTestCase<C>( m_method );
    }

    ///////////////////////////////////////////////////////////////////////////
    virtual bool operator ==
    (
        const ITestCase& other
    )
    const
    {
        const MethodTestCase* mtOther = dynamic_cast<const MethodTestCase*>( &other );
        return mtOther && m_method == mtOther->m_method;
    }

    ///////////////////////////////////////////////////////////////////////////
    virtual bool operator <
    (
        const ITestCase& other
    )
    const
    {
        const MethodTestCase* mtOther = dynamic_cast<const MethodTestCase*>( &other );
        return mtOther && &m_method < &mtOther->m_method;
    }

private:
    void (C::*m_method)();
};

typedef void(*TestFunction)();

struct AutoReg
{
    AutoReg
        (   TestFunction function,
            const char* name,
            const char* description,
            const char* filename,
            std::size_t line
        );

    ///////////////////////////////////////////////////////////////////////////
    template<typename C>
    AutoReg
    (
        void (C::*method)(),
        const char* name,
        const char* description,
        const char* filename,
        std::size_t line
    )
    {
        registerTestCase( new MethodTestCase<C>( method ), name, description, filename, line );
    }

    ///////////////////////////////////////////////////////////////////////////
    void registerTestCase
    (
        ITestCase* testCase,
        const char* name,
        const char* description,
        const char* filename,
        std::size_t line
    );

    ~AutoReg
        ();

private:
    AutoReg
        ( const AutoReg& );

    void operator=
        ( const AutoReg& );
};

} // end namespace Catch

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TESTCASE( Name, Desc ) \
    static void INTERNAL_CATCH_UNIQUE_NAME( catch_internal_TestFunction )(); \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  catch_internal_TestFunction ), Name, Desc, __FILE__, __LINE__ ); }\
    static void INTERNAL_CATCH_UNIQUE_NAME(  catch_internal_TestFunction )()

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TESTCASE_NORETURN( Name, Desc ) \
    static void INTERNAL_CATCH_UNIQUE_NAME( catch_internal_TestFunction )() ATTRIBUTE_NORETURN; \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &INTERNAL_CATCH_UNIQUE_NAME(  catch_internal_TestFunction ), Name, Desc, __FILE__, __LINE__ ); }\
    static void INTERNAL_CATCH_UNIQUE_NAME(  catch_internal_TestFunction )()

///////////////////////////////////////////////////////////////////////////////
#define CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, Name, Desc ) \
    namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( &QualifiedMethod, Name, Desc, __FILE__, __LINE__ ); }

///////////////////////////////////////////////////////////////////////////////
#define TEST_CASE_METHOD( ClassName, TestName, Desc )\
    namespace{ \
        struct INTERNAL_CATCH_UNIQUE_NAME( Catch_FixtureWrapper ) : ClassName{ \
            void test(); \
        }; \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( &INTERNAL_CATCH_UNIQUE_NAME( Catch_FixtureWrapper )::test, TestName, Desc, __FILE__, __LINE__ ); \
    } \
    void INTERNAL_CATCH_UNIQUE_NAME( Catch_FixtureWrapper )::test()

// #included from: internal/catch_capture.hpp

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
#define TWOBLUECUBES_CATCH_CAPTURE_HPP_INCLUDED

// #included from: catch_resultinfo.hpp

/*
 *  catch_resultinfo.hpp
 *  Catch
 *
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_CATCH_RESULT_INFO_HPP_INCLUDED

#include <string>
// #included from: catch_result_type.h

/*
 *  catch_result_type.h
 *  Catch
 *
 *  Created by Phil on 07/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_RESULT_TYPE_H_INCLUDED

namespace Catch
{

struct ResultWas{ enum OfType
    {
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

    struct ResultAction
    {
        enum Value
        {
            None,
            Failed = 1,     // Failure - but no debug break if Debug bit not set
            DebugFailed = 3 // Indicates that the debugger should break, if possible
        };
    };

}



namespace Catch
{
    class ResultInfo
    {
    public:

        ///////////////////////////////////////////////////////////////////////////
        ResultInfo
        ()
        :   m_macroName(),
            m_filename(),
            m_line( 0 ),
            m_expr(),
            m_lhs(),
            m_rhs(),
            m_op(),
            m_message(),
            m_result( ResultWas::Unknown ),
            m_isNot( false )
        {}

        ///////////////////////////////////////////////////////////////////////////
        ResultInfo
        (
            const char* expr,
            ResultWas::OfType result,
            bool isNot,
            const char* filename,
            std::size_t line,
            const char* macroName,
            const char* message
        )
        :   m_macroName( macroName ),
            m_filename( filename ),
            m_line( line ),
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

        ///////////////////////////////////////////////////////////////////////////
        virtual ~ResultInfo
        ()
        {
        }

        ///////////////////////////////////////////////////////////////////////////
        bool ok
        ()
        const
        {
            return ( m_result & ResultWas::FailureBit ) != ResultWas::FailureBit;
        }

        ///////////////////////////////////////////////////////////////////////////
        ResultWas::OfType getResultType
        ()
        const
        {
            return m_result;
        }

        ///////////////////////////////////////////////////////////////////////////
        bool hasExpression
        ()
        const
        {
            return !m_expr.empty();
        }

        ///////////////////////////////////////////////////////////////////////////
        bool hasMessage
        ()
        const
        {
            return !m_message.empty();
        }

        ///////////////////////////////////////////////////////////////////////////
        std::string getExpression
        ()
        const
        {
            return m_expr;
        }

        ///////////////////////////////////////////////////////////////////////////
        std::string getExpandedExpression
        ()
        const
        {
            return hasExpression() ? getExpandedExpressionInternal() : "";
        }

        ///////////////////////////////////////////////////////////////////////////
        std::string getMessage
        ()
        const
        {
            return m_message;
        }

        ///////////////////////////////////////////////////////////////////////////
        std::string getFilename
        ()
        const
        {
            return m_filename;
        }

        ///////////////////////////////////////////////////////////////////////////
        std::size_t getLine
        ()
        const
        {
            return m_line;
        }

        ///////////////////////////////////////////////////////////////////////////
        std::string getTestMacroName
        ()
        const
        {
            return m_macroName;
        }

    protected:

        ///////////////////////////////////////////////////////////////////////////
        std::string getExpandedExpressionInternal
        ()
        const
        {
            if( m_op == "" || m_isNot )
                return m_lhs.empty() ? m_expr : m_op + m_lhs;
            else if( m_op != "!" )
                return m_lhs + " " + m_op + " " + m_rhs;
            else
                return "{can't expand - use " + m_macroName + "_NOT( " + m_expr.substr(1) + " ) instead of " + m_macroName + "( " + m_expr + " ) for better diagnostics}";
        }

        ///////////////////////////////////////////////////////////////////////////
        bool isNotExpression
        (
            const char* expr
        )
        {
            return expr && expr[0] == '!';
        }

    protected:
        std::string m_macroName;
        std::string m_filename;
        std::size_t m_line;
        std::string m_expr, m_lhs, m_rhs, m_op;
        std::string m_message;
        ResultWas::OfType m_result;
        bool m_isNot;
    };

} // end namespace Catch


// #included from: catch_interfaces_capture.h

/*
 *  catch_interfaces_capture.h
 *  Catch
 *
 *  Created by Phil on 07/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_CATCH_INTERFACES_CAPTURE_H_INCLUDED

#include <string>

namespace Catch
{
    class TestCaseInfo;
    class ScopedInfo;
    class MutableResultInfo;
    class ResultInfo;

    struct IResultCapture
    {
        virtual ~IResultCapture
        ()
        {}

        virtual void testEnded
            (   const ResultInfo& result
            ) = 0;
        virtual bool sectionStarted
            (   const std::string& name,
                const std::string& description,
                const std::string& filename,
                std::size_t line,
                std::size_t& successes,
                std::size_t& failures
            ) = 0;
        virtual void sectionEnded
            (   const std::string& name,
                std::size_t successes,
                std::size_t failures
            ) = 0;
        virtual void pushScopedInfo
            (   ScopedInfo* scopedInfo
            ) = 0;
        virtual void popScopedInfo
            (   ScopedInfo* scopedInfo
            ) = 0;
        virtual bool shouldDebugBreak
            () const = 0;

        virtual ResultAction::Value acceptResult
            (   bool result
            ) = 0;
        virtual ResultAction::Value acceptResult
            (   ResultWas::OfType result
            ) = 0;
        virtual ResultAction::Value acceptExpression
            (   const MutableResultInfo& resultInfo
            ) = 0;
        virtual void acceptMessage
            (   const std::string& msg
            ) = 0;

        virtual std::string getCurrentTestName
            () const = 0;
        virtual const ResultInfo* getLastResult
            () const = 0;

    };
}

// #included from: catch_debugger.hpp

/*
 *  catch_debugger.hpp
 *  Catch
 *
 *  Created by Phil on 27/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 * Provides a BreakIntoDebugger() macro for Windows and Mac (so far)
 */

#define TWOBLUECUBES_CATCH_DEBUGGER_HPP_INCLUDED

#include <iostream>

#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#define CATCH_PLATFORM_MAC
#elif defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER)
#define CATCH_PLATFORM_WINDOWS
#endif

#ifdef CATCH_PLATFORM_MAC

    #include <assert.h>
    #include <stdbool.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <sys/sysctl.h>

    namespace Catch
    {
        // The following function is taken directly from the following technical note:
        // http://developer.apple.com/library/mac/#qa/qa2004/qa1361.html

        inline bool isDebuggerActive()
        // Returns true if the current process is being debugged (either
        // running under the debugger or has a debugger attached post facto).
        {
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
            if( Catch::isDebuggerActive() ) \
            { \
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
    inline bool isDebuggerActive()
    {
        return IsDebuggerPresent() != 0;
    }
#else
	   inline void BreakIntoDebugger(){}
	   inline bool isDebuggerActive() { return false; }
#endif

#ifdef CATCH_PLATFORM_WINDOWS
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA( const char* );
inline void writeToDebugConsole( const std::string& text )
{
    ::OutputDebugStringA( text.c_str() );
}
#else
inline void writeToDebugConsole( const std::string& text )
{
    // !TBD: Need a version for Mac/ XCode and other IDEs
    std::cout << text;
}
#endif // CATCH_PLATFORM_WINDOWS

// #included from: catch_evaluate.hpp

/*
 *  catch_evaluate.hpp
 *  Catch
 *
 *  Created by Phil on 04/03/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_EVALUATE_HPP_INCLUDED

namespace Catch
{
namespace Internal
{
    enum Operator
    {
        IsEqualTo,
        IsNotEqualTo,
        IsLessThan,
        IsGreaterThan,
        IsLessThanOrEqualTo,
        IsGreaterThanOrEqualTo
    };

    template<Operator Op>
    struct OperatorTraits{ static const char* getName(){ return "*error - unknown operator*"; } };

    template<>
    struct OperatorTraits<IsEqualTo>{ static const char* getName(){ return "=="; } };

    template<>
    struct OperatorTraits<IsNotEqualTo>{ static const char* getName(){ return "!="; } };

    template<>
    struct OperatorTraits<IsLessThan>{ static const char* getName(){ return "<"; } };

    template<>
    struct OperatorTraits<IsGreaterThan>{ static const char* getName(){ return ">"; } };

    template<>
    struct OperatorTraits<IsLessThanOrEqualTo>{ static const char* getName(){ return "<="; } };

    template<>
    struct OperatorTraits<IsGreaterThanOrEqualTo>{ static const char* getName(){ return ">="; } };

    // So the compare overloads can be operator agnostic we convey the operator as a template
    // enum, which is used to specialise an Evaluator for doing the comparison.
    template<typename T1, typename T2, Operator Op>
    class Evaluator{};

    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsEqualTo>
    {
        static bool evaluate( const T1& lhs, const T2& rhs)
        {
            return const_cast<T1&>( lhs ) == const_cast<T2&>( rhs );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsNotEqualTo>
    {
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return const_cast<T1&>( lhs ) != const_cast<T2&>( rhs );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsLessThan>
    {
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return const_cast<T1&>( lhs ) < const_cast<T2&>( rhs );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsGreaterThan>
    {
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return const_cast<T1&>( lhs ) > const_cast<T2&>( rhs );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsGreaterThanOrEqualTo>
    {
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return const_cast<T1&>( lhs ) >= const_cast<T2&>( rhs );
        }
    };
    template<typename T1, typename T2>
    struct Evaluator<T1, T2, IsLessThanOrEqualTo>
    {
        static bool evaluate( const T1& lhs, const T2& rhs )
        {
            return const_cast<T1&>( lhs ) <= const_cast<T2&>( rhs );
        }
    };

    template<Operator Op, typename T1, typename T2>
    bool applyEvaluator( const T1& lhs, const T2& rhs )
    {
        return Evaluator<T1, T2, Op>::evaluate( lhs, rhs );
    }

    // "base" overload
    template<Operator Op, typename T1, typename T2>
    bool compare( const T1& lhs, const T2& rhs )
    {
        return Evaluator<T1, T2, Op>::evaluate( lhs, rhs );
    }

    // unsigned X to int
    template<Operator Op> bool compare( unsigned int lhs, int rhs )
    {
        return applyEvaluator<Op>( lhs, static_cast<unsigned int>( rhs ) );
    }
    template<Operator Op> bool compare( unsigned long lhs, int rhs )
    {
        return applyEvaluator<Op>( lhs, static_cast<unsigned int>( rhs ) );
    }
    template<Operator Op> bool compare( unsigned char lhs, int rhs )
    {
        return applyEvaluator<Op>( lhs, static_cast<unsigned int>( rhs ) );
    }

    // unsigned X to long
    template<Operator Op> bool compare( unsigned int lhs, long rhs )
    {
        return applyEvaluator<Op>( lhs, static_cast<unsigned long>( rhs ) );
    }
    template<Operator Op> bool compare( unsigned long lhs, long rhs )
    {
        return applyEvaluator<Op>( lhs, static_cast<unsigned long>( rhs ) );
    }
    template<Operator Op> bool compare( unsigned char lhs, long rhs )
    {
        return applyEvaluator<Op>( lhs, static_cast<unsigned long>( rhs ) );
    }

    // int to unsigned X
    template<Operator Op> bool compare( int lhs, unsigned int rhs )
    {
        return applyEvaluator<Op>( static_cast<unsigned int>( lhs ), rhs );
    }
    template<Operator Op> bool compare( int lhs, unsigned long rhs )
    {
        return applyEvaluator<Op>( static_cast<unsigned int>( lhs ), rhs );
    }
    template<Operator Op> bool compare( int lhs, unsigned char rhs )
    {
        return applyEvaluator<Op>( static_cast<unsigned int>( lhs ), rhs );
    }

    // long to unsigned X
    template<Operator Op> bool compare( long lhs, unsigned int rhs )
    {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ) );
    }
    template<Operator Op> bool compare( long lhs, unsigned long rhs )
    {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ) );
    }
    template<Operator Op> bool compare( long lhs, unsigned char rhs )
    {
        return applyEvaluator<Op>( static_cast<unsigned long>( lhs ) );
    }

    template<Operator Op, typename T>
    bool compare( long lhs, const T* rhs )
    {
        return Evaluator<T*, T*, Op>::evaluate( reinterpret_cast<T*>( NULL ), rhs );

    }

    template<Operator Op, typename T>
    bool compare( long lhs, T* rhs )
    {
        return Evaluator<T*, T*, Op>::evaluate( reinterpret_cast<T*>( lhs ), rhs );

    }

} // end of namespace Internal
} // end of namespace Catch

#include <sstream>

namespace Catch
{

namespace Detail
{
    struct NonStreamable
    {
        template<typename T>
        NonStreamable( const T& )
        {
        }
    };

    // If the type does not have its own << overload for ostream then
    // this one will be used instead
    inline std::ostream& operator << ( std::ostream& ss, NonStreamable )
    {
        ss << "{?}";
        return ss;
    }

    template<typename T>
    inline std::string makeString
    (
        const T& value
    )
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

}// end namespace Detail

///////////////////////////////////////////////////////////////////////////////
template<typename T>
std::string toString
(
    const T& value
)
{
    return Detail::makeString( value );
}

// Shortcut overloads

///////////////////////////////////////////////////////////////////////////////
inline std::string toString
(
    const std::string& value
)
{
    return "\"" + value + "\"";
}

///////////////////////////////////////////////////////////////////////////////
inline std::string toString
(
    const std::wstring& value
)
{
    std::ostringstream oss;
    oss << "\"";
    for(size_t i = 0; i < value.size(); ++i )
        oss << static_cast<char>( value[i] <= 0xff ? value[i] : '?');
    oss << "\"";
    return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
inline std::string toString
(
    const char* const value
)
{
    return value ? Catch::toString( std::string( value ) ) : std::string( "{null string}" );
}

///////////////////////////////////////////////////////////////////////////////
inline std::string toString
(
    char* const value
)
{
    return Catch::toString( static_cast<const char* const>( value ) );
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
    unsigned int value
)
{
    std::ostringstream oss;
    if( value > 8192 )
        oss << "0x" << std::hex << value;
    else
        oss << value;
    return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
inline std::string toString
(
    unsigned long value
)
{
    std::ostringstream oss;
    if( value > 8192 )
        oss << "0x" << std::hex << value;
    else
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

///////////////////////////////////////////////////////////////////////////////
inline std::string toString
(
    bool value
)
{
    return value ? "true" : "false";
}

///////////////////////////////////////////////////////////////////////////////
inline std::string toString
(
    void* p
)
{
    if( !p )
        return INTERNAL_CATCH_STRINGIFY( NULL );
    std::ostringstream oss;
    oss << p;
    return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
template<typename T>
inline std::string toString
(
    T* p
)
{
    return Catch::toString( static_cast<void*>( p ) );
}

///////////////////////////////////////////////////////////////////////////////
template<typename T>
inline std::string toString
(
    const T* p
)
{
    return Catch::toString( static_cast<void*>( const_cast<T*>( p ) ) );
}

struct TestFailureException
{
};
struct DummyExceptionType_DontUse
{
};
struct STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison;

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
        const char* macroName,
        const char* message = ""
    )
    : ResultInfo( expr, ResultWas::Unknown, isNot, filename, line, macroName, message )
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
    void setFileAndLine
    (
        const std::string& filename,
        std::size_t line
    )
    {
        m_filename = filename;
        m_line = line;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator ||
    (
        const RhsT&
    );

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    STATIC_ASSERT_Expression_Too_Complex_Please_Rewrite_As_Binary_Comparison& operator &&
    (
        const RhsT&
    );

private:
    friend class ResultBuilder;
    template<typename T> friend class Expression;

    template<typename T> friend class PtrExpression;

    ///////////////////////////////////////////////////////////////////////////
    MutableResultInfo& captureBoolExpression
    (
        bool result
    )
    {
        m_lhs = Catch::toString( result );
        m_op = m_isNot ? "!" : "";
        setResultType( result ? ResultWas::Ok : ResultWas::ExpressionFailed );
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<Internal::Operator Op, typename T1, typename T2>
    MutableResultInfo& captureExpression
    (
        const T1& lhs,
        const T2& rhs
    )
    {
        setResultType( Internal::compare<Op>( lhs, rhs ) ? ResultWas::Ok : ResultWas::ExpressionFailed );
        m_lhs = Catch::toString( lhs );
        m_rhs = Catch::toString( rhs );
        m_op = Internal::OperatorTraits<Op>::getName();
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<Internal::Operator Op, typename T>
    MutableResultInfo& captureExpression
    (
        const T* lhs,
        int rhs
    )
    {
        return captureExpression<Op>( lhs, reinterpret_cast<const T*>( rhs ) );
    }
};

template<typename T>
class Expression
{
	void operator = ( const Expression& );

public:
    ///////////////////////////////////////////////////////////////////////////
    Expression
    (
        MutableResultInfo& result,
        T lhs
    )
    :   m_result( result ),
        m_lhs( lhs )
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator ==
    (
        const RhsT& rhs
    )
    {
        return m_result.captureExpression<Internal::IsEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator !=
    (
        const RhsT& rhs
    )
    {
        return m_result.captureExpression<Internal::IsNotEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator <
    (
        const RhsT& rhs
    )
    {
        return m_result.captureExpression<Internal::IsLessThan>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator >
    (
        const RhsT& rhs
    )
    {
        return m_result.captureExpression<Internal::IsGreaterThan>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator <=
    (
        const RhsT& rhs
    )
    {
        return m_result.captureExpression<Internal::IsLessThanOrEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator >=
    (
        const RhsT& rhs
    )
    {
        return m_result.captureExpression<Internal::IsGreaterThanOrEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    MutableResultInfo& operator ==
    (
        bool rhs
    )
    {
        return m_result.captureExpression<Internal::IsEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    MutableResultInfo& operator !=
    (
        bool rhs
    )
    {
        return m_result.captureExpression<Internal::IsNotEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    operator MutableResultInfo&
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
    MutableResultInfo& m_result;
    T m_lhs;
};

template<typename LhsT>
class PtrExpression
{
public:

    ///////////////////////////////////////////////////////////////////////////
    PtrExpression
    (
        MutableResultInfo& result,
        const LhsT* lhs
    )
    :   m_result( &result ),
        m_lhs( lhs )
    {}

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator ==
    (
        const RhsT* rhs
    )
    {
        return m_result->captureExpression<Internal::IsEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    // This catches NULL
    MutableResultInfo& operator ==
    (
        LhsT* rhs
    )
    {
        return m_result->captureExpression<Internal::IsEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename RhsT>
    MutableResultInfo& operator !=
    (
        const RhsT* rhs
    )
    {
        return m_result->captureExpression<Internal::IsNotEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    // This catches NULL
    MutableResultInfo& operator !=
    (
        LhsT* rhs
    )
    {
        return m_result->captureExpression<Internal::IsNotEqualTo>( m_lhs, rhs );
    }

    ///////////////////////////////////////////////////////////////////////////
    operator MutableResultInfo&
    ()
    {
        return m_result->captureBoolExpression( m_lhs );
    }


private:
    MutableResultInfo* m_result;
    const LhsT* m_lhs;
};

class ResultBuilder
{
public:

    ///////////////////////////////////////////////////////////////////////////
    ResultBuilder
    (
        const char* filename,
        std::size_t line,
        const char* macroName,
        const char* expr = "",
        bool isNot = false
    )
    : m_result( expr, isNot, filename, line, macroName ),
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
    ResultBuilder& setResultType
    (
        ResultWas::OfType resultType
    )
    {
        m_result.setResultType( resultType );
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    operator MutableResultInfo&
    ()
    {
        m_result.setMessage( m_messageStream.str() );
        return m_result;
    }

private:
    MutableResultInfo m_result;
    std::ostringstream m_messageStream;

};

class ScopedInfo
{
public:
    ///////////////////////////////////////////////////////////////////////////
    ScopedInfo
    () : m_oss()
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
#define INTERNAL_CATCH_ACCEPT_EXPR( expr, stopOnFailure ) \
    if( Catch::ResultAction::Value internal_catch_action = Catch::Hub::getResultCapture().acceptExpression( expr )  ) \
    { \
        if( internal_catch_action == Catch::ResultAction::DebugFailed ) BreakIntoDebugger(); \
        if( Catch::isTrue( stopOnFailure ) ) throw Catch::TestFailureException(); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_TEST( expr, isNot, stopOnFailure, macroName ) \
    try \
    { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ResultBuilder( __FILE__, __LINE__, macroName, #expr, isNot )->*expr ), stopOnFailure ); \
        if( Catch::isTrue( false ) ){ bool internal_catch_dummyResult = ( expr ); Catch::isTrue( internal_catch_dummyResult ); } \
    } \
    catch( Catch::TestFailureException& ) \
    { \
        throw; \
    } \
    catch( ... ) \
    { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ResultBuilder( __FILE__, __LINE__, macroName, #expr ) << Catch::Hub::getExceptionTranslatorRegistry().translateActiveException() ).setResultType( Catch::ResultWas::ThrewException ), false ); \
        throw; \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_IF( expr, isNot, stopOnFailure, macroName ) \
    INTERNAL_CATCH_TEST( expr, isNot, stopOnFailure, macroName ); \
    if( Catch::Hub::getResultCapture().getLastResult()->ok() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_ELSE( expr, isNot, stopOnFailure, macroName ) \
    INTERNAL_CATCH_TEST( expr, isNot, stopOnFailure, macroName ); \
    if( !Catch::Hub::getResultCapture().getLastResult()->ok() )

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_NO_THROW( expr, stopOnFailure, macroName ) \
    try \
    { \
        expr; \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::ResultBuilder( __FILE__, __LINE__, macroName, #expr ).setResultType( Catch::ResultWas::Ok ), stopOnFailure ); \
    } \
    catch( ... ) \
    { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ResultBuilder( __FILE__, __LINE__, macroName, #expr ) << Catch::Hub::getExceptionTranslatorRegistry().translateActiveException() ).setResultType( Catch::ResultWas::ThrewException ), stopOnFailure ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS( expr, exceptionType, stopOnFailure, macroName ) \
    try \
    { \
        expr; \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::ResultBuilder( __FILE__, __LINE__, macroName, #expr ).setResultType( Catch::ResultWas::DidntThrowException ), stopOnFailure ); \
    } \
    catch( Catch::TestFailureException& ) \
    { \
        throw; \
    } \
    catch( exceptionType ) \
    { \
        INTERNAL_CATCH_ACCEPT_EXPR( Catch::ResultBuilder( __FILE__, __LINE__, macroName, #expr ).setResultType( Catch::ResultWas::Ok ), stopOnFailure ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_THROWS_AS( expr, exceptionType, stopOnFailure, macroName ) \
    INTERNAL_CATCH_THROWS( expr, exceptionType, stopOnFailure, macroName ) \
    catch( ... ) \
    { \
        INTERNAL_CATCH_ACCEPT_EXPR( ( Catch::ResultBuilder( __FILE__, __LINE__, macroName, #expr ) << Catch::Hub::getExceptionTranslatorRegistry().translateActiveException() ).setResultType( Catch::ResultWas::ThrewException ), stopOnFailure ); \
    }

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_MSG( reason, resultType, stopOnFailure, macroName ) \
    Catch::Hub::getResultCapture().acceptExpression( ( Catch::ResultBuilder( __FILE__, __LINE__, macroName ) << reason ).setResultType( resultType ) );

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_SCOPED_INFO( log ) \
    Catch::ScopedInfo INTERNAL_CATCH_UNIQUE_NAME( info ); \
    INTERNAL_CATCH_UNIQUE_NAME( info ) << log

// #included from: internal/catch_section.hpp

/*
 *  catch_section.hpp
 *  Catch
 *
 *  Created by Phil on 03/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_SECTION_HPP_INCLUDED


#include <string>

namespace Catch
{
    class Section
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        Section
        (
            const std::string& name,
            const std::string& description,
            const std::string& filename,
            std::size_t line
        )
        :   m_name( name ),
            m_successes(0),
            m_failures(0),
            m_sectionIncluded( Hub::getResultCapture().sectionStarted( name, description, filename, line, m_successes, m_failures ) )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        ~Section
        ()
        {
            if( m_sectionIncluded )
                Hub::getResultCapture().sectionEnded( m_name, m_successes, m_failures );
        }

        ///////////////////////////////////////////////////////////////////////
        // This indicates whether the section should be executed or not
        operator bool
        ()
        {
            return m_sectionIncluded;
        }

    private:

        std::string m_name;
        std::size_t m_successes;
        std::size_t m_failures;
        bool m_sectionIncluded;
    };

} // end namespace Catch

#define INTERNAL_CATCH_SECTION( name, desc ) \
    if( Catch::Section INTERNAL_CATCH_UNIQUE_NAME( catch_internal_Section ) = Catch::Section( name, desc, __FILE__, __LINE__ ) )

// #included from: internal/catch_generators.hpp

/*
 *  catch_generators.hpp
 *  Catch
 *
 *  Created by Phil on 27/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_GENERATORS_HPP_INCLUDED


#include <iterator>
#include <vector>
#include <string>
#include <stdlib.h>

namespace Catch
{

template<typename T>
struct IGenerator
{
    virtual ~IGenerator
    ()
    {}

    virtual T getValue
        ( std::size_t index
        ) const = 0;

    virtual std::size_t size
        () const = 0;
};

template<typename T>
class BetweenGenerator : public IGenerator<T>
{
public:
    ///////////////////////////////////////////////////////////////////////////
    BetweenGenerator
    (
        T from,
        T to
    )
    :   m_from( from ),
        m_to( to )
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    virtual T getValue
    (
        std::size_t index
    )
    const
    {
        return m_from+static_cast<T>( index );
    }

    ///////////////////////////////////////////////////////////////////////////
    virtual std::size_t size
    ()
    const
    {
        return 1+m_to-m_from;
    }

private:

    T m_from;
    T m_to;
};

template<typename T>
class ValuesGenerator : public IGenerator<T>
{
public:
    ///////////////////////////////////////////////////////////////////////////
    ValuesGenerator
    ()
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    void add
    (
        T value
    )
    {
        m_values.push_back( value );
    }

    ///////////////////////////////////////////////////////////////////////////
    virtual T getValue
    (
        std::size_t index
    )
    const
    {
        return m_values[index];
    }

    ///////////////////////////////////////////////////////////////////////////
    virtual std::size_t size
    ()
    const
    {
        return m_values.size();
    }

private:

    std::vector<T> m_values;
};

template<typename T>
class CompositeGenerator
{
public:
    ///////////////////////////////////////////////////////////////////////////
    CompositeGenerator()
    :   m_totalSize( 0 )
    {
    }

    ///////////////////////////////////////////////////////////////////////////
	// *** Move semantics, similar to auto_ptr ***
    CompositeGenerator( CompositeGenerator& other )
	:	m_fileInfo( other.m_fileInfo ),
		m_totalSize( 0 )
    {
		move( other );
    }

    ///////////////////////////////////////////////////////////////////////////
    CompositeGenerator& setFileInfo
    (
        const char* fileInfo
    )
    {
        m_fileInfo = fileInfo;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    ~CompositeGenerator
    ()
    {
        deleteAll( m_composed );
    }

    ///////////////////////////////////////////////////////////////////////////
    operator T
    ()
    const
    {
        size_t overallIndex = Hub::getGeneratorIndex( m_fileInfo, m_totalSize );

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

    ///////////////////////////////////////////////////////////////////////////
    void add
    (
        const IGenerator<T>* generator
    )
    {
        m_totalSize += generator->size();
        m_composed.push_back( generator );
    }

    ///////////////////////////////////////////////////////////////////////////
    CompositeGenerator& then
    (
        CompositeGenerator& other
    )
    {
        move( other );
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////
    CompositeGenerator& then
    (
        T value
    )
    {
        ValuesGenerator<T>* valuesGen = new ValuesGenerator<T>();
        valuesGen->add( value );
        add( valuesGen );
        return *this;
    }

private:

    ///////////////////////////////////////////////////////////////////////////
    void move
    (
        CompositeGenerator& other
    )
    {
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
    ///////////////////////////////////////////////////////////////////////////
    template<typename T>
    CompositeGenerator<T> between
    (
        T from,
        T to
    )
    {
        CompositeGenerator<T> generators;
        generators.add( new BetweenGenerator<T>( from, to ) );
        return generators;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename T>
    CompositeGenerator<T> values
    (
        T val1,
        T val2
    )
    {
        CompositeGenerator<T> generators;
        ValuesGenerator<T>* valuesGen = new ValuesGenerator<T>();
        valuesGen->add( val1 );
        valuesGen->add( val2 );
        generators.add( valuesGen );
        return generators;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename T>
    CompositeGenerator<T> values
    (
        T val1,
        T val2,
        T val3
    )
    {
        CompositeGenerator<T> generators;
        ValuesGenerator<T>* valuesGen = new ValuesGenerator<T>();
        valuesGen->add( val1 );
        valuesGen->add( val2 );
        valuesGen->add( val3 );
        generators.add( valuesGen );
        return generators;
    }

    ///////////////////////////////////////////////////////////////////////////
    template<typename T>
    CompositeGenerator<T> values
    (
        T val1,
        T val2,
        T val3,
        T val4
    )
    {
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

/*
 *  catch_exception_interfaces.h
 *  Catch
 *
 *  Created by Phil on 20/04/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_CATCH_INTERFACES_EXCEPTIONS_H_INCLUDED

#include <string>

namespace Catch
{
    typedef std::string(*exceptionTranslateFunction)();

    struct IExceptionTranslator
    {
        virtual ~IExceptionTranslator(){}
        virtual std::string translate() const = 0;
    };

    struct IExceptionTranslatorRegistry
    {
        virtual ~IExceptionTranslatorRegistry
        ()
        {}

        virtual void registerTranslator
            (   IExceptionTranslator* translator
            ) = 0;
        virtual std::string translateActiveException
            () const = 0;

    };

    class ExceptionTranslatorRegistrar
    {
        template<typename T>
        class ExceptionTranslator : public IExceptionTranslator
        {
        public:

            ExceptionTranslator
            (
                std::string(*translateFunction)( T& )
            )
            : m_translateFunction( translateFunction )
            {}

            virtual std::string translate
            ()
            const
            {
                try
                {
                    throw;
                }
                catch( T& ex )
                {
                    return m_translateFunction( ex );
                }
            }

        protected:
            std::string(*m_translateFunction)( T& );
        };

    public:
        template<typename T>
        ExceptionTranslatorRegistrar
        (
            std::string(*translateFunction)( T& )
        )
        {
            Catch::Hub::getExceptionTranslatorRegistry().registerTranslator
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

/*
 *  catch_approx.hpp
 *  Catch
 *
 *  Created by Phil on 28/04/2011.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_CATCH_APPROX_HPP_INCLUDED


#include <cmath>
#include <limits>

namespace Catch
{
    namespace Detail
    {
        class Approx
        {
        public:
            ///////////////////////////////////////////////////////////////////////////
            explicit Approx
            (
                double value
            )
            :   m_epsilon( std::numeric_limits<float>::epsilon()*100 ),
                m_scale( 1.0 ),
                m_value( value )
            {
            }

            ///////////////////////////////////////////////////////////////////////////
            Approx
            (
                const Approx& other
            )
            :   m_epsilon( other.m_epsilon ),
                m_scale( other.m_scale ),
                m_value( other.m_value )
            {
            }

            ///////////////////////////////////////////////////////////////////////////
            static Approx custom
            ()
            {
                return Approx( 0 );
            }

            ///////////////////////////////////////////////////////////////////////////
            Approx operator()
            (
                double value
            )
            {
                Approx approx( value );
                approx.epsilon( m_epsilon );
                approx.scale( m_scale );
                return approx;
            }

            ///////////////////////////////////////////////////////////////////////////
            friend bool operator ==
            (
                double lhs,
                const Approx& rhs
            )
            {
                // Thanks to Richard Harris for his help refining this formula
                return fabs( lhs - rhs.m_value ) < rhs.m_epsilon * (rhs.m_scale + (std::max)( fabs(lhs), fabs(rhs.m_value) ) );
            }

            ///////////////////////////////////////////////////////////////////////////
            friend bool operator ==
            (
                const Approx& lhs,
                double rhs
            )
            {
                return operator==( rhs, lhs );
            }

            ///////////////////////////////////////////////////////////////////////////
            friend bool operator !=
            (
                double lhs,
                const Approx& rhs
            )
            {
                return !operator==( lhs, rhs );
            }

            ///////////////////////////////////////////////////////////////////////////
            friend bool operator !=
            (
                const Approx& lhs,
                double rhs
            )
            {
                return !operator==( rhs, lhs );
            }


            ///////////////////////////////////////////////////////////////////////////
            Approx& epsilon
            (
                double newEpsilon
            )
            {
                m_epsilon = newEpsilon;
                return *this;
            }

            ///////////////////////////////////////////////////////////////////////////
            Approx& scale
            (
                double newScale
            )
            {
                m_scale = newScale;
                return *this;
            }

            ///////////////////////////////////////////////////////////////////////////
            std::string toString() const
            {
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

    ///////////////////////////////////////////////////////////////////////////////
    template<>
    inline std::string toString<Detail::Approx>
    (
        const Detail::Approx& value
    )
    {
        return value.toString();
    }

} // end namespace Catch

// #included from: internal/catch_test_case_info.hpp

/*
 *  catch_test_case_info.hpp
 *  Catch
 *
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_TESTCASEINFO_HPP_INCLUDED

#include <map>
#include <string>

namespace Catch
{
    class TestCaseInfo
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        TestCaseInfo
        (
            ITestCase* testCase,
            const char* name,
            const char* description,
            const char* filename,
            std::size_t line
        )
        :   m_test( testCase ),
            m_name( name ),
            m_description( description ),
            m_filename( filename ),
            m_line( line )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        TestCaseInfo
        ()
        :   m_test( NULL ),
            m_name(),
            m_description(),
            m_filename(),
            m_line( 0 )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        TestCaseInfo
        (
            const TestCaseInfo& other
        )
        :   m_test( other.m_test->clone() ),
            m_name( other.m_name ),
            m_description( other.m_description ),
            m_filename( other.m_filename ),
            m_line( other.m_line )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        TestCaseInfo
        (
            const TestCaseInfo& other,
            const std::string& name
        )
        :   m_test( other.m_test->clone() ),
            m_name( name ),
            m_description( other.m_description ),
            m_filename( other.m_filename ),
            m_line( other.m_line )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        TestCaseInfo& operator =
        (
            const TestCaseInfo& other
        )
        {
            TestCaseInfo temp( other );
            swap( temp );
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        ~TestCaseInfo
        ()
        {
            delete m_test;
        }

        ///////////////////////////////////////////////////////////////////////
        void invoke
        ()
        const
        {
            m_test->invoke();
        }

        ///////////////////////////////////////////////////////////////////////
        const std::string& getName
        ()
        const
        {
            return m_name;
        }

        ///////////////////////////////////////////////////////////////////////
        const std::string& getDescription
        ()
        const
        {
            return m_description;
        }

        ///////////////////////////////////////////////////////////////////////
        const std::string& getFilename
        ()
        const
        {
            return m_filename;
        }

        ///////////////////////////////////////////////////////////////////////
        std::size_t getLine
        ()
        const
        {
            return m_line;
        }

        ///////////////////////////////////////////////////////////////////////
        bool isHidden
        ()
        const
        {
            return m_name.size() >= 2 && m_name[0] == '.' && m_name[1] == '/';
        }

        ///////////////////////////////////////////////////////////////////////
        void swap
        (
            TestCaseInfo& other
        )
        {
            std::swap( m_test, other.m_test );
            m_name.swap( other.m_name );
            m_description.swap( other.m_description );
        }

        ///////////////////////////////////////////////////////////////////////
        bool operator ==
        (
            const TestCaseInfo& other
        )
        const
        {
            return *m_test == *other.m_test && m_name == other.m_name;
        }

        ///////////////////////////////////////////////////////////////////////
        bool operator <
        (
            const TestCaseInfo& other
        )
        const
        {
            return m_name < other.m_name;
        }

    private:
        ITestCase* m_test;
        std::string m_name;
        std::string m_description;
        std::string m_filename;
        std::size_t m_line;

    };

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    class TestSpec
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        TestSpec
        (
            const std::string& rawSpec
        )
        :   m_rawSpec( rawSpec ),
            m_isWildcarded( false )
        {
            if( m_rawSpec[m_rawSpec.size()-1] == '*' )
            {
                m_rawSpec = m_rawSpec.substr( 0, m_rawSpec.size()-1 );
                m_isWildcarded = true;
            }
        }

        ///////////////////////////////////////////////////////////////////////
        bool matches
        (
            const std::string& testName
        )
        const
        {
            if( !m_isWildcarded )
                return m_rawSpec == testName;
            else
                return testName.size() >= m_rawSpec.size() && testName.substr( 0, m_rawSpec.size() ) == m_rawSpec;
        }

    private:
        std::string m_rawSpec;
        bool m_isWildcarded;
    };
}


#ifdef __OBJC__
// #included from: internal/catch_objc.hpp

/*
 *  catch_objc.hpp
 *  Catch
 *
 *  Created by Phil on 14/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_OBJC_HPP_INCLUDED

#import <Foundation/Foundation.h>
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

namespace Catch
{
    class OcMethod : public ITestCase
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        OcMethod
        (
            Class cls,
            SEL sel
        )
        :   m_cls( cls ),
            m_sel( sel )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        virtual void invoke
        ()
        const
        {
            id obj = class_createInstance( m_cls, 0 );
            obj = [obj init];

            if( [obj respondsToSelector: @selector(setUp) ] )
                [obj performSelector: @selector(setUp)];

            if( [obj respondsToSelector: m_sel] )
                [obj performSelector: m_sel];

            if( [obj respondsToSelector: @selector(tearDown) ] )
                [obj performSelector: @selector(tearDown)];

            [obj release];
        }

        ///////////////////////////////////////////////////////////////////////
        virtual ITestCase* clone
        ()
        const
        {
            return new OcMethod( m_cls, m_sel );
        }

        ///////////////////////////////////////////////////////////////////////
        virtual bool operator ==
        (
            const ITestCase& other
        )
        const
        {
            const OcMethod* ocmOther = dynamic_cast<const OcMethod*> ( &other );
            return ocmOther && ocmOther->m_sel == m_sel;
        }

        ///////////////////////////////////////////////////////////////////////
        virtual bool operator <
        (
            const ITestCase& other
        )
        const
        {
            const OcMethod* ocmOther = dynamic_cast<const OcMethod*> ( &other );
            return ocmOther && ocmOther->m_sel < m_sel;
        }

    private:
        Class m_cls;
        SEL m_sel;
    };

    namespace Detail
    {

        ///////////////////////////////////////////////////////////////////////
        inline bool startsWith
        (
            const std::string& str,
            const std::string& sub
        )
        {
            return str.length() > sub.length() && str.substr( 0, sub.length() ) == sub;
        }

        ///////////////////////////////////////////////////////////////////////
        inline const char* getAnnotation
        (
            Class cls,
            const std::string& annotationName,
            const std::string& testCaseName
        )
        {
            NSString* selStr = [[NSString alloc] initWithFormat:@"Catch_%s_%s", annotationName.c_str(), testCaseName.c_str()];
            SEL sel = NSSelectorFromString( selStr );
            [selStr release];
            if( [cls respondsToSelector: sel] )
                return (const char*)[cls performSelector: sel];
            return "";
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    inline size_t registerTestMethods
    ()
    {
        size_t noTestMethods = 0;
        int noClasses = objc_getClassList( NULL, 0 );

        std::vector<Class> classes( noClasses );
        objc_getClassList( &classes[0], noClasses );

        for( int c = 0; c < noClasses; c++ )
        {
            Class cls = classes[c];
            {
                u_int count;
                Method* methods = class_copyMethodList( cls, &count );
                for( int m = 0; m < count ; m++ )
                {
                    SEL selector = method_getName(methods[m]);
                    std::string methodName = sel_getName(selector);
                    if( Detail::startsWith( methodName, "Catch_TestCase_" ) )
                    {
                        std::string testCaseName = methodName.substr( 15 );
                        const char* name = Detail::getAnnotation( cls, "Name", testCaseName );
                        const char* desc = Detail::getAnnotation( cls, "Description", testCaseName );

                        Hub::getTestCaseRegistry().registerTest( TestCaseInfo( new OcMethod( cls, selector ), name, desc, "", 0 ) );
                        noTestMethods++;

                    }
                }
                free(methods);
            }
        }
        return noTestMethods;
    }
}

///////////////////////////////////////////////////////////////////////////////
#define OC_TEST_CASE( name, desc )\
+(const char*) INTERNAL_CATCH_UNIQUE_NAME( Catch_Name_test ) \
{\
return name; \
}\
+(const char*) INTERNAL_CATCH_UNIQUE_NAME( Catch_Description_test ) \
{ \
return desc; \
} \
-(void) INTERNAL_CATCH_UNIQUE_NAME( Catch_TestCase_test )

#endif

#if defined( CATCH_CONFIG_MAIN ) || defined( CATCH_CONFIG_RUNNER )
// #included from: catch_runner.hpp

/*
 *  catch_runner.hpp
 *  Catch
 *
 *  Created by Phil on 31/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED

// #included from: internal/catch_hub_impl.hpp

/*
 *  catch_hub_impl.hpp
 *  Catch
 *
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
// #included from: catch_reporter_registry.hpp

/*
 *  catch_reporter_registry.hpp
 *  Catch
 *
 *  Created by Phil on 29/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_CATCH_REPORTER_REGISTRY_HPP_INCLUDED


#include <map>

namespace Catch
{
    class ReporterRegistry : public IReporterRegistry
    {
    public:

        ///////////////////////////////////////////////////////////////////////
        ~ReporterRegistry
        ()
        {
            deleteAllValues( m_factories );
        }

        ///////////////////////////////////////////////////////////////////////
        virtual IReporter* create
        (
            const std::string& name,
            const IReporterConfig& config
        )
        const
        {
            FactoryMap::const_iterator it =  m_factories.find( name );
            if( it == m_factories.end() )
                return NULL;
            return it->second->create( config );
        }

        ///////////////////////////////////////////////////////////////////////
        void registerReporter
        (
            const std::string& name,
            IReporterFactory* factory
        )
        {
            m_factories.insert( std::make_pair( name, factory ) );
        }

        ///////////////////////////////////////////////////////////////////////
        const FactoryMap& getFactories
        ()
        const
        {
            return m_factories;
        }

    private:
        FactoryMap m_factories;
    };
}

// #included from: catch_test_case_registry_impl.hpp

/*
 *  catch_test_case_registry_impl.hpp
 *  Catch
 *
 *  Created by Phil on 7/1/2011
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */


#include <vector>
#include <set>
#include <sstream>

#include <iostream> // !TBD DBG
namespace Catch
{
    class TestRegistry : public ITestCaseRegistry
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        TestRegistry
        ()
        : m_unnamedCount( 0 )
        {
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void registerTest
        (
            const TestCaseInfo& testInfo
        )
        {
            if( testInfo.getName() == "" )
            {
                std::ostringstream oss;
                oss << testInfo.getName() << "unnamed/" << ++m_unnamedCount;
                return registerTest( TestCaseInfo( testInfo, oss.str() ) );
            }
            if( m_functions.find( testInfo ) == m_functions.end() )
            {
                m_functions.insert( testInfo );
                m_functionsInOrder.push_back( testInfo );
            }
            else
            {
                const TestCaseInfo& prev = *m_functions.find( testInfo );
                std::cerr   << "error: TEST_CASE( \"" << testInfo.getName() << "\" ) already defined.\n"
                            << "\tFirst seen at " << SourceLineInfo( prev.getFilename(), prev.getLine() ) << "\n"
                            << "\tRedefined at " << SourceLineInfo( testInfo.getFilename(), testInfo.getLine() ) << std::endl;
                exit(1);
            }
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual const std::vector<TestCaseInfo>& getAllTests
        ()
        const
        {
            return m_functionsInOrder;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual std::vector<TestCaseInfo> getMatchingTestCases
        (
            const std::string& rawTestSpec
        )
        {
            TestSpec testSpec( rawTestSpec );

            std::vector<TestCaseInfo> testList;
            std::vector<TestCaseInfo>::const_iterator it = m_functionsInOrder.begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = m_functionsInOrder.end();
            for(; it != itEnd; ++it )
            {
                if( testSpec.matches( it->getName() ) )
                {
                    testList.push_back( *it );
                }
            }
            return testList;
        }

    private:

        std::set<TestCaseInfo> m_functions;
        std::vector<TestCaseInfo> m_functionsInOrder;
        size_t m_unnamedCount;
    };

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////


    struct FreeFunctionTestCase : ITestCase
    {
        ///////////////////////////////////////////////////////////////////////////
        FreeFunctionTestCase
        (
            TestFunction fun
        )
        : m_fun( fun )
        {}

        ///////////////////////////////////////////////////////////////////////////
        virtual void invoke
        ()
        const
        {
            m_fun();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual ITestCase* clone
        ()
        const
        {
            return new FreeFunctionTestCase( m_fun );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual bool operator ==
        (
            const ITestCase& other
        )
        const
        {
            const FreeFunctionTestCase* ffOther = dynamic_cast<const FreeFunctionTestCase*> ( &other );
            return ffOther && m_fun == ffOther->m_fun;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual bool operator <
        (
            const ITestCase& other
        )
        const
        {
            const FreeFunctionTestCase* ffOther = dynamic_cast<const FreeFunctionTestCase*> ( &other );
            return ffOther && m_fun < ffOther->m_fun;
        }

    private:
        TestFunction m_fun;
    };

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    AutoReg::AutoReg
    (
        TestFunction function,
        const char* name,
        const char* description,
        const char* filename,
        std::size_t line
    )
    {
        registerTestCase( new FreeFunctionTestCase( function ), name, description, filename, line );
    }

    ///////////////////////////////////////////////////////////////////////////
    AutoReg::~AutoReg
    ()
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    void AutoReg::registerTestCase
    (
        ITestCase* testCase,
        const char* name,
        const char* description,
        const char* filename,
        std::size_t line
    )
    {
        Hub::getTestCaseRegistry().registerTest( TestCaseInfo( testCase, name, description, filename, line ) );
    }

} // end namespace Catch

// #included from: catch_exception_translator_registry.hpp

/*
 *  catch_exception_translator_registry.hpp
 *  Catch
 *
 *  Created by Phil on 20/04/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_CATCH_EXCEPTION_TRANSLATOR_HPP_INCLUDED


namespace Catch
{
    class ExceptionTranslatorRegistry : public IExceptionTranslatorRegistry
    {
        ///////////////////////////////////////////////////////////////////////
        ~ExceptionTranslatorRegistry
        ()
        {
            deleteAll( m_translators );
        }

        ///////////////////////////////////////////////////////////////////////
        virtual void registerTranslator
        (
            IExceptionTranslator* translator
        )
        {
            m_translators.push_back( translator );
        }

        ///////////////////////////////////////////////////////////////////////
        virtual std::string translateActiveException
        ()
        const
        {
            try
            {
                throw;
            }
            catch( std::exception& ex )
            {
                return ex.what();
            }
            catch( std::string& msg )
            {
                return msg;
            }
            catch( const char* msg )
            {
                return msg;
            }
            catch(...)
            {
                return tryTranslators( m_translators.begin() );
            }
        }

        ///////////////////////////////////////////////////////////////////////
        std::string tryTranslators
        (
            std::vector<IExceptionTranslator*>::const_iterator it
        )
        const
        {
            if( it == m_translators.end() )
                return "Unknown exception";

            try
            {
                return (*it)->translate();
            }
            catch(...)
            {
                return tryTranslators( it+1 );
            }
        }

    private:
        std::vector<IExceptionTranslator*> m_translators;
    };
}

// #included from: catch_runner_impl.hpp

 /*
 *  catch_runner.hpp
 *  Catch
 *
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_INTERNAL_CATCH_RUNNER_HPP_INCLUDED

// #included from: catch_interfaces_runner.h

/*
 *  catch_interfaces_runner.h
 *  Catch
 *
 *  Created by Phil on 07/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_INTERNAL_CATCH_INTERFACES_RUNNER_H_INCLUDED

#include <string>

namespace Catch
{
    class TestCaseInfo;

    struct IRunner
    {
        virtual ~IRunner
        ()
        {}

        virtual void runAll
            (   bool runHiddenTests = false
            ) = 0;

        virtual std::size_t runMatching
            ( const std::string& rawTestSpec
            ) = 0;

        virtual std::size_t getSuccessCount
            () const = 0;

        virtual std:: size_t getFailureCount
            () const = 0;

    };
}

// #included from: catch_config.hpp

/*
 *  catch_config.hpp
 *  Catch
 *
 *  Created by Phil on 08/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_RUNNERCONFIG_HPP_INCLUDED


#include <memory>
#include <vector>
#include <string>
#include <iostream>

namespace Catch
{

    class Config : public IReporterConfig
    {
    private:
        Config( const Config& other );
        Config& operator = ( const Config& other );
    public:

        struct Include { enum What
        {
            FailedOnly,
            SuccessfulResults
        }; };

        struct List{ enum What
        {
            None = 0,

            Reports = 1,
            Tests = 2,
            All = 3,

            WhatMask = 0xf,

            AsText = 0x10,
            AsXml = 0x11,

            AsMask = 0xf0
        }; };


        ///////////////////////////////////////////////////////////////////////////
        Config()
        :   m_reporter( NULL ),
            m_listSpec( List::None ),
            m_shouldDebugBreak( false ),
            m_showHelp( false ),
            m_streambuf( NULL ),
            m_os( std::cout.rdbuf() ),
            m_includeWhat( Include::FailedOnly )
        {}

        ///////////////////////////////////////////////////////////////////////////
        ~Config()
        {
            m_os.rdbuf( std::cout.rdbuf() );
            delete m_streambuf;
        }

        ///////////////////////////////////////////////////////////////////////////
        void setReporter( const std::string& reporterName )
        {
            if( m_reporter.get() )
                return setError( "Only one reporter may be specified" );
            setReporter( Hub::getReporterRegistry().create( reporterName, *this ) );
        }

        ///////////////////////////////////////////////////////////////////////////
        void addTestSpec( const std::string& testSpec )
        {
            m_testSpecs.push_back( testSpec );
        }

        ///////////////////////////////////////////////////////////////////////////
        bool testsSpecified() const
        {
            return !m_testSpecs.empty();
        }

        ///////////////////////////////////////////////////////////////////////////
        const std::vector<std::string>& getTestSpecs() const
        {
            return m_testSpecs;
        }

        ///////////////////////////////////////////////////////////////////////////
        List::What getListSpec( void ) const
        {
            return m_listSpec;
        }

        ///////////////////////////////////////////////////////////////////////////
        void setListSpec( List::What listSpec )
        {
            m_listSpec = listSpec;
        }

        ///////////////////////////////////////////////////////////////////////////
        void setFilename( const std::string& filename )
        {
            m_filename = filename;
        }

        ///////////////////////////////////////////////////////////////////////////
        const std::string& getFilename() const
        {
            return m_filename;
        }

        ///////////////////////////////////////////////////////////////////////////
        const std::string& getMessage() const
        {
            return m_message;
        }

        ///////////////////////////////////////////////////////////////////////////
        void setError( const std::string& errorMessage )
        {
            m_message = errorMessage + "\n\n" + "Usage: ...";
        }

        ///////////////////////////////////////////////////////////////////////////
        void setReporter( IReporter* reporter )
        {
            m_reporter = std::auto_ptr<IReporter>( reporter );
        }

        ///////////////////////////////////////////////////////////////////////////
        IReporter* getReporter() const
        {
            if( !m_reporter.get() )
                const_cast<Config*>( this )->setReporter( Hub::getReporterRegistry().create( "basic", *this ) );
            return m_reporter.get();
        }

        ///////////////////////////////////////////////////////////////////////////
        List::What listWhat() const
        {
            return static_cast<List::What>( m_listSpec & List::WhatMask );
        }

        ///////////////////////////////////////////////////////////////////////////
        List::What listAs() const
        {
            return static_cast<List::What>( m_listSpec & List::AsMask );
        }

        ///////////////////////////////////////////////////////////////////////////
        void setIncludeWhat( Include::What includeWhat )
        {
            m_includeWhat = includeWhat;
        }

        ///////////////////////////////////////////////////////////////////////////
        void setShouldDebugBreak( bool shouldDebugBreakFlag )
        {
            m_shouldDebugBreak = shouldDebugBreakFlag;
        }

        ///////////////////////////////////////////////////////////////////////////
        void setName( const std::string& name )
        {
            m_name = name;
        }

        ///////////////////////////////////////////////////////////////////////////
        std::string getName() const
        {
            return m_name;
        }

        ///////////////////////////////////////////////////////////////////////////
        bool shouldDebugBreak() const
        {
            return m_shouldDebugBreak;
        }

        ///////////////////////////////////////////////////////////////////////////
        void setShowHelp( bool showHelpFlag )
        {
            m_showHelp = showHelpFlag;
        }

        ///////////////////////////////////////////////////////////////////////////
        bool showHelp() const
        {
            return m_showHelp;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual std::ostream& stream() const
        {
            return m_os;
        }

        ///////////////////////////////////////////////////////////////////////////
        void setStreamBuf( std::streambuf* buf )
        {
            m_os.rdbuf( buf ? buf : std::cout.rdbuf() );
        }

        ///////////////////////////////////////////////////////////////////////////
        void useStream( const std::string& streamName )
        {
            std::streambuf* newBuf = Hub::createStreamBuf( streamName );
            setStreamBuf( newBuf );
            delete m_streambuf;
            m_streambuf = newBuf;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual bool includeSuccessfulResults() const
        {
            return m_includeWhat == Include::SuccessfulResults;
        }

    private:
        std::auto_ptr<IReporter> m_reporter;
        std::string m_filename;
        std::string m_message;
        List::What m_listSpec;
        std::vector<std::string> m_testSpecs;
        bool m_shouldDebugBreak;
        bool m_showHelp;
        std::streambuf* m_streambuf;
        mutable std::ostream m_os;
        Include::What m_includeWhat;
        std::string m_name;

    };

} // end namespace Catch


#include <set>
#include <string>

namespace Catch
{

    class StreamRedirect
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        StreamRedirect
        (
            std::ostream& stream,
            std::string& targetString
        )
        :   m_stream( stream ),
            m_prevBuf( stream.rdbuf() ),
            m_targetString( targetString )
        {
            stream.rdbuf( m_oss.rdbuf() );
        }

        ///////////////////////////////////////////////////////////////////////
        ~StreamRedirect
        ()
        {
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
    ///////////////////////////////////////////////////////////////////////////
    class SectionInfo
    {
    public:
        enum Status
        {
            Root,
            Unknown,
            NonLeaf,
            TestedLeaf
        };

        ///////////////////////////////////////////////////////////////////////
        SectionInfo
        (
            SectionInfo* parent
        )
        :   m_status( Unknown ),
            m_parent( parent )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        SectionInfo
        ()
        :   m_status( Root ),
            m_parent( NULL )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        ~SectionInfo
        ()
        {
            deleteAllValues( m_subSections );
        }

        ///////////////////////////////////////////////////////////////////////
        bool shouldRun
        ()
        const
        {
            return m_status != TestedLeaf;
        }

        ///////////////////////////////////////////////////////////////////////
        bool ran
        ()
        {
            if( m_status != NonLeaf )
            {
                m_status = TestedLeaf;
                return true;
            }
            return false;
        }

        ///////////////////////////////////////////////////////////////////////
        SectionInfo* findSubSection
        (
            const std::string& name
        )
        {
            std::map<std::string, SectionInfo*>::const_iterator it = m_subSections.find( name );
            return it != m_subSections.end()
                ? it->second
                : NULL;
        }

        ///////////////////////////////////////////////////////////////////////
        SectionInfo* addSubSection
        (
            const std::string& name
        )
        {
            SectionInfo* subSection = new SectionInfo( this );
            m_subSections.insert( std::make_pair( name, subSection ) );
            m_status = NonLeaf;
            return subSection;
        }

        ///////////////////////////////////////////////////////////////////////
        SectionInfo* getParent
        ()
        {
            return m_parent;
        }

        ///////////////////////////////////////////////////////////////////////
        bool hasUntestedSections
        ()
        const
        {
            if( m_status == Unknown )
                return true;

            std::map<std::string, SectionInfo*>::const_iterator it = m_subSections.begin();
            std::map<std::string, SectionInfo*>::const_iterator itEnd = m_subSections.end();
            for(; it != itEnd; ++it )
            {
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

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    class RunningTest
    {
        enum RunStatus
        {
            NothingRun,
            EncounteredASection,
            RanAtLeastOneSection,
            RanToCompletionWithSections,
            RanToCompletionWithNoSections
        };

    public:
        ///////////////////////////////////////////////////////////////////////
        explicit RunningTest
        (
            const TestCaseInfo* info = NULL
        )
        :   m_info( info ),
            m_runStatus( RanAtLeastOneSection ),
            m_currentSection( &m_rootSection ),
            m_changed( false )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        bool wasSectionSeen
        ()
        const
        {
            return  m_runStatus == RanAtLeastOneSection ||
                    m_runStatus == RanToCompletionWithSections;
        }

        ///////////////////////////////////////////////////////////////////////
        void reset
        ()
        {
            m_runStatus = NothingRun;
            m_changed = false;
        }

        ///////////////////////////////////////////////////////////////////////
        void ranToCompletion
        ()
        {
            m_runStatus =   m_runStatus == RanAtLeastOneSection ||
                            m_runStatus == EncounteredASection
                ? RanToCompletionWithSections
                : RanToCompletionWithNoSections;
        }

        ///////////////////////////////////////////////////////////////////////
        bool addSection
        (
            const std::string& name
        )
        {
            if( m_runStatus == NothingRun )
                m_runStatus = EncounteredASection;

            SectionInfo* thisSection = m_currentSection->findSubSection( name );
            if( !thisSection )
            {
                thisSection = m_currentSection->addSubSection( name );
                m_changed = true;
            }

            if( !wasSectionSeen() && thisSection->shouldRun() )
            {
                m_currentSection = thisSection;
                return true;
            }
            return false;
        }

        ///////////////////////////////////////////////////////////////////////
        void endSection
        (
            const std::string&
        )
        {
            if( m_currentSection->ran() )
            {
                m_runStatus = RanAtLeastOneSection;
                m_changed = true;
            }
            m_currentSection = m_currentSection->getParent();
        }

        ///////////////////////////////////////////////////////////////////////
        const TestCaseInfo& getTestCaseInfo
        ()
        const
        {
            return *m_info;
        }

        ///////////////////////////////////////////////////////////////////////
        bool hasUntestedSections
        ()
        const
        {
            return  m_runStatus == RanAtLeastOneSection ||
                    ( m_rootSection.hasUntestedSections() && m_changed );
        }

    private:
        const TestCaseInfo* m_info;
        RunStatus m_runStatus;
        SectionInfo m_rootSection;
        SectionInfo* m_currentSection;
        bool m_changed;
    };

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    class Runner : public IResultCapture, public IRunner
    {
        Runner( const Runner& );
        void operator =( const Runner& );

    public:

        ///////////////////////////////////////////////////////////////////////////
        explicit Runner
        (
            const Config& config
        )
        :   m_runningTest( NULL ),
            m_config( config ),
            m_successes( 0 ),
            m_failures( 0 ),
            m_reporter( m_config.getReporter() ),
            m_prevRunner( &Hub::getRunner() ),
            m_prevResultCapture( &Hub::getResultCapture() )
        {
            Hub::setRunner( this );
            Hub::setResultCapture( this );
            m_reporter->StartTesting();
        }

        ///////////////////////////////////////////////////////////////////////////
        ~Runner
        ()
        {
            m_reporter->EndTesting( m_successes, m_failures );
            Hub::setRunner( m_prevRunner );
            Hub::setResultCapture( m_prevResultCapture );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void runAll
        (
            bool runHiddenTests = false
        )
        {
            std::vector<TestCaseInfo> allTests = Hub::getTestCaseRegistry().getAllTests();
            for( std::size_t i=0; i < allTests.size(); ++i )
            {
                if( runHiddenTests || !allTests[i].isHidden() )
                   runTest( allTests[i] );
            }
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual std::size_t runMatching
        (
            const std::string& rawTestSpec
        )
        {
            TestSpec testSpec( rawTestSpec );

            std::vector<TestCaseInfo> allTests = Hub::getTestCaseRegistry().getAllTests();
            std::size_t testsRun = 0;
            for( std::size_t i=0; i < allTests.size(); ++i )
            {
                if( testSpec.matches( allTests[i].getName() ) )
                {
                    runTest( allTests[i] );
                    testsRun++;
                }
            }
            return testsRun;
        }

        ///////////////////////////////////////////////////////////////////////////
        void runTest
        (
            const TestCaseInfo& testInfo
        )
        {
            std::size_t prevSuccessCount = m_successes;
            std::size_t prevFailureCount = m_failures;

            std::string redirectedCout;
            std::string redirectedCerr;

            m_reporter->StartTestCase( testInfo );

            m_runningTest = new RunningTest( &testInfo );

            do
            {
                do
                {
                    m_currentResult.setFileAndLine( m_runningTest->getTestCaseInfo().getFilename(),
                                                    m_runningTest->getTestCaseInfo().getLine() );
                    runCurrentTest( redirectedCout, redirectedCerr );
                }
                while( m_runningTest->hasUntestedSections() );
            }
            while( Hub::advanceGeneratorsForCurrentTest() );

            delete m_runningTest;
            m_runningTest = NULL;

            m_reporter->EndTestCase( testInfo, m_successes - prevSuccessCount, m_failures - prevFailureCount, redirectedCout, redirectedCerr );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual std::size_t getSuccessCount
        ()
        const
        {
            return m_successes;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual std:: size_t getFailureCount
        ()
        const
        {
            return m_failures;
        }

    private: // IResultCapture

        ///////////////////////////////////////////////////////////////////////////
        virtual ResultAction::Value acceptResult
        (
            bool result
        )
        {
            return acceptResult( result ? ResultWas::Ok : ResultWas::ExpressionFailed );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual ResultAction::Value acceptResult
        (
            ResultWas::OfType result
        )
        {
            m_currentResult.setResultType( result );
            return actOnCurrentResult();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual ResultAction::Value acceptExpression
        (
            const MutableResultInfo& resultInfo
        )
        {
            m_currentResult = resultInfo;
            return actOnCurrentResult();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void acceptMessage
        (
            const std::string& msg
        )
        {
            m_currentResult.setMessage( msg );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void testEnded
        (
            const ResultInfo& result
        )
        {
            if( result.getResultType() == ResultWas::Ok )
            {
                m_successes++;
            }
            else if( !result.ok() )
            {
                m_failures++;

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

        ///////////////////////////////////////////////////////////////////////////
        virtual bool sectionStarted
        (
            const std::string& name,
            const std::string& description,
            const std::string& filename,
            std::size_t line,
            std::size_t& successes,
            std::size_t& failures
        )
        {
            std::ostringstream oss;
            oss << name << "@" << SourceLineInfo( filename, line );

            if( !m_runningTest->addSection( oss.str() ) )
                return false;

            m_currentResult.setFileAndLine( filename, line );
            m_reporter->StartSection( name, description );
            successes = m_successes;
            failures = m_failures;

            return true;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void sectionEnded
        (
            const std::string& name,
            std::size_t prevSuccesses,
            std::size_t prevFailures
        )
        {
            m_runningTest->endSection( name );
            m_reporter->EndSection( name, m_successes - prevSuccesses, m_failures - prevFailures );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void pushScopedInfo
        (
            ScopedInfo* scopedInfo
        )
        {
            m_scopedInfos.push_back( scopedInfo );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void popScopedInfo
        (
            ScopedInfo* scopedInfo
        )
        {
            if( m_scopedInfos.back() == scopedInfo )
                m_scopedInfos.pop_back();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual bool shouldDebugBreak
        ()
        const
        {
            return m_config.shouldDebugBreak();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual std::string getCurrentTestName
        ()
        const
        {
            return m_runningTest
                ? m_runningTest->getTestCaseInfo().getName()
                : "";
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual const ResultInfo* getLastResult
        ()
        const
        {
            return &m_lastResult;
        }

    private:

        ///////////////////////////////////////////////////////////////////////////
        ResultAction::Value actOnCurrentResult
        ()
        {
            testEnded( m_currentResult );
            m_lastResult = m_currentResult;

            m_currentResult = MutableResultInfo();
            if( m_lastResult.ok() )
                return ResultAction::None;
            else if( shouldDebugBreak() )
                return ResultAction::DebugFailed;
            else
                return ResultAction::Failed;
        }

        ///////////////////////////////////////////////////////////////////////////
        void runCurrentTest
        (
            std::string& redirectedCout,
            std::string& redirectedCerr
        )
        {
            try
            {
                m_runningTest->reset();
                if( m_reporter->shouldRedirectStdout() )
                {
                    StreamRedirect coutRedir( std::cout, redirectedCout );
                    StreamRedirect cerrRedir( std::cerr, redirectedCerr );
                    m_runningTest->getTestCaseInfo().invoke();
                }
                else
                {
                    m_runningTest->getTestCaseInfo().invoke();
                }
                m_runningTest->ranToCompletion();
            }
            catch( TestFailureException& )
            {
                // This just means the test was aborted due to failure
            }
            catch(...)
            {
                acceptMessage( Catch::Hub::getExceptionTranslatorRegistry().translateActiveException() );
                acceptResult( ResultWas::ThrewException );
            }
            m_info.clear();
        }

    private:
        RunningTest* m_runningTest;
        MutableResultInfo m_currentResult;
        ResultInfo m_lastResult;

        const Config& m_config;
        std::size_t m_successes;
        std::size_t m_failures;
        IReporter* m_reporter;
        std::vector<ScopedInfo*> m_scopedInfos;
        std::vector<ResultInfo> m_info;
        IRunner* m_prevRunner;
        IResultCapture* m_prevResultCapture;
    };
}

// #included from: catch_generators_impl.hpp

/*
 *  catch_generators_impl.hpp
 *  Catch
 *
 *  Created by Phil on 28/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_GENERATORS_IMPL_HPP_INCLUDED


#include <vector>
#include <string>
#include <map>

namespace Catch
{
    struct GeneratorInfo
    {
        ///////////////////////////////////////////////////////////////////////
        GeneratorInfo
        (
            std::size_t size
        )
        :   m_size( size ),
            m_currentIndex( 0 )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        bool moveNext
        ()
        {
            if( ++m_currentIndex == m_size )
            {
                m_currentIndex = 0;
                return false;
            }
            return true;
        }

        ///////////////////////////////////////////////////////////////////////
        std::size_t getCurrentIndex
        ()
        const
        {
            return m_currentIndex;
        }

        std::size_t m_size;
        std::size_t m_currentIndex;
    };

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    class GeneratorsForTest
    {

    public:
        ///////////////////////////////////////////////////////////////////////
        ~GeneratorsForTest
        ()
        {
            deleteAll( m_generatorsInOrder );
        }

        ///////////////////////////////////////////////////////////////////////
        GeneratorInfo& getGeneratorInfo
        (
            const std::string& fileInfo,
            std::size_t size
        )
        {
            std::map<std::string, GeneratorInfo*>::const_iterator it = m_generatorsByName.find( fileInfo );
            if( it == m_generatorsByName.end() )
            {
                GeneratorInfo* info = new GeneratorInfo( size );
                m_generatorsByName.insert( std::make_pair( fileInfo, info ) );
                m_generatorsInOrder.push_back( info );
                return *info;
            }
            return *it->second;
        }

        ///////////////////////////////////////////////////////////////////////
        bool moveNext
        ()
        {
            std::vector<GeneratorInfo*>::const_iterator it = m_generatorsInOrder.begin();
            std::vector<GeneratorInfo*>::const_iterator itEnd = m_generatorsInOrder.end();
            for(; it != itEnd; ++it )
            {
                if( (*it)->moveNext() )
                    return true;
            }
            return false;
        }

    private:
        std::map<std::string, GeneratorInfo*> m_generatorsByName;
        std::vector<GeneratorInfo*> m_generatorsInOrder;
    };

} // end namespace Catch

#define INTERNAL_CATCH_LINESTR2( line ) #line
#define INTERNAL_CATCH_LINESTR( line ) INTERNAL_CATCH_LINESTR2( line )

#define INTERNAL_CATCH_GENERATE( expr ) expr.setFileInfo( __FILE__ "(" INTERNAL_CATCH_LINESTR( __LINE__ ) ")" )

// #included from: catch_stream.hpp

/*
 *  catch_stream.hpp
 *  Catch
 *
 *  Created by Phil on 17/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_STREAM_HPP_INCLUDED

#include <stdexcept>
#include <cstdio>

namespace Catch
{
    template<typename WriterF, size_t bufferSize=256>
    class StreamBufImpl : public StreamBufBase
    {
        char data[bufferSize];
        WriterF m_writer;

    public:
        ///////////////////////////////////////////////////////////////////////
        StreamBufImpl
        ()
        {
            setp( data, data + sizeof(data) );
        }

        ///////////////////////////////////////////////////////////////////////
        ~StreamBufImpl
        ()
        {
            sync();
        }

    private:
        ///////////////////////////////////////////////////////////////////////
        int	overflow
        (
            int c
        )
        {
            sync();

            if( c != EOF )
            {
                if( pbase() == epptr() )
                    m_writer( std::string( 1, static_cast<char>( c ) ) );
                else
                    sputc( static_cast<char>( c ) );
            }
            return 0;
        }

        ///////////////////////////////////////////////////////////////////////
        int	sync
        ()
        {
            if( pbase() != pptr() )
            {
                m_writer( std::string( pbase(), pptr() - pbase() ) );
                setp( pbase(), epptr() );
            }
            return 0;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    struct OutputDebugWriter
    {
        ///////////////////////////////////////////////////////////////////////
        void operator()
        (
            const std::string &str
        )
        {
            writeToDebugConsole( str );
        }
    };
}


namespace Catch
{
    ///////////////////////////////////////////////////////////////////////////
    Hub::Hub
    ()
    :   m_reporterRegistry( new ReporterRegistry ),
        m_testCaseRegistry( new TestRegistry ),
        m_exceptionTranslatorRegistry( new ExceptionTranslatorRegistry )
    {
    }

    ///////////////////////////////////////////////////////////////////////////
    Hub& Hub::me
    ()
    {
        Hub*& hub = singleInstance();
        if( !hub )
            hub = new Hub();
        return *hub;
    }

    ///////////////////////////////////////////////////////////////////////////
    void Hub::cleanUp
    ()
    {
        Hub*& hub = singleInstance();
        delete hub;
        hub = NULL;
    }

    ///////////////////////////////////////////////////////////////////////////
    Hub*& Hub::singleInstance()
    {
        static Hub* hub = NULL;
        return hub;

    }

    ///////////////////////////////////////////////////////////////////////////
    void Hub::setRunner( IRunner* runner )
    {
        me().m_runner = runner;
    }
    ///////////////////////////////////////////////////////////////////////////
    void Hub::setResultCapture( IResultCapture* resultCapture )
    {
        me().m_resultCapture = resultCapture;
    }

    ///////////////////////////////////////////////////////////////////////////
    IResultCapture& Hub::getResultCapture
    ()
    {
        return *me().m_resultCapture;
    }

    ///////////////////////////////////////////////////////////////////////////
    IRunner& Hub::getRunner
    ()
    {
        return *me().m_runner;
    }

    ///////////////////////////////////////////////////////////////////////////
    IReporterRegistry& Hub::getReporterRegistry
    ()
    {
        return *me().m_reporterRegistry.get();
    }

    ///////////////////////////////////////////////////////////////////////////
    ITestCaseRegistry& Hub::getTestCaseRegistry
    ()
    {
        return *me().m_testCaseRegistry.get();
    }

    ///////////////////////////////////////////////////////////////////////////
    IExceptionTranslatorRegistry& Hub::getExceptionTranslatorRegistry
    ()
    {
        return *me().m_exceptionTranslatorRegistry.get();
    }

    ///////////////////////////////////////////////////////////////////////////
    std::streambuf* Hub::createStreamBuf
    (
        const std::string& streamName
    )
    {
        if( streamName == "stdout" ) return std::cout.rdbuf();
        if( streamName == "stderr" ) return std::cerr.rdbuf();
        if( streamName == "debug" ) return new StreamBufImpl<OutputDebugWriter>;

        throw std::domain_error( "Unknown stream: " + streamName );
    }

    ///////////////////////////////////////////////////////////////////////////
    GeneratorsForTest* Hub::findGeneratorsForCurrentTest
    ()
    {
        std::string testName = getResultCapture().getCurrentTestName();

        std::map<std::string, GeneratorsForTest*>::const_iterator it =
            m_generatorsByTestName.find( testName );
        return it != m_generatorsByTestName.end()
            ? it->second
            : NULL;
    }
    ///////////////////////////////////////////////////////////////////////////
    GeneratorsForTest& Hub::getGeneratorsForCurrentTest
    ()
    {
        GeneratorsForTest* generators = findGeneratorsForCurrentTest();
        if( !generators )
        {
            std::string testName = getResultCapture().getCurrentTestName();
            generators = new GeneratorsForTest();
            m_generatorsByTestName.insert( std::make_pair( testName, generators ) );
        }
        return *generators;
    }

    ///////////////////////////////////////////////////////////////////////////
    size_t Hub::getGeneratorIndex
    (
        const std::string& fileInfo,
        size_t totalSize
    )
    {
        return me().getGeneratorsForCurrentTest()
            .getGeneratorInfo( fileInfo, totalSize )
            .getCurrentIndex();
    }

    ///////////////////////////////////////////////////////////////////////////
    bool Hub::advanceGeneratorsForCurrentTest
    ()
    {
        GeneratorsForTest* generators = me().findGeneratorsForCurrentTest();
        return generators && generators->moveNext();
    }
}

// #included from: internal/catch_commandline.hpp

/*
 *  catch_commandline.hpp
 *  Catch
 *
 *  Created by Phil on 02/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED


namespace Catch
{
    // !TBD: This could be refactored to be more "declarative"
    //       have a table up front that relates the mode, option strings, # arguments, names of arguments
    //       - may not be worth it at this scale

    // -l, --list tests [xml] lists available tests (optionally in xml)
    // -l, --list reporters [xml] lists available reports (optionally in xml)
    // -l, --list all [xml] lists available tests and reports (optionally in xml)
    // -t, --test "testspec" ["testspec", ...]
    // -r, --reporter <type>
    // -o, --out filename to write to
    // -s, --success report successful cases too
    // -b, --break breaks into debugger on test failure
    // -n, --name specifies an optional name for the test run
	class ArgParser : NonCopyable
    {
        enum Mode
        {
            modeNone,
            modeList,
            modeTest,
            modeReport,
            modeOutput,
            modeSuccess,
            modeBreak,
            modeName,
            modeHelp,

            modeError
        };

    public:
        ///////////////////////////////////////////////////////////////////////
        ArgParser
        (
            int argc,
            char * const argv[],
            Config& config
        )
        :   m_mode( modeNone ),
            m_config( config )
        {
            for( int i=1; i < argc; ++i )
            {
                if( argv[i][0] == '-' )
                {
                    std::string cmd = ( argv[i] );
                    if( cmd == "-l" || cmd == "--list" )
                        changeMode( cmd, modeList );
                    else if( cmd == "-t" || cmd == "--test" )
                        changeMode( cmd, modeTest );
                    else if( cmd == "-r" || cmd == "--reporter" )
                        changeMode( cmd, modeReport );
                    else if( cmd == "-o" || cmd == "--out" )
                        changeMode( cmd, modeOutput );
                    else if( cmd == "-s" || cmd == "--success" )
                        changeMode( cmd, modeSuccess );
                    else if( cmd == "-b" || cmd == "--break" )
                        changeMode( cmd, modeBreak );
                    else if( cmd == "-n" || cmd == "--name" )
                        changeMode( cmd, modeName );
                    else if( cmd == "-h" || cmd == "-?" || cmd == "--help" )
                        changeMode( cmd, modeHelp );
                }
                else
                {
                    m_args.push_back( argv[i] );
                }
                if( m_mode == modeError )
                    return;
            }
            changeMode( "", modeNone );
        }

    private:
        ///////////////////////////////////////////////////////////////////////
        std::string argsAsString
        ()
        {
            std::ostringstream oss;
            std::vector<std::string>::const_iterator it = m_args.begin();
            std::vector<std::string>::const_iterator itEnd = m_args.end();
            for( bool first = true; it != itEnd; ++it, first = false )
            {
                if( !first )
                    oss << " ";
                oss << *it;
            }
            return oss.str();
        }

        ///////////////////////////////////////////////////////////////////////
        void changeMode
        (
            const std::string& cmd,
            Mode mode
        )
        {
            m_command = cmd;
            switch( m_mode )
            {
                case modeNone:
                    if( m_args.size() > 0 )
                        return setErrorMode( "Unexpected arguments before " + m_command + ": " + argsAsString() );
                    break;
                case modeList:
                    if( m_args.size() > 2 )
                    {
                        return setErrorMode( m_command + " expected upto 2 arguments but recieved: " + argsAsString() );
                    }
                    else
                    {
                        Config::List::What listSpec = Config::List::All;
                        if( m_args.size() >= 1 )
                        {
                            if( m_args[0] == "tests" )
                                listSpec = Config::List::Tests;
                            else if( m_args[0] == "reporters" )
                                listSpec = Config::List::Reports;
                            else
                                return setErrorMode( m_command + " expected [tests] or [reporters] but recieved: [" + m_args[0] + "]" );
                        }
                        if( m_args.size() >= 2 )
                        {
                            if( m_args[1] == "xml" )
                                listSpec = static_cast<Config::List::What>( listSpec | Config::List::AsXml );
                            else if( m_args[1] == "text" )
                                listSpec = static_cast<Config::List::What>( listSpec | Config::List::AsText );
                            else
                                return setErrorMode( m_command + " expected [xml] or [text] but recieved: [" + m_args[1] + "]" );
                        }
                        m_config.setListSpec( static_cast<Config::List::What>( m_config.getListSpec() | listSpec ) );
                    }
                    break;
                case modeTest:
                    if( m_args.size() == 0 )
                        return setErrorMode( m_command + " expected at least 1 argument but recieved none" );
                    {
                        std::vector<std::string>::const_iterator it = m_args.begin();
                        std::vector<std::string>::const_iterator itEnd = m_args.end();
                        for(; it != itEnd; ++it )
                            m_config.addTestSpec( *it );
                    }
                    break;
                case modeReport:
                    if( m_args.size() != 1 )
                        return setErrorMode( m_command + " expected one argument, recieved: " +  argsAsString() );
                    m_config.setReporter( m_args[0] );
                    break;
                case modeOutput:
                    if( m_args.size() == 0 )
                        return setErrorMode( m_command + " expected filename" );
                    if( m_args[0][0] == '%' )
                        m_config.useStream( m_args[0].substr( 1 ) );
                    else
                        m_config.setFilename( m_args[0] );
                    break;
                case modeSuccess:
                    if( m_args.size() != 0 )
                        return setErrorMode( m_command + " does not accept arguments" );
                    m_config.setIncludeWhat( Config::Include::SuccessfulResults );
                    break;
                case modeBreak:
                    if( m_args.size() != 0 )
                        return setErrorMode( m_command + " does not accept arguments" );
                    m_config.setShouldDebugBreak( true );
                    break;
                case modeName:
                    if( m_args.size() != 1 )
                        return setErrorMode( m_command + " requires exactly one argument (a name)" );
                    m_config.setName( m_args[0] );
                    break;
                case modeHelp:
                    if( m_args.size() != 0 )
                        return setErrorMode( m_command + " does not accept arguments" );
                    m_config.setShowHelp( true );
                    break;
                case modeError:
                default:
                break;
            }
            m_args.clear();
            m_mode = mode;
        }

        ///////////////////////////////////////////////////////////////////////
        void setErrorMode
        (
            const std::string& errorMessage
        )
        {
            m_mode = modeError;
            m_command = "";
            m_config.setError( errorMessage );
        }

    private:

        Mode m_mode;
        std::string m_command;
        std::vector<std::string> m_args;
        Config& m_config;
    };


} // end namespace Catch

// #included from: internal/catch_list.hpp

/*
 *  catch_list.hpp
 *  Catch
 *
 *  Created by Phil on 5/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED

#include <limits>

namespace Catch
{
    ///////////////////////////////////////////////////////////////////////////
    inline int List
    (
        const Config& config
    )
    {
        if( config.listWhat() & Config::List::Reports )
        {
            std::cout << "Available reports:\n";
            IReporterRegistry::FactoryMap::const_iterator it = Hub::getReporterRegistry().getFactories().begin();
            IReporterRegistry::FactoryMap::const_iterator itEnd = Hub::getReporterRegistry().getFactories().end();
            for(; it != itEnd; ++it )
            {
                // !TBD: consider listAs()
                std::cout << "\t" << it->first << "\n\t\t'" << it->second->getDescription() << "'\n";
            }
            std::cout << std::endl;
        }
        if( config.listWhat() & Config::List::Tests )
        {
            std::cout << "Available tests:\n";
            std::vector<TestCaseInfo>::const_iterator it = Hub::getTestCaseRegistry().getAllTests().begin();
            std::vector<TestCaseInfo>::const_iterator itEnd = Hub::getTestCaseRegistry().getAllTests().end();
            for(; it != itEnd; ++it )
            {
                // !TBD: consider listAs()
                std::cout << "\t" << it->getName() << "\n\t\t '" << it->getDescription() << "'\n";
            }
            std::cout << std::endl;
        }
        if( ( config.listWhat() & Config::List::All ) == 0 )
        {
            std::cerr << "Unknown list type" << std::endl;
            return (std::numeric_limits<int>::max)();
        }

        if( config.getReporter() )
        {
            std::cerr << "Reporters ignored when listing" << std::endl;
        }
        if( !config.testsSpecified() )
        {
            std::cerr << "Test specs ignored when listing" << std::endl;
        }
        return 0;

    }

} // end namespace Catch

// #included from: reporters/catch_reporter_basic.hpp

/*
 *  catch_reporter_basic.hpp
 *  Catch
 *
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_CATCH_REPORTER_BASIC_HPP_INCLUDED

// #included from: ../internal/catch_reporter_registrars.hpp

/*
 *  catch_reporter_registrars.hpp
 *  Test
 *
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_CATCH_REPORTER_REGISTRARS_HPP_INCLUDED


namespace Catch
{
    template<typename T>
    class ReporterRegistrar
    {
        class ReporterFactory : public IReporterFactory
        {
            ///////////////////////////////////////////////////////////////////
            virtual IReporter* create
            (
                const IReporterConfig& config
            )
            const
            {
                return new T( config );
            }
            ///////////////////////////////////////////////////////////////////
            virtual std::string getDescription
            ()
            const
            {
                return T::getDescription();
            }
        };

    public:

        ///////////////////////////////////////////////////////////////////////
        ReporterRegistrar
        (
            const std::string& name
        )
        {
            Hub::getReporterRegistry().registerReporter( name, new ReporterFactory() );
        }
    };
}

///////////////////////////////////////////////////////////////////////////////
#define INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType ) \
    Catch::ReporterRegistrar<reporterType> catch_internal_RegistrarFor##reporterType( name );


namespace Catch
{
    class BasicReporter : public IReporter
    {
        struct SpanInfo
        {
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
        ///////////////////////////////////////////////////////////////////////////
        BasicReporter
        (
            const IReporterConfig& config
        )
        :   m_config( config ),
            m_firstSectionInTestCase( true )
        {
        }

        ///////////////////////////////////////////////////////////////////////////
        static std::string getDescription
        ()
        {
            return "Reports test results as lines of text";
        }

    private:

        ///////////////////////////////////////////////////////////////////////////
        void ReportCounts
        (
            std::size_t succeeded,
            std::size_t failed
        )
        {
            if( failed + succeeded == 0 )
                m_config.stream() << "No tests ran";
            else if( failed == 0 )
            {
                if( succeeded == 1 )
                    m_config.stream() << "1 test succeeded";
                else
                    m_config.stream() << "All " << succeeded << " tests succeeded";
            }
            else if( succeeded == 0 )
            {
                if( failed == 1 )
                    m_config.stream() << "1 test failed";
                else
                    m_config.stream() << "All " << failed << " tests failed";
            }
            else
            {
                m_config.stream() << succeeded << " test";
                if( succeeded > 1 )
                    m_config.stream() << "s";

                m_config.stream() << " passed but " << failed << " test";
                if( failed > 1 )
                    m_config.stream() << "s";
                m_config.stream() << " failed";
            }
        }

    private: // IReporter

        ///////////////////////////////////////////////////////////////////////////
        virtual bool shouldRedirectStdout
        ()
        const
        {
            return false;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTesting
        ()
        {
            m_testingSpan = SpanInfo();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTesting
        (
            std::size_t succeeded,
            std::size_t failed
        )
        {
            // Output the overall test results even if "Started Testing" was not emitted
            m_config.stream() << "\n[Testing completed. ";
            ReportCounts( succeeded, failed );
            m_config.stream() << "]\n" << std::endl;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void StartGroup
        (
            const std::string& groupName
        )
        {
            m_groupSpan = groupName;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndGroup
        (
            const std::string& groupName,
            std::size_t succeeded,
            std::size_t failed
        )
        {
            if( m_groupSpan.emitted && !groupName.empty() )
            {
                m_config.stream() << "[End of group: '" << groupName << "'. ";
                ReportCounts( succeeded, failed );
                m_config.stream() << "]\n" << std::endl;
                m_groupSpan = SpanInfo();
            }
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTestCase
        (
            const TestCaseInfo& testInfo
        )
        {
            m_testSpan = testInfo.getName();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void StartSection
        (
            const std::string& sectionName,
            const std::string /*description*/
        )
        {
            m_sectionSpans.push_back( SpanInfo( sectionName ) );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndSection
        (
            const std::string& sectionName,
            std::size_t succeeded,
            std::size_t failed
        )
        {
            SpanInfo& sectionSpan = m_sectionSpans.back();
            if( sectionSpan.emitted && !sectionSpan.name.empty() )
            {
                m_config.stream() << "[End of section: '" << sectionName << "'. ";
                ReportCounts( succeeded, failed );
                m_config.stream() << "]\n" << std::endl;
            }
            m_sectionSpans.pop_back();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void Result
        (
            const ResultInfo& resultInfo
        )
        {
            if( !m_config.includeSuccessfulResults() && resultInfo.getResultType() == ResultWas::Ok )
                return;

            StartSpansLazily();

            if( !resultInfo.getFilename().empty() )
                m_config.stream() << SourceLineInfo( resultInfo.getFilename(), resultInfo.getLine() );

            if( resultInfo.hasExpression() )
            {
                m_config.stream() << resultInfo.getExpression();
                if( resultInfo.ok() )
                    m_config.stream() << " succeeded";
                else
                    m_config.stream() << " failed";
            }
            switch( resultInfo.getResultType() )
            {
                case ResultWas::ThrewException:
                    if( resultInfo.hasExpression() )
                        m_config.stream() << " with unexpected";
                    else
                        m_config.stream() << "Unexpected";
                    m_config.stream() << " exception with message: '" << resultInfo.getMessage() << "'";
                    break;
                case ResultWas::DidntThrowException:
                    if( resultInfo.hasExpression() )
                        m_config.stream() << " because no exception was thrown where one was expected";
                    else
                        m_config.stream() << "No exception thrown where one was expected";
                    break;
                case ResultWas::Info:
                    streamVariableLengthText( "info", resultInfo.getMessage() );
                    break;
                case ResultWas::Warning:
                    m_config.stream() << "warning:\n'" << resultInfo.getMessage() << "'";
                    break;
                case ResultWas::ExplicitFailure:
                    m_config.stream() << "failed with message: '" << resultInfo.getMessage() << "'";
                    break;
                case ResultWas::Unknown: // These cases are here to prevent compiler warnings
                case ResultWas::Ok:
                case ResultWas::FailureBit:
                case ResultWas::ExpressionFailed:
                case ResultWas::Exception:
                default:
                    if( !resultInfo.hasExpression() )
                    {
                        if( resultInfo.ok() )
                            m_config.stream() << " succeeded";
                        else
                            m_config.stream() << " failed";
                    }
                    break;
            }

            if( resultInfo.hasExpression() )
            {
                m_config.stream() << " for: " << resultInfo.getExpandedExpression();
            }
            m_config.stream() << std::endl;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTestCase
        (
            const TestCaseInfo& testInfo,
            std::size_t succeeded,
            std::size_t failed,
            const std::string& stdOut,
            const std::string& stdErr
        )
        {
            if( !stdOut.empty() )
            {
                StartSpansLazily();
                streamVariableLengthText( "stdout", stdOut );
            }

            if( !stdErr.empty() )
            {
                StartSpansLazily();
                streamVariableLengthText( "stderr", stdErr );
            }

            if( m_testSpan.emitted )
            {
                m_config.stream() << "[Finished: " << testInfo.getName() << " ";
                ReportCounts( succeeded, failed );
                m_config.stream() << "]" << std::endl;
            }
        }

    private: // helpers

        ///////////////////////////////////////////////////////////////////////////
        void StartSpansLazily()
        {
            if( !m_testingSpan.emitted )
            {
                if( m_config.getName().empty() )
                    m_config.stream() << "[Started testing]" << std::endl;
                else
                    m_config.stream() << "[Started testing: " << m_config.getName() << "]" << std::endl;
                m_testingSpan.emitted = true;
            }

            if( !m_groupSpan.emitted && !m_groupSpan.name.empty() )
            {
                m_config.stream() << "[Started group: '" << m_groupSpan.name << "']" << std::endl;
                m_groupSpan.emitted = true;
            }

            if( !m_testSpan.emitted )
            {
                m_config.stream() << std::endl << "[Running: " << m_testSpan.name << "]" << std::endl;
                m_testSpan.emitted = true;
            }

            if( !m_sectionSpans.empty() )
            {
                SpanInfo& sectionSpan = m_sectionSpans.back();
                if( !sectionSpan.emitted && !sectionSpan.name.empty() )
                {
                    if( m_firstSectionInTestCase )
                    {
                        m_config.stream() << "\n";
                        m_firstSectionInTestCase = false;
                    }
                    std::vector<SpanInfo>::iterator it = m_sectionSpans.begin();
                    std::vector<SpanInfo>::iterator itEnd = m_sectionSpans.end();
                    for(; it != itEnd; ++it )
                    {
                        SpanInfo& prevSpan = *it;
                        if( !prevSpan.emitted && !prevSpan.name.empty() )
                        {
                            m_config.stream() << "[Started section: '" << prevSpan.name << "']" << std::endl;
                            prevSpan.emitted = true;
                        }
                    }
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////
        void streamVariableLengthText
        (
            const std::string& prefix,
            const std::string& text
        )
        {
            std::string trimmed = trim( text );
            if( trimmed.find_first_of( "\r\n" ) == std::string::npos )
            {
                m_config.stream() << "[" << prefix << ": " << trimmed << "]\n";
            }
            else
            {
                m_config.stream()   << "\n[" << prefix << "] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" << trimmed
                                    << "\n[end of " << prefix << "] <<<<<<<<<<<<<<<<<<<<<<<<\n";
            }
        }

    private:
        const IReporterConfig& m_config;
        bool m_firstSectionInTestCase;

        SpanInfo m_testingSpan;
        SpanInfo m_groupSpan;
        SpanInfo m_testSpan;
        std::vector<SpanInfo> m_sectionSpans;
    };

    INTERNAL_CATCH_REGISTER_REPORTER( "basic", BasicReporter )

} // end namespace Catch

// #included from: reporters/catch_reporter_xml.hpp

/*
 *  catch_reporter_xml.hpp
 *  Catch
 *
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_CATCH_REPORTER_XML_HPP_INCLUDED

// #included from: ../internal/catch_xmlwriter.hpp

/*
 *  catch_xmlwriter.hpp
 *  Catch
 *
 *  Created by Phil on 09/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#define TWOBLUECUBES_CATCH_XMLWRITER_HPP_INCLUDED

#include <sstream>
#include <string>
#include <vector>

namespace Catch
{
    class XmlWriter
    {
    public:

        class ScopedElement
        {
        public:
            ///////////////////////////////////////////////////////////////////
            ScopedElement
            (
                XmlWriter* writer
            )
            :   m_writer( writer )
            {
            }

            ///////////////////////////////////////////////////////////////////
            ScopedElement
            (
                const ScopedElement& other
            )
            :   m_writer( other.m_writer )
            {
                other.m_writer = NULL;
            }

            ///////////////////////////////////////////////////////////////////
            ~ScopedElement
            ()
            {
                if( m_writer )
                    m_writer->endElement();
            }

            ///////////////////////////////////////////////////////////////////
            ScopedElement& writeText
            (
                const std::string& text
            )
            {
                m_writer->writeText( text );
                return *this;
            }

            ///////////////////////////////////////////////////////////////////
            template<typename T>
            ScopedElement& writeAttribute
            (
                const std::string& name,
                const T& attribute
            )
            {
                m_writer->writeAttribute( name, attribute );
                return *this;
            }

        private:
            mutable XmlWriter* m_writer;
        };

        ///////////////////////////////////////////////////////////////////////
        XmlWriter
        ()
        :   m_tagIsOpen( false ),
            m_needsNewline( false ),
            m_os( &std::cout )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter
        (
            std::ostream& os
        )
        :   m_tagIsOpen( false ),
            m_needsNewline( false ),
            m_os( &os )
        {
        }

        ///////////////////////////////////////////////////////////////////////
        ~XmlWriter
        ()
        {
            while( !m_tags.empty() )
            {
                endElement();
            }
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter& operator =
        (
            const XmlWriter& other
        )
        {
            XmlWriter temp( other );
            swap( temp );
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        void swap
        (
            XmlWriter& other
        )
        {
            std::swap( m_tagIsOpen, other.m_tagIsOpen );
            std::swap( m_needsNewline, other.m_needsNewline );
            std::swap( m_tags, other.m_tags );
            std::swap( m_indent, other.m_indent );
            std::swap( m_os, other.m_os );
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter& startElement
        (
            const std::string& name
        )
        {
            ensureTagClosed();
            newlineIfNecessary();
            stream() << m_indent << "<" << name;
            m_tags.push_back( name );
            m_indent += "  ";
            m_tagIsOpen = true;
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        ScopedElement scopedElement
        (
            const std::string& name
        )
        {
            ScopedElement scoped( this );
            startElement( name );
            return scoped;
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter& endElement
        ()
        {
            newlineIfNecessary();
            m_indent = m_indent.substr( 0, m_indent.size()-2 );
            if( m_tagIsOpen )
            {
                stream() << "/>\n";
                m_tagIsOpen = false;
            }
            else
            {
                stream() << m_indent << "</" << m_tags.back() << ">\n";
            }
            m_tags.pop_back();
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter& writeAttribute
        (
            const std::string& name,
            const std::string& attribute
        )
        {
            if( !name.empty() && !attribute.empty() )
            {
                stream() << " " << name << "=\"";
                writeEncodedText( attribute );
                stream() << "\"";
            }
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter& writeAttribute
        (
            const std::string& name,
            bool attribute
        )
        {
            stream() << " " << name << "=\"" << ( attribute ? "true" : "false" ) << "\"";
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        template<typename T>
        XmlWriter& writeAttribute
        (
            const std::string& name,
            const T& attribute
        )
        {
            if( !name.empty() )
            {
                stream() << " " << name << "=\"" << attribute << "\"";
            }
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter& writeText
        (
            const std::string& text
        )
        {
            if( !text.empty() )
            {
                bool tagWasOpen = m_tagIsOpen;
                ensureTagClosed();
                if( tagWasOpen )
                    stream() << m_indent;
                writeEncodedText( text );
                m_needsNewline = true;
            }
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter& writeComment
        (
            const std::string& text
        )
        {
            ensureTagClosed();
            stream() << m_indent << "<!--" << text << "-->";
            m_needsNewline = true;
            return *this;
        }

        ///////////////////////////////////////////////////////////////////////
        XmlWriter& writeBlankLine
        ()
        {
            ensureTagClosed();
            stream() << "\n";
            return *this;
        }

    private:

        ///////////////////////////////////////////////////////////////////////
        std::ostream& stream
        ()
        {
            return *m_os;
        }

        ///////////////////////////////////////////////////////////////////////
        void ensureTagClosed
        ()
        {
            if( m_tagIsOpen )
            {
                stream() << ">\n";
                m_tagIsOpen = false;
            }
        }

        ///////////////////////////////////////////////////////////////////////
        void newlineIfNecessary
        ()
        {
            if( m_needsNewline )
            {
                stream() << "\n";
                m_needsNewline = false;
            }
        }

        ///////////////////////////////////////////////////////////////////////
        void writeEncodedText
        (
            const std::string& text
        )
        {
            static const char* charsToEncode = "<&\"";
            std::string mtext = text;
            std::string::size_type pos = mtext.find_first_of( charsToEncode );
            while( pos != std::string::npos )
            {
                stream() << mtext.substr( 0, pos );

                switch( mtext[pos] )
                {
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

namespace Catch
{
    class XmlReporter : public Catch::IReporter
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        XmlReporter
        (
            const IReporterConfig& config
        )
        : m_config( config )
        {
        }

        ///////////////////////////////////////////////////////////////////////////
        static std::string getDescription
        ()
        {
            return "Reports test results as an XML document";
        }

    private: // IReporter

        ///////////////////////////////////////////////////////////////////////////
        virtual bool shouldRedirectStdout
        ()
        const
        {
            return true;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTesting
        ()
        {
            m_xml = XmlWriter( m_config.stream() );
            m_xml.startElement( "Catch" );
            if( !m_config.getName().empty() )
                m_xml.writeAttribute( "name", m_config.getName() );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTesting
        (
            std::size_t succeeded,
            std::size_t failed
        )
        {
            m_xml.scopedElement( "OverallResults" )
                .writeAttribute( "successes", succeeded )
                .writeAttribute( "failures", failed );
            m_xml.endElement();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void StartGroup
        (
            const std::string& groupName
        )
        {
            m_xml.startElement( "Group" )
                .writeAttribute( "name", groupName );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndGroup
        (
            const std::string& /*groupName*/,
            std::size_t succeeded,
            std::size_t failed
        )
        {
            m_xml.scopedElement( "OverallResults" )
                .writeAttribute( "successes", succeeded )
                .writeAttribute( "failures", failed );
            m_xml.endElement();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void StartSection( const std::string& sectionName, const std::string description )
        {
            m_xml.startElement( "Section" )
                .writeAttribute( "name", sectionName )
                .writeAttribute( "description", description );
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndSection( const std::string& /*sectionName*/, std::size_t succeeded, std::size_t failed )
        {
            m_xml.scopedElement( "OverallResults" )
                .writeAttribute( "successes", succeeded )
                .writeAttribute( "failures", failed );
            m_xml.endElement();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTestCase( const Catch::TestCaseInfo& testInfo )
        {
            m_xml.startElement( "TestCase" ).writeAttribute( "name", testInfo.getName() );
            m_currentTestSuccess = true;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void Result( const Catch::ResultInfo& resultInfo )
        {
            if( !m_config.includeSuccessfulResults() && resultInfo.getResultType() == ResultWas::Ok )
                return;

            if( resultInfo.hasExpression() )
            {
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

            switch( resultInfo.getResultType() )
            {
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
                default:
                    break;
            }
            if( resultInfo.hasExpression() )
                m_xml.endElement();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTestCase( const Catch::TestCaseInfo&, std::size_t /* succeeded */, std::size_t /* failed */, const std::string& /*stdOut*/, const std::string& /*stdErr*/ )
        {
            m_xml.scopedElement( "OverallResult" ).writeAttribute( "success", m_currentTestSuccess );
            m_xml.endElement();
        }

    private:
        const IReporterConfig& m_config;
        bool m_currentTestSuccess;
        XmlWriter m_xml;
    };

    INTERNAL_CATCH_REGISTER_REPORTER( "xml", XmlReporter )

} // end namespace Catch

// #included from: reporters/catch_reporter_junit.hpp

/*
 *  catch_reporter_junit.hpp
 *  Catch
 *
 *  Created by Phil on 26/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#define TWOBLUECUBES_CATCH_REPORTER_JUNIT_HPP_INCLUDED


namespace Catch
{
    class JunitReporter : public Catch::IReporter
    {
        struct TestStats
        {
            std::string m_element;
            std::string m_resultType;
            std::string m_message;
            std::string m_content;
        };

        struct TestCaseStats
        {
            TestCaseStats( const std::string& name = std::string() )
            :   m_name( name )
            {
            }

            double      m_timeInSeconds;
            std::string m_status;
            std::string m_className;
            std::string m_name;
            std::vector<TestStats> m_testStats;
        };

        struct Stats
        {
            Stats( const std::string& name = std::string() )
            :   m_testsCount( 0 ),
                m_failuresCount( 0 ),
                m_disabledCount( 0 ),
                m_errorsCount( 0 ),
                m_timeInSeconds( 0 ),
                m_name( name )
            {
            }

            std::size_t m_testsCount;
            std::size_t m_failuresCount;
            std::size_t m_disabledCount;
            std::size_t m_errorsCount;
            double      m_timeInSeconds;
            std::string m_name;

            std::vector<TestCaseStats> m_testCaseStats;
        };

    public:
        ///////////////////////////////////////////////////////////////////////////
        JunitReporter( const IReporterConfig& config )
        :   m_config( config ),
            m_testSuiteStats( "AllTests" ),
            m_currentStats( &m_testSuiteStats )
        {
        }

        ///////////////////////////////////////////////////////////////////////////
        static std::string getDescription()
        {
            return "Reports test results in an XML format that looks like Ant's junitreport target";
        }

    private: // IReporter

        ///////////////////////////////////////////////////////////////////////////
        virtual bool shouldRedirectStdout
        ()
        const
        {
            return true;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTesting()
        {
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void StartGroup( const std::string& groupName )
        {

            m_statsForSuites.push_back( Stats( groupName ) );
            m_currentStats = &m_statsForSuites.back();
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndGroup( const std::string&, std::size_t succeeded, std::size_t failed )
        {
            m_currentStats->m_testsCount = failed+succeeded;
            m_currentStats = &m_testSuiteStats;
        }

        virtual void StartSection( const std::string& /*sectionName*/, const std::string /*description*/ )
        {
        }

        virtual void EndSection( const std::string& /*sectionName*/, std::size_t /*succeeded*/, std::size_t /*failed*/ )
        {
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void StartTestCase( const Catch::TestCaseInfo& testInfo )
        {
            m_currentStats->m_testCaseStats.push_back( TestCaseStats( testInfo.getName() ) );

        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void Result( const Catch::ResultInfo& resultInfo )
        {
            if( resultInfo.getResultType() != ResultWas::Ok || m_config.includeSuccessfulResults() )
            {
                TestCaseStats& testCaseStats = m_currentStats->m_testCaseStats.back();
                TestStats stats;
                std::ostringstream oss;
                if( !resultInfo.getMessage().empty() )
                {
                    oss << resultInfo.getMessage() << " at ";
                }
                oss << SourceLineInfo( resultInfo.getFilename(), resultInfo.getLine() );
                stats.m_content = oss.str();
                stats.m_message = resultInfo.getExpandedExpression();
                stats.m_resultType = resultInfo.getTestMacroName();
                switch( resultInfo.getResultType() )
                {
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
                    default:
                        stats.m_element = "unknown";
                        break;
                }
                testCaseStats.m_testStats.push_back( stats );

            }
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTestCase( const Catch::TestCaseInfo&, std::size_t /* succeeded */, std::size_t /* failed */, const std::string& stdOut, const std::string& stdErr )
        {
            if( !stdOut.empty() )
                m_stdOut << stdOut << "\n";
            if( !stdErr.empty() )
                m_stdErr << stdErr << "\n";
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual void EndTesting( std::size_t /* succeeded */, std::size_t /* failed */ )
        {
            std::ostream& str = m_config.stream();
            {
                XmlWriter xml( str );

                if( m_statsForSuites.size() > 0 )
                    xml.startElement( "testsuites" );

                std::vector<Stats>::const_iterator it = m_statsForSuites.begin();
                std::vector<Stats>::const_iterator itEnd = m_statsForSuites.end();

                for(; it != itEnd; ++it )
                {
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
                xml.scopedElement( "system-err" ).writeText( trim( m_stdOut.str() ) );
            }
        }

        ///////////////////////////////////////////////////////////////////////////
        void OutputTestCases( XmlWriter& xml, const Stats& stats )
        {
            std::vector<TestCaseStats>::const_iterator it = stats.m_testCaseStats.begin();
            std::vector<TestCaseStats>::const_iterator itEnd = stats.m_testCaseStats.end();
            for(; it != itEnd; ++it )
            {
                xml.writeBlankLine();
                xml.writeComment( "Test case" );

                XmlWriter::ScopedElement e = xml.scopedElement( "testcase" );
                xml.writeAttribute( "classname", it->m_className );
                xml.writeAttribute( "name", it->m_name );
                xml.writeAttribute( "time", "tbd" );

                OutputTestResult( xml, *it );
            }
        }


        ///////////////////////////////////////////////////////////////////////////
        void OutputTestResult( XmlWriter& xml, const TestCaseStats& stats )
        {
            std::vector<TestStats>::const_iterator it = stats.m_testStats.begin();
            std::vector<TestStats>::const_iterator itEnd = stats.m_testStats.end();
            for(; it != itEnd; ++it )
            {
                if( it->m_element != "success" )
                {
                    XmlWriter::ScopedElement e = xml.scopedElement( it->m_element );

                    xml.writeAttribute( "message", it->m_message );
                    xml.writeAttribute( "type", it->m_resultType );
                    if( !it->m_content.empty() )
                        xml.writeText( it->m_content );
                }
            }
        }

    private:
        const IReporterConfig& m_config;
        bool m_currentTestSuccess;

        Stats m_testSuiteStats;
        Stats* m_currentStats;
        std::vector<Stats> m_statsForSuites;
        std::ostringstream m_stdOut;
        std::ostringstream m_stdErr;
    };

    INTERNAL_CATCH_REGISTER_REPORTER( "junit", JunitReporter )

} // end namespace Catch


#include <fstream>
#include <stdlib.h>
#include <limits>

namespace Catch
{
    //////////////////////////////////////////////////////////////////////////
    inline int Main
    (
        Config& config
    )
    {
        // Handle list request
        if( config.listWhat() != Config::List::None )
            return List( config );

        // Open output file, if specified
        std::ofstream ofs;
        if( !config.getFilename().empty() )
        {
            ofs.open( config.getFilename().c_str() );
            if( ofs.fail() )
            {
                std::cerr << "Unable to open file: '" << config.getFilename() << "'" << std::endl;
                return (std::numeric_limits<int>::max)();
            }
            config.setStreamBuf( ofs.rdbuf() );
        }

        Runner runner( config );

        // Run test specs specified on the command line - or default to all
        if( !config.testsSpecified() )
        {
            config.getReporter()->StartGroup( "" );
            runner.runAll();
            config.getReporter()->EndGroup( "", runner.getSuccessCount(), runner.getFailureCount() );
        }
        else
        {
            // !TBD We should get all the testcases upfront, report any missing,
            // then just run them
            std::vector<std::string>::const_iterator it = config.getTestSpecs().begin();
            std::vector<std::string>::const_iterator itEnd = config.getTestSpecs().end();
            for(; it != itEnd; ++it )
            {
                size_t prevSuccess = runner.getSuccessCount();
                size_t prevFail = runner.getFailureCount();
                config.getReporter()->StartGroup( *it );
                if( runner.runMatching( *it ) == 0 )
                {
                    // Use reporter?
//                    std::cerr << "\n[Unable to match any test cases with: " << *it << "]" << std::endl;
                }
                config.getReporter()->EndGroup( *it, runner.getSuccessCount()-prevSuccess, runner.getFailureCount()-prevFail );
            }
        }

        return static_cast<int>( runner.getFailureCount() );
    }

    //////////////////////////////////////////////////////////////////////////
    inline void showHelp
    (
        std::string exeName
    )
    {
        std::string::size_type pos = exeName.find_last_of( "/\\" );
        if( pos != std::string::npos )
        {
            exeName = exeName.substr( pos+1 );
        }

        std::cout   << exeName << " is a CATCH host application. Options are as follows:\n\n"
        << "\t-l, --list <tests | reporters> [xml]\n"
        << "\t-t, --test <testspec> [<testspec>...]\n"
        << "\t-r, --reporter <reporter name>\n"
        << "\t-o, --out <file name>|<%stream name>\n"
        << "\t-s, --success\n"
        << "\t-b, --break\n"
        << "\t-n, --name <name>\n\n"
        << "For more detail usage please see: https://github.com/philsquared/Catch/wiki/Command-line" << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////
    inline int Main
    (
        int argc,
        char* const argv[],
        Config& config
    )
    {
        ArgParser( argc, argv, config );

        if( !config.getMessage().empty() )
        {
            std::cerr << config.getMessage() << std::endl;
            return (std::numeric_limits<int>::max)();
        }

        // Handle help
        if( config.showHelp() )
        {
            showHelp( argv[0] );
            return 0;
        }

        return Main( config );
    }

    //////////////////////////////////////////////////////////////////////////
    inline int Main
    (
        int argc,
        char* const argv[]
    )
    {
        Config config;
//        if( isDebuggerActive() )
//            config.useStream( "debug" );
        return Main( argc, argv, config );
    }

} // end namespace Catch

#endif

#ifdef CATCH_CONFIG_MAIN
// #included from: internal/catch_default_main.hpp

/*
 *  catch_default_main.hpp
 *  Catch
 *
 *  Created by Phil on 20/05/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#define TWOBLUECUBES_CATCH_DEFAULT_MAIN_HPP_INCLUDED

int main (int argc, char * const argv[])
{
#ifdef __OBJC__

    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    Catch::registerTestMethods();

    int result = Catch::Main( argc, (char* const*)argv );

    [pool drain];

#else

    int result =Catch::Main( argc, argv );

#endif
    Catch::Hub::cleanUp();
    return result;
}

#endif

//////

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

#define INFO( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Info, false, "INFO" )
#define WARN( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::Warning, false, "WARN" )
#define FAIL( msg ) INTERNAL_CATCH_MSG( msg, Catch::ResultWas::ExplicitFailure, true, "FAIL" )
#define SCOPED_INFO( msg ) INTERNAL_CATCH_SCOPED_INFO( msg )
#define CAPTURE( msg ) INTERNAL_CATCH_MSG( #msg " := " << msg, Catch::ResultWas::Info, false, "CAPTURE" )

#define SECTION( name, description ) INTERNAL_CATCH_SECTION( name, description )

#define TEST_CASE( name, description ) INTERNAL_CATCH_TESTCASE( name, description )
#define TEST_CASE_NORETURN( name, description ) INTERNAL_CATCH_TESTCASE_NORETURN( name, description )
#define ANON_TEST_CASE() INTERNAL_CATCH_TESTCASE( "", "Anonymous test case" )
#define METHOD_AS_TEST_CASE( method, name, description ) CATCH_METHOD_AS_TEST_CASE( method, name, description )

#define REGISTER_REPORTER( name, reporterType ) INTERNAL_CATCH_REGISTER_REPORTER( name, reporterType )
#define CATCH_TRANSLATE_EXCEPTION( signature ) INTERNAL_CATCH_TRANSLATE_EXCEPTION( signature )

#define GENERATE( expr) INTERNAL_CATCH_GENERATE( expr )

///////////////
// Still to be implemented
#define CHECK_NOFAIL( expr ) // !TBD - reports violation, but doesn't fail Test

using Catch::Detail::Approx;

#endif // TWOBLUECUBES_CATCH_HPP_INCLUDED

