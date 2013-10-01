/*
 *  Created by Phil on 14/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED

#include "catch.hpp"

// Use this external guard here as if we're using the single header version
// this will already be defined
#ifndef TWOBLUECUBES_CATCH_INTERFACES_REGISTRY_HUB_H_INCLUDED
#include "catch_interfaces_registry_hub.h"
#endif

#include "set"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

namespace Catch {

    class NullStreamingReporter : public SharedImpl<IStreamingReporter> {
    public:

        virtual ~NullStreamingReporter();

        static std::string getDescription() {
            return "null reporter";
        }
        
    private: // IStreamingReporter
        
        virtual ReporterPreferences getPreferences() const {
            return ReporterPreferences();
        }

        virtual void noMatchingTestCases( std::string const& ) {}
        virtual void testRunStarting( TestRunInfo const& ) {}
        virtual void testGroupStarting( GroupInfo const& ) {}
        virtual void testCaseStarting( TestCaseInfo const& ) {}
        virtual void sectionStarting( SectionInfo const& ) {}
        virtual void assertionStarting( AssertionInfo const& ) {}
        virtual bool assertionEnded( AssertionStats const& ) { return false; }
        virtual void sectionEnded( SectionStats const& ) {}
        virtual void testCaseEnded( TestCaseStats const& ) {}
        virtual void testGroupEnded( TestGroupStats const& ) {}
        virtual void testRunEnded( TestRunStats const& ) {}
    };

    class EmbeddedRunner {

    public:
        EmbeddedRunner() : m_reporter( new NullStreamingReporter() ) {}
        
        Totals runMatching( const std::string& rawTestSpec,
                            std::size_t groupIndex,
                            std::size_t groupsCount,
                            const std::string& reporter = "console" );
        
    private:
        Ptr<IStreamingReporter> m_reporter;
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
            forEach(    getRegistryHub().getTestCaseRegistry().getMatchingTestCases( testSpec ), 
                        MetaTestRunner( expectedResult, groupIndex, groupsCount ) );
        }
        
        void operator()( const TestCase& testCase ) {
            std::string name;
            Totals totals;
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
    

    struct LineInfoRegistry {

        static LineInfoRegistry& get() {
            static LineInfoRegistry s_instance;
            return s_instance;
        }
        
        void registerLineInfo(  const std::string& name, 
                                const SourceLineInfo& info ) {
            m_registry.insert( std::make_pair( name, info ) );
        }
        
        const SourceLineInfo* find( const std::string& name ) const {
            std::map<std::string, SourceLineInfo>::const_iterator it = m_registry.find( name );
            return it == m_registry.end() ? NULL : &(it->second);
        }

        const std::string infoForName( const std::string& name ) const {
            std::map<std::string, SourceLineInfo>::const_iterator it = m_registry.find( name );
            if( it == m_registry.end() )
                return "";
            std::ostringstream oss;
            oss << it->second;
            return oss.str();
        }
        
        std::map<std::string, SourceLineInfo> m_registry;
    };
    
    struct LineInfoRegistrar {
        LineInfoRegistrar( const char* name, const SourceLineInfo& lineInfo ) {
            LineInfoRegistry::get().registerLineInfo( name, lineInfo );
        }
    };
    
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#define CATCH_REGISTER_LINE_INFO( name ) ::Catch::LineInfoRegistrar INTERNAL_CATCH_UNIQUE_NAME( lineRegistrar )( name, ::Catch::SourceLineInfo( __FILE__, __LINE__ ) );
#define CATCH_GET_LINE_INFO( name ) ::Catch::LineInfoRegistry::get().infoForName( name )

#endif // TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED
