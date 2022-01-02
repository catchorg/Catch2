/*
 *  Created by Phil on 5/12/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_CONSOLE_H_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_CONSOLE_H_INCLUDED

#include "catch_reporter_bases.hpp"

#if defined( _MSC_VER )
#    pragma warning( push )
#    pragma warning( \
        disable : 4061 ) // Not all labels are EXPLICITLY handled in switch
                         // Note that 4062 (not all labels are handled
                         // and default is missing) is enabled
#endif

namespace Catch {
    // Fwd decls
    struct SummaryColumn;
    class TablePrinter;

    struct GTestReporter : StreamingReporterBase<GTestReporter> {
        std::unique_ptr<TablePrinter> m_tablePrinter;

        GTestReporter( ReporterConfig const& config );
        ~GTestReporter() override;
        static std::string getDescription();

        void noMatchingTestCases( std::string const& spec ) override;

        void reportInvalidArguments( std::string const& arg ) override;

        void assertionStarting( AssertionInfo const& ) override;

        bool assertionEnded( AssertionStats const& _assertionStats ) override;

        void sectionStarting( SectionInfo const& _sectionInfo ) override;
        void sectionEnded( SectionStats const& _sectionStats ) override;

#if defined( CATCH_CONFIG_ENABLE_BENCHMARKING )
        void benchmarkPreparing( std::string const& name ) override;
        void benchmarkStarting( BenchmarkInfo const& info ) override;
        void benchmarkEnded( BenchmarkStats<> const& stats ) override;
        void benchmarkFailed( std::string const& error ) override;
#endif // CATCH_CONFIG_ENABLE_BENCHMARKING

        void testCaseEnded( TestCaseStats const& _testCaseStats ) override;
        void testGroupEnded( TestGroupStats const& _testGroupStats ) override;
        void testRunEnded( TestRunStats const& _testRunStats ) override;
        void testRunStarting( TestRunInfo const& _testRunInfo ) override;

    private:
        void lazyPrint();

        void lazyPrintWithoutClosingBenchmarkTable();
        void lazyPrintRunInfo();
        void lazyPrintGroupInfo();
        void printTestCaseAndSectionHeader();

        void printClosedHeader( std::string const& _name );
        void printOpenHeader( std::string const& _name );

        // if string has a : in first line will set indent to follow it on
        // subsequent lines
        void printHeaderString( std::string const& _string,
                                std::size_t indent = 0 );

        void printTotals( Totals const& totals );
        void printSummaryRow( std::string const& label,
                              std::vector<SummaryColumn> const& cols,
                              std::size_t row );

        void printTotalsDivider( Totals const& totals );
        void printSummaryDivider();
        void printTestFilters();

    private:
        bool m_headerPrinted = false;
    };

} // end namespace Catch

#if defined( _MSC_VER )
#    pragma warning( pop )
#endif

#endif // TWOBLUECUBES_CATCH_REPORTER_CONSOLE_H_INCLUDED