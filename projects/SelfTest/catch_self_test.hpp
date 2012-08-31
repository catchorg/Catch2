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

namespace Catch {

    class MockReporter : public SharedImpl<IReporter> {
    public:
        
        static const std::string recordGroups;
        static const std::string recordTestCases;
        static const std::string recordSections;
        
        void recordAll() {
            addRecorder( recordGroups );
            addRecorder( recordTestCases );
            addRecorder( recordSections );
        }
                
        MockReporter( const ReporterConfig& ) {
            recordAll();
        }
        
        MockReporter() {
            recordAll();
        }
        
        void addRecorder( const std::string& recorder ) {
            m_recorders.insert( recorder );
        }
        
        static std::string getDescription() {
            return "mock reporter";
        }
        
        std::string getLog() const {
            return m_log.str();
        }
        
    private: // IReporter
        
        virtual bool shouldRedirectStdout() const {
            return false;
        }
        
        virtual void StartTesting() {}
        
        virtual void EndTesting( const Totals& ) {}
        
        virtual void StartGroup( const std::string& groupName ) {
            openLabel( recordGroups, groupName );
        }
        
        virtual void EndGroup( const std::string& groupName, const Totals& ) {
            closeLabel( recordGroups, groupName );
        }
        
        virtual void StartSection( const std::string& sectionName, const std::string& ) {
            openLabel( recordSections, sectionName );
        }

        virtual void NoAssertionsInSection( const std::string& ) {}
        virtual void NoAssertionsInTestCase( const std::string& ) {}

        virtual void EndSection( const std::string& sectionName, const Counts& ) {
            closeLabel( recordSections, sectionName );
        }
        
        virtual void StartTestCase( const TestCaseInfo& testInfo ) {
            openLabel( recordTestCases, testInfo.getName()  );
        }
        
        virtual void Aborted(){}
        
        virtual void EndTestCase(   const TestCaseInfo& testInfo, 
                                    const Totals&,
                                    const std::string&, 
                                    const std::string& ) {
            closeLabel( recordTestCases, testInfo.getName()  );
        }
        
        virtual void Result( const ResultInfo& resultInfo );

        
    private:
        
        bool shouldRecord( const std::string& recorder ) const {
            return m_recorders.find( recorder ) != m_recorders.end();
        }
        
        void openLabel( const std::string& label, const std::string& arg = "" );
        void closeLabel( const std::string& label, const std::string& arg = "" );
        
        std::string m_indent;
        std::ostringstream m_log;
        std::set<std::string> m_recorders;
    };
    
    class EmbeddedRunner {

    public:
        EmbeddedRunner() : m_reporter( new MockReporter() ) {}
        
        Totals runMatching( const std::string& rawTestSpec,
                            const std::string& reporter = "basic" );
        
        std::string getOutput() {
            return m_output;
        }

        void addRecorder( const std::string& recorder ) {
            m_reporter->addRecorder( recorder );
        }

        std::string getLog() const {
            return m_reporter->getLog();
        }

    private:
        std::string m_output;
        Ptr<MockReporter> m_reporter;
    };

    class MetaTestRunner {

    public:
        struct Expected { enum Result {
            ToSucceed,
            ToFail
        }; };
        
        MetaTestRunner( Expected::Result expectedResult ) : m_expectedResult( expectedResult ) {}
        
        static void runMatching(    const std::string& testSpec, 
                                    Expected::Result expectedResult ) {
            forEach(    getRegistryHub().getTestCaseRegistry().getMatchingTestCases( testSpec ), 
                        MetaTestRunner( expectedResult ) );
        }
        
        void operator()( const TestCaseInfo& testCase ) {
            EmbeddedRunner runner;
            Totals totals = runner.runMatching( testCase.getName() );
            switch( m_expectedResult ) {
                case Expected::ToSucceed:
                    if( totals.assertions.failed > 0 ) {
                        INFO( runner.getOutput() );
                        FAIL( "Expected test case '" 
                             << testCase.getName() 
                             << "' to succeed but there was/ were " 
                             << totals.assertions.failed << " failure(s)" );
                    }
                    else {
                        SUCCEED( "Tests passed, as expected" );
                    }
                    break;
                case Expected::ToFail:
                    if( totals.assertions.passed > 0 ) {
                        INFO( runner.getOutput() );
                        FAIL( "Expected test case '" 
                             << testCase.getName() 
                             << "' to fail but there was/ were " 
                             << totals.assertions.passed << " success(es)" );
                    }
                    else {
                        SUCCEED( "Tests failed, as expected" );
                    }
                    break;
            }        
        };
        
    private:
        Expected::Result m_expectedResult;
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

#define CATCH_REGISTER_LINE_INFO( name ) ::Catch::LineInfoRegistrar INTERNAL_CATCH_UNIQUE_NAME( lineRegistrar )( name, ::Catch::SourceLineInfo( __FILE__, __LINE__ ) );
#define CATCH_GET_LINE_INFO( name ) ::Catch::LineInfoRegistry::get().infoForName( name )

#endif // TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED
