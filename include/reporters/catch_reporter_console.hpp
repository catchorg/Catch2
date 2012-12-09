/*
 *  Created by Phil on 5/12/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED

#include "../internal/catch_interfaces_reporter.h"
#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_console_colour.hpp"

namespace Catch {

    struct ConsoleReporter : AccumulatingReporter {
        ConsoleReporter( ReporterConfig const& _config )
        : AccumulatingReporter( _config )
        {}

        virtual ~ConsoleReporter();
        static std::string getDescription() {
            return "Reports test results as plain lines of text";
        }
        virtual ReporterPreferences getPreferences() const {
            ReporterPreferences prefs;
            prefs.shouldRedirectStdOut = false;
            return prefs;

        }
        void lazyPrintRunInfo() {
            stream << "[Started testing: " << testRunInfo->name << "]" << std::endl;
            testRunInfo.reset();
        }
        void lazyPrintGroupInfo() {
            if( !unusedGroupInfo->name.empty() )
//                stream << "[Group: '" << unusedGroupInfo->name << "']" << std::endl;
                stream << "[Started group: '" << unusedGroupInfo->name << "']" << std::endl;
            unusedGroupInfo.reset();
        }
        void lazyPrintTestCaseInfo() {
//            stream << "[Test case: '" << unusedTestCaseInfo->name << "']" << std::endl;
            stream << "[Running: " << unusedTestCaseInfo->name << "]" << std::endl;
            unusedTestCaseInfo.reset();
        }
        std::string makeSectionPath( ThreadedSectionInfo const * section, std::string const& delimiter ) {
            std::string sectionPath = "'" + section->name + "'";
            while( ( section = section->parent.get() ) )
                sectionPath = "'" + section->name + "'" + delimiter + sectionPath;
            return sectionPath;
        }
        void lazyPrintSectionInfo() {
            // !TBD use printed flag
            ThreadedSectionInfo* section = unusedSectionInfo.get();

            std::string sectionPath = makeSectionPath( section, ", " );
            if( sectionPath.size() > 60 )
                sectionPath = makeSectionPath( section, ",\n          " );

            stream << "[Section: " << sectionPath << "]" << std::endl;
            unusedSectionInfo.reset();
        }
        
        void lazyPrintSectionInfoLegacy() {
            std::vector<ThreadedSectionInfo*> sections;
            for(    ThreadedSectionInfo* section = unusedSectionInfo.get();
                    section && !section->printed;
                    section = section->parent.get() )
                sections.push_back( section );

            typedef std::vector<ThreadedSectionInfo*>::const_reverse_iterator It;
            for( It it = sections.rbegin(), itEnd = sections.rend(); it != itEnd; ++it ) {
                stream << "[Started section: " << "'" + (*it)->name + "'" << "]" << std::endl;
                (*it)->printed = true;
            }
            unusedSectionInfo.reset();
        }
        void lazyPrint() {
            if( testRunInfo )
                lazyPrintRunInfo();
            if( unusedGroupInfo )
                lazyPrintGroupInfo();
            if( unusedTestCaseInfo )
                lazyPrintTestCaseInfo();
            if( currentSectionInfo && !currentSectionInfo->printed )
                lazyPrintSectionInfoLegacy(); // !TBD
        }

        virtual void assertionStarting( AssertionInfo const& ) {
        }
        virtual void assertionEnded( Ptr<AssertionStats const> const& _assertionStats ) {

            AssertionResult const& result = _assertionStats->assertionResult;
            
            // Drop out if result was successful and we're not printing those
            if( !m_config.includeSuccessfulResults() && result.isOk() )
                return;

            lazyPrint();

            if( !result.getSourceInfo().empty() ) {
                TextColour colour( TextColour::FileName );
                stream << result.getSourceInfo();
            }

            if( result.hasExpression() ) {
                TextColour colour( TextColour::OriginalExpression );
                stream << result.getExpression();
                if( result.succeeded() ) {
                    TextColour successColour( TextColour::Success );
                    stream << " succeeded";
                }
                else {
                    TextColour errorColour( TextColour::Error );
                    stream << " failed";
                    if( result.isOk() ) {
                        TextColour okAnywayColour( TextColour::Success );
                        stream << " - but was ok";
                    }
                }
            }

            switch( result.getResultType() ) {
                case ResultWas::ThrewException:
                {
                    TextColour colour( TextColour::Error );
                    if( result.hasExpression() )
                        stream << " with unexpected";
                    else
                        stream << "Unexpected";
                    stream << " exception with message: '" << result.getMessage() << "'";
                }
                    break;
                case ResultWas::DidntThrowException:
                {
                    TextColour colour( TextColour::Error );
                    if( result.hasExpression() )
                        stream << " because no exception was thrown where one was expected";
                    else
                        stream << "No exception thrown where one was expected";
                }
                    break;
                case ResultWas::Info:
                {
                    TextColour colour( TextColour::ReconstructedExpression );
                    streamVariableLengthText( "info", result.getMessage() );
                }
                    break;
                case ResultWas::Warning:
                {
                    TextColour colour( TextColour::ReconstructedExpression );
                    streamVariableLengthText( "warning", result.getMessage() );
                }
                    break;
                case ResultWas::ExplicitFailure:
                {
                    TextColour colour( TextColour::Error );
                    stream << "failed with message: '" << result.getMessage() << "'";
                }
                    break;
                case ResultWas::Unknown: // These cases are here to prevent compiler warnings
                case ResultWas::Ok:
                case ResultWas::FailureBit:
                case ResultWas::ExpressionFailed:
                case ResultWas::Exception:
                    if( !result.hasExpression() ) {
                        if( result.succeeded() ) {
                            TextColour colour( TextColour::Success );
                            stream << " succeeded";
                        }
                        else {
                            TextColour colour( TextColour::Error );
                            stream << " failed";
                            if( result.isOk() ) {
                                TextColour okAnywayColour( TextColour::Success );
                                stream << " - but was ok";
                            }
                        }
                    }
                    if( result.hasMessage() ) {
                        stream << "\n";
                        TextColour colour( TextColour::ReconstructedExpression );
                        streamVariableLengthText( "with message", result.getMessage() );
                    }
                    break;
            }

            if( result.hasExpandedExpression() ) {
                stream << " for: ";
                if( result.getExpandedExpression().size() > 40 ) {
                    stream << "\n";
                    if( result.getExpandedExpression().size() < 70 )
                        stream << "\t";
                }
                TextColour colour( TextColour::ReconstructedExpression );
                stream << result.getExpandedExpression();
            }
            
            stream << std::endl;
        }
        
        void streamVariableLengthText( std::string const& prefix, std::string const& text ) {
            std::string trimmed = trim( text );
            if( trimmed.find_first_of( "\r\n" ) == std::string::npos ) {
                stream << "[" << prefix << ": " << trimmed << "]";
            }
            else {
                stream  << "\n[" << prefix << "] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" << trimmed
                        << "\n[end of " << prefix << "] <<<<<<<<<<<<<<<<<<<<<<<<\n";
            }
        }
        
        void printAssertionCounts( std::string const& label, Counts const& counts, std::string const& allPrefix = "All " ) {
            if( counts.passed )
                stream << counts.failed << " of " << counts.total() << " " << label << "s failed";
            else
                stream << ( counts.failed > 1 ? allPrefix : "" ) << pluralise( counts.failed, label ) << " failed";
        }

        void printTotals( const Totals& totals, const std::string& allPrefix = "All " ) {
            if( totals.assertions.total() == 0 ) {
                stream << "No tests ran";
            }
            else if( totals.assertions.failed ) {
                TextColour colour( TextColour::ResultError );
                printAssertionCounts( "test case", totals.testCases, allPrefix );
                if( totals.testCases.failed > 0 ) {
                    stream << " (";
                    printAssertionCounts( "assertion", totals.assertions, allPrefix );
                    stream << ")";
                }
            }
            else {
                TextColour colour( TextColour::ResultSuccess );
                stream << allPrefix << "tests passed ("
                    << pluralise( totals.assertions.passed, "assertion" ) << " in "
                    << pluralise( totals.testCases.passed, "test case" ) << ")";
            }
        }

        virtual void sectionEnded( Ptr<SectionStats const> const& _sectionStats ) {
            if( _sectionStats->missingAssertions ) {
                lazyPrint();
                TextColour colour( TextColour::ResultError );
                stream << "\nNo assertions in section, '" << _sectionStats->sectionInfo.name << "'\n" << std::endl;
            }
            if( currentSectionInfo && currentSectionInfo->printed ) {
//                stream << "[Summary for section '" << _sectionStats->sectionInfo.name << "': ";
                stream << "[End of section: '" << _sectionStats->sectionInfo.name << "' ";
                Counts const& assertions = _sectionStats->assertions;
                if( assertions.failed ) {
                    TextColour colour( TextColour::ResultError );
                    printAssertionCounts( "assertion", assertions );
                }
                else {
                    TextColour colour( TextColour::ResultSuccess );
                    stream  << ( assertions.passed > 1 ? "All " : "" )
                            << pluralise( assertions.passed, "assertion" ) << " passed" ;
                }
                stream << "]\n" << std::endl;
            }
            AccumulatingReporter::sectionEnded( _sectionStats );
        }
        virtual void testCaseEnded( Ptr<TestCaseStats const> const& _testCaseStats ) {
            if( _testCaseStats->missingAssertions ) {
                lazyPrint();
                TextColour colour( TextColour::ResultError );
                stream << "\nNo assertions in test case, '" << _testCaseStats->testInfo.name << "'\n" << std::endl;
            }
            if( !unusedTestCaseInfo ) {
//                stream << "[Summary for test case '" << _testCaseStats->testInfo.name << "': ";
                stream << "[Finished: '" << _testCaseStats->testInfo.name << "' ";
                printTotals( _testCaseStats->totals );
                stream << "]\n" << std::endl;
            }
            AccumulatingReporter::testCaseEnded( _testCaseStats );
        }
        virtual void testGroupEnded( Ptr<TestGroupStats const> const& _testGroupStats ) {
            if( !unusedGroupInfo ) {
//                stream << "[Summary for group '" << _testGroupStats->groupInfo.name << "': ";
                stream << "[End of group '" << _testGroupStats->groupInfo.name << "'. ";
                printTotals( _testGroupStats->totals );
                stream << "]\n" << std::endl;
            }
            AccumulatingReporter::testGroupEnded( _testGroupStats );
        }
        virtual void testRunEnded( Ptr<TestRunStats const> const& _testRunStats ) {
            if( !unusedTestCaseInfo ) {
//                stream << "[Summary for '" << _testRunStats->runInfo.name << "': ";
                stream << "[Testing completed. ";
                printTotals( _testRunStats->totals );
                stream << "]\n" << std::endl;
            }
            AccumulatingReporter::testRunEnded( _testRunStats );
        }

    };

    INTERNAL_CATCH_REGISTER_REPORTER( "console", ConsoleReporter )

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_CONSOLE_HPP_INCLUDED
