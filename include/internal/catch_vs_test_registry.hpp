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
#ifdef INTERNAL_CATCH_VS_MANAGED
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
        assignName(_name);
    }
    NameAndDesc( const wchar_t* _name, int  )
    : name(), description( "" )
    {
        assignName(_name);
    }
    void assignName(const wchar_t* _name)
    {
        stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t> > conv1;
        std::string tmp = conv1.to_bytes(_name);
        if( tmp.empty() )
        {
            name = tmp;
        }
        else
        {
            std::string::iterator startIter = tmp.begin();
            if(*startIter == '\"')
            {
                ++startIter;
            }
            std::string::reverse_iterator endIter = tmp.rbegin();
            if(*endIter == '\"')
            {
                ++endIter;
            }
            name.assign(startIter, endIter.base());
        }
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

#ifdef INTERNAL_CATCH_VS_MANAGED

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

#define INTERNAL_CATCH_TEST_METHOD( Count, UniqueExt, Name, Desc ) \
        public: \
            [TestMethod] \
            [Description( CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name) )] \
            [TestProperty( "Description", CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name) )] \
            void INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___M_E_T_H_O_D___, UniqueExt) () \
            { \
                Catch::NameAndDesc name_desc( CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name), Desc ); \
                CATCH_INTERNAL_RUN_SINGLE_TEST(Count); \
            }

#define BEGIN_INTERNAL_CATCH_BATCH_METHOD( Tags, UniqueExt ) \
        public: \
            [TestMethod] \
            [TestCategory( CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Tags) )] \
            [Description( CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Tags) )] \
            [TestProperty( "Description", CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Tags) )] \
            void INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___M_E_T_H_O_D___, UniqueExt) ()

#define CHECK_FOR_TEST_CASE_CLASH 

#define INTERNAL_CATCH_MAP_CATEGORY_TO_TAG( Category, Tag ) \
    INTERNAL_CATCH_MAP_CATEGORY_TO_TAG2( #Category, Tag, __COUNTER__ )

#define INTERNAL_CATCH_MAP_CATEGORY_TO_LIST( Category ) \
    INTERNAL_CATCH_MAP_CATEGORY_TO_LIST2( #Category, #Category, __COUNTER__ )

#define FAIL_STRING( str ) _T( str )

#else // detect CLR

// Native tests

#define INTERNAL_CATCH_CLASS_DEFINITION( cls ) \
            TEST_CLASS( cls )

#define INTERNAL_CATCH_CLASS_CONTEXT

#define CATCH_INTERNAL_NAMESPACE( Ext ) INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___N_S_, Ext )

#define TEST2( ... ) TEST_IMPL_2( (__VA_ARGS__, 2, 1) )
#define TEST_IMPL_2(tuple) TEST_IMPL2 tuple
#define TEST_IMPL2( INTERNAL_CATCH_SPLIT_ARG_1,INTERNAL_CATCH_SPLIT_ARG_2,N,...) L#INTERNAL_CATCH_SPLIT_ARG_1

#define CATCH_INTERNAL_HANDLE_EMPTY_PARAM(...) CATCH_INTERNAL_HANDLE_EMPTY_PARAM_IMPLW( (__VA_ARGS__, 2, 1) )
#define CATCH_INTERNAL_HANDLE_EMPTY_PARAM_IMPLW(tuple) CATCH_INTERNAL_HANDLE_EMPTY_PARAM_IMPL2W tuple
#define CATCH_INTERNAL_HANDLE_EMPTY_PARAM_IMPL2W( INTERNAL_CATCH_SPLIT_ARG_1,INTERNAL_CATCH_SPLIT_ARG_2,N,...) L#INTERNAL_CATCH_SPLIT_ARG_1

#define INTERNAL_CATCH_TEST_METHOD( Count, UniqueExt, Name, Desc ) \
        public: \
	    BEGIN_TEST_METHOD_ATTRIBUTE( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___M_E_T_H_O_D___, UniqueExt) ) \
            TEST_OWNER( CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name) ) \
            TEST_DESCRIPTION( CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name) ) \
	    END_TEST_METHOD_ATTRIBUTE() \
    	TEST_METHOD( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___M_E_T_H_O_D___, UniqueExt) ) \
        { \
            Catch::NameAndDesc name_desc(CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name), Desc ); \
            CATCH_INTERNAL_RUN_SINGLE_TEST(Count); \
        }

#define BEGIN_INTERNAL_CATCH_BATCH_METHOD( Tags, UniqueExt ) \
        public: \
	        BEGIN_TEST_METHOD_ATTRIBUTE( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___M_E_T_H_O_D___, UniqueExt) ) \
                TEST_OWNER( CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Tags) ) \
                TEST_DESCRIPTION( CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Tags) ) \
	        END_TEST_METHOD_ATTRIBUTE() \
            TEST_METHOD( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H___M_E_T_H_O_D___, UniqueExt) )

#define CHECK_FOR_TEST_CASE_CLASH void INTERNAL_CATCH_UNIQUE_NAME_LINE( if_you_get_this_error_you_have_a_test_case_name_clash_please_put_a_namespace_around_the_test_case_at_line_, __LINE__ )() {}

#define INTERNAL_CATCH_MAP_CATEGORY_TO_TAG( Category, Tag ) \
    INTERNAL_CATCH_MAP_CATEGORY_TO_TAG2( Category, Tag, __COUNTER__ )

#define INTERNAL_CATCH_MAP_CATEGORY_TO_LIST( Category ) \
    INTERNAL_CATCH_MAP_CATEGORY_TO_LIST2( Category, #Category, __COUNTER__ )

#define FAIL_STRING( str ) WIDEN( str )

#endif // detect CLR

#define INTERNAL_CATCH_CONCAT_LINE_COUNTER( count ) INTERNAL_CATCH_UNIQUE_NAME_LINE( INTERNAL_CATCH_UNIQUE_NAME_LINE( __LINE__, _ ), count )

#define CATCH_INTERNAL_CONFIG_SHOW_SUCCESS2( v, Count ) \
    namespace { CatchOverrides::ConfigShowSuccessfulTests<Catch::IConfig const*> INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H_____O_V_E_R_R_I_D_E____, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )(__FILE__, Count, v); }

#define CATCH_INTERNAL_CONFIG_WARN_MISSING_ASSERTIONS2( v, Count ) \
    namespace { CatchOverrides::ConfigWarnMissingAssertions<Catch::IConfig const*> INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H_____O_V_E_R_R_I_D_E____, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )(__FILE__, Count, v); }

#define CATCH_INTERNAL_CONFIG_ABORT_AFTER2( v, Count ) \
    namespace { CatchOverrides::ConfigAbortAfter<Catch::IConfig const*> INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H_____O_V_E_R_R_I_D_E____, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )(__FILE__, Count, v); }

#define CATCH_INTERNAL_CONFIG_ADD_TEST2( v, Count ) \
    namespace { CatchOverrides::ConfigAddTest<Catch::IConfig const*> INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H_____O_V_E_R_R_I_D_E____, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )(__FILE__, Count, v); }

#define CATCH_INTERNAL_CONFIG_SHOW_SUCCESS( v ) \
    CATCH_INTERNAL_CONFIG_SHOW_SUCCESS2( v, __COUNTER__)

#define CATCH_INTERNAL_CONFIG_WARN_MISSING_ASSERTIONS( v ) \
    CATCH_INTERNAL_CONFIG_WARN_MISSING_ASSERTIONS2( v, __COUNTER__)

#define CATCH_INTERNAL_CONFIG_ABORT_AFTER( v ) \
    CATCH_INTERNAL_CONFIG_ABORT_AFTER2( v, __COUNTER__)

#define CATCH_INTERNAL_CONFIG_ADD_TEST( v ) \
    CATCH_INTERNAL_CONFIG_ADD_TEST2( v, __COUNTER__)

#define CATCH_INTERNAL_RUN_SINGLE_TEST( Count ) \
        {   CatchOverrides::ConfigGuard cg; \
            Catch::ConfigData cd(cg.value().get()); \
            cd.name = name_desc.name; \
            cd.abortAfter = 1; \
            cd.showSuccessfulTests = CatchOverrides::Config<Catch::IConfig const*>::instance().includeSuccessfulResults(__FILE__, Count ); \
            cd.warnings            = (CatchOverrides::Config<Catch::IConfig const*>::instance().warnAboutMissingAssertions(__FILE__, Count ) ? Catch::WarnAbout::NoAssertions : Catch::WarnAbout::Nothing); \
            cd.abortAfter          = CatchOverrides::Config<Catch::IConfig const*>::instance().abortAfter(__FILE__, Count ); \
            Catch::Ptr<Catch::Config> config(new Catch::Config(cd)); \
            Catch::ReporterRegistrar<Catch::MSTestReporter> reporterReg("vs_reporter"); \
            Catch::RunContext context(config.get(), Catch::getRegistryHub().getReporterRegistry().create( "vs_reporter", config.get())); \
            std::vector<Catch::TestCase> testCase = Catch::getRegistryHub().getTestCaseRegistry().getMatchingTestCases(name_desc.name); \
            if( testCase.empty() ) Assert::Fail(FAIL_STRING("No tests match")); \
            if( testCase.size() > 1 ) Assert::Fail(FAIL_STRING("More than one test with the same name")); \
            context.testGroupStarting( "", 0, 1 ); \
            Catch::Totals totals = context.runTest(*testCase.begin()); \
            context.testGroupEnded( "", totals, 0, 1 ); \
            if( totals.assertions.failed > 0 ) { \
                INTERNAL_CATCH_TEST_THROW_FAILURE \
            } \
        }

#define INTERNAL_CATCH_TESTCASE2( Count, Name, Desc ) \
    CHECK_FOR_TEST_CASE_CLASH \
    static void INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )(); \
    namespace CATCH_INTERNAL_NAMESPACE( INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) { \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( & INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ), CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc(CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name), Desc) ); \
        CatchOverrides::ConfigReset<Catch::IConfig const*> INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_O_N_F_I_G___, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )(__FILE__, Count, 1); \
        INTERNAL_CATCH_CLASS_DEFINITION( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_L_A_S_S___, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) ) \
        { \
            INTERNAL_CATCH_CLASS_CONTEXT \
            INTERNAL_CATCH_TEST_METHOD( Count, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ), Name, Desc ) \
        }; \
    } \
    void INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )()

#define INTERNAL_CATCH_METHOD_AS_TEST_CASE2( QualifiedMethod, Count, Name, Desc ) \
    CHECK_FOR_TEST_CASE_CLASH \
    namespace CATCH_INTERNAL_NAMESPACE( INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) { \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( & QualifiedMethod, "&" # QualifiedMethod, Catch::NameAndDesc(CATCH_INTERNAL_HANDLE_EMPTY_PARAM(Name), Desc), CATCH_INTERNAL_LINEINFO ); \
        CatchOverrides::ConfigReset<Catch::IConfig const*> INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_O_N_F_I_G___, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )(__FILE__, Count, 1); \
        INTERNAL_CATCH_CLASS_DEFINITION( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_L_A_S_S___, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) ) \
        { \
            INTERNAL_CATCH_CLASS_CONTEXT \
            INTERNAL_CATCH_TEST_METHOD( Count, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ), Name, Desc ) \
        }; \
    };

#define INTERNAL_CATCH_TEST_CASE_METHOD2( ClassName, Count, TestName, Desc ) \
    CHECK_FOR_TEST_CASE_CLASH \
    struct INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) : ClassName { \
        void test(); \
        static void invoke() { INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) tmp; tmp.test(); } \
    }; \
    namespace CATCH_INTERNAL_NAMESPACE( INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) { \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( & INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )::invoke, CATCH_INTERNAL_LINEINFO, Catch::NameAndDesc(CATCH_INTERNAL_HANDLE_EMPTY_PARAM(TestName), Desc) ); \
        CatchOverrides::ConfigReset<Catch::IConfig const*> INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_O_N_F_I_G___, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )(__FILE__, Count, 1); \
        INTERNAL_CATCH_CLASS_DEFINITION( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_L_A_S_S___, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) ) \
        { \
            INTERNAL_CATCH_CLASS_CONTEXT \
            INTERNAL_CATCH_TEST_METHOD( Count, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ), TestName, Desc ) \
        }; \
    } \
    void INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )::test()

#if defined(INTERNAL_CATCH_VS_MANAGED)

    #define INTERNAL_CATCH_TEST_REPORT_BATCH_FAILURE( count ) \
    { \
        std::stringstream _sf; \
        _sf << count << " assertions failed - check output for results."; \
        std::string fail = _sf.str(); \
        Assert::Fail(Catch::convert_string_for_assert(fail)); \
    }

#else

    #define INTERNAL_CATCH_TEST_REPORT_BATCH_FAILURE( count ) \
    { \
        std::wstringstream _s; \
        _s << count << " assertions failed - check output for results."; \
        std::wstring ws = _s.str(); \
        Assert::Fail(ws.c_str()); \
    }
#endif

#define INTERNAL_CATCH_MAP_CATEGORY_TO_TAG2( Category, Tag, Count ) \
    CHECK_FOR_TEST_CASE_CLASH \
    namespace CATCH_INTERNAL_NAMESPACE( INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) { \
        CatchOverrides::ConfigReset<Catch::IConfig const*> INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_O_N_F_I_G___, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )(__FILE__, Count, -1); \
        INTERNAL_CATCH_CLASS_DEFINITION( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_L_A_S_S___, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) ) \
        { \
            INTERNAL_CATCH_CLASS_CONTEXT \
	        BEGIN_INTERNAL_CATCH_BATCH_METHOD( Category, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) \
            { \
                Catch::ConfigData cd; \
                cd.showSuccessfulTests = CatchOverrides::Config<Catch::IConfig const*>::instance().includeSuccessfulResults(__FILE__, Count ); \
                cd.warnings            = (CatchOverrides::Config<Catch::IConfig const*>::instance().warnAboutMissingAssertions(__FILE__, Count ) ? Catch::WarnAbout::NoAssertions : Catch::WarnAbout::Nothing); \
                cd.abortAfter          = CatchOverrides::Config<Catch::IConfig const*>::instance().abortAfter(__FILE__, Count ); \
                cd.reporterName = "vs_reporterlf"; \
                cd.name = "Batch run using tag : " Tag; \
                cd.testsOrTags.push_back( Tag ); \
                Catch::Ptr<Catch::Config> config(new Catch::Config(cd)); \
                Catch::ReporterRegistrar<Catch::MSTestReporterLineFeed> reporterReg("vs_reporterlf"); \
                Catch::Runner runner(config); \
                Catch::Totals totals = runner.runTests(); \
                if( totals.assertions.failed > 0 ) { \
                    INTERNAL_CATCH_TEST_REPORT_BATCH_FAILURE(totals.assertions.failed) \
                } \
            } \
        }; \
    }

#define INTERNAL_CATCH_MAP_CATEGORY_TO_LIST2( Category, CategoryName, Count ) \
    CHECK_FOR_TEST_CASE_CLASH \
    namespace CATCH_INTERNAL_NAMESPACE( INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) { \
        CatchOverrides::ConfigReset<Catch::IConfig const*> INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_O_N_F_I_G___, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) )(__FILE__, Count, -1); \
        INTERNAL_CATCH_CLASS_DEFINITION( INTERNAL_CATCH_UNIQUE_NAME_LINE( C_A_T_C_H____T_E_S_T____C_L_A_S_S___, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) ) \
        { \
            INTERNAL_CATCH_CLASS_CONTEXT \
	        BEGIN_INTERNAL_CATCH_BATCH_METHOD( Category, INTERNAL_CATCH_CONCAT_LINE_COUNTER( Count ) ) \
            { \
                Catch::ConfigData cd; \
                cd.showSuccessfulTests = CatchOverrides::Config<Catch::IConfig const*>::instance().includeSuccessfulResults(__FILE__, Count ); \
                cd.warnings            = (CatchOverrides::Config<Catch::IConfig const*>::instance().warnAboutMissingAssertions(__FILE__, Count ) ? Catch::WarnAbout::NoAssertions : Catch::WarnAbout::Nothing); \
                cd.abortAfter          = CatchOverrides::Config<Catch::IConfig const*>::instance().abortAfter(__FILE__, Count ); \
                cd.reporterName = "vs_reporterlf"; \
                cd.name = "Batch run using category : " CategoryName; \
                std::vector<std::string> stringNames = CatchOverrides::Config<Catch::IConfig const*>::instance().listOfTests(__FILE__, Count ); \
                Catch::Ptr<Catch::Config> config(new Catch::Config(cd)); \
                Catch::ReporterRegistrar<Catch::MSTestReporterLineFeed> reporterReg("vs_reporterlf"); \
                Catch::RunContext context(config.get(), Catch::getRegistryHub().getReporterRegistry().create( "vs_reporterlf", config.get())); \
                Catch::Totals totals; \
                context.testGroupStarting( "", 0, 1 ); \
                for( std::vector<std::string>::iterator it = stringNames.begin(); it != stringNames.end(); ++it ) { \
                    std::vector<Catch::TestCase> testCase = Catch::getRegistryHub().getTestCaseRegistry().getMatchingTestCases(*it); \
                    if( testCase.empty() ) Assert::Fail(FAIL_STRING("No tests match")); \
                    if( testCase.size() > 1 ) Assert::Fail(FAIL_STRING("More than one test with the same name")); \
                    totals += context.runTest(*testCase.begin()); \
                } \
                context.testGroupEnded( "", totals, 0, 1 ); \
                if( totals.assertions.failed > 0 ) { \
                    INTERNAL_CATCH_TEST_REPORT_BATCH_FAILURE(totals.assertions.failed) \
                } \
            } \
        }; \
    }

//#undef CATCH_CONFIG_VARIADIC_MACROS

#ifdef CATCH_CONFIG_VARIADIC_MACROS

    #define INTERNAL_CATCH_SPLIT_ARGS_2( ... ) INTERNAL_CATCH_SPLIT_ARGS_IMPL_2((__VA_ARGS__, 2,1))
    #define INTERNAL_CATCH_SPLIT_ARGS_IMPL_2(tuple) INTERNAL_CATCH_SPLIT_ARGS_IMPL2 tuple
    #define INTERNAL_CATCH_SPLIT_ARGS_IMPL2(INTERNAL_CATCH_SPLIT_ARG_1,INTERNAL_CATCH_SPLIT_ARG_2,N,...) INTERNAL_CATCH_SPLIT_ARG_1
    #define INTERNAL_CATCH_SPLIT_TAGS( ... ) INTERNAL_CATCH_SPLIT_TAGS_IMPL((__VA_ARGS__, 2,1))
    #define INTERNAL_CATCH_SPLIT_TAGS_IMPL(tuple) INTERNAL_CATCH_SPLIT_TAGS_IMPL_ tuple
    #define INTERNAL_CATCH_SPLIT_TAGS_IMPL_(INTERNAL_CATCH_SPLIT_ARG_1,INTERNAL_CATCH_SPLIT_ARG_2,N,...) INTERNAL_CATCH_SPLIT_ARG_2

    #define INTERNAL_CATCH_TESTCASE( ... ) \
        INTERNAL_CATCH_TESTCASE2( __COUNTER__ , INTERNAL_CATCH_SPLIT_ARGS_2(__VA_ARGS__), INTERNAL_CATCH_SPLIT_TAGS(__VA_ARGS__) )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, ... ) \
        INTERNAL_CATCH_METHOD_AS_TEST_CASE2( QualifiedMethod, __COUNTER__, INTERNAL_CATCH_SPLIT_ARGS_2(__VA_ARGS__), INTERNAL_CATCH_SPLIT_TAGS(__VA_ARGS__) )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TEST_CASE_METHOD( ClassName, ... )\
        INTERNAL_CATCH_TEST_CASE_METHOD2(ClassName, __COUNTER__, INTERNAL_CATCH_SPLIT_ARGS_2(__VA_ARGS__), INTERNAL_CATCH_SPLIT_TAGS(__VA_ARGS__) )

#else
    ///////////////////////////////////////////////////////////////////////////////

    #define INTERNAL_CATCH_TESTCASE( Name, Desc ) \
        INTERNAL_CATCH_TESTCASE2( __COUNTER__ , Name, Desc )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, Name, Desc ) \
        INTERNAL_CATCH_METHOD_AS_TEST_CASE2( QualifiedMethod, __COUNTER__, Name, Desc )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TEST_CASE_METHOD( ClassName, TestName, Desc )\
        INTERNAL_CATCH_TEST_CASE_METHOD2(ClassName, __COUNTER__, TestName, Desc )

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
