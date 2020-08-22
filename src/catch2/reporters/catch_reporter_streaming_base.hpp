#ifndef CATCH_REPORTER_STREAMING_BASE_HPP_INCLUDED
#define CATCH_REPORTER_STREAMING_BASE_HPP_INCLUDED

#include <catch2/interfaces/catch_interfaces_reporter.hpp>

#include <iosfwd>
#include <string>
#include <vector>

namespace Catch {
    // Returns double formatted as %.3f (format expected on output)
    std::string getFormattedDuration( double duration );

    //! Should the reporter show
    bool shouldShowDuration( IConfig const& config, double duration );

    std::string serializeFilters( std::vector<std::string> const& container );

    struct StreamingReporterBase : IStreamingReporter {

        StreamingReporterBase( ReporterConfig const& _config ):
            m_config( _config.fullConfig() ), stream( _config.stream() ) {
        }


        ~StreamingReporterBase() override;

        void noMatchingTestCases(std::string const&) override {}

        void reportInvalidArguments(std::string const&) override {}

        void testRunStarting( TestRunInfo const& _testRunInfo ) override;

        void testGroupStarting( GroupInfo const& _groupInfo ) override;

        void testCaseStarting(TestCaseInfo const& _testInfo) override  {
            currentTestCaseInfo = &_testInfo;
        }
        void sectionStarting(SectionInfo const& _sectionInfo) override {
            m_sectionStack.push_back(_sectionInfo);
        }

        void sectionEnded(SectionStats const& /* _sectionStats */) override {
            m_sectionStack.pop_back();
        }
        void testCaseEnded(TestCaseStats const& /* _testCaseStats */) override {
            currentTestCaseInfo = nullptr;
        }
        void testGroupEnded( TestGroupStats const& ) override;
        void testRunEnded( TestRunStats const& /* _testRunStats */ ) override;

        void skipTest(TestCaseInfo const&) override {
            // Don't do anything with this by default.
            // It can optionally be overridden in the derived class.
        }

        IConfig const* m_config;
        std::ostream& stream;

        LazyStat<TestRunInfo> currentTestRunInfo;
        LazyStat<GroupInfo> currentGroupInfo;
        TestCaseInfo const* currentTestCaseInfo = nullptr;

        std::vector<SectionInfo> m_sectionStack;
    };

    struct lineOfChars {
        char c;
        constexpr lineOfChars(char c):
            c(c)
        {}

        friend std::ostream& operator<< (std::ostream& out, lineOfChars value);
    };

    struct TestEventListenerBase : StreamingReporterBase {
        TestEventListenerBase( ReporterConfig const& _config );

        void assertionStarting(AssertionInfo const&) override;
        bool assertionEnded(AssertionStats const&) override;

        // Event listeners should not use the default listing impl
        void listReporters(std::vector<ReporterDescription> const&, IConfig const&) override {}
        void listTests(std::vector<TestCaseHandle> const&, IConfig const&) override {}
        void listTags(std::vector<TagInfo> const&, IConfig const&) override {}
    };

} // end namespace Catch

#endif // CATCH_REPORTER_STREAMING_BASE_HPP_INCLUDED
