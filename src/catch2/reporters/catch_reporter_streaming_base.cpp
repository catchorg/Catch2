#include <catch2/reporters/catch_reporter_streaming_base.hpp>

namespace Catch {

    TestEventListenerBase::TestEventListenerBase(
        ReporterConfig const& _config ):
        StreamingReporterBase( _config ) {}

    void TestEventListenerBase::assertionStarting( AssertionInfo const& ) {}

    bool TestEventListenerBase::assertionEnded( AssertionStats const& ) {
        return false;
    }

    StreamingReporterBase::~StreamingReporterBase() = default;

    void
    StreamingReporterBase::testRunStarting( TestRunInfo const& _testRunInfo ) {
        currentTestRunInfo = _testRunInfo;
    }

    void
    StreamingReporterBase::testGroupStarting( GroupInfo const& _groupInfo ) {
        currentGroupInfo = _groupInfo;
    }

    void StreamingReporterBase::testGroupEnded( TestGroupStats const& ) {
        currentGroupInfo.reset();
    }

    void StreamingReporterBase::testRunEnded( TestRunStats const& ) {
        currentTestCaseInfo = nullptr;
        currentGroupInfo.reset();
        currentTestRunInfo.reset();
    }

} // end namespace Catch
