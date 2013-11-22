/*
 *  Created by Malcolm on 6/11/2013.
 *  Copyright 2013 Malcolm Noyes. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_MSTEST_REGISTRY_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_MSTEST_REGISTRY_HPP_INCLUDED

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif

#include "catch_common.h"
#include "catch_interfaces_testcase.h"
#include "internal/catch_compiler_capabilities.h"
#include "internal/clara.h"
#include <tchar.h>

namespace Catch {

class MethodTestCase : public SharedImpl<ITestCase> {

    struct placeholder
    {
        virtual ~placeholder() {}
        virtual placeholder* clone() const = 0;
        virtual void invoke() const = 0;
    };

    template <typename C>
    struct holder : public placeholder
    {
        holder( void (C::*method)() ) : m_method( method ) {}
        virtual placeholder* clone() const {return new holder(*this);}
        void invoke() const {
            C obj;
            (obj.*m_method)();
        }
        void (C::*m_method)();
    };

    virtual void invoke() const
    {
        if( held ) held->invoke();
    }
public:
    template<typename C>
    MethodTestCase( void (C::*method)() ) : held(new holder<C>(method) ) {}
    ~MethodTestCase() { delete held;}


private:
    MethodTestCase();                       // not implemented
    MethodTestCase(const MethodTestCase&);  // not implemented
    MethodTestCase& operator=(const MethodTestCase&);  // not implemented

    placeholder* held;
};

typedef void(*TestFunction)();

struct NameAndDesc {
#if (_MANAGED == 1) || (_M_CEE == 1) // detect CLR
    NameAndDesc( const char* _name = "", const char* _description= "" )
    : name( _name ), description( _description )
    {}
    NameAndDesc( const char* _name, int )
    : name( _name ), description( "" )
    {}
#else
    NameAndDesc( const wchar_t* _name, const char* _description= ""  )
    : name(), description( _description )
    {
        stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t> > conv1;
        name = conv1.to_bytes(_name);
    }
    NameAndDesc( const wchar_t* _name, int  )
    : name(), description( "" )
    {
        stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t> > conv1;
        name = conv1.to_bytes(_name);
    }
#endif

    std::string name;
    std::string description;
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
        registerTestCase(   new MethodTestCase( method ),
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
    AutoReg( AutoReg const& );
    void operator= ( AutoReg const& );
};

} // end namespace Catch

#if (_MANAGED == 1) || (_M_CEE == 1) // detect CLR

#define CATCH_INTERNAL_HANDLE_EMPTY_PARAM2( name ) name##""
#define CATCH_INTERNAL_HANDLE_EMPTY_PARAM(...) CATCH_INTERNAL_HANDLE_EMPTY_PARAM2( INTERNAL_CATCH_SPLIT_ARGS_2(__VA_ARGS__) )

#define INTERNAL_CATCH_CLASS_DEFINITION( cls ) \
            [TestClass] \
            public ref class cls

#define INTERNAL_CATCH_CLASS_CONTEXT \
        private: \
	        TestContext^ testContextInstance; \
        public: \
	        property Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ TestContext \
	        { \
		        Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ get() \
		        { \
			        return testContextInstance; \
		        } \
		        System::Void set(Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ value) \
		        { \
			        testContextInstance = value; \
		        } \
	        };

#define CATCH_INTERNAL_NAMESPACE( Ext )

#define INTERNAL_CATCH_TEST_METHOD( Method, UniqueExt, Name, Desc ) \
        public: \
            [TestMethod] \
            [Description( CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name) )] \
            [TestProperty( "Description", CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name) )] \
            void INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___M_E_T_H_O_D___, UniqueExt) () \
            { \
                Catch::NameAndDesc name_desc( CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name), Desc ); \
                CATCH_INTERNAL_RUN_SINGLE_TEST( Method ); \
            }

#define INTERNAL_CATCH_TEST_CLASS_METHOD( Method, UniqueExt, Name, Desc ) \
        public: \
            [TestMethod] \
            [Description( CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name) )] \
            [TestProperty( "Description", CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name) )] \
            void INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___M_E_T_H_O_D___, UniqueExt) () \
            { \
                Catch::NameAndDesc name_desc( CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name), Desc ); \
                CATCH_INTERNAL_RUN_SINGLE_CLASS_TEST( Method ); \
            }

#define CHECK_FOR_TEST_CASE_CLASH 

#else // detect CLR

// Native tests

#define INTERNAL_CATCH_CLASS_DEFINITION( cls ) \
            TEST_CLASS( cls )

#define INTERNAL_CATCH_CLASS_CONTEXT

#define CATCH_INTERNAL_NAMESPACE( Ext ) INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___N_S_, Ext )

#define TEST2( ... ) TEST_IMPL_2( (__VA_ARGS__, 2, 1) )
#define TEST_IMPL_2(tuple) TEST_IMPL2 tuple
#define TEST_IMPL2( INTERNAL_CATCH_SPLIT_ARG_1,INTERNAL_CATCH_SPLIT_ARG_2,N,...) L#INTERNAL_CATCH_SPLIT_ARG_1

#define CATCH_INTERNAL_HANDLE_EMPTY_PARAM(...) CATCH_INTERNAL_HANDLE_EMPTY_PARAM_IMPL( (__VA_ARGS__, 2, 1) )
#define CATCH_INTERNAL_HANDLE_EMPTY_PARAM_IMPL(tuple) CATCH_INTERNAL_HANDLE_EMPTY_PARAM_IMPL2 tuple
#define CATCH_INTERNAL_HANDLE_EMPTY_PARAM_IMPL2( INTERNAL_CATCH_SPLIT_ARG_1,INTERNAL_CATCH_SPLIT_ARG_2,N,...) #INTERNAL_CATCH_SPLIT_ARG_1
#define CATCH_INTERNAL_HANDLE_EMPTY_PARAMW(...) CATCH_INTERNAL_HANDLE_EMPTY_PARAM_IMPLW( (__VA_ARGS__, 2, 1) )
#define CATCH_INTERNAL_HANDLE_EMPTY_PARAM_IMPLW(tuple) CATCH_INTERNAL_HANDLE_EMPTY_PARAM_IMPL2W tuple
#define CATCH_INTERNAL_HANDLE_EMPTY_PARAM_IMPL2W( INTERNAL_CATCH_SPLIT_ARG_1,INTERNAL_CATCH_SPLIT_ARG_2,N,...) L#INTERNAL_CATCH_SPLIT_ARG_1

#define INTERNAL_CATCH_TEST_METHOD( Method, UniqueExt, Name, Desc ) \
        public: \
	    BEGIN_TEST_METHOD_ATTRIBUTE( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___M_E_T_H_O_D___, UniqueExt) ) \
            TEST_OWNER( CATCH_INTERNAL_HANDLE_EMPTY_PARAMW(Name) ) \
            TEST_DESCRIPTION( CATCH_INTERNAL_HANDLE_EMPTY_PARAMW(Name) ) \
	    END_TEST_METHOD_ATTRIBUTE() \
    	TEST_METHOD( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___M_E_T_H_O_D___, UniqueExt) ) \
        { \
            Catch::NameAndDesc name_desc(CATCH_INTERNAL_HANDLE_EMPTY_PARAMW(Name), Desc ); \
            CATCH_INTERNAL_RUN_SINGLE_TEST( Method ); \
        }

#define INTERNAL_CATCH_TEST_CLASS_METHOD( Method, UniqueExt, Name, Desc ) \
        public: \
	    BEGIN_TEST_METHOD_ATTRIBUTE( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___M_E_T_H_O_D___, UniqueExt) ) \
            TEST_OWNER( CATCH_INTERNAL_HANDLE_EMPTY_PARAMW(Name) ) \
            TEST_DESCRIPTION( CATCH_INTERNAL_HANDLE_EMPTY_PARAMW(Name) ) \
	    END_TEST_METHOD_ATTRIBUTE() \
    	TEST_METHOD( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___M_E_T_H_O_D___, UniqueExt) ) \
        { \
            Catch::NameAndDesc name_desc( CATCH_INTERNAL_HANDLE_EMPTY_PARAMW(Name), Desc ); \
            CATCH_INTERNAL_RUN_SINGLE_CLASS_TEST( Method ); \
        }

#define CHECK_FOR_TEST_CASE_CLASH void INTERNAL_CATCH_UNIQUE_NAME_LINE( if_you_get_this_error_you_have_a_test_case_name_clash_please_put_a_namespace_around_the_test_case_at_line_, __LINE__ )() {}

#endif // detect CLR

#define INTERNAL_CATCH_CONCAT_LINE_COUNTER INTERNAL_CATCH_UNIQUE_NAME_LINE( INTERNAL_CATCH_UNIQUE_NAME_LINE( __LINE__, _ ), __COUNTER__ )

#define CATCH_INTERNAL_RUN_SINGLE_TEST( Method ) \
        {   Catch::ConfigData cd; \
            cd.name = name_desc.name; \
            cd.abortAfter = 1; \
            Catch::Ptr<Catch::Config> config(new Catch::Config(cd)); \
            Catch::MSTestReporter* rep = new Catch::MSTestReporter(config.get()); \
            Catch::RunContext tr(config.get(), rep); \
            std::vector<Catch::TestCase> testCase = Catch::getRegistryHub().getTestCaseRegistry().getMatchingTestCases(name_desc.name); \
            if( testCase.empty() ) Assert::Fail("No tests match"); \
            if( testCase.size() > 1 ) Assert::Fail("More than one test with the same name"); \
            tr.runTest(*testCase.begin()); \
        }

#define CATCH_INTERNAL_RUN_SINGLE_CLASS_TEST( ClassMethod ) \
        {   Catch::ConfigData cd; \
            cd.name = name_desc.name; \
            cd.abortAfter = 1; \
            Catch::Ptr<Catch::Config> config(new Catch::Config(cd)); \
            Catch::MSTestReporter* rep = new Catch::MSTestReporter(config.get()); \
            Catch::RunContext tr(config.get(), rep); \
            std::vector<Catch::TestCase> testCase = Catch::getRegistryHub().getTestCaseRegistry().getMatchingTestCases(name_desc.name); \
            if( testCase.empty() ) Assert::Fail("No tests match"); \
            if( testCase.size() > 1 ) Assert::Fail("More than one test with the same name"); \
            tr.runTest(*testCase.begin()); \
        }

#define INTERNAL_CATCH_TESTCASE2( UniqueExt, Name, Desc ) \
    CHECK_FOR_TEST_CASE_CLASH \
    static void INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, UniqueExt )(); \
    namespace CATCH_INTERNAL_NAMESPACE( UniqueExt ) { \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( & INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, UniqueExt ), CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc(Name, Desc) ); \
        INTERNAL_CATCH_CLASS_DEFINITION( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_L_A_S_S___, UniqueExt ) ) \
        { \
            INTERNAL_CATCH_CLASS_CONTEXT \
            INTERNAL_CATCH_TEST_METHOD( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, UniqueExt ), UniqueExt, Name, Desc ) \
        }; \
    } \
    void INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, UniqueExt )()

#define INTERNAL_CATCH_METHOD_AS_TEST_CASE2( QualifiedMethod, UniqueExt, Name, Desc ) \
    CHECK_FOR_TEST_CASE_CLASH \
    namespace CATCH_INTERNAL_NAMESPACE( UniqueExt ) { \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( & QualifiedMethod, "&" # QualifiedMethod, Catch::NameAndDesc(Name, Desc), CATCH_INTERNAL_LINEINFO ); \
        INTERNAL_CATCH_CLASS_DEFINITION( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_L_A_S_S___, UniqueExt ) ) \
        { \
            INTERNAL_CATCH_CLASS_CONTEXT \
            INTERNAL_CATCH_TEST_CLASS_METHOD( QualifiedMethod, UniqueExt, Name, Desc ) \
        }; \
    };

#define INTERNAL_CATCH_TEST_CASE_METHOD2( ClassName, UniqueExt, TestName, Desc ) \
    CHECK_FOR_TEST_CASE_CLASH \
    struct INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, UniqueExt ) : ClassName { \
        void test(); \
        static void invoke() { INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, UniqueExt ) tmp; tmp.test(); } \
    }; \
    namespace CATCH_INTERNAL_NAMESPACE( UniqueExt ) { \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( & INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, UniqueExt )::invoke, CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc(TestName, Desc) ); \
        INTERNAL_CATCH_CLASS_DEFINITION( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_L_A_S_S___, UniqueExt ) ) \
        { \
            INTERNAL_CATCH_CLASS_CONTEXT \
            INTERNAL_CATCH_TEST_METHOD( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, UniqueExt )::invoke, UniqueExt, TestName, Desc ) \
        }; \
    } \
    void INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, UniqueExt )::test()

//#undef CATCH_CONFIG_VARIADIC_MACROS

#ifdef CATCH_CONFIG_VARIADIC_MACROS

    #define INTERNAL_CATCH_SPLIT_ARGS_2( ... ) INTERNAL_CATCH_SPLIT_ARGS_IMPL_2((__VA_ARGS__, 2,1))
    #define INTERNAL_CATCH_SPLIT_ARGS_IMPL_2(tuple) INTERNAL_CATCH_SPLIT_ARGS_IMPL2 tuple
    #define INTERNAL_CATCH_SPLIT_ARGS_IMPL2(INTERNAL_CATCH_SPLIT_ARG_1,INTERNAL_CATCH_SPLIT_ARG_2,N,...) INTERNAL_CATCH_SPLIT_ARG_1
    #define INTERNAL_CATCH_SPLIT_TAGS( ... ) INTERNAL_CATCH_SPLIT_TAGS_IMPL((__VA_ARGS__, 2,1))
    #define INTERNAL_CATCH_SPLIT_TAGS_IMPL(tuple) INTERNAL_CATCH_SPLIT_TAGS_IMPL_ tuple
    #define INTERNAL_CATCH_SPLIT_TAGS_IMPL_(INTERNAL_CATCH_SPLIT_ARG_1,INTERNAL_CATCH_SPLIT_ARG_2,N,...) INTERNAL_CATCH_SPLIT_ARG_2

    #define INTERNAL_CATCH_TESTCASE( ... ) \
        INTERNAL_CATCH_TESTCASE2( INTERNAL_CATCH_CONCAT_LINE_COUNTER, INTERNAL_CATCH_SPLIT_ARGS_2(__VA_ARGS__), INTERNAL_CATCH_SPLIT_TAGS(__VA_ARGS__) )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, ... ) \
        INTERNAL_CATCH_METHOD_AS_TEST_CASE2( QualifiedMethod, INTERNAL_CATCH_CONCAT_LINE_COUNTER, INTERNAL_CATCH_SPLIT_ARGS_2(__VA_ARGS__), INTERNAL_CATCH_SPLIT_TAGS(__VA_ARGS__) )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TEST_CASE_METHOD( ClassName, ... )\
        INTERNAL_CATCH_TEST_CASE_METHOD2(ClassName, INTERNAL_CATCH_CONCAT_LINE_COUNTER, INTERNAL_CATCH_SPLIT_ARGS_2(__VA_ARGS__), INTERNAL_CATCH_SPLIT_TAGS(__VA_ARGS__) )

#else
    ///////////////////////////////////////////////////////////////////////////////

    #define INTERNAL_CATCH_TESTCASE( Name, Desc ) \
        INTERNAL_CATCH_TESTCASE2( INTERNAL_CATCH_CONCAT_LINE_COUNTER, Name, Desc )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, Name, Desc ) \
        INTERNAL_CATCH_METHOD_AS_TEST_CASE2( QualifiedMethod, INTERNAL_CATCH_CONCAT_LINE_COUNTER, Name, Desc )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TEST_CASE_METHOD( ClassName, TestName, Desc )\
        INTERNAL_CATCH_TEST_CASE_METHOD2(ClassName, INTERNAL_CATCH_CONCAT_LINE_COUNTER, TestName, Desc )

#endif

#include "catch_test_case_info.hpp"
#include "catch_assertionresult.hpp"
#include "catch_expressionresult_builder.hpp"
#include "catch_version.hpp"
#include "catch_text.h"
#include "catch_text.hpp"
#include "catch_runner_impl.hpp"
#include "catch_message.hpp"
#include "catch_context_impl.hpp"
#include "catch_generators_impl.hpp"
#include "catch_test_case_info.hpp"
#include "catch_notimplemented_exception.hpp"

#include "catch_exception_translator_registry.hpp"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // TWOBLUECUBES_CATCH_MSTEST_REGISTRY_HPP_INCLUDED
