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

#if defined(INTERNAL_CATCH_VS_MANAGED) || defined(INTERNAL_CATCH_VS_NATIVE)
    CATCH_MAP_CATEGORY_TO_TAG(all, "[vsall]");
#endif
}
