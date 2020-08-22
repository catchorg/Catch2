#include <catch2/reporters/catch_reporter_streaming_base.hpp>

#include <catch2/internal/catch_console_width.hpp>
#include <catch2/internal/catch_errno_guard.hpp>
#include <catch2/internal/catch_stream.hpp>
#include <catch2/interfaces/catch_interfaces_config.hpp>

#include <cfloat>
#include <cstdio>
#include <ostream>

namespace Catch {

    // Because formatting using c++ streams is stateful, drop down to C is
    // required Alternatively we could use stringstream, but its performance
    // is... not good.
    std::string getFormattedDuration( double duration ) {
        // Max exponent + 1 is required to represent the whole part
        // + 1 for decimal point
        // + 3 for the 3 decimal places
        // + 1 for null terminator
        const std::size_t maxDoubleSize = DBL_MAX_10_EXP + 1 + 1 + 3 + 1;
        char buffer[maxDoubleSize];

        // Save previous errno, to prevent sprintf from overwriting it
        ErrnoGuard guard;
#ifdef _MSC_VER
        sprintf_s( buffer, "%.3f", duration );
#else
        std::sprintf( buffer, "%.3f", duration );
#endif
        return std::string( buffer );
    }

    bool shouldShowDuration( IConfig const& config, double duration ) {
        if ( config.showDurations() == ShowDurations::Always ) {
            return true;
        }
        if ( config.showDurations() == ShowDurations::Never ) {
            return false;
        }
        const double min = config.minDuration();
        return min >= 0 && duration >= min;
    }

    std::string serializeFilters( std::vector<std::string> const& container ) {
        ReusableStringStream oss;
        bool first = true;
        for ( auto&& filter : container ) {
            if ( !first )
                oss << ' ';
            else
                first = false;

            oss << filter;
        }
        return oss.str();
    }

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

    std::ostream& operator<<( std::ostream& out, lineOfChars value ) {
        for ( size_t idx = 0; idx < CATCH_CONFIG_CONSOLE_WIDTH - 1; ++idx ) {
            out.put( value.c );
        }
        return out;
    }

} // end namespace Catch
