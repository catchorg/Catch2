/*
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch.hpp"
#include "internal/catch_text.h"
#include "internal/catch_console_colour.hpp"

namespace AllTestsRunner {

#ifdef OLD_RUNNER
    class NullStreamingReporter : public Catch::SharedImpl<Catch::IStreamingReporter> {
    public:

        virtual ~NullStreamingReporter();

        static std::string getDescription() {
            return "null reporter";
        }
        
    private: // IStreamingReporter
        
        virtual Catch::ReporterPreferences getPreferences() const {
            return Catch::ReporterPreferences();
        }

        virtual void noMatchingTestCases( std::string const& ) {}
        virtual void testRunStarting( Catch::TestRunInfo const& ) {}
        virtual void testGroupStarting( Catch::GroupInfo const& ) {}
        virtual void testCaseStarting( Catch::TestCaseInfo const& ) {}
        virtual void sectionStarting( Catch::SectionInfo const& ) {}
        virtual void assertionStarting( Catch::AssertionInfo const& ) {}
        virtual bool assertionEnded( Catch::AssertionStats const& ) { return false; }
        virtual void sectionEnded( Catch::SectionStats const& ) {}
        virtual void testCaseEnded( Catch::TestCaseStats const& ) {}
        virtual void testGroupEnded( Catch::TestGroupStats const& ) {}
        virtual void testRunEnded( Catch::TestRunStats const& ) {}
    };

    class EmbeddedRunner {

    public:
        EmbeddedRunner() : m_reporter( new NullStreamingReporter() ) {}
        
        Catch::Totals runMatching( const std::string& rawTestSpec,
                            std::size_t groupIndex,
                            std::size_t groupsCount,
                            const std::string& reporter = "console" );
        
    private:
        Catch::Ptr<Catch::IStreamingReporter> m_reporter;
    };

    NullStreamingReporter::~NullStreamingReporter() {}

    Catch::Totals EmbeddedRunner::runMatching( const std::string& rawTestSpec, std::size_t groupIndex, std::size_t groupsCount, const std::string& ) {
        std::ostringstream oss;
        Catch::Ptr<Catch::Config> config = new Catch::Config();
        config->setStreamBuf( oss.rdbuf() );
        
        Catch::Totals totals;

        // Scoped because RunContext doesn't report EndTesting until its destructor
        {
            Catch::RunContext runner( config.get(), m_reporter.get() );
            totals = runner.runMatching( rawTestSpec, groupIndex, groupsCount );
        }
        return totals;
    }

    class MetaTestRunner {

    public:
        struct Expected { enum Result {
            ToSucceed,
            ToFail
        }; };
        
        MetaTestRunner( Expected::Result expectedResult, std::size_t groupIndex, std::size_t groupsCount )
        :   m_expectedResult( expectedResult ),
            m_groupIndex( groupIndex ),
            m_groupsCount( groupsCount )
        {}
        
        static void runMatching(    const std::string& testSpec, 
                                    Expected::Result expectedResult,
                                    std::size_t groupIndex,
                                    std::size_t groupsCount ) {
            forEach(    Catch::getRegistryHub().getTestCaseRegistry().getMatchingTestCases( testSpec ), 
                        MetaTestRunner( expectedResult, groupIndex, groupsCount ) );
        }
        
        void operator()( const Catch::TestCase& testCase ) {
            std::string name;
            Catch::Totals totals;
            {
                EmbeddedRunner runner;
                name = testCase.getTestCaseInfo().name;
                totals = runner.runMatching( name, m_groupIndex, m_groupsCount );
            }
            switch( m_expectedResult ) {
                case Expected::ToSucceed:
                    if( totals.assertions.failed > 0 ) {
                        FAIL( "Expected test case '"
                             << name
                             << "' to succeed but there was/ were " 
                             << totals.assertions.failed << " failure(s)" );
                    }
                    else {
                        SUCCEED( "Tests passed, as expected" );
                    }
                    break;
                case Expected::ToFail:
                    if( totals.assertions.failed == 0 ) {
                        FAIL( "Expected test case '"
                             << name
                             << "' to fail but there was/ were " 
                             << totals.assertions.passed << " success(es)" );
                    }
                    else {
                        SUCCEED( "Tests failed, as expected" );
                    }
                    break;
            }        
        }

    private:
        Expected::Result m_expectedResult;
        std::size_t m_groupIndex;
        std::size_t m_groupsCount;
    };
    
    TEST_CASE( "Run all failing and succeeding tests", "[vsall]" ) {

        ///////////////////////////////////////////////////////////////////////////
        SECTION(    "selftest/expected result",
                    "Tests do what they claim" ) {

#ifdef _UNICODE
            std::cout << "using Unicode..." << std::endl;
#else
            std::cout << "using Mbcs..." << std::endl;
#endif
                
            SECTION(    "selftest/expected result/failing tests", 
                        "Tests in the 'failing' branch fail" ) {
                std::cout << "Tests in the 'failing' branch fail" << std::endl;
                MetaTestRunner::runMatching( "./failing/*",  MetaTestRunner::Expected::ToFail, 0, 2 );
            }
        
            SECTION(    "selftest/expected result/succeeding tests", 
                        "Tests in the 'succeeding' branch succeed" ) {
                std::cout << "Tests in the 'succeeding' branch succeed" << std::endl;
                MetaTestRunner::runMatching( "./succeeding/*",  MetaTestRunner::Expected::ToSucceed, 1, 2 );
            }
        }

        ///////////////////////////////////////////////////////////////////////////
        SECTION(    "selftest/test counts", 
                    "Number of test cases that run is fixed" ) {
            EmbeddedRunner runner;
        
            SECTION(    "selftest/test counts/succeeding tests", 
                        "Number of 'succeeding' tests is fixed" ) {
                std::cout << "Number of 'succeeding' tests is fixed" << std::endl;
                Catch::Totals totals = runner.runMatching( "./succeeding/*", 0, 2 );
                CHECK( totals.assertions.passed == 298 );
                CHECK( totals.assertions.failed == 0 );
            }

            SECTION(    "selftest/test counts/failing tests", 
                        "Number of 'failing' tests is fixed" ) {
                std::cout << "Number of 'failing' tests is fixed" << std::endl;
                Catch::Totals totals = runner.runMatching( "./failing/*", 1, 2 );
                CHECK( totals.assertions.passed == 2 );
                CHECK( totals.assertions.failed == 77 );
            }
        }
    }

    TEST_CASE( "Run all failing and succeeding tests", "[sw4][vs]" ) {
        CatchOverrides::ConfigGuard cg;
        Catch::ConfigData cd(cg.value().get());
        cd.name = "Test sw4";
        cd.abortAfter = 1;
        cd.showSuccessfulTests = true;
        cd.warnings            = Catch::WarnAbout::NoAssertions;
        cd.abortAfter          = -1;
        Catch::Ptr<Catch::Config> config(new Catch::Config(cd));
        Catch::MSTestReporter* rep = new Catch::MSTestReporter(config.get());
        Catch::RunContext tr(config.get(), rep);
        std::string names[] = {"one","two","three"};
        std::vector<std::string> stringNames(names, names + (sizeof(names)/sizeof(std::string)));
        std::vector<Catch::TestCase> testCase = Catch::getRegistryHub().getTestCaseRegistry().getMatchingTestCases( "Some simple comparisons between doubles" );
        //std::vector<Catch::TestCase> testCase = Catch::getRegistryHub().getTestCaseRegistry().getMatchingTestCases(name_desc.name);
        if( testCase.empty() ) Assert::Fail(FAIL_STRING("No tests match"));
        if( testCase.size() > 1 ) Assert::Fail(FAIL_STRING("More than one test with the same name"));
        Catch::Totals totals = tr.runTest(*testCase.begin());
        if( totals.assertions.failed > 0 ) {
            INTERNAL_CATCH_TEST_THROW_FAILURE
        }
        /*for(std::vector<Catch::TestCase>::iterator it = tests.begin(); it != tests.end(); ++it )
        {
            Catch::Totals totals;
            std::size_t groupIndex(0);
            std::size_t groupsCount(0);
            {
                EmbeddedRunner runner;
                std::string name = it->getTestCaseInfo().name;
                totals = runner.runMatching( name, groupIndex, groupsCount );
            }
        }*/
    }
#endif

    // mstest /TestContainer:Debug\ManagedTestCatch.dll /category:"all"
    #if defined(INTERNAL_CATCH_VS_MANAGED) || defined(INTERNAL_CATCH_VS_NATIVE)
        CATCH_MAP_CATEGORY_TO_TAG(all, "~[vs]");

        CATCH_CONFIG_SHOW_SUCCESS(true)
        CATCH_CONFIG_WARN_MISSING_ASSERTIONS(true)
        CATCH_MAP_CATEGORY_TO_TAG(allSucceeding, "~[vs]");

        CATCH_CONFIG_SHOW_SUCCESS(true)
        CATCH_CONFIG_WARN_MISSING_ASSERTIONS(true)
        CATCH_CONFIG_ABORT_AFTER(4)
        CATCH_INTERNAL_CONFIG_ADD_TEST("Some simple comparisons between doubles")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Approximate comparisons with different epsilons")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Approximate comparisons with floats")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Approximate comparisons with ints")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Approximate comparisons with mixed numeric types")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Use a custom approx")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Approximate PI")
        CATCH_INTERNAL_CONFIG_ADD_TEST("A METHOD_AS_TEST_CASE based test run that succeeds")
        CATCH_INTERNAL_CONFIG_ADD_TEST("A METHOD_AS_TEST_CASE based test run that fails")
        CATCH_INTERNAL_CONFIG_ADD_TEST("A TEST_CASE_METHOD based test run that succeeds")
        CATCH_INTERNAL_CONFIG_ADD_TEST("A TEST_CASE_METHOD based test run that fails")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Equality checks that should succeed")
        CATCH_INTERNAL_CONFIG_ADD_TEST("Equality checks that should fail]")
        INTERNAL_CATCH_MAP_CATEGORY_TO_LIST(allSucceedingAborting);
    #endif

}
