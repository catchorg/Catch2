/*
 *  catch_self_test.hpp
 *  Catch
 *
 *  Created by Phil on 14/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED

#include "catch.hpp"

#include "set"

namespace Catch
{
    class MockReporter : public SharedImpl<IReporter>
    {
    public:
        
        static const std::string recordGroups;
        static const std::string recordTestCases;
        static const std::string recordSections;
        
        void recordAll
        ()
        {
            addRecorder( recordGroups );
            addRecorder( recordTestCases );
            addRecorder( recordSections );
        }
                
        MockReporter
        (
            const IReporterConfig& config
        )
        {
            recordAll();
        }
        
        MockReporter
        ()
        {
            recordAll();
        }
        
        void addRecorder
        (
            const std::string& recorder
        )
        {
            m_recorders.insert( recorder );
        }
        
        static std::string getDescription
        ()
        {
            return "mock reporter";
        }
        
        std::string getLog
        ()
        const
        {
            return m_log.str();
        }
        
    private: // IReporter
        
        virtual bool shouldRedirectStdout
        ()
        const
        {
            return false;
        }
        
        virtual void StartTesting
        ()
        {
        }
        
        virtual void EndTesting
        (   
            const Totals& totals
        )
        {
        }
        
        virtual void StartGroup
        (
            const std::string& groupName
        )
        {
            if( shouldRecord( recordGroups ) )
                m_log << recordGroups << "( " << groupName << " ){ ";
        }
        
        virtual void EndGroup
        (   
            const std::string& groupName, 
            const Totals& totals
        )
        {
            if( shouldRecord( recordGroups ) )
                m_log << " } " << recordGroups << "( " << groupName << " )";
        }
        
        virtual void StartSection
        (   
            const std::string& sectionName, 
            const std::string description 
        )
        {
            if( shouldRecord( recordSections ) )
                m_log << recordSections << "( " << sectionName << " ){ ";
        }
        
        virtual void EndSection
        (   
            const std::string& sectionName, 
            const Counts& assertions 
        )
        {
            if( shouldRecord( recordSections ) )
                m_log << " } " << recordSections << "( " << sectionName << " )";
        }
        
        virtual void StartTestCase
        (   
            const TestCaseInfo& testInfo 
        )
        {
            if( shouldRecord( recordTestCases ) )
                m_log << recordTestCases << "( " << testInfo.getName() << " ){ ";
        }
        
        virtual void EndTestCase
        (   
            const TestCaseInfo& testInfo, 
            const Totals& totals,
            const std::string& stdOut, 
            const std::string& stdErr 
        )
        {
            if( shouldRecord( recordTestCases ) )
                m_log << " } " << recordTestCases << "( " << testInfo.getName() << " )";
        }
        
        virtual void Result
        (   
            const ResultInfo& resultInfo          
        )
        {
            if( resultInfo.getResultType() == ResultWas::Ok )
                return;
            
            
            switch( resultInfo.getResultType() )
            {                    
                case ResultWas::Info:
                    m_log << "Info";
                    break;
                case ResultWas::Warning:
                    m_log << "Warning";
                    break;
                case ResultWas::ExplicitFailure:
                    m_log << "ExplicitFailure";
                    break;
                case ResultWas::ExpressionFailed:
                    m_log << "ExpressionFailed";
                    break;
                case ResultWas::Unknown:
                    m_log << "Unknown";
                    break;
                case ResultWas::ThrewException:
                    m_log << "ThrewException";
                    break;
                case ResultWas::DidntThrowException:
                    m_log << "DidntThrowException";
                    break;

                // We shouldn't ever see these
                case ResultWas::Ok:
                    m_log << "Ok";
                    break;
                case ResultWas::FailureBit:
                    m_log << "FailureBit";
                    break;
                case ResultWas::Exception:
                    m_log << "Exception";
                    break;
                default:
                    m_log << "{unrecognised ResultType enum value}";
                    break;
            }
            
            if( resultInfo.hasExpression() )
                m_log << resultInfo.getExpression();
            
            if( resultInfo.hasMessage() )
                m_log << "'" << resultInfo.getMessage() << "'";
            
            if( resultInfo.hasExpandedExpression() )
                m_log << resultInfo.getExpandedExpression();        
        }
        
    private:
        
        bool shouldRecord( const std::string& recorder ) const
        {
            return m_recorders.find( recorder ) != m_recorders.end();
        }
        std::ostringstream m_log;
        std::set<std::string> m_recorders;
    };    
    
    class EmbeddedRunner
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        EmbeddedRunner
        ()
        : m_reporter( new MockReporter() )
        {
        }
        
        std::size_t runMatching
            (   const std::string& rawTestSpec,
                const std::string& reporter = "basic" );
        
        ///////////////////////////////////////////////////////////////////////////
        std::string getOutput
        ()
        {
            return m_output;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        const Totals& getTotals
        ()
        const
        {
            return m_totals;
        }

        ///////////////////////////////////////////////////////////////////////////
        void addRecorder( const std::string& recorder )
        {
            m_reporter->addRecorder( recorder );
        }

        ///////////////////////////////////////////////////////////////////////////
        std::string getLog() const
        {
            return m_reporter->getLog();
        }
        
    private:
        Totals m_totals;
        std::string m_output;
        Ptr<MockReporter> m_reporter;
    };

    class MetaTestRunner
    {
    public:
        struct Expected
        {
            enum Result
            {
                ToSucceed,
                ToFail
            };
        };
        
        ///////////////////////////////////////////////////////////////////////////
        MetaTestRunner
        (
            Expected::Result expectedResult
        )
        : m_expectedResult( expectedResult )
        {        
        }
        
        ///////////////////////////////////////////////////////////////////////////
        static void runMatching
        (
            const std::string& testSpec, 
            Expected::Result expectedResult
        )
        {
            forEach(    Hub::getTestCaseRegistry().getMatchingTestCases( testSpec ), 
                        MetaTestRunner( expectedResult ) );
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void operator()
        (
            const TestCaseInfo& testCase
        )
        {
            EmbeddedRunner runner;
            runner.runMatching( testCase.getName() );
            Totals totals = runner.getTotals();
            switch( m_expectedResult )
            {
                case Expected::ToSucceed:
                    if( totals.assertions.failed > 0 )
                    {
                        INFO( runner.getOutput() );
                        FAIL( "Expected test case '" 
                             << testCase.getName() 
                             << "' to succeed but there was/ were " 
                             << totals.assertions.failed << " failure(s)" );
                    }
                    break;
                case Expected::ToFail:
                    if( totals.assertions.passed > 0 )
                    {
                        INFO( runner.getOutput() );
                        FAIL( "Expected test case '" 
                             << testCase.getName() 
                             << "' to fail but there was/ were " 
                             << totals.assertions.passed << " success(es)" );
                    }
                    break;
            }        
        };
        
    private:
        Expected::Result m_expectedResult;
    };
    

    struct LineInfoRegistry
    {
        static LineInfoRegistry& get
        ()
        {
            static LineInfoRegistry s_instance;
            return s_instance;
        }
        
        void registerLineInfo
        (
            const std::string& name, 
            const SourceLineInfo& info 
        )
        {
            m_registry.insert( std::make_pair( name, info ) );
        }
        
        const SourceLineInfo* find( const std::string& name ) const
        {
            std::map<std::string, SourceLineInfo>::const_iterator it = m_registry.find( name );
            return it == m_registry.end() ? NULL : &(it->second);
        }

        const std::string infoForName( const std::string& name ) const
        {
            std::map<std::string, SourceLineInfo>::const_iterator it = m_registry.find( name );
            if( it == m_registry.end() )
                return "";
            std::ostringstream oss;
            oss << it->second;
            return oss.str();
        }
        
        std::map<std::string, SourceLineInfo> m_registry;
    };
    
    struct LineInfoRegistrar
    {
        LineInfoRegistrar( const char* name, const SourceLineInfo& lineInfo )
        {
            LineInfoRegistry::get().registerLineInfo( name, lineInfo );
        }
    };
    
}

#define CATCH_REGISTER_LINE_INFO( name ) ::Catch::LineInfoRegistrar INTERNAL_CATCH_UNIQUE_NAME( lineRegistrar )( name, ::Catch::SourceLineInfo( __FILE__, __LINE__ ) );
#define CATCH_GET_LINE_INFO( name ) ::Catch::LineInfoRegistry::get().infoForName( name )

#endif // TWOBLUECUBES_CATCH_SELF_TEST_HPP_INCLUDED
