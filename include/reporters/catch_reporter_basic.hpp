/*
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_BASIC_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_BASIC_HPP_INCLUDED

#include "../internal/catch_capture.hpp"
#include "../internal/catch_interfaces_reporter.h"
#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_console_colour.hpp"

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

#endif // TWOBLUECUBES_CATCH_REPORTER_BASIC_HPP_INCLUDED
