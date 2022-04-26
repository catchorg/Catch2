
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_REPORTER_CONSOLE_HPP_INCLUDED
#define CATCH_REPORTER_CONSOLE_HPP_INCLUDED

#include <catch2/internal/catch_dll_public.hpp>
#include <catch2/internal/catch_unique_ptr.hpp>
#include <catch2/reporters/catch_reporter_streaming_base.hpp>

namespace Catch {
    // Fwd decls
    struct SummaryColumn;
    class TablePrinter;

    struct CATCH_DLL_PUBLIC ConsoleReporter final : StreamingReporterBase {
        Detail::unique_ptr<TablePrinter> m_tablePrinter;

        ConsoleReporter(ReporterConfig const& config);
        ~ConsoleReporter() override;
        static std::string getDescription();

        void noMatchingTestCases( StringRef unmatchedSpec ) override;
        void reportInvalidTestSpec( StringRef arg ) override;

        void assertionStarting(AssertionInfo const&) override;

        void assertionEnded(AssertionStats const& _assertionStats) override;

        void sectionStarting(SectionInfo const& _sectionInfo) override;
        void sectionEnded(SectionStats const& _sectionStats) override;

        void benchmarkPreparing( StringRef name ) override;
        void benchmarkStarting(BenchmarkInfo const& info) override;
        void benchmarkEnded(BenchmarkStats<> const& stats) override;
        void benchmarkFailed( StringRef error ) override;

        void testCaseEnded(TestCaseStats const& _testCaseStats) override;
        void testRunEnded(TestRunStats const& _testRunStats) override;
        void testRunStarting(TestRunInfo const& _testRunInfo) override;
    private:

        void lazyPrint();

        void lazyPrintWithoutClosingBenchmarkTable();
        void lazyPrintRunInfo();
        void printTestCaseAndSectionHeader();

        void printClosedHeader(std::string const& _name);
        void printOpenHeader(std::string const& _name);

        // if string has a : in first line will set indent to follow it on
        // subsequent lines
        void printHeaderString(std::string const& _string, std::size_t indent = 0);


        void printTotals(Totals const& totals);
        void printSummaryRow(StringRef label, std::vector<SummaryColumn> const& cols, std::size_t row);

        void printTotalsDivider(Totals const& totals);
        void printSummaryDivider();
        void printTestFilters();

    private:
        bool m_headerPrinted = false;
        bool m_testRunInfoPrinted = false;
    };

} // end namespace Catch

#endif // CATCH_REPORTER_CONSOLE_HPP_INCLUDED
