
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

//  Catch3 v0.3.0.2, on the Catch2 v3.16.1 engine
//  ----------------------------------------------------------
//  This file is an amalgamation of multiple different files.
//  You probably shouldn't edit it directly.
//
//  The parts that come from src/Progmasoft, the Catch3 runner, keep
//  their own copyright and license lines (MPL-2.0 WITH
//  AdditionRef-Progmasoft-Exception-2.0). The runner needs C++17.
//  ----------------------------------------------------------

#include "catch_amalgamated.hpp"




#if defined(CATCH_PLATFORM_WINDOWS)

// We might end up with the define made globally through the compiler,
// and we don't want to trigger warnings for this
#if !defined(NOMINMAX)
#  define NOMINMAX
#endif
#if !defined(WIN32_LEAN_AND_MEAN)
#  define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#endif // defined(CATCH_PLATFORM_WINDOWS)

// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <cassert>
#include <optional>
#include <string>
#include <utility>
#include <variant>

namespace Progmasoft::Catch3::Runner
{
    /// A failure with a message written for the person who has to act on it.
    class Error final
    {
    public:
        explicit Error(std::string message)
            : message_{std::move(message)}
        {
        }

        [[nodiscard]] const std::string &
        Message() const noexcept
        {
            return message_;
        }

        /// The same failure, with what was being done in front of it.
        [[nodiscard]] Error
        Context(const std::string &context) const
        {
            return Error{context + ": " + message_};
        }

    private:
        std::string message_;
    };

    /// A value, or the error that prevented it.
    template<typename Value>
    class [[nodiscard]] Expected final
    {
    public:
        Expected(Value value)
            : state_{std::in_place_index<0>, std::move(value)}
        {
        }

        Expected(Error error)
            : state_{std::in_place_index<1>, std::move(error)}
        {
        }

        [[nodiscard]] explicit
        operator bool() const noexcept
        {
            return state_.index() == 0;
        }

        [[nodiscard]] Value &
        operator*() &
        {
            assert(*this);
            return std::get<0>(state_);
        }

        [[nodiscard]] const Value &
        operator*() const &
        {
            assert(*this);
            return std::get<0>(state_);
        }

        [[nodiscard]] Value &&
        operator*() &&
        {
            assert(*this);
            return std::get<0>(std::move(state_));
        }

        [[nodiscard]] Value *
        operator->()
        {
            assert(*this);
            return &std::get<0>(state_);
        }

        [[nodiscard]] const Value *
        operator->() const
        {
            assert(*this);
            return &std::get<0>(state_);
        }

        [[nodiscard]] const Error &
        GetError() const
        {
            assert(!*this);
            return std::get<1>(state_);
        }

    private:
        std::variant<Value, Error> state_;
    };

    /// Success, or the error that happened instead.
    class [[nodiscard]] Status final
    {
    public:
        Status() = default;

        Status(Error error)
            : error_{std::move(error)}
        {
        }

        [[nodiscard]] static Status
        Ok()
        {
            return Status{};
        }

        [[nodiscard]] explicit
        operator bool() const noexcept
        {
            return !error_.has_value();
        }

        [[nodiscard]] const Error &
        GetError() const
        {
            assert(error_.has_value());
            return *error_;
        }

    private:
        std::optional<Error> error_;
    };
} // namespace Progmasoft::Catch3::Runner

#include <filesystem>
#include <string>
#include <vector>

namespace Progmasoft::Catch3::Runner
{
    /// What Git says about the checkout the tests come from.
    struct Changes final
    {
        std::filesystem::path root;
        std::string commit;
        /// Changed since the revision, staged or not, and untracked files;
        /// absolute paths.
        std::vector<std::filesystem::path> files;
    };

    /// The changes since `revision` (HEAD when empty) in the checkout that
    /// holds `directory`. An error when Git or the checkout is missing.
    [[nodiscard]] Expected<Changes>
    ReadChanges(const std::filesystem::path &directory,
                const std::string &revision);

    /// The commit checked out in the checkout that holds `directory`, or
    /// empty.
    [[nodiscard]] std::string
    CurrentCommit(const std::filesystem::path &directory);

    /// Whether two spellings name the same source file: equal after
    /// normalisation, or one a relative path the other ends with.
    [[nodiscard]] bool
    SameSourceFile(const std::filesystem::path &left,
                   const std::filesystem::path &right);
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Progmasoft::Catch3
{
    /// How one attempt at a test ended.
    enum class Outcome : std::uint8_t
    {
        kPassed,
        /// An assertion failed, or the test threw.
        kFailed,
        /// It failed, but `[!shouldfail]` or `[!mayfail]` allows that.
        kFailedAsExpected,
        /// `SKIP` was called.
        kSkipped,
        /// The process running it died: a signal, a structured exception or
        /// an exit in the middle of the test.
        kCrashed,
        /// It ran past its time limit and was stopped.
        kTimedOut,
        /// A test it depends on did not pass, so it was not run.
        kBlocked,
        /// The run stopped before reaching it.
        kNotRun,
        /// It passed last time and nothing it depends on changed, so the
        /// incremental run kept that result.
        kUpToDate,
    };

    /// Why a failure happened, as far as the runner can tell.
    enum class FailureCategory : std::uint8_t
    {
        kNone,
        /// A REQUIRE or CHECK expression was false.
        kAssertion,
        /// An exception escaped the test or an assertion.
        kUnexpectedException,
        /// REQUIRE_THROWS and its kind did not see the expected exception.
        kMissingException,
        /// FAIL or FAIL_CHECK.
        kExplicitFailure,
        /// A signal or structured exception inside the test.
        kCrash,
        kTimeout,
        /// The worker could not run the test at all, for example because it
        /// failed to start.
        kInfrastructure,
        /// A test it depends on failed.
        kDependency,
    };

    /// How a failure relates to earlier runs.
    enum class FailureHistory : std::uint8_t
    {
        /// There is no history to compare with.
        kUnknown,
        /// The test passed in the previous run.
        kNew,
        /// The test failed in the previous run too.
        kPersistent,
        /// The test passes and fails with the same inputs.
        kIntermittent,
    };

    /// One failed or reported assertion.
    struct AssertionRecord final
    {
        std::string file;
        std::size_t line = 0;
        /// The macro, such as REQUIRE.
        std::string macro;
        /// The expression as written.
        std::string expression;
        /// The expression with its operands' values.
        std::string expansion;
        std::string message;
        /// INFO, CAPTURE and UNSCOPED_INFO messages in force.
        std::vector<std::string> context;
        /// The sections the assertion was in, outermost first.
        std::vector<std::string> sections;
        FailureCategory category = FailureCategory::kAssertion;
        bool passed = false;
        /// The failure is allowed by `[!mayfail]` or `[!shouldfail]`.
        bool allowed = false;
    };

    /// The statistics of one BENCHMARK.
    struct BenchmarkRecord final
    {
        std::string name;
        std::size_t samples = 0;
        std::size_t iterations = 0;
        double meanNanoseconds = 0.0;
        double lowMeanNanoseconds = 0.0;
        double highMeanNanoseconds = 0.0;
        double standardDeviationNanoseconds = 0.0;
        double outlierVariance = 0.0;
    };

    /// One section entered while a test ran, with its timing.
    struct SectionRecord final
    {
        std::string name;
        std::string file;
        std::size_t line = 0;
        /// Microseconds since the start of the attempt.
        std::int64_t startMicroseconds = 0;
        /// -1 for a section that never ended: the attempt crashed or timed
        /// out inside it.
        std::int64_t durationMicroseconds = 0;
        /// 0 for a section directly in the test case.
        std::size_t depth = 0;
    };

    /// One run of a test in a worker. A test may be tried several times:
    /// retries, repeats and the probes that look for an order dependency.
    struct Attempt final
    {
        Outcome outcome = Outcome::kNotRun;
        FailureCategory category = FailureCategory::kNone;
        std::size_t number = 1;
        /// The worker that ran it, and the tests it ran before, in order.
        std::size_t worker = 0;
        std::vector<std::string> precededBy;
        bool freshProcess = false;
        std::uint32_t seed = 0;
        std::int64_t startMicroseconds = 0;
        std::int64_t durationMicroseconds = 0;
        std::size_t assertionsPassed = 0;
        std::size_t assertionsFailed = 0;
        std::vector<AssertionRecord> assertions;
        std::vector<BenchmarkRecord> benchmarks;
        std::vector<SectionRecord> sections;
        /// What the test wrote to std::cout and std::cerr, captured.
        std::string standardOutput;
        std::string standardError;
        /// What reached the worker's own streams while the test ran, such as
        /// printf output or a sanitizer report.
        std::string processOutput;
        /// The fatal condition the worker reported before it died.
        std::string fatalMessage;
        /// How the worker ended, for a crash.
        std::string exitDescription;
        /// The last assertion started before a crash or a timeout.
        std::string lastLocation;
        /// The directory the test had for its temporary files; kept when the
        /// attempt failed, so its contents can be inspected.
        std::string temporaryDirectory;
    };

    /// One test of the run and everything the runner learned about it.
    struct TestResult final
    {
        /// Identifies the test across runs and builds: its name, with its
        /// class and, when names repeat, its tags. History is kept by it.
        std::string id;
        std::string name;
        std::string className;
        std::string file;
        std::size_t line = 0;
        std::vector<std::string> tags;
        /// The verdict over all attempts.
        Outcome outcome = Outcome::kNotRun;
        FailureCategory category = FailureCategory::kNone;
        FailureHistory history = FailureHistory::kUnknown;
        /// It failed and then passed with the same inputs.
        bool flaky = false;
        /// It is in quarantine, so its failure does not fail the run.
        bool quarantined = false;
        /// Why it was selected, when not every test runs: "failed last
        /// run", "covers a changed file", "dependency of ...".
        std::string selectionReason;
        /// A stable identifier of the failure: its category, location and
        /// message with numbers removed. Tests that share it probably share a
        /// cause.
        std::string signature;
        /// What the runner found out about the failure, one finding per
        /// entry, most useful first.
        std::vector<std::string> analysis;
        /// A command line that runs the failure again the way it happened.
        std::string replayCommand;
        std::vector<Attempt> attempts;
        std::int64_t predictedDurationMicroseconds = 0;
        double predictedFailureProbability = 0.0;

        /// The sum of the attempts' durations.
        [[nodiscard]] std::int64_t
        DurationMicroseconds() const noexcept;

        /// Whether the result fails the run.
        [[nodiscard]] bool
        FailsRun() const noexcept;
    };

    /// The totals of a run, by verdict.
    struct RunSummary final
    {
        std::size_t tests = 0;
        std::size_t passed = 0;
        std::size_t failed = 0;
        std::size_t failedAsExpected = 0;
        std::size_t skipped = 0;
        std::size_t crashed = 0;
        std::size_t timedOut = 0;
        std::size_t blocked = 0;
        std::size_t notRun = 0;
        std::size_t upToDate = 0;
        std::size_t flaky = 0;
        std::size_t quarantined = 0;
        std::size_t attempts = 0;
        std::size_t assertionsPassed = 0;
        std::size_t assertionsFailed = 0;
        /// Wall-clock time of the run, and the time its tests took added up;
        /// their ratio is the speed-up of running in parallel.
        std::int64_t wallMicroseconds = 0;
        std::int64_t testMicroseconds = 0;
    };

    /// A whole run of one test binary.
    struct RunResult final
    {
        std::string binary;
        std::uint32_t seed = 0;
        std::size_t workers = 0;
        /// The shard, when the run was one of several.
        unsigned shardIndex = 0;
        unsigned shardCount = 1;
        /// The tests that matched before selection narrowed them.
        std::size_t candidates = 0;
        std::vector<TestResult> tests;
        /// Test specs that matched nothing.
        std::vector<std::string> unmatchedSpecs;
        /// Messages about the run itself, such as a history that could not
        /// be read.
        std::vector<std::string> notes;
        std::int64_t startUnixMilliseconds = 0;
        std::int64_t wallMicroseconds = 0;
    };

    [[nodiscard]] RunSummary
    Summarize(const RunResult &run);

    /// The name an outcome has in reports: "passed", "timed-out", ...
    [[nodiscard]] const char *
    OutcomeName(Outcome outcome) noexcept;

    [[nodiscard]] std::optional<Outcome>
    OutcomeFromName(const std::string &name) noexcept;

    [[nodiscard]] const char *
    CategoryName(FailureCategory category) noexcept;

    [[nodiscard]] std::optional<FailureCategory>
    CategoryFromName(const std::string &name) noexcept;

    [[nodiscard]] const char *
    HistoryName(FailureHistory history) noexcept;

    /// The failure of an attempt as text: each failed assertion with its
    /// expansion, messages and sections, or what is known about a crash or a
    /// timeout. Empty for an attempt that did not fail.
    [[nodiscard]] std::string
    DescribeFailure(const Attempt &attempt);

    /// The first line of `DescribeFailure`, for a summary.
    [[nodiscard]] std::string
    SummarizeFailure(const Attempt &attempt);

    /// Whether an outcome counts as a failure of the attempt.
    [[nodiscard]] bool
    IsFailure(Outcome outcome) noexcept;
} // namespace Progmasoft::Catch3
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0


#include <cstdint>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Progmasoft::Catch3::Runner
{
    class JsonValue;

    /// The members of an object, in the order they were added or read.
    using JsonMembers = std::vector<std::pair<std::string, JsonValue>>;

    /// One JSON value: the messages between the runner and its workers, the
    /// reports, the history and the coverage exports of LLVM are all JSON.
    ///
    /// Numbers are kept as `double`, which represents every integer the
    /// runner writes (counts, microseconds, 32-bit seeds) exactly.
    class JsonValue final
    {
    public:
        enum class Kind : std::uint8_t
        {
            kNull,
            kBoolean,
            kNumber,
            kString,
            kArray,
            kObject,
        };

        JsonValue() = default;
        JsonValue(std::nullptr_t) {}
        JsonValue(bool value);
        JsonValue(double value);
        JsonValue(int value);
        JsonValue(unsigned value);
        JsonValue(long value);
        JsonValue(unsigned long value);
        JsonValue(long long value);
        JsonValue(unsigned long long value);
        JsonValue(const char *value);
        JsonValue(std::string value);
        JsonValue(std::string_view value);

        JsonValue(const JsonValue &other);
        JsonValue(JsonValue &&other) noexcept;
        JsonValue &
        operator=(const JsonValue &other);
        JsonValue &
        operator=(JsonValue &&other) noexcept;
        ~JsonValue();

        [[nodiscard]] static JsonValue
        Array(std::vector<JsonValue> items = {});

        [[nodiscard]] static JsonValue
        Object(JsonMembers members = {});

        [[nodiscard]] Kind
        GetKind() const noexcept
        {
            return kind_;
        }

        [[nodiscard]] bool
        IsNull() const noexcept
        {
            return kind_ == Kind::kNull;
        }

        [[nodiscard]] bool
        IsObject() const noexcept
        {
            return kind_ == Kind::kObject;
        }

        [[nodiscard]] bool
        IsArray() const noexcept
        {
            return kind_ == Kind::kArray;
        }

        [[nodiscard]] bool
        IsString() const noexcept
        {
            return kind_ == Kind::kString;
        }

        [[nodiscard]] bool
        IsNumber() const noexcept
        {
            return kind_ == Kind::kNumber;
        }

        /// The value as a Boolean; anything else reads as `fallback`.
        [[nodiscard]] bool
        AsBoolean(bool fallback = false) const noexcept;

        [[nodiscard]] double
        AsNumber(double fallback = 0.0) const noexcept;

        [[nodiscard]] std::int64_t
        AsInteger(std::int64_t fallback = 0) const noexcept;

        [[nodiscard]] std::uint64_t
        AsUnsigned(std::uint64_t fallback = 0) const noexcept;

        /// The text of a string; anything else reads as the empty string.
        [[nodiscard]] const std::string &
        AsString() const noexcept;

        /// The items of an array; anything else has none.
        [[nodiscard]] const std::vector<JsonValue> &
        Items() const noexcept;

        [[nodiscard]] std::vector<JsonValue> &
        MutableItems();

        /// The members of an object; anything else has none.
        [[nodiscard]] const JsonMembers &
        Members() const noexcept;

        [[nodiscard]] JsonMembers &
        MutableMembers();

        /// The member named `key`, or null when there is none.
        [[nodiscard]] const JsonValue &
        operator[](std::string_view key) const noexcept;

        /// The member named `key` of an object, added when missing.
        JsonValue &
        Set(std::string_view key, JsonValue value);

        /// Appends to an array.
        void
        Append(JsonValue value);

        /// The compact JSON text of the value, on one line.
        [[nodiscard]] std::string
        ToString() const;

        /// The JSON text with two spaces of indentation per level.
        [[nodiscard]] std::string
        ToPrettyString() const;

    private:
        void
        Destroy() noexcept;

        void
        CopyFrom(const JsonValue &other);

        void
        MoveFrom(JsonValue &&other) noexcept;

        Kind kind_ = Kind::kNull;
        bool boolean_ = false;
        double number_ = 0.0;
        std::string string_;
        std::unique_ptr<std::vector<JsonValue>> items_;
        std::unique_ptr<JsonMembers> members_;
    };

    /// Reads one JSON document; whitespace may surround it.
    [[nodiscard]] Expected<JsonValue>
    ParseJson(std::string_view text);

    /// Appends `text` to `output` as a JSON string literal.
    void
    AppendJsonString(std::string &output, std::string_view text);
} // namespace Progmasoft::Catch3::Runner

namespace Progmasoft::Catch3::Runner
{
    /// One result of one test in one run.
    struct TestRecord final
    {
        std::int64_t run = 0;
        Outcome outcome = Outcome::kNotRun;
        FailureCategory category = FailureCategory::kNone;
        std::int64_t durationMicroseconds = 0;
        std::size_t attempts = 0;
        bool flaky = false;
        std::string signature;
        /// What the test's result depended on; equal fingerprints mean the
        /// same inputs.
        std::string fingerprint;
    };

    /// How to run a failure again the way it happened.
    struct ReplayRecord final
    {
        std::int64_t run = 0;
        std::uint32_t seed = 0;
        /// The tests the worker ran before, in order.
        std::vector<std::string> precededBy;
        bool freshProcess = false;
    };

    struct QuarantineRecord final
    {
        std::int64_t since = 0;
        std::string reason;
    };

    /// Everything remembered about one test.
    struct TestHistory final
    {
        /// The most recent last.
        std::vector<TestRecord> records;
        std::optional<QuarantineRecord> quarantine;
        std::optional<ReplayRecord> lastFailure;

        [[nodiscard]] const TestRecord *
        Last() const noexcept;

        /// A percentile of the durations of recent runs that finished on
        /// their own; nothing without such runs.
        [[nodiscard]] std::optional<std::int64_t>
        DurationPercentile(double fraction) const;

        /// The probability that the next run fails: a failure rate in which
        /// each older run counts less, starting from a prior for tests with
        /// little history.
        [[nodiscard]] double
        FailureProbability() const;

        /// Flaky results, and failures that alternate with passes on the
        /// same fingerprint, among the last `window` runs.
        [[nodiscard]] std::size_t
        FlakyEvents(std::size_t window) const;

        /// Passes in a row, counting back from the latest run.
        [[nodiscard]] std::size_t
        PassStreak() const;
    };

    /// One run, as the history remembers it.
    struct RunRecord final
    {
        std::int64_t id = 0;
        std::int64_t unixMilliseconds = 0;
        std::string commit;
        std::string binaryFingerprint;
        std::uint32_t seed = 0;
        std::size_t tests = 0;
        std::size_t failed = 0;
        std::int64_t wallMicroseconds = 0;
    };

    /// When to put a test into quarantine and let it out.
    struct QuarantinePolicy final
    {
        std::size_t window = 20;
        std::size_t flakyEventsToEnter = 2;
        std::size_t passesToLeave = 10;
    };

    /// The results of earlier runs of one test binary.
    ///
    /// It lives in one JSON file. Several runs of the same binary may finish
    /// at the same time, so `Commit` changes the file under a lock: it reads
    /// it again, adds its run and replaces it in one rename.
    class History final
    {
    public:
        History() = default;

        /// Reads the file; a missing file is an empty history.
        [[nodiscard]] static Expected<History>
        Load(const std::filesystem::path &file);

        /// Adds `run` to the history in `file`, updates quarantine and
        /// returns the history as written.
        [[nodiscard]] static Expected<History>
        Commit(const std::filesystem::path &file,
               const RunResult &run,
               const RunRecord &record,
               const std::map<std::string, std::string> &fingerprints,
               const QuarantinePolicy &policy,
               std::chrono::milliseconds lockWait = std::chrono::seconds(10));

        [[nodiscard]] const TestHistory *
        Find(const std::string &key) const;

        [[nodiscard]] const std::map<std::string, TestHistory> &
        Tests() const noexcept
        {
            return tests_;
        }

        [[nodiscard]] const std::vector<RunRecord> &
        Runs() const noexcept
        {
            return runs_;
        }

        [[nodiscard]] JsonValue
        ToJson() const;

        [[nodiscard]] static Expected<History>
        FromJson(const JsonValue &value);

        /// The key a test result is filed under.
        [[nodiscard]] static std::string
        KeyOf(const TestResult &test);

    private:
        void
        Apply(const RunResult &run,
              RunRecord record,
              const std::map<std::string, std::string> &fingerprints,
              const QuarantinePolicy &policy);

        std::int64_t nextRun_ = 1;
        std::vector<RunRecord> runs_;
        std::map<std::string, TestHistory> tests_;
    };

    /// The file the history of a binary is kept in under `directory`.
    [[nodiscard]] std::filesystem::path
    HistoryFile(const std::filesystem::path &directory,
                const std::string &binaryName);
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>


namespace Catch
{
    class IConfig;
} // namespace Catch

namespace Progmasoft::Catch3::Runner
{
    /// One registered test and what its tags tell the runner.
    struct TestEntry final
    {
        /// The position in registration order, the same in every process
        /// started from one binary.
        std::size_t index = 0;
        /// A name that identifies the test across runs and builds: the test
        /// name, with its class when it has one, and its tags when another
        /// test has the same name.
        std::string key;
        std::string name;
        std::string className;
        std::string file;
        std::size_t line = 0;
        std::vector<std::string> tags;
        bool hidden = false;
        bool benchmark = false;
        bool mayFail = false;
        bool shouldFail = false;

        // From the tags the runner reserves.
        /// `[!serial]`: nothing else runs at the same time.
        bool serial = false;
        /// `[!isolated]`: runs in a process of its own.
        bool isolated = false;
        /// `[!quarantine]`: in quarantine whatever its history says.
        bool quarantined = false;
        /// `[!resource:NAME]`: tests sharing a resource never overlap.
        std::vector<std::string> resources;
        /// `[!depends:NAME]`: runs after these tests, and only if they pass.
        std::vector<std::string> dependsOn;
        /// `[!timeout:SECONDS]`.
        std::optional<std::chrono::milliseconds> timeout;
        /// `[!retries:N]`.
        std::optional<unsigned> retries;
    };

    /// The tests registered in this binary.
    class Catalog final
    {
    public:
        /// Reads the test registry. Problems with a reserved tag, such as a
        /// timeout that is not a number, are errors.
        [[nodiscard]] static Expected<Catalog>
        Load();

        /// A catalog of the given tests, without a registry behind it: the
        /// tests cannot run, but they can be planned. Each entry's index
        /// becomes its position, and an empty key becomes its name.
        [[nodiscard]] static Catalog
        FromEntries(std::vector<TestEntry> entries);

        [[nodiscard]] const std::vector<TestEntry> &
        Tests() const noexcept
        {
            return tests_;
        }

        [[nodiscard]] const std::vector<Catch::TestCaseHandle> &
        Handles() const noexcept
        {
            return handles_;
        }

        /// The test of a key or of a name; null when there is none.
        [[nodiscard]] const TestEntry *
        Find(const std::string &keyOrName) const;

        /// A digest of every key in order. A worker reports it, so the runner
        /// knows the worker sees the same tests.
        [[nodiscard]] const std::string &
        Identity() const noexcept
        {
            return identity_;
        }

        /// The tests the specs of `config` select, in registration order, as
        /// Catch2 selects them: hidden tests only when a spec names them.
        /// Specs that matched nothing are added to `unmatched`.
        [[nodiscard]] std::vector<std::size_t>
        Match(const Catch::IConfig &config,
              std::vector<std::string> &unmatched) const;

    private:
        std::vector<TestEntry> tests_;
        std::vector<Catch::TestCaseHandle> handles_;
        std::string identity_;
    };
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0


#include <chrono>
#include <cstdint>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Progmasoft::Catch3::Runner
{
    /// What the command line asks for.
    enum class Mode : std::uint8_t
    {
        kRun,
        kListTests,
        kListTags,
        kAnalytics,
        /// Read the reports of other test frameworks.
        kImport,
        kHelp,
        kVersion,
        /// Internal: run tests on behalf of a runner, over the protocol.
        kWorker,
    };

    enum class ListFormat : std::uint8_t
    {
        kText,
        kNames,
        kJson,
    };

    /// Which of the matching tests to run.
    enum class Selection : std::uint8_t
    {
        /// Every matching test.
        kAll,
        /// Those that failed, crashed, timed out or were flaky last time.
        kFailed,
        /// Those defined in, or covering, files changed since a revision.
        kChanged,
        /// Those without a recorded result.
        kNew,
        /// Changed, failed and new tests, and those most likely to fail.
        kSmart,
    };

    enum class Order : std::uint8_t
    {
        /// Most likely to fail per second of running time first.
        kPriority,
        kDeclared,
        kName,
        kRandom,
    };

    enum class RetryMode : std::uint8_t
    {
        kOff,
        /// Retry only what may pass on a second try: crashes, timeouts,
        /// tests with a flaky history and failures that do not repeat.
        kAuto,
        /// Retry every failure up to the limit.
        kAlways,
    };

    enum class QuarantineMode : std::uint8_t
    {
        /// Tests that keep flaking go into quarantine on their own; a failure
        /// in quarantine is reported but does not fail the run.
        kAuto,
        /// Quarantine is reported, but failures still fail the run.
        kReport,
        kOff,
    };

    /// The reports `--import` reads.
    enum class ImportFormat : std::uint8_t
    {
        /// Whichever the contents show.
        kAuto,
        /// Catch3's own: `--json` reports and catch3-results JSON Lines.
        kCatch3,
        kJUnit,
        kXUnit,
        kTrx,
        kTap,
        kCtrf,
    };

    enum class ColorMode : std::uint8_t
    {
        kAuto,
        kAlways,
        kNever,
    };

    enum class Verbosity : std::uint8_t
    {
        kQuiet,
        kNormal,
        kVerbose,
    };

    /// One shard of a run: `index` counts from zero.
    struct Shard final
    {
        unsigned index = 0;
        unsigned count = 1;
    };

    /// Everything the command line can say, already checked and typed.
    struct Options final
    {
        Mode mode = Mode::kRun;
        ListFormat listFormat = ListFormat::kText;
        std::string programName;

        // Selecting tests.
        std::vector<std::string> testSpecs;
        std::vector<std::string> sections;
        /// Path filters: "c:SECTION" or "g:INDEX" (a generator value, or *).
        std::vector<std::string> paths;
        Selection selection = Selection::kAll;
        std::string changesSince;
        bool incremental = false;
        Shard shard;
        std::string shardDurations;
        bool filenamesAsTags = false;

        // Running.
        unsigned jobs = 0;
        bool inProcess = false;
        bool isolate = false;
        /// Variables the tests keep although their names look like secrets.
        std::vector<std::string> passedEnvironment;
        /// The per-test limit; zero disables it.
        std::chrono::milliseconds timeout{0};
        bool timeoutGiven = false;
        bool adaptiveTimeout = true;
        RetryMode retryMode = RetryMode::kAuto;
        unsigned retries = 2;
        unsigned repeat = 1;
        unsigned failFast = 0;
        Order order = Order::kPriority;
        std::uint32_t seed = 0;
        bool seedGiven = false;
        bool bisectOrder = true;

        // How tests behave.
        bool includeSuccessful = false;
        bool breakIntoDebugger = false;
        bool noThrow = false;
        bool warnNoAssertions = false;
        bool warnUnmatchedSpecs = false;
        bool warnInfiniteGenerators = false;
        bool allowNoTests = false;
        bool skipBenchmarks = false;
        bool benchmarkNoAnalysis = false;
        unsigned benchmarkSamples = 100;
        unsigned benchmarkResamples = 100000;
        double benchmarkConfidenceInterval = 0.95;
        std::chrono::milliseconds benchmarkWarmupTime{100};

        // History and analysis.
        std::string historyDirectory;
        bool historyEnabled = true;
        QuarantineMode quarantine = QuarantineMode::kAuto;
        bool replayFailures = false;
        bool analyticsJson = false;

        // Importing the reports of other frameworks.
        std::vector<std::string> importFiles;
        ImportFormat importFormat = ImportFormat::kAuto;
        /// The history they go to; "imported" when empty.
        std::string importName;
        /// What was tested: a verdict that changes on one revision counts
        /// as flaky. Without it, only retries in the reports do.
        std::string importRevision;

        // Output.
        Verbosity verbosity = Verbosity::kNormal;
        ColorMode color = ColorMode::kAuto;
        std::string junitPath;
        std::string jsonPath;
        std::string tracePath;
        unsigned slowest = 0;
        /// Created when the run starts and removed when it ends normally, so
        /// a file left behind shows that the run ended early.
        std::string exitGuardPath;

        // Configuration.
        /// The `catch3.json` named by --config; after ParseCommandLine, the
        /// one that was read, if any.
        std::string configPath;
        bool noConfig = false;
    };

    /// Reads a command line on top of `options`. `arguments` does not include
    /// the program.
    [[nodiscard]] Expected<Options>
    ParseOptions(const std::vector<std::string> &arguments,
                 Options options = {});

    /// How many single-character edits turn `left` into `right`, for
    /// suggesting the name that was meant.
    [[nodiscard]] std::size_t
    EditDistance(std::string_view left, std::string_view right);

    /// The help text, for `--help`.
    [[nodiscard]] std::string
    HelpText(const std::string &programName);

    /// The arguments a worker is started with to behave like `options`.
    [[nodiscard]] std::vector<std::string>
    WorkerArguments(const Options &options);
} // namespace Progmasoft::Catch3::Runner

#include <chrono>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace Progmasoft::Catch3::Runner
{
    /// One test the run will start, with everything decided in advance.
    struct PlannedTest final
    {
        const TestEntry *entry = nullptr;
        std::string selectionReason;
        double failureProbability = 0.0;
        std::int64_t predictedMicroseconds = 0;
        /// Zero: no limit.
        std::chrono::milliseconds timeout{0};
        bool timeoutLearned = false;
        unsigned retryLimit = 0;
        bool quarantined = false;
        std::string quarantineReason;
        /// The history shows the test passing and failing with the same
        /// inputs.
        bool flakyHistory = false;
        /// Positions in `Plan::tests` of the tests this one waits for.
        std::vector<std::size_t> dependencies;
        /// What the result depends on; equal for an unchanged test.
        std::string fingerprint;
        /// For --replay: the tests (catalog indices) to run before it in the
        /// same fresh process, and the seed it failed with.
        std::optional<std::vector<std::size_t>> replaySequence;
        std::optional<std::uint32_t> replaySeed;
    };

    /// What a run will do, decided before the first test starts.
    struct Plan final
    {
        /// In the order the scheduler should prefer.
        std::vector<PlannedTest> tests;
        /// Tests whose earlier result stands: up to date or out of the
        /// selection but needed by nothing.
        std::vector<TestResult> kept;
        std::vector<std::string> unmatchedSpecs;
        std::vector<std::string> notes;
        /// Tests that matched the specs, before selection and sharding.
        std::size_t candidates = 0;
        std::map<std::string, std::string> fingerprints;
    };

    /// What the planner reads besides the options.
    struct PlanInputs final
    {
        const Catalog *catalog = nullptr;
        /// The tests the specs matched, in registration order.
        std::vector<std::size_t> matched;
        std::vector<std::string> unmatchedSpecs;
        const History *history = nullptr;
        /// Present when Git answered.
        const Changes *changes = nullptr;
        /// A digest of the test binary.
        std::string binaryFingerprint;
    };

    [[nodiscard]] Expected<Plan>
    MakePlan(const Options &options, const PlanInputs &inputs);

    /// The shard of a group of tests that must stay together, by a hash of
    /// its first key. Every machine computes the same answer.
    [[nodiscard]] unsigned
    StableShard(const std::string &key, unsigned count) noexcept;
} // namespace Progmasoft::Catch3::Runner


#include <string>

namespace Progmasoft::Catch3::Runner
{
    struct AnalysisInputs final
    {
        /// The history before this run.
        const History *history = nullptr;
        const Changes *changes = nullptr;
        const Plan *plan = nullptr;
        /// The binary, for the replay command.
        std::string program;
        std::string changesSince;
    };

    /// Classifies every failure of the run and writes down what the runner
    /// can tell about its cause: a signature shared with other failures,
    /// the failure's history, the change it follows, where a crash or a
    /// hang happened, and a command that replays it.
    void
    Analyze(RunResult &run, const AnalysisInputs &inputs);

    /// The signature of a failed attempt: its category, the location of the
    /// first failure and its message with numbers and addresses replaced.
    [[nodiscard]] std::string
    FailureSignature(const Attempt &attempt);

    /// A test spec that matches exactly this name.
    [[nodiscard]] std::string
    QuoteTestSpec(const std::string &name);
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0



#include <cstddef>
#include <cstdio>
#include <string>

namespace Progmasoft::Catch3::Runner
{
    /// The report on the terminal: progress while the run goes on, then the
    /// failures with their analysis and a summary.
    class ConsoleReport final
    {
    public:
        ConsoleReport(const Options &options, std::FILE *output);

        void
        RunStarting(std::size_t tests,
                    std::size_t workers,
                    std::uint32_t seed,
                    const std::vector<std::string> &notes);

        /// The start of `--import`: results read from `files` reports.
        void
        ImportStarting(std::size_t tests, std::size_t files);

        void
        TestFinished(const TestResult &test);

        void
        Retrying(const TestResult &test, const Attempt &attempt);

        void
        RunEnded(const RunResult &run);

        /// Text in a colour, when colours are on.
        [[nodiscard]] std::string
        Paint(std::string_view text, const char *colour) const;

    private:
        void
        ClearProgress();

        void
        PrintFailure(const TestResult &test);

        const Options &options_;
        std::FILE *output_;
        bool colour_ = false;
        bool terminal_ = false;
        std::size_t total_ = 0;
        std::size_t finished_ = 0;
        std::size_t failed_ = 0;
        bool progressShown_ = false;
    };

    /// Writes a trace of the run in the Trace Event Format that
    /// chrome://tracing and Perfetto open: one track per worker, one slice
    /// per attempt and section.
    [[nodiscard]] std::string
    TraceJson(const RunResult &run);
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <functional>
#include <memory>
#include <string>


namespace Catch
{
    struct ConfigData;
} // namespace Catch

namespace Progmasoft::Catch3::Runner
{
    /// The Catch2 configuration a process that runs tests uses for
    /// `options`.
    [[nodiscard]] Catch::ConfigData
    ToConfigData(const Options &options, const std::string &programName);

    /// Receives the events of the tests a `TestExecutor` runs, as the JSON
    /// messages a worker sends; see `Protocol.hpp`.
    using EventSink = std::function<void(const JsonValue &)>;

    /// Runs tests of this binary in this process with Catch2's engine: the
    /// sections, generators, assertions and listeners are Catch2's.
    ///
    /// One executor lives for the whole process, because Catch2 allows one
    /// run context per process; listeners see one test run, from the first
    /// test to the destruction of the executor.
    class TestExecutor final
    {
    public:
        TestExecutor(const Options &options,
                     const Catalog &catalog,
                     const std::string &programName,
                     EventSink sink);
        TestExecutor(const TestExecutor &) = delete;
        TestExecutor &
        operator=(const TestExecutor &) = delete;
        ~TestExecutor();

        /// Runs one test. `temporaryDirectory` becomes TMPDIR, TMP and TEMP
        /// while it runs; empty leaves them alone.
        void
        Run(std::size_t index,
            std::size_t attempt,
            const std::string &temporaryDirectory);

        /// The executor's Catch2 configuration and run context.
        struct State;

    private:
        std::unique_ptr<State> state_;
    };

    /// Serves a runner over the standard streams until it says to stop.
    [[nodiscard]] int
    RunWorker(const Options &options, const std::string &programName);
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0


#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>

namespace Progmasoft::Catch3::Runner
{
    /// A lock between processes: a file created exclusively beside the one
    /// it protects. A lock file older than `stale` belongs to a process that
    /// died and is broken.
    class FileLock final
    {
    public:
        FileLock(const std::filesystem::path &file,
                 std::chrono::milliseconds wait,
                 std::chrono::milliseconds stale = std::chrono::seconds(30));
        FileLock(const FileLock &) = delete;
        FileLock &
        operator=(const FileLock &) = delete;
        ~FileLock();

        /// Whether the lock was taken before `wait` ran out.
        [[nodiscard]] bool
        Held() const noexcept
        {
            return held_;
        }

    private:
        std::filesystem::path path_;
        bool held_ = false;
    };

    [[nodiscard]] Expected<std::string>
    ReadFile(const std::filesystem::path &file);

    /// Replaces `file` with `contents` in one rename, so that a reader sees
    /// the old or the new file, never half of one. Creates the directory.
    Status
    WriteFileAtomically(const std::filesystem::path &file,
                        std::string_view contents);

    /// Creates an empty file only the user can read and write, truncating a
    /// regular file of that name. A symbolic link or anything else that is
    /// not a regular file is refused, so the path cannot redirect the write.
    Status
    CreatePrivateFile(const std::filesystem::path &file);

    /// A name for a temporary file that no other process or thread uses.
    [[nodiscard]] std::string
    UniqueName();
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0


#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

namespace Progmasoft::Catch3::Runner
{
    /// SHA-256, for fingerprints that must not collide by accident: the
    /// test binary, the source files a test depends on.
    class Sha256 final
    {
    public:
        Sha256();

        void
        Update(const void *data, std::size_t size);

        void
        Update(std::string_view text)
        {
            Update(text.data(), text.size());
        }

        /// The digest as 64 lowercase hexadecimal digits. The object cannot
        /// be updated afterwards.
        [[nodiscard]] std::string
        HexDigest();

    private:
        void
        Compress(const std::uint8_t *block);

        std::array<std::uint32_t, 8> state_{};
        std::array<std::uint8_t, 64> buffer_{};
        std::size_t bufferSize_ = 0;
        std::uint64_t totalSize_ = 0;
    };

    [[nodiscard]] std::string
    Sha256Hex(std::string_view text);

    /// The SHA-256 of a file's contents.
    [[nodiscard]] Expected<std::string>
    Sha256File(const std::filesystem::path &path);

    /// FNV-1a: a fast, stable 64-bit hash for sharding and signatures,
    /// where a collision only costs balance or merges two groups.
    [[nodiscard]] std::uint64_t
    Fnv1a64(std::string_view text) noexcept;

    /// The value as 16 lowercase hexadecimal digits.
    [[nodiscard]] std::string
    ToHex(std::uint64_t value);
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


namespace Progmasoft::Catch3::Runner
{
    /// How a process ended.
    struct ExitStatus final
    {
        enum class Kind : std::uint8_t
        {
            /// It returned from `main` or called `exit` with `code`.
            kExited,
            /// A signal ended it (POSIX); `code` is the signal number.
            kSignaled,
            /// It ended with a structured exception (Windows); `code` is the
            /// exception code, such as 0xC0000005 for an access violation.
            kException,
        };

        Kind kind = Kind::kExited;
        std::int64_t code = 0;

        [[nodiscard]] bool
        Succeeded() const noexcept
        {
            return kind == Kind::kExited && code == 0;
        }

        /// Whether the process died rather than exiting.
        [[nodiscard]] bool
        Crashed() const noexcept
        {
            return kind != Kind::kExited;
        }

        /// A description such as "exit code 3", "signal 11 (SIGSEGV)" or
        /// "exception 0xC0000005 (access violation)".
        [[nodiscard]] std::string
        Describe() const;
    };

    /// The end of a pipe this process reads from.
    class PipeReader final
    {
    public:
        PipeReader() = default;
        explicit PipeReader(std::intptr_t handle) noexcept
            : handle_{ handle }
        {}
        PipeReader(const PipeReader &) = delete;
        PipeReader &
        operator=(const PipeReader &) = delete;
        PipeReader(PipeReader &&other) noexcept;
        PipeReader &
        operator=(PipeReader &&other) noexcept;
        ~PipeReader();

        /// Reads at most `size` bytes. Returns the number read, 0 at the end
        /// of the stream and -1 on an error.
        [[nodiscard]] std::int64_t
        Read(char *buffer, std::size_t size) noexcept;

        void
        Close() noexcept;

        [[nodiscard]] bool
        IsOpen() const noexcept;

    private:
        std::intptr_t handle_ = -1;
    };

    /// The end of a pipe this process writes to.
    class PipeWriter final
    {
    public:
        PipeWriter() = default;
        explicit PipeWriter(std::intptr_t handle) noexcept
            : handle_{ handle }
        {}
        PipeWriter(const PipeWriter &) = delete;
        PipeWriter &
        operator=(const PipeWriter &) = delete;
        PipeWriter(PipeWriter &&other) noexcept;
        PipeWriter &
        operator=(PipeWriter &&other) noexcept;
        ~PipeWriter();

        /// Writes all of `data`, or reports why it could not.
        Status
        Write(std::string_view data) noexcept;

        void
        Close() noexcept;

        [[nodiscard]] bool
        IsOpen() const noexcept;

    private:
        std::intptr_t handle_ = -1;
    };

    /// What to start, and how.
    struct ChildOptions final
    {
        /// The program and its arguments. The program is a path; it is not
        /// looked up on `PATH` unless `searchPath` is set.
        std::vector<std::string> arguments;
        /// Variables added to, or replacing, those of this process.
        std::vector<std::pair<std::string, std::string>> environment;
        /// Variables of this process the child keeps although their names
        /// look like secrets; see `IsSecretVariable`.
        std::vector<std::string> keptEnvironment;
        /// The working directory; empty keeps the current one.
        std::filesystem::path workingDirectory;
        bool searchPath = false;
    };

    /// A child process with its standard streams connected to pipes.
    ///
    /// The child and every process it starts in turn form one group (a job
    /// object on Windows, a process group elsewhere), so `Kill` ends a test
    /// together with the helpers it spawned. Pipes are not inherited by
    /// any other child, so the end of a stream is seen as soon as its
    /// process is gone.
    class ChildProcess final
    {
    public:
        ChildProcess(const ChildProcess &) = delete;
        ChildProcess &
        operator=(const ChildProcess &) = delete;
        ~ChildProcess();

        [[nodiscard]] static Expected<std::unique_ptr<ChildProcess>>
        Start(const ChildOptions &options);

        /// The child's standard input.
        [[nodiscard]] PipeWriter &
        Input() noexcept
        {
            return input_;
        }

        /// The child's standard output.
        [[nodiscard]] PipeReader &
        Output() noexcept
        {
            return output_;
        }

        /// The child's standard error.
        [[nodiscard]] PipeReader &
        Errors() noexcept
        {
            return errors_;
        }

        /// Ends the child and every process it started. Safe to call more
        /// than once and after the child ended.
        void
        Kill() noexcept;

        /// Waits for the child to end.
        [[nodiscard]] ExitStatus
        Wait() noexcept;

        /// Waits at most `timeout`; nothing when the child still runs.
        [[nodiscard]] std::optional<ExitStatus>
        WaitFor(std::chrono::milliseconds timeout) noexcept;

        [[nodiscard]] std::int64_t
        Id() const noexcept
        {
            return id_;
        }

    private:
        ChildProcess() = default;

        PipeWriter input_;
        PipeReader output_;
        PipeReader errors_;
        std::int64_t id_ = 0;
        std::intptr_t process_ = -1;
        std::intptr_t group_ = -1;
        std::optional<ExitStatus> exit_;
    };

    /// The result of a command run to completion.
    struct CapturedRun final
    {
        ExitStatus status;
        std::string output;
        std::string errors;
    };

    /// Runs a command with no input and collects its output.
    [[nodiscard]] Expected<CapturedRun>
    RunAndCapture(const ChildOptions &options);

    /// The two streams a worker talks to the runner over.
    struct WorkerStreams final
    {
        PipeReader commands;
        PipeWriter events;
    };

    /// Takes standard input and output of this process for the runner
    /// protocol. Afterwards standard input reads nothing and standard output
    /// goes to standard error, so what a test prints cannot corrupt a
    /// message.
    [[nodiscard]] Expected<WorkerStreams>
    TakeStandardStreams();

    /// The path of the running executable.
    [[nodiscard]] std::filesystem::path
    CurrentExecutable(const char *argumentZero);

    /// The number of hardware threads, at least 1.
    [[nodiscard]] unsigned
    HardwareThreads() noexcept;

    /// Whether a variable of this name holds a secret that a child does not
    /// inherit: its name contains, in any case, TOKEN, SECRET, PASSWORD,
    /// PASSWD, CREDENTIAL, API_KEY, APIKEY, PRIVATE_KEY or ACCESS_KEY.
    [[nodiscard]] bool
    IsSecretVariable(std::string_view name);

    /// The value of an environment variable; empty when it is not set.
    [[nodiscard]] std::string
    ReadEnvironment(const char *name);

    [[nodiscard]] bool
    HasEnvironment(const char *name);

    /// Whether standard output is a terminal.
    [[nodiscard]] bool
    IsStandardOutputTerminal() noexcept;

    /// Turns on ANSI colour sequences on a Windows console; true when the
    /// terminal shows them.
    [[nodiscard]] bool
    EnableTerminalColours() noexcept;

    /// Keeps a crash of a worker on Windows from opening an error-reporting
    /// dialog that would wait for a person. Elsewhere it does nothing.
    void
    QuietCrashReporting() noexcept;

    /// The id of this process.
    [[nodiscard]] std::int64_t
    CurrentProcessId() noexcept;

    /// Writes `text` to descriptor 2 directly, past any stream buffer or
    /// redirection, as far as it goes; for use while the process exits.
    void
    WriteStandardError(std::string_view text) noexcept;
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>


namespace Progmasoft::Catch3::Runner
{
    /// The name of the project configuration file.
    inline constexpr std::string_view kProjectConfigName = "catch3.json";

    /// The `catch3.json` that applies in `start`: the nearest one in `start`
    /// or a directory above it, up to the root of the Git repository that
    /// holds `start`. Outside a repository only `start` itself is searched,
    /// so a stray file in a home directory never applies.
    [[nodiscard]] std::optional<std::filesystem::path>
    FindProjectConfig(const std::filesystem::path &start);

    /// Applies the text of a `catch3.json` to `options`. Its keys are the long
    /// options of the command line, and its values go through the same checks;
    /// relative paths are read from `directory`, the file's own. `file` names
    /// the file in messages.
    [[nodiscard]] Expected<Options>
    ApplyProjectConfig(std::string_view text,
                       const std::filesystem::path &directory,
                       std::string_view file,
                       Options options = {});

    /// Reads the command line together with the `catch3.json` it selects:
    /// --config names one, --no-config reads none, and otherwise
    /// FindProjectConfig searches from `workingDirectory`. Options on the
    /// command line win over the file, and test specs on the command line
    /// replace those of the file. Workers, --help and --version read no file.
    [[nodiscard]] Expected<Options>
    ParseCommandLine(const std::vector<std::string> &arguments,
                     const std::filesystem::path &workingDirectory);
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0


#include <mutex>
#include <optional>
#include <string>

namespace Progmasoft::Catch3::Runner
{
    /// The runner and its workers exchange one JSON object per line. Every
    /// object has a member "t" naming its kind.
    ///
    /// From the runner to a worker:
    ///   run    {index, attempt, temp}  run one test
    ///   quit   {}                      end the process
    ///
    /// From a worker to the runner:
    ///   hello     {pid, identity, tests, protocol}
    ///   start     {index, attempt}
    ///   section   {begin: true, name, file, line, depth, at}
    ///   section   {begin: false, depth, at, duration}
    ///   at        {location}           the last assertion started
    ///   assertion {an AssertionRecord}
    ///   benchmark {a BenchmarkRecord}
    ///   fatal     {message, location}  sent before the worker dies
    ///   end       {index, attempt, outcome, category, passed, failed,
    ///              duration, stdout, stderr}
    inline constexpr int kWorkerProtocolVersion = 1;

    /// Reads lines from a pipe.
    class LineReader final
    {
    public:
        explicit LineReader(PipeReader &pipe)
            : pipe_{pipe}
        {
        }

        /// The next line without its end, or nothing at the end of the
        /// stream.
        [[nodiscard]] std::optional<std::string>
        Next();

    private:
        PipeReader &pipe_;
        std::string buffer_;
        std::size_t scanned_ = 0;
        bool ended_ = false;
    };

    /// Writes messages to a pipe, one line each; safe from several
    /// threads, since a test may assert from threads of its own.
    class MessageWriter final
    {
    public:
        explicit MessageWriter(PipeWriter &pipe)
            : pipe_{pipe}
        {
        }

        Status
        Send(const JsonValue &message);

    private:
        PipeWriter &pipe_;
        std::mutex mutex_;
    };

    /// A message of kind `kind` with no other members yet.
    [[nodiscard]] JsonValue
    Message(std::string_view kind);
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>


namespace Progmasoft::Catch3::Runner
{
    /// The format of a report, from its contents; nothing when none fits.
    [[nodiscard]] std::optional<ImportFormat>
    DetectImportFormat(std::string_view text);

    /// The results a report holds, in its order. A test the report names
    /// twice comes back twice; `MergeImported` makes one test of them.
    /// `source` names the report, for a format whose tests have no identity
    /// of their own.
    [[nodiscard]] Expected<std::vector<TestResult>>
    ParseImport(std::string_view text,
                ImportFormat format,
                const std::string &source = {});

    /// One test per identifier: results that share one are attempts at that
    /// test, in the order given, and the verdict is worked out over them. A
    /// test that both failed and passed is flaky, as it is in a run of the
    /// runner.
    [[nodiscard]] std::vector<TestResult>
    MergeImported(std::vector<TestResult> results);

    /// A result without attempts yet.
    [[nodiscard]] TestResult
    ImportedTest(std::string name, std::string className);

    /// One attempt as a report describes it. A failure gets an assertion
    /// record with `message` and `details`, so the analysis treats it as it
    /// treats a failure of Catch3's own; `category` defaults by outcome.
    [[nodiscard]] Attempt
    ImportedAttempt(Outcome outcome,
                    std::int64_t durationMicroseconds,
                    std::string message = {},
                    std::string details = {},
                    FailureCategory category = FailureCategory::kNone);

    /// Seconds as a report gives them; what makes no sense as a duration
    /// counts as zero.
    [[nodiscard]] std::int64_t
    ImportedMicroseconds(double seconds);

    [[nodiscard]] std::string_view
    ImportTrim(std::string_view text);

    [[nodiscard]] bool
    ImportStartsWith(std::string_view text, std::string_view prefix);

    /// The lines of a text, without their line endings.
    [[nodiscard]] std::vector<std::string_view>
    ImportLines(std::string_view text);

    /// A format of Catch3's own or CTRF, from a parsed document.
    [[nodiscard]] std::optional<ImportFormat>
    ImportJsonFormat(const JsonValue &value);

    /// Whether a line reports a test in TAP.
    [[nodiscard]] bool
    IsTapTestLine(std::string_view line);

    /// A `--json` report or catch3-results lines.
    [[nodiscard]] Expected<std::vector<TestResult>>
    ParseCatch3Report(std::string_view text);

    [[nodiscard]] Expected<std::vector<TestResult>>
    ParseCtrf(std::string_view text);

    /// `source` becomes the class of its tests: TAP names them only within
    /// one stream.
    [[nodiscard]] Expected<std::vector<TestResult>>
    ParseTap(std::string_view text, const std::string &source);

    [[nodiscard]] Expected<std::vector<TestResult>>
    ParseJUnit(std::string_view text);

    [[nodiscard]] Expected<std::vector<TestResult>>
    ParseXUnit(std::string_view text);

    [[nodiscard]] Expected<std::vector<TestResult>>
    ParseTrx(std::string_view text);
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0



namespace Progmasoft::Catch3::Runner
{
    /// The version of the JSON shapes below. It is written into every
    /// report and protocol stream; a reader refuses a newer major version.
    inline constexpr int kResultsFormatVersion = 1;

    [[nodiscard]] JsonValue
    ToJson(const AssertionRecord &assertion);

    [[nodiscard]] AssertionRecord
    AssertionFromJson(const JsonValue &value);

    [[nodiscard]] JsonValue
    ToJson(const BenchmarkRecord &benchmark);

    [[nodiscard]] BenchmarkRecord
    BenchmarkFromJson(const JsonValue &value);

    [[nodiscard]] JsonValue
    ToJson(const SectionRecord &section);

    [[nodiscard]] SectionRecord
    SectionFromJson(const JsonValue &value);

    [[nodiscard]] JsonValue
    ToJson(const Attempt &attempt);

    [[nodiscard]] Attempt
    AttemptFromJson(const JsonValue &value);

    [[nodiscard]] JsonValue
    ToJson(const TestResult &test);

    [[nodiscard]] TestResult
    TestResultFromJson(const JsonValue &value);

    /// The whole run, with a summary: the `--json` report.
    [[nodiscard]] JsonValue
    ToJson(const RunResult &run);

    [[nodiscard]] Expected<RunResult>
    RunResultFromJson(const JsonValue &value);
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0



#include <functional>
#include <string>
#include <vector>

namespace Progmasoft::Catch3::Runner
{
    /// Hooks the console report uses to show progress.
    struct SchedulerEvents final
    {
        /// A test has its verdict.
        std::function<void(const TestResult &)> testFinished;
        /// An attempt ended and another will follow: a retry, a repeat.
        std::function<void(const TestResult &, const Attempt &)> retrying;
    };

    struct ScheduleOutcome final
    {
        /// One result per planned test, in plan order.
        std::vector<TestResult> tests;
        std::size_t workers = 0;
        std::vector<std::string> notes;
        /// The workers could not be started at all.
        bool infrastructureFailed = false;
    };

    /// Runs a plan: in worker processes started from `program`, or in this
    /// process with `--in-process`.
    ///
    /// Tests go to whichever worker is free, most important first. A worker
    /// that crashes or hangs is replaced; the test it was running is
    /// recorded as crashed or timed out. A failure is retried as the plan
    /// allows, first in a fresh process: a test that then passes was either
    /// flaky or broken by a test that ran before it in the same process,
    /// and the scheduler finds that test by bisecting the ones that ran
    /// before.
    [[nodiscard]] ScheduleOutcome
    RunPlan(const Options &options,
            const Plan &plan,
            const Catalog &catalog,
            const std::string &program,
            const std::string &programName,
            const SchedulerEvents &events);
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

// The state the scheduler keeps of jobs, workers and tests.

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>


namespace Progmasoft::Catch3::Runner::Scheduling
{
    using Clock = std::chrono::steady_clock;

    inline constexpr std::size_t kMaximumRecordedAssertions = 200;
    inline constexpr std::size_t kMaximumProcessOutput = 64 * 1024;
    inline constexpr std::size_t kMaximumProbes = 12;
    inline constexpr std::size_t kStartupFailuresToGiveUp = 3;
    inline constexpr std::chrono::milliseconds kStartupTimeout{ 60000 };

    inline std::int64_t
    Microseconds(Clock::duration duration)
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(duration)
            .count();
    }

    inline void
    AppendBounded(std::string &target, std::string_view text)
    {
        target.append(text);
        if (target.size() > kMaximumProcessOutput)
        {
            // The end of the output says most about a crash.
            target.erase(0, target.size() - kMaximumProcessOutput);
        }
    }

    inline bool
    Satisfies(Outcome outcome)
    {
        return outcome == Outcome::kPassed
               || outcome == Outcome::kFailedAsExpected
               || outcome == Outcome::kUpToDate;
    }

    inline bool
    Passing(Outcome outcome)
    {
        return outcome == Outcome::kPassed
               || outcome == Outcome::kFailedAsExpected
               || outcome == Outcome::kSkipped;
    }

    /// What a worker is asked to do: run tests in order, the last one
    /// being the one the job is about.
    struct Job final
    {
        enum class Kind : std::uint8_t
        {
            /// An attempt of a planned test.
            kAttempt,
            /// A run that tells whether a failure depends on the tests
            /// before it; its result is not an attempt.
            kProbe,
        };

        Kind kind = Kind::kAttempt;
        std::size_t test = 0;
        /// Catalog indices to run first, in the same process.
        std::vector<std::size_t> before;
        bool fresh = false;
        std::optional<std::uint32_t> seed;
        /// For a probe: the tests that ran before it.
        std::vector<std::size_t> probeSequence;
    };

    /// One test being run in a worker.
    struct Running final
    {
        std::size_t catalogIndex = 0;
        bool target = false;
        Attempt attempt;
        Clock::time_point start;
        Clock::time_point deadline = Clock::time_point::max();
        bool fatal = false;
    };

    struct Worker final
    {
        std::size_t id = 0;
        std::unique_ptr<ChildProcess> process;
        std::thread outputReader;
        std::thread errorReader;
        bool ready = false;
        bool dedicated = false;
        bool quitting = false;
        bool outputClosed = false;
        bool errorsClosed = false;
        bool killedForTimeout = false;
        std::optional<std::uint32_t> seed;
        Clock::time_point started;
        std::optional<Job> job;
        /// Index into `job->before`, or its size for the target.
        std::size_t step = 0;
        std::optional<Running> running;
        /// Catalog indices this process has run, in order.
        std::vector<std::size_t> history;
        std::string startupOutput;
    };

    struct Event final
    {
        enum class Kind : std::uint8_t
        {
            kLine,
            kErrors,
            kOutputClosed,
            kErrorsClosed,
        };
        std::size_t worker = 0;
        Kind kind = Kind::kLine;
        std::string text;
    };

    /// The search for the test that leaves state behind.
    struct Bisection final
    {
        enum class Stage : std::uint8_t
        {
            kNone,
            kConfirming,
            kSearching,
            kDone,
        };
        Stage stage = Stage::kNone;
        std::vector<std::size_t> candidates;
        std::vector<std::size_t> probing;
        std::size_t probes = 0;
        std::optional<std::size_t> culprit;
        bool reproduced = false;
    };

    struct TestState final
    {
        TestResult result;
        bool queued = false;
        bool finished = false;
        std::size_t pendingJobs = 0;
        std::size_t retriesUsed = 0;
        std::size_t repeatsLeft = 0;
        Bisection bisection;
    };
} // namespace Progmasoft::Catch3::Runner::Scheduling
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <array>
#include <optional>
#include <string>

namespace Progmasoft::Catch3::Runner
{
    /// Gives the test that runs while it lives a temporary directory of its
    /// own: it creates the directory, points TMPDIR, TMP and TEMP at it and
    /// makes `TestTemporaryDirectory` return it. The variables get their
    /// previous values back afterwards; the directory stays, because the
    /// runner decides whether to keep it.
    class TemporaryDirectoryScope final
    {
    public:
        explicit TemporaryDirectoryScope(const std::string &directory);
        TemporaryDirectoryScope(const TemporaryDirectoryScope &) = delete;
        TemporaryDirectoryScope &
        operator=(const TemporaryDirectoryScope &) = delete;
        ~TemporaryDirectoryScope();

    private:
        bool active_ = false;
        std::array<std::optional<std::string>, 3> previous_;
    };
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <filesystem>
#include <string>
#include <vector>

namespace Progmasoft::Catch3
{
    /// Runs the tests of this binary as `arguments` say and returns the exit
    /// code. `arguments` starts with the program, as `argv` does.
    ///
    /// `Catch::Session::run` and the `main` of `Catch2::Catch2WithMain` call
    /// this; a test binary with its own `main` may call it directly.
    [[nodiscard]] int
    RunTests(const std::vector<std::string> &arguments);

    /// Prints the options of the runner to standard output.
    void
    PrintHelp(const std::string &programName);

    /// The directory the running test may keep temporary files in.
    ///
    /// Every attempt of every test gets its own, so tests that run at the
    /// same time in different workers never share a file by accident. The
    /// runner also points TMPDIR, TMP and TEMP at it while the test runs, so
    /// `std::filesystem::temp_directory_path()` and `tmpfile` use it too.
    /// It is removed after a passing attempt and kept after a failing one,
    /// for inspection. Outside a test it is the system temporary directory.
    [[nodiscard]] std::filesystem::path
    TestTemporaryDirectory();
} // namespace Progmasoft::Catch3
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

namespace Progmasoft::Catch3
{
    /// The version of Catch3: of its runner, its tools and its formats.
    inline constexpr const char *kVersion = "0.3.0.2";
} // namespace Progmasoft::Catch3
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <iosfwd>
#include <string>

namespace Progmasoft::Catch3
{
    struct RunResult;

    /// Writes a run as JUnit XML without timestamps or global state, so the
    /// same run gives the same bytes.
    ///
    /// A failed test gets `<failure>`, a crash, timeout or infrastructure
    /// problem `<error>`, and earlier failed attempts `<rerunFailure>`. A
    /// test that failed and then passed is a success with `<flakyFailure>`
    /// elements, the convention of Maven Surefire that Jenkins, GitLab and
    /// most CI dashboards read. Skipped, blocked and not-run tests are
    /// `<skipped>` with the reason as the message.
    class JUnitXmlWriter final
    {
    public:
        /// Returns false when the stream failed.
        [[nodiscard]] static bool
        Write(const RunResult &run, std::ostream &output);

        [[nodiscard]] static std::string
        ToString(const RunResult &run);
    };
} // namespace Progmasoft::Catch3



#include <cmath>
#include <limits>

namespace {

// Performs equivalent check of std::fabs(lhs - rhs) <= margin
// But without the subtraction to allow for INFINITY in comparison
bool marginComparison(double lhs, double rhs, double margin) {
    return (lhs + margin >= rhs) && (rhs + margin >= lhs);
}

}

namespace Catch {

    Approx::Approx ( double value )
    :   m_epsilon( static_cast<double>(std::numeric_limits<float>::epsilon())*100. ),
        m_margin( 0.0 ),
        m_scale( 0.0 ),
        m_value( value )
    {}

    Approx Approx::custom() {
        return Approx( 0 );
    }

    Approx Approx::operator-() const {
        auto temp(*this);
        temp.m_value = -temp.m_value;
        return temp;
    }


    std::string Approx::toString() const {
        ReusableStringStream rss;
        rss << "Approx( " << ::Catch::Detail::stringify( m_value ) << " )";
        return rss.str();
    }

    bool Approx::equalityComparisonImpl(const double other) const {
        // First try with fixed margin, then compute margin based on epsilon, scale and Approx's value
        // Thanks to Richard Harris for his help refining the scaled margin value
        return marginComparison(m_value, other, m_margin)
            || marginComparison(m_value, other, m_epsilon * (m_scale + std::fabs(std::isinf(m_value)? 0 : m_value)));
    }

    void Approx::setMargin(double newMargin) {
        CATCH_ENFORCE(newMargin >= 0,
            "Invalid Approx::margin: " << newMargin << '.'
            << " Approx::Margin has to be non-negative.");
        m_margin = newMargin;
    }

    void Approx::setEpsilon(double newEpsilon) {
        CATCH_ENFORCE(newEpsilon >= 0 && newEpsilon <= 1.0,
            "Invalid Approx::epsilon: " << newEpsilon << '.'
            << " Approx::epsilon has to be in [0, 1]");
        m_epsilon = newEpsilon;
    }

namespace literals {
    Approx operator ""_a(long double val) {
        return Approx(val);
    }
    Approx operator ""_a(unsigned long long val) {
        return Approx(val);
    }
} // end namespace literals

std::string StringMaker<Catch::Approx>::convert(Catch::Approx const& value) {
    return value.toString();
}

} // end namespace Catch



namespace Catch {

    AssertionResultData::AssertionResultData(ResultWas::OfType _resultType, LazyExpression const& _lazyExpression):
        lazyExpression(_lazyExpression),
        resultType(_resultType) {}

    std::string AssertionResultData::reconstructExpression() const {

        if( reconstructedExpression.empty() ) {
            if( lazyExpression ) {
                ReusableStringStream rss;
                rss << lazyExpression;
                reconstructedExpression = rss.str();
            }
        }
        return reconstructedExpression;
    }

    AssertionResult::AssertionResult( AssertionInfo const& info, AssertionResultData&& data )
    :   m_info( info ),
        m_resultData( CATCH_MOVE(data) )
    {}

    // Result was a success
    bool AssertionResult::succeeded() const {
        return Catch::isOk( m_resultData.resultType );
    }

    // Result was a success, or failure is suppressed
    bool AssertionResult::isOk() const {
        return Catch::isOk( m_resultData.resultType ) || shouldSuppressFailure( m_info.resultDisposition );
    }

    ResultWas::OfType AssertionResult::getResultType() const {
        return m_resultData.resultType;
    }

    bool AssertionResult::hasExpression() const {
        return !m_info.capturedExpression.empty();
    }

    bool AssertionResult::hasMessage() const {
        return !m_resultData.message.empty();
    }

    std::string AssertionResult::getExpression() const {
        // Possibly overallocating by 3 characters should be basically free
        std::string expr; expr.reserve(m_info.capturedExpression.size() + 3);
        if (isFalseTest(m_info.resultDisposition)) {
            expr += "!(";
        }
        expr += m_info.capturedExpression;
        if (isFalseTest(m_info.resultDisposition)) {
            expr += ')';
        }
        return expr;
    }

    std::string AssertionResult::getExpressionInMacro() const {
        if ( m_info.macroName.empty() ) {
            return static_cast<std::string>( m_info.capturedExpression );
        }
        std::string expr;
        expr.reserve( m_info.macroName.size() + m_info.capturedExpression.size() + 4 );
        expr += m_info.macroName;
        expr += "( ";
        expr += m_info.capturedExpression;
        expr += " )";
        return expr;
    }

    bool AssertionResult::hasExpandedExpression() const {
        return hasExpression() && getExpandedExpression() != getExpression();
    }

    std::string AssertionResult::getExpandedExpression() const {
        std::string expr = m_resultData.reconstructExpression();
        return expr.empty()
                ? getExpression()
                : expr;
    }

    StringRef AssertionResult::getMessage() const {
        return m_resultData.message;
    }
    SourceLineInfo AssertionResult::getSourceInfo() const {
        return m_info.lineInfo;
    }

    StringRef AssertionResult::getTestMacroName() const {
        return m_info.macroName;
    }

} // end namespace Catch




#include <algorithm>
#include <chrono>
#include <cmath>

namespace Catch {
    namespace Benchmark {
        namespace Detail {

            Environment measure_environment_default() {
                return Detail::measure_environment<default_clock>();
            }

            ExecutionPlan prepare_default( const IConfig& cfg,
                                           Environment env,
                                           BenchmarkFunction&& fun ) {
                // This mirrors Benchmark::prepare<default_clock>(), but with the
                // clock fixed so it is instantiated once here in the library.
                auto min_time = env.clock_resolution.mean * Detail::minimum_ticks;
                auto run_time = std::max(
                    min_time,
                    std::chrono::duration_cast<decltype( min_time )>(
                        cfg.benchmarkWarmupTime() ) );
                auto&& test = Detail::run_for_at_least<default_clock>(
                    std::chrono::duration_cast<IDuration>( run_time ), 1, fun );
                int new_iters = static_cast<int>(
                    std::ceil( min_time * test.iterations / test.elapsed ) );
                return { new_iters,
                         test.elapsed / test.iterations * new_iters *
                             cfg.benchmarkSamples(),
                         CATCH_MOVE( fun ),
                         std::chrono::duration_cast<FDuration>(
                             cfg.benchmarkWarmupTime() ),
                         Detail::warmup_iterations };
            }

            std::vector<FDuration> run_plan_default( ExecutionPlan const& plan,
                                                     const IConfig& cfg,
                                                     Environment env ) {
                return plan.run<default_clock>( cfg, env );
            }

        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch




namespace Catch {
    namespace Benchmark {
        namespace Detail {
            ChronometerConcept::~ChronometerConcept() = default;
        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch


// Adapted from donated nonius code.


#include <vector>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            SampleAnalysis analyse(const IConfig &cfg, FDuration* first, FDuration* last) {
                if (!cfg.benchmarkNoAnalysis()) {
                    std::vector<double> samples;
                    samples.reserve(static_cast<size_t>(last - first));
                    for (auto current = first; current != last; ++current) {
                        samples.push_back( current->count() );
                    }

                    auto analysis = Catch::Benchmark::Detail::analyse_samples(
                        cfg.benchmarkConfidenceInterval(),
                        cfg.benchmarkResamples(),
                        samples.data(),
                        samples.data() + samples.size() );
                    auto outliers = Catch::Benchmark::Detail::classify_outliers(
                        samples.data(), samples.data() + samples.size() );

                    auto wrap_estimate = [](Estimate<double> e) {
                        return Estimate<FDuration>{
                            FDuration( e.point ),
                            FDuration( e.lower_bound ),
                            FDuration( e.upper_bound ),
                            e.confidence_interval,
                        };
                    };
                    std::vector<FDuration> samples2;
                    samples2.reserve(samples.size());
                    for (auto s : samples) {
                        samples2.push_back( FDuration( s ) );
                    }

                    return {
                        CATCH_MOVE(samples2),
                        wrap_estimate(analysis.mean),
                        wrap_estimate(analysis.standard_deviation),
                        outliers,
                        analysis.outlier_variance,
                    };
                } else {
                    std::vector<FDuration> samples;
                    samples.reserve(static_cast<size_t>(last - first));

                    FDuration mean = FDuration(0);
                    int i = 0;
                    for (auto it = first; it < last; ++it, ++i) {
                        samples.push_back(*it);
                        mean += *it;
                    }
                    mean /= i;

                    return SampleAnalysis{
                        CATCH_MOVE(samples),
                        Estimate<FDuration>{ mean, mean, mean, 0.0 },
                        Estimate<FDuration>{ FDuration( 0 ),
                                             FDuration( 0 ),
                                             FDuration( 0 ),
                                             0.0 },
                        OutlierClassification{},
                        0.0
                    };
                }
            }
        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch




namespace Catch {
    namespace Benchmark {
        namespace Detail {
            struct do_nothing {
                void operator()() const {}
            };

            BenchmarkFunction::callable::~callable() = default;
            BenchmarkFunction::BenchmarkFunction():
                f( new model<do_nothing>{ {} } ){}
        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch




#include <exception>

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            struct optimized_away_error final : std::exception {
                const char* what() const noexcept override;
            };

            const char* optimized_away_error::what() const noexcept {
                return "could not measure benchmark, maybe it was optimized away";
            }

            void throw_optimized_away_error() {
                Catch::throw_exception(optimized_away_error{});
            }

        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch


// Adapted from donated nonius code.



#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <random>


#if defined(CATCH_CONFIG_USE_ASYNC)
#include <future>
#endif

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            namespace {

                template <typename URng, typename Estimator>
                static sample
                resample( URng& rng,
                          unsigned int resamples,
                          double const* first,
                          double const* last,
                          Estimator& estimator ) {
                    auto n = static_cast<size_t>( last - first );
                    Catch::uniform_integer_distribution<size_t> dist( 0, n - 1 );

                    sample out;
                    out.reserve( resamples );
                    std::vector<double> resampled;
                    resampled.reserve( n );
                    for ( size_t i = 0; i < resamples; ++i ) {
                        resampled.clear();
                        for ( size_t s = 0; s < n; ++s ) {
                            resampled.push_back( first[dist( rng )] );
                        }
                        const auto estimate =
                            estimator( resampled.data(), resampled.data() + resampled.size() );
                        out.push_back( estimate );
                    }
                    std::sort( out.begin(), out.end() );
                    return out;
                }

                static double outlier_variance( Estimate<double> mean,
                                                Estimate<double> stddev,
                                                int n ) {
                    double sb = stddev.point;
                    double mn = mean.point / n;
                    double mg_min = mn / 2.;
                    double sg = (std::min)( mg_min / 4., sb / std::sqrt( n ) );
                    double sg2 = sg * sg;
                    double sb2 = sb * sb;

                    auto c_max = [n, mn, sb2, sg2]( double x ) -> double {
                        double k = mn - x;
                        double d = k * k;
                        double nd = n * d;
                        double k0 = -n * nd;
                        double k1 = sb2 - n * sg2 + nd;
                        double det = k1 * k1 - 4 * sg2 * k0;
                        return static_cast<int>( -2. * k0 /
                                                 ( k1 + std::sqrt( det ) ) );
                    };

                    auto var_out = [n, sb2, sg2]( double c ) {
                        double nc = n - c;
                        return ( nc / n ) * ( sb2 - nc * sg2 );
                    };

                    return (std::min)( var_out( 1 ),
                                       var_out(
                                           (std::min)( c_max( 0. ),
                                                       c_max( mg_min ) ) ) ) /
                           sb2;
                }

                static double erf_inv( double x ) {
                    // Code accompanying the article "Approximating the erfinv
                    // function" in GPU Computing Gems, Volume 2
                    double w, p;

                    w = -log( ( 1.0 - x ) * ( 1.0 + x ) );

                    if ( w < 6.250000 ) {
                        w = w - 3.125000;
                        p = -3.6444120640178196996e-21;
                        p = -1.685059138182016589e-19 + p * w;
                        p = 1.2858480715256400167e-18 + p * w;
                        p = 1.115787767802518096e-17 + p * w;
                        p = -1.333171662854620906e-16 + p * w;
                        p = 2.0972767875968561637e-17 + p * w;
                        p = 6.6376381343583238325e-15 + p * w;
                        p = -4.0545662729752068639e-14 + p * w;
                        p = -8.1519341976054721522e-14 + p * w;
                        p = 2.6335093153082322977e-12 + p * w;
                        p = -1.2975133253453532498e-11 + p * w;
                        p = -5.4154120542946279317e-11 + p * w;
                        p = 1.051212273321532285e-09 + p * w;
                        p = -4.1126339803469836976e-09 + p * w;
                        p = -2.9070369957882005086e-08 + p * w;
                        p = 4.2347877827932403518e-07 + p * w;
                        p = -1.3654692000834678645e-06 + p * w;
                        p = -1.3882523362786468719e-05 + p * w;
                        p = 0.0001867342080340571352 + p * w;
                        p = -0.00074070253416626697512 + p * w;
                        p = -0.0060336708714301490533 + p * w;
                        p = 0.24015818242558961693 + p * w;
                        p = 1.6536545626831027356 + p * w;
                    } else if ( w < 16.000000 ) {
                        w = sqrt( w ) - 3.250000;
                        p = 2.2137376921775787049e-09;
                        p = 9.0756561938885390979e-08 + p * w;
                        p = -2.7517406297064545428e-07 + p * w;
                        p = 1.8239629214389227755e-08 + p * w;
                        p = 1.5027403968909827627e-06 + p * w;
                        p = -4.013867526981545969e-06 + p * w;
                        p = 2.9234449089955446044e-06 + p * w;
                        p = 1.2475304481671778723e-05 + p * w;
                        p = -4.7318229009055733981e-05 + p * w;
                        p = 6.8284851459573175448e-05 + p * w;
                        p = 2.4031110387097893999e-05 + p * w;
                        p = -0.0003550375203628474796 + p * w;
                        p = 0.00095328937973738049703 + p * w;
                        p = -0.0016882755560235047313 + p * w;
                        p = 0.0024914420961078508066 + p * w;
                        p = -0.0037512085075692412107 + p * w;
                        p = 0.005370914553590063617 + p * w;
                        p = 1.0052589676941592334 + p * w;
                        p = 3.0838856104922207635 + p * w;
                    } else {
                        w = sqrt( w ) - 5.000000;
                        p = -2.7109920616438573243e-11;
                        p = -2.5556418169965252055e-10 + p * w;
                        p = 1.5076572693500548083e-09 + p * w;
                        p = -3.7894654401267369937e-09 + p * w;
                        p = 7.6157012080783393804e-09 + p * w;
                        p = -1.4960026627149240478e-08 + p * w;
                        p = 2.9147953450901080826e-08 + p * w;
                        p = -6.7711997758452339498e-08 + p * w;
                        p = 2.2900482228026654717e-07 + p * w;
                        p = -9.9298272942317002539e-07 + p * w;
                        p = 4.5260625972231537039e-06 + p * w;
                        p = -1.9681778105531670567e-05 + p * w;
                        p = 7.5995277030017761139e-05 + p * w;
                        p = -0.00021503011930044477347 + p * w;
                        p = -0.00013871931833623122026 + p * w;
                        p = 1.0103004648645343977 + p * w;
                        p = 4.8499064014085844221 + p * w;
                    }
                    return p * x;
                }

                static double
                standard_deviation( double const* first, double const* last ) {
                    auto m = Catch::Benchmark::Detail::mean( first, last );
                    double variance =
                        std::accumulate( first,
                                         last,
                                         0.,
                                         [m]( double a, double b ) {
                                             double diff = b - m;
                                             return a + diff * diff;
                                         } ) /
                        static_cast<double>( last - first );
                    return std::sqrt( variance );
                }

                static sample jackknife( double ( *estimator )( double const*,
                                                                double const* ),
                                         double* first,
                                         double* last ) {
                    const auto second = first + 1;
                    sample results;
                    results.reserve( static_cast<size_t>( last - first ) );

                    for ( auto it = first; it != last; ++it ) {
                        std::iter_swap( it, first );
                        results.push_back( estimator( second, last ) );
                    }

                    return results;
                }


            } // namespace
        }     // namespace Detail
    }         // namespace Benchmark
} // namespace Catch

namespace Catch {
    namespace Benchmark {
        namespace Detail {

            double weighted_average_quantile( int k,
                                              int q,
                                              double* first,
                                              double* last ) {
                auto count = last - first;
                double idx = static_cast<double>((count - 1) * k) / static_cast<double>(q);
                int j = static_cast<int>(idx);
                double g = idx - j;
                std::nth_element(first, first + j, last);
                auto xj = first[j];
                if ( Catch::Detail::directCompare( g, 0 ) ) {
                    return xj;
                }

                auto xj1 = *std::min_element(first + (j + 1), last);
                return xj + g * (xj1 - xj);
            }

            OutlierClassification
            classify_outliers( double const* first, double const* last ) {
                std::vector<double> copy( first, last );

                auto q1 = weighted_average_quantile( 1, 4, copy.data(), copy.data() + copy.size() );
                auto q3 = weighted_average_quantile( 3, 4, copy.data(), copy.data() + copy.size() );
                auto iqr = q3 - q1;
                auto los = q1 - ( iqr * 3. );
                auto lom = q1 - ( iqr * 1.5 );
                auto him = q3 + ( iqr * 1.5 );
                auto his = q3 + ( iqr * 3. );

                OutlierClassification o;
                for ( ; first != last; ++first ) {
                    const double t = *first;
                    if ( t < los ) {
                        ++o.low_severe;
                    } else if ( t < lom ) {
                        ++o.low_mild;
                    } else if ( t > his ) {
                        ++o.high_severe;
                    } else if ( t > him ) {
                        ++o.high_mild;
                    }
                    ++o.samples_seen;
                }
                return o;
            }

            double mean( double const* first, double const* last ) {
                auto count = last - first;
                double sum = 0.;
                while (first != last) {
                    sum += *first;
                    ++first;
                }
                return sum / static_cast<double>(count);
            }

            double normal_cdf( double x ) {
                return std::erfc( -x / std::sqrt( 2.0 ) ) / 2.0;
            }

            double erfc_inv(double x) {
                return erf_inv(1.0 - x);
            }

            double normal_quantile(double p) {
                static const double ROOT_TWO = std::sqrt(2.0);

                double result = 0.0;
                assert(p >= 0 && p <= 1);
                if (p < 0 || p > 1) {
                    return result;
                }

                result = -erfc_inv(2.0 * p);
                // result *= normal distribution standard deviation (1.0) * sqrt(2)
                result *= /*sd * */ ROOT_TWO;
                // result += normal disttribution mean (0)
                return result;
            }

            Estimate<double>
            bootstrap( double confidence_level,
                       double* first,
                       double* last,
                       sample const& resample,
                       double ( *estimator )( double const*, double const* ) ) {
                auto n_samples = last - first;

                double point = estimator( first, last );
                // Degenerate case with a single sample
                if ( n_samples == 1 )
                    return { point, point, point, confidence_level };

                sample jack = jackknife( estimator, first, last );
                double jack_mean =
                    mean( jack.data(), jack.data() + jack.size() );
                double sum_squares = 0, sum_cubes = 0;
                for ( double x : jack ) {
                    auto difference = jack_mean - x;
                    auto square = difference * difference;
                    auto cube = square * difference;
                    sum_squares += square;
                    sum_cubes += cube;
                }

                double accel = sum_cubes / ( 6 * std::pow( sum_squares, 1.5 ) );
                long n = static_cast<long>( resample.size() );
                double prob_n = static_cast<double>(
                    std::count_if( resample.begin(),
                                   resample.end(),
                                   [point]( double x ) { return x < point; } )) /
                    static_cast<double>( n );
                // degenerate case with uniform samples
                if ( Catch::Detail::directCompare( prob_n, 0. ) ) {
                    return { point, point, point, confidence_level };
                }

                double bias = normal_quantile( prob_n );
                double z1 = normal_quantile( ( 1. - confidence_level ) / 2. );

                auto cumn = [n]( double x ) -> long {
                    return std::lround( normal_cdf( x ) *
                                        static_cast<double>( n ) );
                };
                auto a = [bias, accel]( double b ) {
                    return bias + b / ( 1. - accel * b );
                };
                double b1 = bias + z1;
                double b2 = bias - z1;
                double a1 = a( b1 );
                double a2 = a( b2 );
                auto lo = static_cast<size_t>( (std::max)( cumn( a1 ), 0l ) );
                auto hi =
                    static_cast<size_t>( (std::min)( cumn( a2 ), n - 1 ) );

                return { point, resample[lo], resample[hi], confidence_level };
            }

            bootstrap_analysis analyse_samples(double confidence_level,
                                               unsigned int n_resamples,
                                               double* first,
                                               double* last) {
                auto mean = &Detail::mean;
                auto stddev = &standard_deviation;

#if defined(CATCH_CONFIG_USE_ASYNC)
                auto Estimate = [=](double(*f)(double const*, double const*)) {
                    std::random_device rd;
                    auto seed = rd();
                    return std::async(std::launch::async, [=] {
                        SimplePcg32 rng( seed );
                        auto resampled = resample(rng, n_resamples, first, last, f);
                        return bootstrap(confidence_level, first, last, resampled, f);
                    });
                };

                auto mean_future = Estimate(mean);
                auto stddev_future = Estimate(stddev);

                auto mean_estimate = mean_future.get();
                auto stddev_estimate = stddev_future.get();
#else
                auto Estimate = [=](double(*f)(double const* , double const*)) {
                    std::random_device rd;
                    auto seed = rd();
                    SimplePcg32 rng( seed );
                    auto resampled = resample(rng, n_resamples, first, last, f);
                    return bootstrap(confidence_level, first, last, resampled, f);
                };

                auto mean_estimate = Estimate(mean);
                auto stddev_estimate = Estimate(stddev);
#endif // CATCH_USE_ASYNC

                auto n = static_cast<int>(last - first); // seriously, one can't use integral types without hell in C++
                double outlier_variance = Detail::outlier_variance(mean_estimate, stddev_estimate, n);

                return { mean_estimate, stddev_estimate, outlier_variance };
            }
        } // namespace Detail
    } // namespace Benchmark
} // namespace Catch



namespace Catch {

    bool operator==( PathFilter const& lhs, PathFilter const& rhs ) {
        return lhs.type == rhs.type && lhs.filter == rhs.filter;
    }

    Config::Config( ConfigData const& data ):
        m_data( data ) {
        // We need to trim filter specs to avoid trouble with superfluous
        // whitespace (esp. important for bdd macros, as those are manually
        // aligned with whitespace).
        for (auto& elem : m_data.testsOrTags) {
            elem = trim(elem);
        }

        TestSpecParser parser( ITagAliasRegistry::get() );
        if ( !m_data.testsOrTags.empty() ) {
            m_hasTestFilters = true;
            for ( auto const& testOrTags : m_data.testsOrTags ) {
                parser.parse( testOrTags );
            }
        }
        m_testSpec = parser.testSpec();
    }

    Config::~Config() = default;

    std::vector<std::string> const& Config::getTestsOrTags() const { return m_data.testsOrTags; }
    std::vector<PathFilter> const& Config::getPathFilters() const { return m_data.pathFilters; }
    bool Config::useNewFilterBehaviour() const { return m_data.useNewPathFilteringBehaviour; }

    TestSpec const& Config::testSpec() const { return m_testSpec; }
    bool Config::hasTestFilters() const { return m_hasTestFilters; }

    // IConfig interface
    bool Config::allowThrows() const                   { return !m_data.noThrow; }
    StringRef Config::name() const { return m_data.name.empty() ? m_data.processName : m_data.name; }
    bool Config::includeSuccessfulResults() const      { return m_data.showSuccessfulTests; }
    bool Config::warnAboutMissingAssertions() const {
        return !!( m_data.warnings & WarnAbout::NoAssertions );
    }
    bool Config::warnAboutUnmatchedTestSpecs() const {
        return !!( m_data.warnings & WarnAbout::UnmatchedTestSpec );
    }
    bool Config::warnAboutInfiniteGenerators() const {
        return !!( m_data.warnings & WarnAbout::InfiniteGenerator );
    }
    bool Config::zeroTestsCountAsSuccess() const       { return m_data.allowZeroTests; }
    ShowDurations Config::showDurations() const        { return m_data.showDurations; }
    double Config::minDuration() const                 { return m_data.minDuration; }
    TestRunOrder Config::runOrder() const              { return m_data.runOrder; }
    uint32_t Config::rngSeed() const                   { return m_data.rngSeed; }
    bool Config::rngSeedWasFixed() const               { return m_data.rngSeedWasFixed; }
    unsigned int Config::shardCount() const            { return m_data.shardCount; }
    unsigned int Config::shardIndex() const            { return m_data.shardIndex; }
    ColourMode Config::defaultColourMode() const       { return m_data.defaultColourMode; }
    bool Config::shouldDebugBreak() const              { return m_data.shouldDebugBreak; }
    int Config::abortAfter() const                     { return m_data.abortAfter; }
    bool Config::showInvisibles() const                { return m_data.showInvisibles; }
    Verbosity Config::verbosity() const                { return m_data.verbosity; }

    bool Config::skipBenchmarks() const                           { return m_data.skipBenchmarks; }
    bool Config::benchmarkNoAnalysis() const                      { return m_data.benchmarkNoAnalysis; }
    unsigned int Config::benchmarkSamples() const                 { return m_data.benchmarkSamples; }
    double Config::benchmarkConfidenceInterval() const            { return m_data.benchmarkConfidenceInterval; }
    unsigned int Config::benchmarkResamples() const               { return m_data.benchmarkResamples; }
    std::chrono::milliseconds Config::benchmarkWarmupTime() const { return std::chrono::milliseconds(m_data.benchmarkWarmupTime); }

} // end namespace Catch


#include <ostream>

namespace Catch {

    Version::Version
        (   unsigned int _majorVersion,
            unsigned int _minorVersion,
            unsigned int _patchNumber,
            char const * const _branchName,
            unsigned int _buildNumber )
    :   majorVersion( _majorVersion ),
        minorVersion( _minorVersion ),
        patchNumber( _patchNumber ),
        branchName( _branchName ),
        buildNumber( _buildNumber )
    {}

    std::ostream& operator << ( std::ostream& os, Version const& version ) {
        os  << version.majorVersion << '.'
            << version.minorVersion << '.'
            << version.patchNumber;
        // branchName is never null -> 0th char is \0 if it is empty
        if (version.branchName[0]) {
            os << '-' << version.branchName
               << '.' << version.buildNumber;
        }
        return os;
    }

    Version const& libraryVersion() {
        static Version version( 3, 16, 1, "", 0 );
        return version;
    }

}




namespace Catch {

    const char* GeneratorException::what() const noexcept {
        return m_msg;
    }

} // end namespace Catch




namespace Catch {

    IGeneratorTracker::~IGeneratorTracker() = default;

namespace Generators {

    GeneratorUntypedBase::~GeneratorUntypedBase() = default;

    IGeneratorTracker* acquireGeneratorTracker(StringRef generatorName, SourceLineInfo const& lineInfo ) {
        return getResultCapture().acquireGeneratorTracker( generatorName, lineInfo );
    }

    IGeneratorTracker* createGeneratorTracker( StringRef generatorName,
                                 SourceLineInfo lineInfo,
                                 GeneratorBasePtr&& generator ) {
        return getResultCapture().createGeneratorTracker(
            generatorName, lineInfo, CATCH_MOVE( generator ) );
    }

} // namespace Generators
} // namespace Catch




#include <random>

namespace Catch {
    namespace Generators {
        namespace Detail {
            std::uint32_t getSeed() { return sharedRng()(); }
        } // namespace Detail

        struct RandomFloatingGenerator<long double>::PImpl {
            PImpl( long double a, long double b, uint32_t seed ):
                rng( seed ), dist( a, b ) {}

            Catch::SimplePcg32 rng;
            std::uniform_real_distribution<long double> dist;
        };

        RandomFloatingGenerator<long double>::RandomFloatingGenerator(
            long double a, long double b, std::uint32_t seed) :
            m_pimpl(Catch::Detail::make_unique<PImpl>(a, b, seed)) {
            static_cast<void>( next() );
        }

        RandomFloatingGenerator<long double>::~RandomFloatingGenerator() =
            default;
        bool RandomFloatingGenerator<long double>::next() {
            m_current_number = m_pimpl->dist( m_pimpl->rng );
            return true;
        }

        bool RandomFloatingGenerator<long double>::isFinite() const {
            return false;
        }

    } // namespace Generators
} // namespace Catch




namespace Catch {
    namespace Generators {
        namespace Detail {

            [[noreturn]]
            void throw_generator_exception( char const* msg ) {
                Catch::throw_exception( GeneratorException{ msg } );
            }

        } // namespace Detail
    } // namespace Generators
} // namespace Catch





namespace Catch {
    std::uint32_t getSeed() {
        return getCurrentContext().getConfig()->rngSeed();
    }
}




namespace Catch {
    EventListenerFactory::~EventListenerFactory() = default;
}




namespace Catch {
    IConfig::~IConfig() = default;
}



#include <cassert>

namespace Catch {

    AssertionStats::AssertionStats( AssertionResult const& _assertionResult,
                                    std::vector<MessageInfo> const& _infoMessages,
                                    Totals const& _totals )
    :   assertionResult( _assertionResult ),
        infoMessages( _infoMessages ),
        totals( _totals )
    {
        if( assertionResult.hasMessage() ) {
            // Copy message into messages list.
            // !TBD This should have been done earlier, somewhere
            MessageBuilder builder( assertionResult.getTestMacroName(), assertionResult.getSourceInfo(), assertionResult.getResultType() );
            builder.m_info.message = static_cast<std::string>(assertionResult.getMessage());

            infoMessages.push_back( CATCH_MOVE(builder.m_info) );
        }
    }

    SectionStats::SectionStats(  SectionInfo&& _sectionInfo,
                                 Counts const& _assertions,
                                 double _durationInSeconds,
                                 bool _missingAssertions )
    :   sectionInfo( CATCH_MOVE(_sectionInfo) ),
        assertions( _assertions ),
        durationInSeconds( _durationInSeconds ),
        missingAssertions( _missingAssertions )
    {}


    TestCaseStats::TestCaseStats(  TestCaseInfo const& _testInfo,
                                   Totals const& _totals,
                                   std::string&& _stdOut,
                                   std::string&& _stdErr,
                                   bool _aborting )
    : testInfo( &_testInfo ),
        totals( _totals ),
        stdOut( CATCH_MOVE(_stdOut) ),
        stdErr( CATCH_MOVE(_stdErr) ),
        aborting( _aborting )
    {}


    TestRunStats::TestRunStats(   TestRunInfo const& _runInfo,
                    Totals const& _totals,
                    bool _aborting )
    :   runInfo( _runInfo ),
        totals( _totals ),
        aborting( _aborting )
    {}

    IEventListener::~IEventListener() = default;

} // end namespace Catch




namespace Catch {
    IExceptionTranslator::~IExceptionTranslator() = default;
    IExceptionTranslatorRegistry::~IExceptionTranslatorRegistry() = default;
}




#include <string>

namespace Catch {
    namespace Generators {

        bool GeneratorUntypedBase::countedNext() {
            auto ret = next();
            if ( ret ) {
                m_stringReprCache.clear();
                ++m_currentElementIndex;
            }
            return ret;
        }

        void GeneratorUntypedBase::skipToNthElementImpl( std::size_t n ) {
            for ( size_t i = m_currentElementIndex; i < n; ++i ) {
                bool isValid = next();
                if ( !isValid ) {
                    Detail::throw_generator_exception(
                        "Could not jump to Nth element: not enough elements" );
                }
            }
        }

        void GeneratorUntypedBase::skipToNthElement( std::size_t n ) {
            if ( n < m_currentElementIndex ) {
                Detail::throw_generator_exception(
                    "Tried to jump generator backwards" );
            }
            if ( n == m_currentElementIndex ) { return; }

            skipToNthElementImpl(n);
            // Fixup tracking after moving the generator forward
            //  * Ensure that the correct element index is set after skipping
            //  * Invalidate cache
            m_currentElementIndex = n;
            m_stringReprCache.clear();
        }

        StringRef GeneratorUntypedBase::currentElementAsString() const {
            if ( m_stringReprCache.empty() ) {
                m_stringReprCache = stringifyImpl();
            }
            return m_stringReprCache;
        }

        bool GeneratorUntypedBase::isFinite() const { return true; }

    } // namespace Generators
} // namespace Catch




namespace Catch {
    IRegistryHub::~IRegistryHub() = default;
    IMutableRegistryHub::~IMutableRegistryHub() = default;
}




namespace Catch {
    namespace Detail {
        void missingCaptureInstance() {
            CATCH_INTERNAL_ERROR( "No result capture instance" );
        }
    } // namespace Detail

    IResultCapture::~IResultCapture() = default;
} // namespace Catch




namespace Catch {
    ITestCaseRegistry::~ITestCaseRegistry() = default;
}



namespace Catch {

    void AssertionHandler::finishIncomplete() {
        m_resultCapture.handleIncomplete( m_assertionInfo );
    }

    AssertionHandler::AssertionHandler
        (   StringRef macroName,
            SourceLineInfo const& lineInfo,
            StringRef capturedExpression,
            ResultDisposition::Flags resultDisposition )
    :   m_assertionInfo{ macroName, lineInfo, capturedExpression, resultDisposition },
        m_resultCapture( static_cast<RunContext&>(getResultCapture()) )
    {
        m_resultCapture.notifyAssertionStarted( m_assertionInfo );
    }

    void AssertionHandler::handleExpr( ITransientExpression const& expr ) {
        m_resultCapture.handleExpr( m_assertionInfo, expr, m_reaction );
    }
    void AssertionHandler::handleMessage(ResultWas::OfType resultType, std::string&& message) {
        m_resultCapture.handleMessage( m_assertionInfo, resultType, CATCH_MOVE(message), m_reaction );
    }

    auto AssertionHandler::allowThrows() const -> bool {
        return getCurrentContext().getConfig()->allowThrows();
    }

    void AssertionHandler::complete() {
        m_completed = true;
        if( m_reaction.shouldDebugBreak ) {

            // If you find your debugger stopping you here then go one level up on the
            // call-stack for the code that caused it (typically a failed assertion)

            // (To go back to the test and change execution, jump over the throw, next)
            CATCH_BREAK_INTO_DEBUGGER();
        }
        if (m_reaction.shouldThrow) {
            throw_test_failure_exception();
        }
        if ( m_reaction.shouldSkip ) {
            throw_test_skip_exception();
        }
    }

    void AssertionHandler::handleUnexpectedInflightException() noexcept {
        m_resultCapture.handleUnexpectedInflightException( m_assertionInfo, Catch::translateActiveException(), m_reaction );
    }

    void AssertionHandler::handleExceptionThrownAsExpected() {
        m_resultCapture.handleNonExpr(m_assertionInfo, ResultWas::Ok, m_reaction);
    }
    void AssertionHandler::handleExceptionNotThrownAsExpected() {
        m_resultCapture.handleNonExpr(m_assertionInfo, ResultWas::Ok, m_reaction);
    }

    void AssertionHandler::handleUnexpectedExceptionNotThrown() {
        m_resultCapture.handleUnexpectedExceptionNotThrown( m_assertionInfo, m_reaction );
    }

    void AssertionHandler::handleThrowingCallSkipped() {
        m_resultCapture.handleNonExpr(m_assertionInfo, ResultWas::Ok, m_reaction);
    }

    // This is the overload that takes a string and infers the Equals matcher from it
    // The more general overload, that takes any string matcher, is in catch_capture_matchers.cpp
    void handleExceptionMatchExpr( AssertionHandler& handler, std::string const& str ) {
        handleExceptionMatchExpr( handler, Matchers::Equals( str ) );
    }

} // namespace Catch




#include <algorithm>

namespace Catch {
    namespace Detail {

        bool CaseInsensitiveLess::operator()( StringRef lhs,
                                              StringRef rhs ) const {
            return std::lexicographical_compare(
                lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end(),
                []( char l, char r ) { return toLower( l ) < toLower( r ); } );
        }

        bool
        CaseInsensitiveEqualTo::operator()( StringRef lhs,
                                            StringRef rhs ) const {
            return std::equal(
                lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end(),
                []( char l, char r ) { return toLower( l ) == toLower( r ); } );
        }

    } // namespace Detail
} // namespace Catch



namespace Catch {

    Context Context::currentContext;

    Context& getCurrentMutableContext() {
        return Context::currentContext;
    }

    SimplePcg32& sharedRng() {
        static SimplePcg32 s_rng;
        return s_rng;
    }

}



#if defined(CATCH_PLATFORM_MAC) || defined(CATCH_PLATFORM_IPHONE)

#  include <cassert>
#  include <sys/types.h>
#  include <unistd.h>
#  include <cstddef>
#  include <ostream>

#ifdef __apple_build_version__
    // These headers will only compile with AppleClang (XCode)
    // For other compilers (Clang, GCC, ... ) we need to exclude them
#  include <sys/sysctl.h>
#endif

    namespace Catch {
        #ifdef __apple_build_version__
        // The following function is taken directly from the following technical note:
        // https://developer.apple.com/library/archive/qa/qa1361/_index.html

        // Returns true if the current process is being debugged (either
        // running under the debugger or has a debugger attached post facto).
        bool isDebuggerActive(){
            int                 mib[4];
            struct kinfo_proc   info;
            std::size_t         size;

            // Initialize the flags so that, if sysctl fails for some bizarre
            // reason, we get a predictable result.

            info.kp_proc.p_flag = 0;

            // Initialize mib, which tells sysctl the info we want, in this case
            // we're looking for information about a specific process ID.

            mib[0] = CTL_KERN;
            mib[1] = KERN_PROC;
            mib[2] = KERN_PROC_PID;
            mib[3] = getpid();

            // Call sysctl.

            size = sizeof(info);
            if( sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, nullptr, 0) != 0 ) {
                Catch::cerr() << "\n** Call to sysctl failed - unable to determine if debugger is active **\n\n" << std::flush;
                return false;
            }

            // We're being debugged if the P_TRACED flag is set.

            return ( (info.kp_proc.p_flag & P_TRACED) != 0 );
        }
        #else
        bool isDebuggerActive() {
            // We need to find another way to determine this for non-appleclang compilers on macOS
            return false;
        }
        #endif
    } // namespace Catch

#elif defined(CATCH_PLATFORM_LINUX) || defined(CATCH_PLATFORM_QNX)
    #include <fstream>
    #include <string>

    namespace Catch{
        // The standard POSIX way of detecting a debugger is to attempt to
        // ptrace() the process, but this needs to be done from a child and not
        // this process itself to still allow attaching to this process later
        // if wanted, so is rather heavy. Under Linux we have the PID of the
        // "debugger" (which doesn't need to be gdb, of course, it could also
        // be strace, for example) in /proc/$PID/status, so just get it from
        // there instead.
        bool isDebuggerActive(){
            // Libstdc++ has a bug, where std::ifstream sets errno to 0
            // This way our users can properly assert over errno values
            ErrnoGuard guard;
            std::ifstream in("/proc/self/status");
            for( std::string line; std::getline(in, line); ) {
                static const int PREFIX_LEN = 11;
                if( line.compare(0, PREFIX_LEN, "TracerPid:\t") == 0 ) {
                    // We're traced if the PID is not 0 and no other PID starts
                    // with 0 digit, so it's enough to check for just a single
                    // character.
                    return line.length() > PREFIX_LEN && line[PREFIX_LEN] != '0';
                }
            }

            return false;
        }
    } // namespace Catch
#elif defined(_MSC_VER)
    extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
    namespace Catch {
        bool isDebuggerActive() {
            return IsDebuggerPresent() != 0;
        }
    }
#elif defined(__MINGW32__)
    extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
    namespace Catch {
        bool isDebuggerActive() {
            return IsDebuggerPresent() != 0;
        }
    }
#else
    namespace Catch {
       bool isDebuggerActive() { return false; }
    }
#endif // Platform




namespace Catch {

    void formatReconstructedExpression( std::ostream &os, std::string const& lhs, StringRef op, std::string const& rhs ) {
        if( lhs.size() + rhs.size() < 40 &&
                lhs.find('\n') == std::string::npos &&
                rhs.find('\n') == std::string::npos )
            os << lhs << ' ' << op << ' ' << rhs;
        else
            os << lhs << '\n' << op << '\n' << rhs;
    }
}



#include <stdexcept>


namespace Catch {
#if defined(CATCH_CONFIG_DISABLE_EXCEPTIONS) && !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS_CUSTOM_HANDLER)
    [[noreturn]]
    void throw_exception(std::exception const& e) {
        Catch::cerr() << "Catch will terminate because it needed to throw an exception.\n"
                      << "The message was: " << e.what() << '\n';
        std::terminate();
    }
#endif

    [[noreturn]]
    void throw_logic_error(std::string const& msg) {
        throw_exception(std::logic_error(msg));
    }

    [[noreturn]]
    void throw_domain_error(std::string const& msg) {
        throw_exception(std::domain_error(msg));
    }

    [[noreturn]]
    void throw_runtime_error(std::string const& msg) {
        throw_exception(std::runtime_error(msg));
    }



} // namespace Catch;




#include <cassert>

namespace Catch {

    namespace Detail {

        namespace {
            // Extracts the actual name part of an enum instance
            // In other words, it returns the Blue part of Bikeshed::Colour::Blue
            StringRef extractInstanceName(StringRef enumInstance) {
                // Find last occurrence of ":"
                size_t name_start = enumInstance.size();
                while (name_start > 0 && enumInstance[name_start - 1] != ':') {
                    --name_start;
                }
                return enumInstance.substr(name_start, enumInstance.size() - name_start);
            }
        }

        std::vector<StringRef> parseEnums( StringRef enums ) {
            auto enumValues = splitStringRef( enums, ',' );
            std::vector<StringRef> parsed;
            parsed.reserve( enumValues.size() );
            for( auto const& enumValue : enumValues ) {
                parsed.push_back(trim(extractInstanceName(enumValue)));
            }
            return parsed;
        }

        StringRef EnumInfo::lookup( int64_t value ) const {
            for( auto const& valueToName : m_values ) {
                if( valueToName.first == value )
                    return valueToName.second;
            }
            return "{** unexpected enum value **}"_sr;
        }

        EnumInfo makeEnumInfo( StringRef enumName, StringRef allValueNames, std::vector<int64_t> const& values ) {
            EnumInfo enumInfo;
            enumInfo.m_name = enumName;
            enumInfo.m_values.reserve( values.size() );

            const auto valueNames = Catch::Detail::parseEnums( allValueNames );
            assert( valueNames.size() == values.size() );
            for (size_t i = 0; i < values.size(); ++i) {
                enumInfo.m_values.emplace_back( values[i], valueNames[i] );
            }

            return enumInfo;
        }

    } // Detail
} // Catch





#include <cerrno>

namespace Catch {
        ErrnoGuard::ErrnoGuard():m_oldErrno(errno){}
        ErrnoGuard::~ErrnoGuard() { errno = m_oldErrno; }
}



#include <exception>

namespace Catch {

#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
    namespace {
        static std::string tryTranslators(
            std::vector<
                Detail::unique_ptr<IExceptionTranslator const>> const& translators ) {
            if ( translators.empty() ) {
                std::rethrow_exception( std::current_exception() );
            } else {
                return translators[0]->translate( translators.begin() + 1,
                                                  translators.end() );
            }
        }

    }
#endif //!defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)

    ExceptionTranslatorRegistry::~ExceptionTranslatorRegistry() = default;

    void ExceptionTranslatorRegistry::registerTranslator( Detail::unique_ptr<IExceptionTranslator>&& translator ) {
        m_translators.push_back( CATCH_MOVE( translator ) );
    }

#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
    std::string ExceptionTranslatorRegistry::translateActiveException() const {
        // Compiling a mixed mode project with MSVC means that CLR
        // exceptions will be caught in (...) as well. However, these do
        // do not fill-in std::current_exception and thus lead to crash
        // when attempting rethrow.
        // /EHa switch also causes structured exceptions to be caught
        // here, but they fill-in current_exception properly, so
        // at worst the output should be a little weird, instead of
        // causing a crash.
        if ( std::current_exception() == nullptr ) {
            return "Non C++ exception. Possibly a CLR exception.";
        }

        // First we try user-registered translators. If none of them can
        // handle the exception, it will be rethrown handled by our defaults.
        try {
            return tryTranslators(m_translators);
        }
        // To avoid having to handle TFE explicitly everywhere, we just
        // rethrow it so that it goes back up the caller.
        catch( TestFailureException& ) {
            return "{ nested assertion failed }";
        }
        catch( TestSkipException& ) {
            return "{ nested SKIP() called }";
        }
        catch( std::exception const& ex ) {
            return ex.what();
        }
        catch( std::string const& msg ) {
            return msg;
        }
        catch( const char* msg ) {
            return msg;
        }
        catch(...) {
            return "Unknown exception";
        }
    }

#else // ^^ Exceptions are enabled // Exceptions are disabled vv
    std::string ExceptionTranslatorRegistry::translateActiveException() const {
        CATCH_INTERNAL_ERROR("Attempted to translate active exception under CATCH_CONFIG_DISABLE_EXCEPTIONS!");
    }
#endif

}



/** \file
 * This file provides platform specific implementations of FatalConditionHandler
 *
 * This means that there is a lot of conditional compilation, and platform
 * specific code. Currently, Catch2 supports a dummy handler (if no
 * handler is desired), and 2 platform specific handlers:
 *  * Windows' SEH
 *  * POSIX signals
 *
 * Consequently, various pieces of code below are compiled if either of
 * the platform specific handlers is enabled, or if none of them are
 * enabled. It is assumed that both cannot be enabled at the same time,
 * and doing so should cause a compilation error.
 *
 * If another platform specific handler is added, the compile guards
 * below will need to be updated taking these assumptions into account.
 */



#include <algorithm>

#if !defined( CATCH_CONFIG_WINDOWS_SEH ) && !defined( CATCH_CONFIG_POSIX_SIGNALS )

namespace Catch {

    // If neither SEH nor signal handling is required, the handler impls
    // do not have to do anything, and can be empty.
    void FatalConditionHandler::engage_platform() {}
    void FatalConditionHandler::disengage_platform() noexcept {}
    FatalConditionHandler::FatalConditionHandler() = default;
    FatalConditionHandler::~FatalConditionHandler() = default;

} // end namespace Catch

#endif // !CATCH_CONFIG_WINDOWS_SEH && !CATCH_CONFIG_POSIX_SIGNALS

#if defined( CATCH_CONFIG_WINDOWS_SEH ) && defined( CATCH_CONFIG_POSIX_SIGNALS )
#error "Inconsistent configuration: Windows' SEH handling and POSIX signals cannot be enabled at the same time"
#endif // CATCH_CONFIG_WINDOWS_SEH && CATCH_CONFIG_POSIX_SIGNALS

#if defined( CATCH_CONFIG_WINDOWS_SEH ) || defined( CATCH_CONFIG_POSIX_SIGNALS )

namespace {
    //! Signals fatal error message to the run context
    void reportFatal( char const * const message ) {
        Catch::getCurrentContext().getResultCapture()->handleFatalErrorCondition( message );
    }

    //! Minimal size Catch2 needs for its own fatal error handling.
    //! Picked empirically, so it might not be sufficient on all
    //! platforms, and for all configurations.
    constexpr std::size_t minStackSizeForErrors = 32 * 1024;
} // end unnamed namespace

#endif // CATCH_CONFIG_WINDOWS_SEH || CATCH_CONFIG_POSIX_SIGNALS

#if defined( CATCH_CONFIG_WINDOWS_SEH )

namespace Catch {

    struct SignalDefs { DWORD id; const char* name; };

    // There is no 1-1 mapping between signals and windows exceptions.
    // Windows can easily distinguish between SO and SigSegV,
    // but SigInt, SigTerm, etc are handled differently.
    static constexpr SignalDefs signalDefs[] = {
        { EXCEPTION_ILLEGAL_INSTRUCTION,  "SIGILL - Illegal instruction signal" },
        { EXCEPTION_STACK_OVERFLOW, "SIGSEGV - Stack overflow" },
        { EXCEPTION_ACCESS_VIOLATION, "SIGSEGV - Segmentation violation signal" },
        { EXCEPTION_INT_DIVIDE_BY_ZERO, "Divide by zero error" },
    };

    // Since we do not support multiple instantiations, we put these
    // into global variables and rely on cleaning them up in outlined
    // constructors/destructors
    static LPTOP_LEVEL_EXCEPTION_FILTER previousTopLevelExceptionFilter = nullptr;


    static LONG CALLBACK topLevelExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo) {
        for (auto const& def : signalDefs) {
            if (ExceptionInfo->ExceptionRecord->ExceptionCode == def.id) {
                reportFatal(def.name);
            }
        }
        // If a filter was previously registered, invoke it
        if (previousTopLevelExceptionFilter) {
            return previousTopLevelExceptionFilter(ExceptionInfo);
        }
        // Otherwise, pass along all exceptions.
        // This stops us from eating debugger breaks etc.
        return EXCEPTION_CONTINUE_SEARCH;
    }

    // For MSVC, we reserve part of the stack memory for handling
    // memory overflow structured exception.
    FatalConditionHandler::FatalConditionHandler() {
        ULONG guaranteeSize = static_cast<ULONG>(minStackSizeForErrors);
        if (!SetThreadStackGuarantee(&guaranteeSize)) {
            // We do not want to fully error out, because needing
            // the stack reserve should be rare enough anyway.
            Catch::cerr()
                << "Failed to reserve piece of stack."
                << " Stack overflows will not be reported successfully.";
        }
    }

    // We do not attempt to unset the stack guarantee, because
    // Windows does not support lowering the stack size guarantee.
    FatalConditionHandler::~FatalConditionHandler() = default;


    void FatalConditionHandler::engage_platform() {
        // Register as a the top level exception filter.
        previousTopLevelExceptionFilter = SetUnhandledExceptionFilter(topLevelExceptionFilter);
    }

    void FatalConditionHandler::disengage_platform() noexcept {
        if (SetUnhandledExceptionFilter(previousTopLevelExceptionFilter) != topLevelExceptionFilter) {
            Catch::cerr()
                << "Unexpected SEH unhandled exception filter on disengage."
                << " The filter was restored, but might be rolled back unexpectedly.";
        }
        previousTopLevelExceptionFilter = nullptr;
    }

} // end namespace Catch

#endif // CATCH_CONFIG_WINDOWS_SEH

#if defined( CATCH_CONFIG_POSIX_SIGNALS )

#include <signal.h>

namespace Catch {

    struct SignalDefs {
        int id;
        const char* name;
    };

    static constexpr SignalDefs signalDefs[] = {
        { SIGINT,  "SIGINT - Terminal interrupt signal" },
        { SIGILL,  "SIGILL - Illegal instruction signal" },
        { SIGFPE,  "SIGFPE - Floating point error signal" },
        { SIGSEGV, "SIGSEGV - Segmentation violation signal" },
        { SIGTERM, "SIGTERM - Termination request signal" },
        { SIGABRT, "SIGABRT - Abort (abnormal termination) signal" }
    };

// Older GCCs trigger -Wmissing-field-initializers for T foo = {}
// which is zero initialization, but not explicit. We want to avoid
// that.
#if defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

    static char* altStackMem = nullptr;
    static std::size_t altStackSize = 0;
    static stack_t oldSigStack{};
    static struct sigaction oldSigActions[sizeof(signalDefs) / sizeof(SignalDefs)]{};

    static void restorePreviousSignalHandlers() noexcept {
        // We set signal handlers back to the previous ones. Hopefully
        // nobody overwrote them in the meantime, and doesn't expect
        // their signal handlers to live past ours given that they
        // installed them after ours..
        for (std::size_t i = 0; i < sizeof(signalDefs) / sizeof(SignalDefs); ++i) {
            sigaction(signalDefs[i].id, &oldSigActions[i], nullptr);
        }
        // Return the old stack
        sigaltstack(&oldSigStack, nullptr);
    }

    static void handleSignal( int sig ) {
        char const * name = "<unknown signal>";
        for (auto const& def : signalDefs) {
            if (sig == def.id) {
                name = def.name;
                break;
            }
        }
        // We need to restore previous signal handlers and let them do
        // their thing, so that the users can have the debugger break
        // when a signal is raised, and so on.
        restorePreviousSignalHandlers();
        reportFatal( name );
        raise( sig );
    }

    FatalConditionHandler::FatalConditionHandler() {
        assert(!altStackMem && "Cannot initialize POSIX signal handler when one already exists");
        if (altStackSize == 0) {
            altStackSize = std::max(static_cast<size_t>(SIGSTKSZ), minStackSizeForErrors);
        }
        altStackMem = new char[altStackSize]();
    }

    FatalConditionHandler::~FatalConditionHandler() {
        delete[] altStackMem;
        // We signal that another instance can be constructed by zeroing
        // out the pointer.
        altStackMem = nullptr;
    }

    void FatalConditionHandler::engage_platform() {
        stack_t sigStack;
        sigStack.ss_sp = altStackMem;
        sigStack.ss_size = altStackSize;
        sigStack.ss_flags = 0;
        sigaltstack(&sigStack, &oldSigStack);
        struct sigaction sa = { };

        sa.sa_handler = handleSignal;
        sa.sa_flags = SA_ONSTACK;
        for (std::size_t i = 0; i < sizeof(signalDefs)/sizeof(SignalDefs); ++i) {
            sigaction(signalDefs[i].id, &sa, &oldSigActions[i]);
        }
    }

#if defined(__GNUC__)
#    pragma GCC diagnostic pop
#endif


    void FatalConditionHandler::disengage_platform() noexcept {
        restorePreviousSignalHandlers();
    }

} // end namespace Catch

#endif // CATCH_CONFIG_POSIX_SIGNALS




#include <cstring>

namespace Catch {
    namespace Detail {

        uint32_t convertToBits(float f) {
            static_assert(sizeof(float) == sizeof(uint32_t), "Important ULP matcher assumption violated");
            uint32_t i;
            std::memcpy(&i, &f, sizeof(f));
            return i;
        }

        uint64_t convertToBits(double d) {
            static_assert(sizeof(double) == sizeof(uint64_t), "Important ULP matcher assumption violated");
            uint64_t i;
            std::memcpy(&i, &d, sizeof(d));
            return i;
        }

#if defined( __GNUC__ ) || defined( __clang__ )
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
        bool directCompare( float lhs, float rhs ) { return lhs == rhs; }
        bool directCompare( double lhs, double rhs ) { return lhs == rhs; }
#if defined( __GNUC__ ) || defined( __clang__ )
#    pragma GCC diagnostic pop
#endif


    } // end namespace Detail
} // end namespace Catch






#include <cstdlib>

namespace Catch {
    namespace Detail {

#if !defined (CATCH_CONFIG_GETENV)
        char const* getEnv( char const* ) { return nullptr; }
#else

        char const* getEnv( char const* varName ) {
#    if defined( _MSC_VER )
#        pragma warning( push )
#        pragma warning( disable : 4996 ) // use getenv_s instead of getenv
#    endif

            return std::getenv( varName );

#    if defined( _MSC_VER )
#        pragma warning( pop )
#    endif
        }
#endif
} // namespace Detail
} // namespace Catch




namespace Catch {

    auto operator << (std::ostream& os, LazyExpression const& lazyExpr) -> std::ostream& {
        if (lazyExpr.m_isNegated)
            os << '!';

        if (lazyExpr) {
            if (lazyExpr.m_isNegated && lazyExpr.m_transientExpression->isBinaryExpression())
                os << '(' << *lazyExpr.m_transientExpression << ')';
            else
                os << *lazyExpr.m_transientExpression;
        } else {
            os << "{** error - unchecked empty expression requested **}";
        }
        return os;
    }

} // namespace Catch




#ifdef CATCH_CONFIG_WINDOWS_CRTDBG
#include <crtdbg.h>

namespace Catch {

    LeakDetector::LeakDetector() {
        int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        flag |= _CRTDBG_LEAK_CHECK_DF;
        flag |= _CRTDBG_ALLOC_MEM_DF;
        _CrtSetDbgFlag(flag);
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
        // Change this to leaking allocation's number to break there
        _CrtSetBreakAlloc(-1);
    }
}

#else // ^^ Windows crt debug heap enabled // Windows crt debug heap disabled vv

    Catch::LeakDetector::LeakDetector() = default;

#endif // CATCH_CONFIG_WINDOWS_CRTDBG

Catch::LeakDetector::~LeakDetector() {
    Catch::cleanUp();
}



namespace Catch {
    CATCH_INTERNAL_START_WARNINGS_SUPPRESSION
    CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS
    static const LeakDetector leakDetector;
    CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION
}

// Allow users of amalgamated .cpp file to remove our main and provide their own.
#if !defined(CATCH_AMALGAMATED_CUSTOM_MAIN)

#if defined(CATCH_CONFIG_WCHAR) && defined(CATCH_PLATFORM_WINDOWS) && defined(_UNICODE) && !defined(DO_NOT_USE_WMAIN)
// Standard C/C++ Win32 Unicode wmain entry point
extern "C" int __cdecl wmain (int argc, wchar_t * argv[], wchar_t * []) {
#else
// Standard C/C++ main entry point
int main (int argc, char * argv[]) {
#endif

    // We want to force the linker not to discard the global variable
    // and its constructor, as it (optionally) registers leak detector
    (void)&Catch::leakDetector;

    return Catch::Session().run( argc, argv );
}

#endif // !defined(CATCH_AMALGAMATED_CUSTOM_MAIN




namespace Catch {

    namespace {
        // Messages are owned by their individual threads, so the counter should
        // be thread-local as well. Alternative consideration: atomic counter,
        // so threads don't share IDs and things are easier to debug.
        static CATCH_INTERNAL_THREAD_LOCAL unsigned int messageIDCounter = 0;
    }

    MessageInfo::MessageInfo( StringRef _macroName,
                              SourceLineInfo const& _lineInfo,
                              ResultWas::OfType _type )
    :   macroName( _macroName ),
        lineInfo( _lineInfo ),
        type( _type ),
        sequence( ++messageIDCounter )
    {}

} // end namespace Catch



#include <cstdio>
#include <cstring>
#include <iosfwd>
#include <sstream>

#if defined( CATCH_CONFIG_NEW_CAPTURE )
#    if defined( _MSC_VER )
#        include <io.h> //_dup and _dup2
#        define dup _dup
#        define dup2 _dup2
#        define fileno _fileno
#    else
#        include <unistd.h> // dup and dup2
#    endif
#endif

namespace Catch {

    namespace {
        //! A no-op implementation, used if no reporter wants output
        //! redirection.
        class NoopRedirect final : public OutputRedirect {
            void activateImpl() override {}
            void deactivateImpl() override {}
            std::string getStdout() override { return {}; }
            std::string getStderr() override { return {}; }
            void clearBuffers() override {}
        };

        /**
         * Redirects specific stream's rdbuf with another's.
         *
         * Redirection can be stopped and started on-demand, assumes
         * that the underlying stream's rdbuf aren't changed by other
         * users while it is active. The rdbuf to restore is the one the
         * stream had when the redirection started, not when this object
         * was made.
         */
        class RedirectedStreamNew {
            std::ostream& m_originalStream;
            std::ostream& m_redirectionStream;
            std::streambuf* m_prevBuf = nullptr;

        public:
            RedirectedStreamNew( std::ostream& originalStream,
                                 std::ostream& redirectionStream ):
                m_originalStream( originalStream ),
                m_redirectionStream( redirectionStream ) {}

            void startRedirect() {
                m_prevBuf = m_originalStream.rdbuf();
                m_originalStream.rdbuf( m_redirectionStream.rdbuf() );
            }
            void stopRedirect() { m_originalStream.rdbuf( m_prevBuf ); }
        };

        /**
         * Redirects the `std::cout`, `std::cerr`, `std::clog` streams,
         * but does not touch the actual `stdout`/`stderr` file descriptors.
         */
        class StreamRedirect final : public OutputRedirect {
            ReusableStringStream m_redirectedOut, m_redirectedErr;
            RedirectedStreamNew m_cout, m_cerr, m_clog;

        public:
            StreamRedirect():
                m_cout( Catch::cout(), m_redirectedOut.get() ),
                m_cerr( Catch::cerr(), m_redirectedErr.get() ),
                m_clog( Catch::clog(), m_redirectedErr.get() ) {}

            void activateImpl() override {
                m_cout.startRedirect();
                m_cerr.startRedirect();
                m_clog.startRedirect();
            }
            void deactivateImpl() override {
                m_cout.stopRedirect();
                m_cerr.stopRedirect();
                m_clog.stopRedirect();
            }
            std::string getStdout() override { return m_redirectedOut.str(); }
            std::string getStderr() override { return m_redirectedErr.str(); }
            void clearBuffers() override {
                m_redirectedOut.str( "" );
                m_redirectedErr.str( "" );
            }
        };

#if defined( CATCH_CONFIG_NEW_CAPTURE )

        // Windows's implementation of std::tmpfile is terrible (it tries
        // to create a file inside system folder, thus requiring elevated
        // privileges for the binary), so we have to use tmpnam(_s) and
        // create the file ourselves there.
        class TempFile {
        public:
            TempFile( TempFile const& ) = delete;
            TempFile& operator=( TempFile const& ) = delete;
            TempFile( TempFile&& ) = delete;
            TempFile& operator=( TempFile&& ) = delete;

#    if defined( _MSC_VER )
            TempFile() {
                if ( tmpnam_s( m_buffer ) ) {
                    CATCH_RUNTIME_ERROR( "Could not get a temp filename" );
                }
                if ( fopen_s( &m_file, m_buffer, "wb+" ) ) {
                    char buffer[100];
                    if ( strerror_s( buffer, errno ) ) {
                        CATCH_RUNTIME_ERROR(
                            "Could not translate errno to a string" );
                    }
                    CATCH_RUNTIME_ERROR( "Could not open the temp file: '"
                                         << m_buffer
                                         << "' because: " << buffer );
                }
            }
#    else
            TempFile() {
                m_file = std::tmpfile();
                if ( !m_file ) {
                    CATCH_RUNTIME_ERROR( "Could not create a temp file." );
                }
            }
#    endif

            ~TempFile() {
                // TBD: What to do about errors here?
                std::fclose( m_file );
                // We manually create the file on Windows only, on Linux
                // it will be autodeleted
#    if defined( _MSC_VER )
                std::remove( m_buffer );
#    endif
            }

            std::FILE* getFile() { return m_file; }
            std::string getContents() {
                ReusableStringStream sstr;
                constexpr long buffer_size = 100;
                char buffer[buffer_size + 1] = {};
                long current_pos = ftell( m_file );
                CATCH_ENFORCE( current_pos >= 0,
                               "ftell failed, errno: " << errno );
                std::rewind( m_file );
                while ( current_pos > 0 ) {
                    auto read_characters =
                        std::fread( buffer,
                                    1,
                                    std::min( buffer_size, current_pos ),
                                    m_file );
                    buffer[read_characters] = '\0';
                    sstr << buffer;
                    current_pos -= static_cast<long>( read_characters );
                }
                return sstr.str();
            }

            void clear() { std::rewind( m_file ); }

        private:
            std::FILE* m_file = nullptr;
            char m_buffer[L_tmpnam] = { 0 };
        };

        /**
         * Redirects the actual `stdout`/`stderr` file descriptors.
         *
         * Works by replacing the file descriptors numbered 1 and 2
         * with an open temporary file.
         */
        class FileRedirect final : public OutputRedirect {
            TempFile m_outFile, m_errFile;
            int m_originalOut = -1;
            int m_originalErr = -1;

            // Flushes cout/cerr/clog streams and stdout/stderr FDs
            void flushEverything() {
                Catch::cout() << std::flush;
                fflush( stdout );
                // Since we support overriding these streams, we flush cerr
                // even though std::cerr is unbuffered
                Catch::cerr() << std::flush;
                Catch::clog() << std::flush;
                fflush( stderr );
            }

        public:
            FileRedirect():
                m_originalOut( dup( fileno( stdout ) ) ),
                m_originalErr( dup( fileno( stderr ) ) ) {
                CATCH_ENFORCE( m_originalOut >= 0, "Could not dup stdout" );
                CATCH_ENFORCE( m_originalErr >= 0, "Could not dup stderr" );
            }

            std::string getStdout() override { return m_outFile.getContents(); }
            std::string getStderr() override { return m_errFile.getContents(); }
            void clearBuffers() override {
                m_outFile.clear();
                m_errFile.clear();
            }

            void activateImpl() override {
                // We flush before starting redirect, to ensure that we do
                // not capture the end of message sent before activation.
                flushEverything();

                int ret;
                ret = dup2( fileno( m_outFile.getFile() ), fileno( stdout ) );
                CATCH_ENFORCE( ret >= 0,
                               "dup2 to stdout has failed, errno: " << errno );
                ret = dup2( fileno( m_errFile.getFile() ), fileno( stderr ) );
                CATCH_ENFORCE( ret >= 0,
                               "dup2 to stderr has failed, errno: " << errno );
            }
            void deactivateImpl() override {
                // We flush before ending redirect, to ensure that we
                // capture all messages sent while the redirect was active.
                flushEverything();

                int ret;
                ret = dup2( m_originalOut, fileno( stdout ) );
                CATCH_ENFORCE(
                    ret >= 0,
                    "dup2 of original stdout has failed, errno: " << errno );
                ret = dup2( m_originalErr, fileno( stderr ) );
                CATCH_ENFORCE(
                    ret >= 0,
                    "dup2 of original stderr has failed, errno: " << errno );
            }
        };

#endif // CATCH_CONFIG_NEW_CAPTURE

    } // end namespace

    bool isRedirectAvailable( OutputRedirect::Kind kind ) {
        switch ( kind ) {
        // These two are always available
        case OutputRedirect::None:
        case OutputRedirect::Streams:
            return true;
#if defined( CATCH_CONFIG_NEW_CAPTURE )
        case OutputRedirect::FileDescriptors:
            return true;
#endif
        default:
            return false;
        }
    }

    Detail::unique_ptr<OutputRedirect> makeOutputRedirect( bool actual ) {
        if ( actual ) {
            // TODO: Clean this up later
#if defined( CATCH_CONFIG_NEW_CAPTURE )
            return Detail::make_unique<FileRedirect>();
#else
            return Detail::make_unique<StreamRedirect>();
#endif
        } else {
            return Detail::make_unique<NoopRedirect>();
        }
    }

    RedirectGuard scopedActivate( OutputRedirect& redirectImpl ) {
        return RedirectGuard( true, redirectImpl );
    }

    RedirectGuard scopedDeactivate( OutputRedirect& redirectImpl ) {
        return RedirectGuard( false, redirectImpl );
    }

    OutputRedirect::~OutputRedirect() = default;

    RedirectGuard::RedirectGuard( bool activate, OutputRedirect& redirectImpl ):
        m_redirect( &redirectImpl ),
        m_activate( activate ),
        m_previouslyActive( redirectImpl.isActive() ) {

        // Skip cases where there is no actual state change.
        if ( m_activate == m_previouslyActive ) { return; }

        if ( m_activate ) {
            m_redirect->activate();
        } else {
            m_redirect->deactivate();
        }
    }

    RedirectGuard::~RedirectGuard() noexcept( false ) {
        if ( m_moved ) { return; }
        // Skip cases where there is no actual state change.
        if ( m_activate == m_previouslyActive ) { return; }

        if ( m_activate ) {
            m_redirect->deactivate();
        } else {
            m_redirect->activate();
        }
    }

    RedirectGuard::RedirectGuard( RedirectGuard&& rhs ) noexcept:
        m_redirect( rhs.m_redirect ),
        m_activate( rhs.m_activate ),
        m_previouslyActive( rhs.m_previouslyActive ),
        m_moved( false ) {
        rhs.m_moved = true;
    }

    RedirectGuard& RedirectGuard::operator=( RedirectGuard&& rhs ) noexcept {
        m_redirect = rhs.m_redirect;
        m_activate = rhs.m_activate;
        m_previouslyActive = rhs.m_previouslyActive;
        m_moved = false;
        rhs.m_moved = true;
        return *this;
    }

} // namespace Catch

#if defined( CATCH_CONFIG_NEW_CAPTURE )
#    if defined( _MSC_VER )
#        undef dup
#        undef dup2
#        undef fileno
#    endif
#endif




#include <limits>
#include <stdexcept>

namespace Catch {

    Optional<unsigned int> parseUInt(std::string const& input, int base) {
        auto trimmed = trim( input );
        // std::stoull is annoying and accepts numbers starting with '-',
        // it just negates them into unsigned int
        if ( trimmed.empty() || trimmed[0] == '-' ) {
            return {};
        }

        CATCH_TRY {
            size_t pos = 0;
            const auto ret = std::stoull( trimmed, &pos, base );

            // We did not consume the whole input, so there is an issue
            // This can be bunch of different stuff, like multiple numbers
            // in the input, or invalid digits/characters and so on. Either
            // way, we do not want to return the partially parsed result.
            if ( pos != trimmed.size() ) {
                return {};
            }
            // Too large
            if ( ret > std::numeric_limits<unsigned int>::max() ) {
                return {};
            }
            return static_cast<unsigned int>(ret);
        }
        CATCH_CATCH_ANON( std::invalid_argument const& ) {
            // no conversion could be performed
        }
        CATCH_CATCH_ANON( std::out_of_range const& ) {
            // the input does not fit into an unsigned long long
        }
        return {};
    }

} // namespace Catch




#include <cmath>

namespace Catch {

#if !defined(CATCH_CONFIG_POLYFILL_ISNAN)
    bool isnan(float f) {
        return std::isnan(f);
    }
    bool isnan(double d) {
        return std::isnan(d);
    }
#else
    // For now we only use this for embarcadero
    bool isnan(float f) {
        return std::_isnan(f);
    }
    bool isnan(double d) {
        return std::_isnan(d);
    }
#endif

#if !defined( CATCH_CONFIG_GLOBAL_NEXTAFTER )
    float nextafter( float x, float y ) { return std::nextafter( x, y ); }
    double nextafter( double x, double y ) { return std::nextafter( x, y ); }
#else
    float nextafter( float x, float y ) { return ::nextafterf( x, y ); }
    double nextafter( double x, double y ) { return ::nextafter( x, y ); }
#endif

} // end namespace Catch



#if defined( __clang__ )
#    define CATCH2_CLANG_NO_SANITIZE_INTEGER \
        __attribute__( ( no_sanitize( "unsigned-integer-overflow" ) ) )
#else
#    define CATCH2_CLANG_NO_SANITIZE_INTEGER
#endif
namespace Catch {

namespace {

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4146) // we negate uint32 during the rotate
#endif
        // Safe rotr implementation thanks to John Regehr
        CATCH2_CLANG_NO_SANITIZE_INTEGER
        uint32_t rotate_right(uint32_t val, uint32_t count) {
            const uint32_t mask = 31;
            count &= mask;
            return (val >> count) | (val << (-count & mask));
        }

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

}


    SimplePcg32::SimplePcg32(result_type seed_) {
        seed(seed_);
    }


    void SimplePcg32::seed(result_type seed_) {
        m_state = 0;
        (*this)();
        m_state += seed_;
        (*this)();
    }

    void SimplePcg32::discard(uint64_t skip) {
        // We could implement this to run in O(log n) steps, but this
        // should suffice for our use case.
        for (uint64_t s = 0; s < skip; ++s) {
            static_cast<void>((*this)());
        }
    }

    CATCH2_CLANG_NO_SANITIZE_INTEGER
    SimplePcg32::result_type SimplePcg32::operator()() {
        // prepare the output value
        const uint32_t xorshifted = static_cast<uint32_t>(((m_state >> 18u) ^ m_state) >> 27u);
        const auto output = rotate_right(xorshifted, static_cast<uint32_t>(m_state >> 59u));

        // advance state
        m_state = m_state * 6364136223846793005ULL + s_inc;

        return output;
    }

    bool operator==(SimplePcg32 const& lhs, SimplePcg32 const& rhs) {
        return lhs.m_state == rhs.m_state;
    }

    bool operator!=(SimplePcg32 const& lhs, SimplePcg32 const& rhs) {
        return lhs.m_state != rhs.m_state;
    }
}





#include <ctime>
#include <random>

namespace Catch {

    std::uint32_t generateRandomSeed( GenerateFrom from ) {
        switch ( from ) {
        case GenerateFrom::Time:
            return static_cast<std::uint32_t>( std::time( nullptr ) );

        case GenerateFrom::Default:
        case GenerateFrom::RandomDevice: {
            std::random_device rd;
            return Detail::fillBitsFrom<std::uint32_t>( rd );
        }

        default:
            CATCH_ERROR("Unknown generation method");
        }
    }

} // end namespace Catch




namespace Catch {
    struct ReporterRegistry::ReporterRegistryImpl {
        std::vector<Detail::unique_ptr<EventListenerFactory>> listeners;
    };

    ReporterRegistry::ReporterRegistry():
        m_impl( Detail::make_unique<ReporterRegistryImpl>() ) {}

    ReporterRegistry::~ReporterRegistry() = default;

    void ReporterRegistry::registerListener(
        Detail::unique_ptr<EventListenerFactory> factory ) {
        m_impl->listeners.push_back( CATCH_MOVE( factory ) );
    }

    std::vector<Detail::unique_ptr<EventListenerFactory>> const&
    ReporterRegistry::getListeners() const {
        return m_impl->listeners;
    }
} // namespace Catch



#include <cstdio>
#include <list>
#include <sstream>
#include <tuple>
#include <vector>

namespace Catch {

    // This class encapsulates the idea of a pool of ostringstreams that can be reused.
    struct StringStreams {
        std::vector<Detail::unique_ptr<std::ostringstream>> m_streams;
        // The library can be linked into ASan-instrumented clients without
        // instrumenting Catch3 itself. A list avoids mixed-mode libc++ vector
        // container annotations for the reusable-index pool.
        std::list<std::size_t> m_unused;
        std::ostringstream m_referenceStream; // Used for copy state/ flags from
        Detail::Mutex m_mutex;

        auto add() -> std::pair<std::size_t, std::ostringstream*> {
            Detail::LockGuard _( m_mutex );
            if( m_unused.empty() ) {
                m_streams.push_back( Detail::make_unique<std::ostringstream>() );
                return { m_streams.size()-1, m_streams.back().get() };
            }
            else {
                auto index = m_unused.back();
                m_unused.pop_back();
                return { index, m_streams[index].get() };
            }
        }

        void release( std::size_t index, std::ostream* originalPtr ) {
            assert( originalPtr );
            originalPtr->copyfmt( m_referenceStream );  // Restore initial flags and other state

            Detail::LockGuard _( m_mutex );
            assert( originalPtr == m_streams[index].get() && "Mismatch between release index and stream ptr" );
            m_unused.push_back( index );
        }
    };

    ReusableStringStream::ReusableStringStream() {
        std::tie( m_index, m_oss ) =
            Singleton<StringStreams>::getMutable().add();
    }

    ReusableStringStream::~ReusableStringStream() {
        static_cast<std::ostringstream*>( m_oss )->str("");
        m_oss->clear();
        Singleton<StringStreams>::getMutable().release( m_index, m_oss );
    }

    std::string ReusableStringStream::str() const {
        return static_cast<std::ostringstream*>( m_oss )->str();
    }

    void ReusableStringStream::str( std::string const& str ) {
        static_cast<std::ostringstream*>( m_oss )->str( str );
    }


}




#include <cassert>
#include <algorithm>

namespace Catch {

    namespace Generators {
        namespace {
            struct GeneratorTracker final : TestCaseTracking::TrackerBase,
                                      IGeneratorTracker {
                GeneratorBasePtr m_generator;
                // Filtered generator has moved to specific index due to
                // a filter, it needs special handling of `countedNext()`
                bool m_isFiltered = false;

                GeneratorTracker(
                    TestCaseTracking::NameAndLocation&& nameAndLocation,
                    TrackerContext& ctx,
                    ITracker* parent,
                    GeneratorBasePtr&& generator ):
                    TrackerBase( CATCH_MOVE( nameAndLocation ), ctx, parent ),
                    m_generator( CATCH_MOVE( generator ) ) {
                    assert( m_generator &&
                            "Cannot create tracker without generator" );

                    // Handle potential filter and move forward here...
                    // Old style filters do not affect generators at all
                    if (m_newStyleFilters && m_allTrackerDepth < m_filterRef->size()) {
                        auto const& filter =
                            ( *m_filterRef )[m_allTrackerDepth];
                        // Generator cannot be un-entered the way a section
                        // can be, so the tracker has to throw for a wrong
                        // filter to stop the execution flow.
                        if (filter.type == PathFilter::For::Section) {
                            // We want the semantics of `SKIP()`, but we inline it
                            // to avoid issues with conditionally prefixed macros
                            INTERNAL_CATCH_MSG(
                                "SKIP",
                                Catch::ResultWas::ExplicitSkip,
                                Catch::ResultDisposition::Normal,
                                "" );
                            Catch::Detail::Unreachable();
                        }
                        // '*' is the wildcard for "all elements in generator"
                        // used for filtering sections below the generator, but
                        // not the generator itself.
                        if ( filter.filter != "*" ) {
                            m_isFiltered = true;
                            // TBD: We assume that the filter was validated as
                            //      number during parsing. We should pass it
                            //      as number from the CLI parser.
                            size_t targetIndex = std::stoul( filter.filter );
                            m_generator->skipToNthElement( targetIndex );
                        }
                    }
                }

                static GeneratorTracker*
                acquire( TrackerContext& ctx,
                         TestCaseTracking::NameAndLocationRef const&
                             nameAndLocation ) {
                    GeneratorTracker* tracker;

                    ITracker& currentTracker = ctx.currentTracker();
                    // Under specific circumstances, the generator we want
                    // to acquire is also the current tracker. If this is
                    // the case, we have to avoid looking through current
                    // tracker's children, and instead return the current
                    // tracker.
                    // A case where this check is important is e.g.
                    //     for (int i = 0; i < 5; ++i) {
                    //         int n = GENERATE(1, 2);
                    //     }
                    //
                    // without it, the code above creates 5 nested generators.
                    if ( currentTracker.nameAndLocation() == nameAndLocation ) {
                        auto thisTracker = currentTracker.parent()->findChild(
                            nameAndLocation );
                        assert( thisTracker );
                        assert( thisTracker->isGeneratorTracker() );
                        tracker = static_cast<GeneratorTracker*>( thisTracker );
                    } else if ( ITracker* childTracker =
                                    currentTracker.findChild(
                                        nameAndLocation ) ) {
                        assert( childTracker );
                        assert( childTracker->isGeneratorTracker() );
                        tracker =
                            static_cast<GeneratorTracker*>( childTracker );
                    } else {
                        return nullptr;
                    }

                    if ( !tracker->isComplete() ) { tracker->open(); }

                    return tracker;
                }

                // TrackerBase interface
                bool isGeneratorTracker() const override { return true; }
                void close() override {
                    TrackerBase::close();
                    // If a generator has a child (it is followed by a section)
                    // and none of its children have started, then we must wait
                    // until later to start consuming its values.
                    // This catches cases where `GENERATE` is placed between two
                    // `SECTION`s.
                    // **The check for m_children.empty cannot be removed**.
                    // doing so would break `GENERATE` _not_ followed by
                    // `SECTION`s.
                    const bool should_wait_for_child = [&]() {
                        // No children -> nobody to wait for
                        if ( m_children.empty() ) { return false; }
                        // If at least one child started executing, don't wait
                        if ( std::find_if(
                                 m_children.begin(),
                                 m_children.end(),
                                 []( TestCaseTracking::ITrackerPtr const&
                                         tracker ) {
                                     return tracker->hasStarted();
                                 } ) != m_children.end() ) {
                            return false;
                        }

                        // No children have started. We need to check if they
                        // _can_ start, and thus we should wait for them, or
                        // they cannot start (due to filters), and we shouldn't
                        // wait for them

                        // No filters left -> no restrictions on running sections
                        size_t childDepth = 1 + (m_newStyleFilters ? m_allTrackerDepth : m_sectionOnlyDepth);
                        if ( childDepth >= m_filterRef->size() ) {
                            return true;
                        }

                        // If we are using the new style filters, we need to check
                        // whether the successive filter is for section or a generator.
                        if ( m_newStyleFilters
                            && (*m_filterRef)[childDepth].type != PathFilter::For::Section ) {
                            return false;
                        }
                        // Look for any child section that could match the remaining filters
                        for ( auto const& child : m_children ) {
                            if ( child->isSectionTracker() &&
                                 static_cast<SectionTracker const&>( *child )
                                         .trimmedName() == StringRef((*m_filterRef)[childDepth].filter) ) {
                                return true;
                            }
                        }
                        return false;
                    }();

                    // This check is a bit tricky, because m_generator->next()
                    // has a side-effect, where it consumes generator's current
                    // value, but we do not want to invoke the side-effect if
                    // this generator is still waiting for any child to start.
                    assert( m_generator && "Tracker without generator" );
                    if ( should_wait_for_child
                        ||  ( m_runState == CompletedSuccessfully
                            && !m_isFiltered // filtered generators cannot meaningfully move forward, as they would get past the filter
                            && m_generator->countedNext() ) ) {
                        m_children.clear();
                        m_runState = Executing;
                    }
                }

                // IGeneratorTracker interface
                auto getGenerator() const -> GeneratorBasePtr const& override {
                    return m_generator;
                }

                bool isFilteredImpl() const override { return m_isFiltered; }
            };
        } // namespace
    }

    namespace Detail {
        // Assertions are owned by the thread that is executing them.
        // This allows for lock-free progress in common cases where we
        // do not need to send the assertion events to the reporter.
        // This also implies that messages are owned by their respective
        // threads, and should not be shared across different threads.
        //
        // This implies that various pieces of metadata referring to last
        // assertion result/source location/message handling, etc
        // should also be thread local. For now we just use naked globals
        // below, in the future we will want to allocate piece of memory
        // from heap, to avoid consuming too much thread-local storage.
        //
        // Note that we also don't want non-trivial the thread-local variables
        // below be initialized for every thread, only for those that touch
        // Catch2. To make this work with both GCC/Clang and MSVC, we have to
        // make them thread-local magic statics. (Class-level statics have the
        // desired semantics on GCC, but not on MSVC).

        // This is used for the "if" part of CHECKED_IF/CHECKED_ELSE
        static CATCH_INTERNAL_THREAD_LOCAL bool g_lastAssertionPassed = false;

        // This is the source location for last encountered macro. It is
        // used to provide the users with more precise location of error
        // when an unexpected exception/fatal error happens.
        static CATCH_INTERNAL_THREAD_LOCAL SourceLineInfo
            g_lastKnownLineInfo( "DummyLocation", static_cast<size_t>( -1 ) );

        // Should we clear message scopes before sending off the messages to
        // reporter? Set in `assertionPassedFastPath` to avoid doing the full
        // clear there for performance reasons.
        static CATCH_INTERNAL_THREAD_LOCAL bool g_clearMessageScopes = false;


        // Holds the data for both scoped and unscoped messages together,
        // to avoid issues where their lifetimes start in wrong order,
        // and then are destroyed in wrong order.
        class MessageHolder {
            // The actual message vector passed to the reporters
            std::vector<MessageInfo> messages;
            // IDs of messages from UNSCOPED_X macros, which we have to
            // remove manually.
            std::vector<unsigned int> unscoped_ids;

        public:
            // We do not need to special-case the unscoped messages when
            // we only keep around the raw msg ids.
            ~MessageHolder() = default;

            void addUnscopedMessage( MessageInfo&& info ) {
                repairUnscopedMessageInvariant();
                unscoped_ids.push_back( info.sequence );
                messages.push_back( CATCH_MOVE( info ) );
            }

            void addUnscopedMessage(MessageBuilder&& builder) {
                MessageInfo info( CATCH_MOVE( builder.m_info ) );
                info.message = builder.m_stream.str();
                addUnscopedMessage( CATCH_MOVE( info ) );
            }

            void addScopedMessage(MessageInfo&& info) {
                messages.push_back( CATCH_MOVE( info ) );
            }

            std::vector<MessageInfo> const& getMessages() const {
                return messages;
            }

            void removeMessage( unsigned int messageId ) {
                // Note: On average, it would probably be better to look for
                //       the message backwards. However, we do not expect to have
                //       to  deal with more messages than low single digits, so
                //       the improvement is tiny, and we would have to hand-write
                //       the loop to avoid terrible codegen of reverse iterators
                //       in debug mode.
                auto iter =
                    std::find_if( messages.begin(),
                                  messages.end(),
                                  [messageId]( MessageInfo const& msg ) {
                                      return msg.sequence == messageId;
                                  } );
                assert( iter != messages.end() &&
                        "Trying to remove non-existent message." );
                messages.erase( iter );
            }

            void removeUnscopedMessages() {
                for ( const auto messageId : unscoped_ids ) {
                    removeMessage( messageId );
                }
                unscoped_ids.clear();
                g_clearMessageScopes = false;
            }

            void repairUnscopedMessageInvariant() {
                if ( g_clearMessageScopes ) { removeUnscopedMessages(); }
                g_clearMessageScopes = false;
            }
        };

        CATCH_INTERNAL_START_WARNINGS_SUPPRESSION
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS
        static MessageHolder& g_messageHolder() {
            static CATCH_INTERNAL_THREAD_LOCAL MessageHolder value;
            return value;
        }
        CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION


        void pushScopedMessage( MessageInfo&& message ) {
            Detail::g_messageHolder().addScopedMessage(  CATCH_MOVE( message ) );
        }

        void popScopedMessage( unsigned int messageId ) {
            Detail::g_messageHolder().removeMessage( messageId );
        }

        void emplaceUnscopedMessage( MessageBuilder&& builder ) {
            Detail::g_messageHolder().addUnscopedMessage( CATCH_MOVE( builder ) );
        }

        void addUnscopedMessage( MessageInfo&& message ) {
            Detail::g_messageHolder().addUnscopedMessage( CATCH_MOVE( message ) );
        }

        bool lastAssertionPassed() { return Detail::g_lastAssertionPassed; }

    } // namespace Detail

    RunContext::RunContext(IConfig const* _config, IEventListenerPtr&& reporter)
    :   m_runInfo(_config->name()),
        m_config(_config),
        m_reporter(CATCH_MOVE(reporter)),
        m_outputRedirect( makeOutputRedirect( m_reporter->getPreferences().shouldRedirectStdOut ) ),
        m_abortAfterXFailedAssertions( m_config->abortAfter() ),
        m_reportAssertionStarting( m_reporter->getPreferences().shouldReportAllAssertionStarts ),
        m_includeSuccessfulResults( m_config->includeSuccessfulResults() || m_reporter->getPreferences().shouldReportAllAssertions ),
        m_shouldDebugBreak( m_config->shouldDebugBreak() )
    {
        getCurrentMutableContext().setResultCapture( this );
        m_reporter->testRunStarting(m_runInfo);

        // TODO: HACK!
        //       We need to make sure the underlying cache is initialized
        //       while we are guaranteed to be running in a single thread,
        //       because the initialization is not thread-safe.
        ReusableStringStream rss;
        (void)rss;
    }

    RunContext::~RunContext() {
        updateTotalsFromAtomics();
        m_reporter->testRunEnded(TestRunStats(m_runInfo, m_totals, aborting()));
        // What outlives the run, such as a mock of static lifetime, must not
        // find a capture that is gone.
        if ( getCurrentContext().getResultCapture() == this ) {
            getCurrentMutableContext().setResultCapture( nullptr );
        }
    }

    Totals RunContext::runTest(TestCaseHandle const& testCase) {
        updateTotalsFromAtomics();
        const Totals prevTotals = m_totals;

        auto const& testInfo = testCase.getTestCaseInfo();
        m_reporter->testCaseStarting(testInfo);
        testCase.prepareTestCase();
        m_activeTestCase = &testCase;


        ITracker& rootTracker = m_trackerContext.startRun();
        assert(rootTracker.isSectionTracker());
        rootTracker.setFilters( &m_config->getPathFilters(),
                                m_config->useNewFilterBehaviour() );

        // We intentionally only seed the internal RNG once per test case,
        // before it is first invoked. The reason for that is a complex
        // interplay of generator/section implementation details and the
        // Random*Generator types.
        //
        // The issue boils down to us needing to seed the Random*Generators
        // with different seed each, so that they return different sequences
        // of random numbers. We do this by giving them a number from the
        // shared RNG instance as their seed.
        //
        // However, this runs into an issue if the reseeding happens each
        // time the test case is entered (as opposed to first time only),
        // because multiple generators could get the same seed, e.g. in
        // ```cpp
        // TEST_CASE() {
        //     auto i = GENERATE(take(10, random(0, 100));
        //     SECTION("A") {
        //         auto j = GENERATE(take(10, random(0, 100));
        //     }
        //     SECTION("B") {
        //         auto k = GENERATE(take(10, random(0, 100));
        //     }
        // }
        // ```
        // `i` and `j` would properly return values from different sequences,
        // but `i` and `k` would return the same sequence, because their seed
        // would be the same.
        // (The reason their seeds would be the same is that the generator
        //  for k would be initialized when the test case is entered the second
        //  time, after the shared RNG instance was reset to the same value
        //  it had when the generator for i was initialized.)
        seedRng( *m_config );

        uint64_t testRuns = 0;
        std::string redirectedCout;
        std::string redirectedCerr;
        do {
            m_trackerContext.startCycle();
            m_testCaseTracker = &SectionTracker::acquire(m_trackerContext, TestCaseTracking::NameAndLocationRef(testInfo.name, testInfo.lineInfo));

            m_reporter->testCasePartialStarting(testInfo, testRuns);

            updateTotalsFromAtomics();
            const auto beforeRunTotals = m_totals;
            runCurrentTest();
            std::string oneRunCout = m_outputRedirect->getStdout();
            std::string oneRunCerr = m_outputRedirect->getStderr();
            m_outputRedirect->clearBuffers();
            redirectedCout += oneRunCout;
            redirectedCerr += oneRunCerr;

            updateTotalsFromAtomics();
            const auto singleRunTotals = m_totals.delta(beforeRunTotals);
            auto statsForOneRun = TestCaseStats(testInfo, singleRunTotals, CATCH_MOVE(oneRunCout), CATCH_MOVE(oneRunCerr), aborting());
            m_reporter->testCasePartialEnded(statsForOneRun, testRuns);

            ++testRuns;
        } while (!m_testCaseTracker->isSuccessfullyCompleted() && !aborting());

        Totals deltaTotals = m_totals.delta(prevTotals);
        if (testInfo.expectedToFail() && deltaTotals.testCases.passed > 0) {
            deltaTotals.assertions.failed++;
            deltaTotals.testCases.passed--;
            deltaTotals.testCases.failed++;
        }
        m_totals.testCases += deltaTotals.testCases;
        testCase.tearDownTestCase();
        m_reporter->testCaseEnded(TestCaseStats(testInfo,
                                  deltaTotals,
                                  CATCH_MOVE(redirectedCout),
                                  CATCH_MOVE(redirectedCerr),
                                  aborting()));

        m_activeTestCase = nullptr;
        m_testCaseTracker = nullptr;

        return deltaTotals;
    }


    void RunContext::assertionEnded(AssertionResult&& result) {
        Detail::g_lastKnownLineInfo = result.m_info.lineInfo;
        if (result.getResultType() == ResultWas::Ok) {
            m_atomicAssertionCount.passed++;
            Detail::g_lastAssertionPassed = true;
        } else if (result.getResultType() == ResultWas::ExplicitSkip) {
            m_atomicAssertionCount.skipped++;
            Detail::g_lastAssertionPassed = true;
        } else if (!result.succeeded()) {
            Detail::g_lastAssertionPassed = false;
            if (result.isOk()) {}
            else if( m_activeTestCase->getTestCaseInfo().okToFail() ) { // Read from a shared state established before the threads could start, this is fine
                m_atomicAssertionCount.failedButOk++;
            } else {
                m_atomicAssertionCount.failed++;
            }
        } else {
            Detail::g_lastAssertionPassed = true;
        }

        auto& msgHolder = Detail::g_messageHolder();
        msgHolder.repairUnscopedMessageInvariant();

        // From here, we are touching shared state and need mutex.
        Detail::LockGuard lock( m_assertionMutex );
        {
            auto _ = scopedDeactivate( *m_outputRedirect );
            updateTotalsFromAtomics();
            m_reporter->assertionEnded( AssertionStats( result, msgHolder.getMessages(), m_totals ) );
        }

        if ( result.getResultType() != ResultWas::Warning ) {
            msgHolder.removeUnscopedMessages();
        }

        // Reset working state. assertion info will be reset after
        // populateReaction is run if it is needed
        m_lastResult = CATCH_MOVE( result );
    }

    void RunContext::notifyAssertionStarted( AssertionInfo const& info ) {
        if (m_reportAssertionStarting) {
            Detail::LockGuard lock( m_assertionMutex );
            auto _ = scopedDeactivate( *m_outputRedirect );
            m_reporter->assertionStarting( info );
        }
    }

    bool RunContext::sectionStarted( StringRef sectionName,
                                     SourceLineInfo const& sectionLineInfo,
                                     Counts& assertions ) {
        ITracker& sectionTracker =
            SectionTracker::acquire( m_trackerContext,
                                     TestCaseTracking::NameAndLocationRef(
                                         sectionName, sectionLineInfo ) );

        if (!sectionTracker.isOpen())
            return false;
        m_activeSections.push_back(&sectionTracker);

        SectionInfo sectionInfo( sectionLineInfo, static_cast<std::string>(sectionName) );
        Detail::g_lastKnownLineInfo = sectionLineInfo;

        {
            auto _ = scopedDeactivate( *m_outputRedirect );
            m_reporter->sectionStarting( sectionInfo );
        }

        updateTotalsFromAtomics();
        assertions = m_totals.assertions;

        return true;
    }
    IGeneratorTracker*
    RunContext::acquireGeneratorTracker( StringRef generatorName,
                                         SourceLineInfo const& lineInfo ) {
        auto* tracker = Generators::GeneratorTracker::acquire(
            m_trackerContext,
            TestCaseTracking::NameAndLocationRef(
                 generatorName, lineInfo ) );
        Detail::g_lastKnownLineInfo = lineInfo;
        return tracker;
    }

    IGeneratorTracker* RunContext::createGeneratorTracker(
        StringRef generatorName,
        SourceLineInfo lineInfo,
        Generators::GeneratorBasePtr&& generator ) {

        auto nameAndLoc = TestCaseTracking::NameAndLocation( static_cast<std::string>( generatorName ), lineInfo );
        auto& currentTracker = m_trackerContext.currentTracker();
        assert(
            currentTracker.nameAndLocation() != nameAndLoc &&
            "Trying to create tracker for a generator that already has one" );

        auto newTracker =
            Catch::Detail::make_unique<Generators::GeneratorTracker>(
                CATCH_MOVE( nameAndLoc ),
                m_trackerContext,
                &currentTracker,
                CATCH_MOVE( generator ) );

        // The warning shouldn't fire if the generator is infinite, **but** filtered down.
        if ( m_config->warnAboutInfiniteGenerators() &&
             !newTracker->m_generator->isFinite() &&
             !newTracker->isFiltered() ) {
            // We want the semantics of `FAIL()`, but we inline it
            // to avoid issues with conditionally prefixed macros
            INTERNAL_CATCH_MSG( "FAIL",
                                Catch::ResultWas::ExplicitFailure,
                                Catch::ResultDisposition::Normal,
                                "GENERATE() would run infinitely" );
        }

        auto returnPtr = newTracker.get();
        currentTracker.addChild( CATCH_MOVE( newTracker ) );

        returnPtr->open();
        return returnPtr;
    }

    bool RunContext::testForMissingAssertions(Counts& assertions) {
        if (assertions.total() != 0)
            return false;
        if (!m_config->warnAboutMissingAssertions())
            return false;
        if (m_trackerContext.currentTracker().hasChildren())
            return false;
        m_atomicAssertionCount.failed++;
        assertions.failed++;
        return true;
    }

    void RunContext::sectionEnded(SectionEndInfo&& endInfo) {
        updateTotalsFromAtomics();
        Counts assertions = m_totals.assertions - endInfo.prevAssertions;
        bool missingAssertions = testForMissingAssertions(assertions);

        if (!m_activeSections.empty()) {
            m_activeSections.back()->close();
            m_activeSections.pop_back();
        }

        {
            auto _ = scopedDeactivate( *m_outputRedirect );
            m_reporter->sectionEnded(
                SectionStats( CATCH_MOVE( endInfo.sectionInfo ),
                              assertions,
                              endInfo.durationInSeconds,
                              missingAssertions ) );
        }
    }

    void RunContext::sectionEndedEarly(SectionEndInfo&& endInfo) {
        if ( m_unfinishedSections.empty() ) {
            m_activeSections.back()->fail();
        } else {
            m_activeSections.back()->close();
        }
        m_activeSections.pop_back();

        m_unfinishedSections.push_back(CATCH_MOVE(endInfo));
    }

    void RunContext::benchmarkPreparing( StringRef name ) {
        auto _ = scopedDeactivate( *m_outputRedirect );
        m_reporter->benchmarkPreparing( name );
    }
    void RunContext::benchmarkStarting( BenchmarkInfo const& info ) {
        auto _ = scopedDeactivate( *m_outputRedirect );
        m_reporter->benchmarkStarting( info );
    }
    void RunContext::benchmarkEnded( BenchmarkStats<> const& stats ) {
        auto _ = scopedDeactivate( *m_outputRedirect );
        m_reporter->benchmarkEnded( stats );
    }
    void RunContext::benchmarkFailed( StringRef error ) {
        auto _ = scopedDeactivate( *m_outputRedirect );
        m_reporter->benchmarkFailed( error );
    }

    std::string RunContext::getCurrentTestName() const {
        return m_activeTestCase
            ? m_activeTestCase->getTestCaseInfo().name
            : std::string();
    }

    const AssertionResult * RunContext::getLastResult() const {
        // m_lastResult is updated inside the assertion slow-path, under
        // a mutex, so the read needs to happen under mutex as well.

        // TBD: The last result only makes sense if it is a thread-local
        //      thing, because the answer is different per thread, like
        //      last line info, whether last assertion passed, and so on.
        //
        //      However, the last result was also never updated in the
        //      assertion fast path, so it was always somewhat broken,
        //      and since IResultCapture::getLastResult is deprecated,
        //      we will leave it as is, until it is finally removed.
        Detail::LockGuard _( m_assertionMutex );
        return &*m_lastResult;
    }

    void RunContext::exceptionEarlyReported() {
        m_shouldReportUnexpected = false;
    }

    void RunContext::handleFatalErrorCondition( StringRef message ) {
        // We lock only when touching the reporters directly, to avoid
        // deadlocks when we call into other functions that also want
        // to lock the mutex before touching reporters.
        //
        // This does mean that we allow other threads to run while handling
        // a fatal error, but this is all a best effort attempt anyway.
        {
            Detail::LockGuard lock( m_assertionMutex );
            // TODO: scoped deactivate here? Just give up and do best effort?
            //       the deactivation can break things further, OTOH so can the
            //       capture
            auto _ = scopedDeactivate( *m_outputRedirect );

            // First notify reporter that bad things happened
            m_reporter->fatalErrorEncountered( message );
        }

        // Don't rebuild the result -- the stringification itself can cause more fatal errors
        // Instead, fake a result data.
        AssertionResultData tempResult( ResultWas::FatalErrorCondition, { false } );
        tempResult.message = static_cast<std::string>(message);
        AssertionResult result( makeDummyAssertionInfo(),
                                CATCH_MOVE( tempResult ) );

        assertionEnded(CATCH_MOVE(result) );


        // At this point we touch sections/test cases from this thread
        // to try and end them. Technically that is not supported when
        // using multiple threads, but the worst thing that can happen
        // is that the process aborts harder :-D
        Detail::LockGuard lock( m_assertionMutex );

        // Best effort cleanup for sections that have not been destructed yet
        // Since this is a fatal error, we have not had and won't have the opportunity to destruct them properly
        while (!m_activeSections.empty()) {
            auto const& nl = m_activeSections.back()->nameAndLocation();
            SectionEndInfo endInfo{ SectionInfo(nl.location, nl.name), {}, 0.0 };
            sectionEndedEarly(CATCH_MOVE(endInfo));
        }
        handleUnfinishedSections();

        // Recreate section for test case (as we will lose the one that was in scope)
        auto const& testCaseInfo = m_activeTestCase->getTestCaseInfo();
        SectionInfo testCaseSection(testCaseInfo.lineInfo, testCaseInfo.name);

        Counts assertions;
        assertions.failed = 1;
        SectionStats testCaseSectionStats(CATCH_MOVE(testCaseSection), assertions, 0, false);
        m_reporter->sectionEnded( testCaseSectionStats );

        auto const& testInfo = m_activeTestCase->getTestCaseInfo();

        Totals deltaTotals;
        deltaTotals.testCases.failed = 1;
        deltaTotals.assertions.failed = 1;
        m_reporter->testCaseEnded(TestCaseStats(testInfo,
                                  deltaTotals,
                                  std::string(),
                                  std::string(),
                                  false));
        m_totals.testCases.failed++;
        updateTotalsFromAtomics();
        m_reporter->testRunEnded(TestRunStats(m_runInfo, m_totals, false));
    }

    void RunContext::assertionPassedFastPath(SourceLineInfo lineInfo) {
        // We want to save the line info for better experience with unexpected assertions
        Detail::g_lastKnownLineInfo = lineInfo;
        ++m_atomicAssertionCount.passed;
        Detail::g_lastAssertionPassed = true;
        Detail::g_clearMessageScopes = true;
    }

    void RunContext::updateTotalsFromAtomics() {
        m_totals.assertions = Counts{
            m_atomicAssertionCount.passed,
            m_atomicAssertionCount.failed,
            m_atomicAssertionCount.failedButOk,
            m_atomicAssertionCount.skipped,
        };
    }

    bool RunContext::aborting() const {
        return m_atomicAssertionCount.failed >= m_abortAfterXFailedAssertions;
    }

    std::string RunContext::endCapture() {
        if ( !m_outputRedirect->isActive() ) {
            return {};
        }
        m_outputRedirect->deactivate();
        return m_outputRedirect->getStdout() + m_outputRedirect->getStderr();
    }

    void RunContext::runCurrentTest() {
        auto const& testCaseInfo = m_activeTestCase->getTestCaseInfo();
        SectionInfo testCaseSection(testCaseInfo.lineInfo, testCaseInfo.name);
        m_reporter->sectionStarting(testCaseSection);
        updateTotalsFromAtomics();
        Counts prevAssertions = m_totals.assertions;
        double duration = 0;
        m_shouldReportUnexpected = true;
        Detail::g_lastKnownLineInfo = testCaseInfo.lineInfo;

        Timer timer;
        CATCH_TRY {
            {
                auto _ = scopedActivate( *m_outputRedirect );
                timer.start();
                invokeActiveTestCase();
            }
            duration = timer.getElapsedSeconds();
        } CATCH_CATCH_ANON (TestFailureException&) {
            // This just means the test was aborted due to failure
        } CATCH_CATCH_ANON (TestSkipException&) {
            // This just means the test was explicitly skipped
        } CATCH_CATCH_ALL {
            // Under CATCH_CONFIG_FAST_COMPILE, unexpected exceptions under REQUIRE assertions
            // are reported without translation at the point of origin.
            if ( m_shouldReportUnexpected ) {
                AssertionReaction dummyReaction;
                handleUnexpectedInflightException( makeDummyAssertionInfo(),
                                                   translateActiveException(),
                                                   dummyReaction );
            }
        }
        updateTotalsFromAtomics();
        Counts assertions = m_totals.assertions - prevAssertions;
        bool missingAssertions = testForMissingAssertions(assertions);

        m_testCaseTracker->close();
        handleUnfinishedSections();
        auto& msgHolder = Detail::g_messageHolder();
        msgHolder.removeUnscopedMessages();
        assert( msgHolder.getMessages().empty() &&
                "There should be no leftover messages after the test ends" );

        SectionStats testCaseSectionStats(CATCH_MOVE(testCaseSection), assertions, duration, missingAssertions);
        m_reporter->sectionEnded(testCaseSectionStats);
    }

    void RunContext::invokeActiveTestCase() {
        // We need to engage a handler for signals/structured exceptions
        // before running the tests themselves, or the binary can crash
        // without failed test being reported.
        FatalConditionHandlerGuard _(&m_fatalConditionhandler);
        // We keep having issue where some compilers warn about an unused
        // variable, even though the type has non-trivial constructor and
        // destructor. This is annoying and ugly, but it makes them stfu.
        (void)_;

        m_activeTestCase->invoke();
    }

    void RunContext::handleUnfinishedSections() {
        // If sections ended prematurely due to an exception we stored their
        // infos here so we can tear them down outside the unwind process.
        for ( auto it = m_unfinishedSections.rbegin(),
                   itEnd = m_unfinishedSections.rend();
              it != itEnd;
              ++it ) {
            sectionEnded( CATCH_MOVE( *it ) );
        }
        m_unfinishedSections.clear();
    }

    void RunContext::handleExpr(
        AssertionInfo const& info,
        ITransientExpression const& expr,
        AssertionReaction& reaction
    ) {
        bool negated = isFalseTest( info.resultDisposition );
        bool result = expr.getResult() != negated;

        if( result ) {
            if (!m_includeSuccessfulResults) {
                assertionPassedFastPath(info.lineInfo);
            }
            else {
                reportExpr(info, ResultWas::Ok, &expr, negated);
            }
        }
        else {
            reportExpr(info, ResultWas::ExpressionFailed, &expr, negated );
            populateReaction(
                reaction, info.resultDisposition & ResultDisposition::Normal );
        }
    }
    void RunContext::reportExpr(
            AssertionInfo const &info,
            ResultWas::OfType resultType,
            ITransientExpression const *expr,
            bool negated ) {

        Detail::g_lastKnownLineInfo = info.lineInfo;
        AssertionResultData data( resultType, LazyExpression( negated ) );

        AssertionResult assertionResult{ info, CATCH_MOVE( data ) };
        assertionResult.m_resultData.lazyExpression.m_transientExpression = expr;

        assertionEnded( CATCH_MOVE(assertionResult) );
    }

    void RunContext::handleMessage(
            AssertionInfo const& info,
            ResultWas::OfType resultType,
            std::string&& message,
            AssertionReaction& reaction
    ) {
        Detail::g_lastKnownLineInfo = info.lineInfo;

        AssertionResultData data( resultType, LazyExpression( false ) );
        data.message = CATCH_MOVE( message );
        AssertionResult assertionResult{ info,
                                         CATCH_MOVE( data ) };

        const auto isOk = assertionResult.isOk();
        assertionEnded( CATCH_MOVE(assertionResult) );
        if ( !isOk ) {
            populateReaction(
                reaction, info.resultDisposition & ResultDisposition::Normal );
        } else if ( resultType == ResultWas::ExplicitSkip ) {
            // TODO: Need to handle this explicitly, as ExplicitSkip is
            // considered "OK"
            reaction.shouldSkip = true;
        }
    }

    void RunContext::handleUnexpectedExceptionNotThrown(
            AssertionInfo const& info,
            AssertionReaction& reaction
    ) {
        handleNonExpr(info, Catch::ResultWas::DidntThrowException, reaction);
    }

    void RunContext::handleUnexpectedInflightException(
            AssertionInfo const& info,
            std::string&& message,
            AssertionReaction& reaction
    ) {
        Detail::g_lastKnownLineInfo = info.lineInfo;

        AssertionResultData data( ResultWas::ThrewException, LazyExpression( false ) );
        data.message = CATCH_MOVE(message);
        AssertionResult assertionResult{ info, CATCH_MOVE(data) };
        assertionEnded( CATCH_MOVE(assertionResult) );
        populateReaction( reaction,
                          info.resultDisposition & ResultDisposition::Normal );
    }

    void RunContext::populateReaction( AssertionReaction& reaction,
                                       bool has_normal_disposition ) const {
        reaction.shouldDebugBreak = m_shouldDebugBreak;
        reaction.shouldThrow = aborting() || has_normal_disposition;
    }

    AssertionInfo RunContext::makeDummyAssertionInfo() {
        const bool testCaseJustStarted =
            Detail::g_lastKnownLineInfo ==
            m_activeTestCase->getTestCaseInfo().lineInfo;

        return AssertionInfo{
            testCaseJustStarted ? "TEST_CASE"_sr : StringRef(),
            Detail::g_lastKnownLineInfo,
            testCaseJustStarted ? StringRef() : "{Unknown expression after the reported line}"_sr,
            ResultDisposition::Normal
        };
    }

    void RunContext::handleIncomplete(
            AssertionInfo const& info
    ) {
        using namespace std::string_literals;
        Detail::g_lastKnownLineInfo = info.lineInfo;

        AssertionResultData data( ResultWas::ThrewException, LazyExpression( false ) );
        data.message = "Exception translation was disabled by CATCH_CONFIG_FAST_COMPILE"s;
        AssertionResult assertionResult{ info, CATCH_MOVE( data ) };
        assertionEnded( CATCH_MOVE(assertionResult) );
    }

    void RunContext::handleNonExpr(
            AssertionInfo const &info,
            ResultWas::OfType resultType,
            AssertionReaction &reaction
    ) {
        AssertionResultData data( resultType, LazyExpression( false ) );
        AssertionResult assertionResult{ info, CATCH_MOVE( data ) };

        const auto isOk = assertionResult.isOk();
        if ( isOk && !m_includeSuccessfulResults ) {
            assertionPassedFastPath( info.lineInfo );
            return;
        }

        assertionEnded( CATCH_MOVE(assertionResult) );
        if ( !isOk ) {
            populateReaction(
                reaction, info.resultDisposition & ResultDisposition::Normal );
        }
    }

    void seedRng(IConfig const& config) {
        sharedRng().seed(config.rngSeed());
    }

    unsigned int rngSeed() {
        return getCurrentContext().getConfig()->rngSeed();
    }

}



namespace Catch {

    Section::Section( SectionInfo&& info ):
        m_info( CATCH_MOVE( info ) ),
        m_sectionIncluded(
            getResultCapture().sectionStarted( m_info.name, m_info.lineInfo, m_assertions ) ) {
        // Non-"included" sections will not use the timing information
        // anyway, so don't bother with the potential syscall.
        if (m_sectionIncluded) {
            m_timer.start();
        }
    }

    Section::Section( SourceLineInfo const& _lineInfo,
                      StringRef _name,
                      const char* const ):
        m_info( { "invalid", static_cast<std::size_t>( -1 ) }, std::string{} ),
        m_sectionIncluded(
            getResultCapture().sectionStarted( _name, _lineInfo, m_assertions ) ) {
        // We delay initialization the SectionInfo member until we know
        // this section needs it, so we avoid allocating std::string for name.
        // We also delay timer start to avoid the potential syscall unless we
        // will actually use the result.
        if ( m_sectionIncluded ) {
            m_info.name = static_cast<std::string>( _name );
            m_info.lineInfo = _lineInfo;
            m_timer.start();
        }
    }

    Section::~Section() {
        if( m_sectionIncluded ) {
            SectionEndInfo endInfo{ CATCH_MOVE(m_info), m_assertions, m_timer.getElapsedSeconds() };
            if ( uncaught_exceptions() ) {
                getResultCapture().sectionEndedEarly( CATCH_MOVE(endInfo) );
            } else {
                getResultCapture().sectionEnded( CATCH_MOVE( endInfo ) );
            }
        }
    }

    // This indicates whether the section should be executed or not
    Section::operator bool() const {
        return m_sectionIncluded;
    }


} // end namespace Catch



#include <vector>

namespace Catch {

    namespace {
        static auto getSingletons() -> std::vector<ISingleton*>*& {
            static std::vector<ISingleton*>* g_singletons = nullptr;
            if( !g_singletons )
                g_singletons = new std::vector<ISingleton*>();
            return g_singletons;
        }
    }

    ISingleton::~ISingleton() = default;

    void addSingleton(ISingleton* singleton ) {
        getSingletons()->push_back( singleton );
    }
    void cleanupSingletons() {
        auto& singletons = getSingletons();
        for( auto singleton : *singletons )
            delete singleton;
        delete singletons;
        singletons = nullptr;
    }

} // namespace Catch



#include <cstring>
#include <ostream>

namespace Catch {

    bool SourceLineInfo::operator == ( SourceLineInfo const& other ) const noexcept {
        return line == other.line && (file == other.file || std::strcmp(file, other.file) == 0);
    }
    bool SourceLineInfo::operator < ( SourceLineInfo const& other ) const noexcept {
        // We can assume that the same file will usually have the same pointer.
        // Thus, if the pointers are the same, there is no point in calling the strcmp
        return line < other.line || ( line == other.line && file != other.file && (std::strcmp(file, other.file) < 0));
    }

    std::ostream& operator << ( std::ostream& os, SourceLineInfo const& info ) {
#ifndef __GNUG__
        os << info.file << '(' << info.line << ')';
#else
        os << info.file << ':' << info.line;
#endif
        return os;
    }

} // end namespace Catch




namespace Catch {
#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
    void StartupExceptionRegistry::add( std::exception_ptr const& exception ) noexcept {
        CATCH_TRY {
            m_exceptions.push_back(exception);
        } CATCH_CATCH_ALL {
            // If we run out of memory during start-up there's really not a lot more we can do about it
            std::terminate();
        }
    }

    std::vector<std::exception_ptr> const& StartupExceptionRegistry::getExceptions() const noexcept {
        return m_exceptions;
    }
#endif

} // end namespace Catch





#include <iostream>

namespace Catch {

// If you #define this you must implement these functions
#if !defined( CATCH_CONFIG_NOSTDOUT )
    std::ostream& cout() { return std::cout; }
    std::ostream& cerr() { return std::cerr; }
    std::ostream& clog() { return std::clog; }
#endif

} // namespace Catch



#include <ostream>
#include <cstring>
#include <vector>

namespace Catch {

    bool startsWith( std::string const& s, std::string const& prefix ) {
        return s.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), s.begin());
    }
    bool startsWith( StringRef s, char prefix ) {
        return !s.empty() && s[0] == prefix;
    }
    bool endsWith( std::string const& s, std::string const& suffix ) {
        return s.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
    }
    bool endsWith( std::string const& s, char suffix ) {
        return !s.empty() && s[s.size()-1] == suffix;
    }
    bool contains( std::string const& s, std::string const& infix ) {
        return s.find( infix ) != std::string::npos;
    }
    void toLowerInPlace( std::string& s ) {
        for ( char& c : s ) {
            c = toLower( c );
        }
    }
    std::string toLower( std::string const& s ) {
        std::string lc = s;
        toLowerInPlace( lc );
        return lc;
    }

    std::string trim( std::string const& str ) {
        static char const* whitespaceChars = "\n\r\t ";
        std::string::size_type start = str.find_first_not_of( whitespaceChars );
        std::string::size_type end = str.find_last_not_of( whitespaceChars );

        return start != std::string::npos ? str.substr( start, 1+end-start ) : std::string();
    }

    StringRef trim(StringRef ref) {
        const auto is_ws = [](char c) {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r';
        };
        size_t real_begin = 0;
        while (real_begin < ref.size() && is_ws(ref[real_begin])) { ++real_begin; }
        size_t real_end = ref.size();
        while (real_end > real_begin && is_ws(ref[real_end - 1])) { --real_end; }

        return ref.substr(real_begin, real_end - real_begin);
    }

    bool replaceInPlace( std::string& str, std::string const& replaceThis, std::string const& withThis ) {
        std::size_t i = str.find( replaceThis );
        if (i == std::string::npos) {
            return false;
        }
        std::size_t copyBegin = 0;
        std::string origStr = CATCH_MOVE(str);
        str.clear();
        // There is at least one replacement, so reserve with the best guess
        // we can make without actually counting the number of occurrences.
        str.reserve(origStr.size() - replaceThis.size() + withThis.size());
        do {
            str.append(origStr, copyBegin, i-copyBegin );
            str += withThis;
            copyBegin = i + replaceThis.size();
            if( copyBegin < origStr.size() )
                i = origStr.find( replaceThis, copyBegin );
            else
                i = std::string::npos;
        } while( i != std::string::npos );
        if ( copyBegin < origStr.size() ) {
            str.append(origStr, copyBegin, origStr.size() );
        }
        return true;
    }

    std::vector<StringRef> splitStringRef( StringRef str, char delimiter ) {
        std::vector<StringRef> subStrings;
        std::size_t start = 0;
        for(std::size_t pos = 0; pos < str.size(); ++pos ) {
            if( str[pos] == delimiter ) {
                if( pos - start > 1 )
                    subStrings.push_back( str.substr( start, pos-start ) );
                start = pos+1;
            }
        }
        if( start < str.size() )
            subStrings.push_back( str.substr( start, str.size()-start ) );
        return subStrings;
    }

    std::ostream& operator << ( std::ostream& os, pluralise const& pluraliser ) {
        os << pluraliser.m_count << ' ' << pluraliser.m_label;
        if( pluraliser.m_count != 1 )
            os << 's';
        return os;
    }

}



#include <algorithm>
#include <ostream>
#include <cstring>

namespace Catch {
    StringRef::StringRef( char const* rawChars ) noexcept
    : StringRef( rawChars, std::strlen(rawChars) )
    {}


    bool StringRef::operator<(StringRef rhs) const noexcept {
        if (m_size < rhs.m_size) {
            return strncmp(m_start, rhs.m_start, m_size) <= 0;
        }
        return strncmp(m_start, rhs.m_start, rhs.m_size) < 0;
    }

    int StringRef::compare( StringRef rhs ) const {
        auto cmpResult =
            strncmp( m_start, rhs.m_start, std::min( m_size, rhs.m_size ) );

        // This means that strncmp found a difference before the strings
        // ended, and we can return it directly
        if ( cmpResult != 0 ) {
            return cmpResult;
        }

        // If strings are equal up to length, then their comparison results on
        // their size
        if ( m_size < rhs.m_size ) {
            return -1;
        } else if ( m_size > rhs.m_size ) {
            return 1;
        } else {
            return 0;
        }
    }

    auto operator << ( std::ostream& os, StringRef str ) -> std::ostream& {
        return os.write(str.data(), static_cast<std::streamsize>(str.size()));
    }

    std::string operator+(StringRef lhs, StringRef rhs) {
        std::string ret;
        ret.reserve(lhs.size() + rhs.size());
        ret += lhs;
        ret += rhs;
        return ret;
    }

    auto operator+=( std::string& lhs, StringRef rhs ) -> std::string& {
        lhs.append(rhs.data(), rhs.size());
        return lhs;
    }

} // namespace Catch



namespace Catch {

    TagAliasRegistry::~TagAliasRegistry() = default;

    TagAlias const* TagAliasRegistry::find( std::string const& alias ) const {
        auto it = m_registry.find( alias );
        if ( it != m_registry.end() ) {
            return &it->second;
        } else {
            return nullptr;
        }
    }

    std::string TagAliasRegistry::expandAliases( std::string const& unexpandedTestSpec ) const {
        std::string expandedTestSpec = unexpandedTestSpec;
        for( auto const& registryKvp : m_registry ) {
            std::size_t pos = expandedTestSpec.find( registryKvp.first );
            if( pos != std::string::npos ) {
                expandedTestSpec =  expandedTestSpec.substr( 0, pos ) +
                                    registryKvp.second.tag +
                                    expandedTestSpec.substr( pos + registryKvp.first.size() );
            }
        }
        return expandedTestSpec;
    }

    void TagAliasRegistry::add( std::string const& alias, std::string const& tag, SourceLineInfo const& lineInfo ) {
        CATCH_ENFORCE( startsWith(alias, "[@") && endsWith(alias, ']'),
                      "error: tag alias, '" << alias << "' is not of the form [@alias name].\n" << lineInfo );

        CATCH_ENFORCE( m_registry.insert(std::make_pair(alias, TagAlias(tag, lineInfo))).second,
                      "error: tag alias, '" << alias << "' already registered.\n"
                      << "\tFirst seen at: " << find(alias)->lineInfo << "\n"
                      << "\tRedefined at: " << lineInfo );
    }

    ITagAliasRegistry::~ITagAliasRegistry() = default;

    ITagAliasRegistry const& ITagAliasRegistry::get() {
        return getRegistryHub().getTagAliasRegistry();
    }

} // end namespace Catch




namespace Catch {
    TestCaseInfoHasher::TestCaseInfoHasher( hash_t seed ): m_seed( seed ) {}

    uint32_t TestCaseInfoHasher::operator()( TestCaseInfo const& t ) const {
        // FNV-1a hash algorithm that is designed for uniqueness:
        const hash_t prime = 1099511628211u;
        hash_t hash = 14695981039346656037u;
        for ( const char c : t.name ) {
            hash ^= c;
            hash *= prime;
        }
        for ( const char c : t.className ) {
            hash ^= c;
            hash *= prime;
        }
        for ( const Tag& tag : t.tags ) {
            for ( const char c : tag.original ) {
                hash ^= c;
                hash *= prime;
            }
        }
        hash ^= m_seed;
        hash *= prime;
        const uint32_t low{ static_cast<uint32_t>( hash ) };
        const uint32_t high{ static_cast<uint32_t>( hash >> 32 ) };
        return low * high;
    }
} // namespace Catch




#include <algorithm>

namespace Catch {

    namespace {
        // Picked small-ish number at random
        static size_t kInitialTestCount = 120;

        static void enforceNoDuplicateTestCases(
            std::vector<TestCaseHandle> const& tests ) {
            // Flat set instead of std::set for performance.
            std::vector<TestCaseInfo const*> seenTests;
            seenTests.reserve( tests.size() );
            for ( auto const& test : tests ) {
                seenTests.push_back( &test.getTestCaseInfo() );
            }
            // We want to keep order of ~equal~ tests
            // stable_sort so that duplicates are reported with the
            // registration order preserved ("first seen" really is first)
            std::stable_sort(
                seenTests.begin(),
                seenTests.end(),
                []( TestCaseInfo const* lhs, TestCaseInfo const* rhs ) {
                    return *lhs < *rhs;
                }
            );
            const auto duplicate = std::adjacent_find(
                seenTests.begin(),
                seenTests.end(),
                []( TestCaseInfo const* lhs, TestCaseInfo const* rhs ) {
                    return *lhs == *rhs;
                }
            );

            if ( duplicate != seenTests.end() ) {
                const auto infoPtr = *std::next( duplicate );
                CATCH_ERROR( "error: test case \""
                             << infoPtr->name << "\", with tags \""
                             << infoPtr->tagsAsString()
                             << "\" already defined.\n"
                             << "\tFirst seen at "
                             << ( *duplicate )->lineInfo << "\n"
                             << "\tRedefined at " << infoPtr->lineInfo );
            }
        }

        static bool matchTest( TestCaseHandle const& testCase,
                               TestSpec const& testSpec,
                               IConfig const& config ) {
            return testSpec.matches( testCase.getTestCaseInfo() ) &&
                   isThrowSafe( testCase, config );
        }

    } // end unnamed namespace

    std::vector<TestCaseHandle> sortTests( IConfig const& config, std::vector<TestCaseHandle> const& unsortedTestCases ) {
        switch (config.runOrder()) {
        case TestRunOrder::Declared:
            return unsortedTestCases;

        case TestRunOrder::LexicographicallySorted: {
            std::vector<TestCaseHandle> sorted = unsortedTestCases;
            std::sort(
                sorted.begin(),
                sorted.end(),
                []( TestCaseHandle const& lhs, TestCaseHandle const& rhs ) {
                    return lhs.getTestCaseInfo() < rhs.getTestCaseInfo();
                }
            );
            return sorted;
        }
        case TestRunOrder::Randomized: {
            using TestWithHash = std::pair<TestCaseInfoHasher::hash_t, TestCaseHandle>;

            TestCaseInfoHasher h{ config.rngSeed() };
            std::vector<TestWithHash> indexed_tests;
            indexed_tests.reserve(unsortedTestCases.size());

            for (auto const& handle : unsortedTestCases) {
                indexed_tests.emplace_back(h(handle.getTestCaseInfo()), handle);
            }

            std::sort( indexed_tests.begin(),
                       indexed_tests.end(),
                       []( TestWithHash const& lhs, TestWithHash const& rhs ) {
                           if ( lhs.first == rhs.first ) {
                               return lhs.second.getTestCaseInfo() <
                                      rhs.second.getTestCaseInfo();
                           }
                           return lhs.first < rhs.first;
                       } );

            std::vector<TestCaseHandle> randomized;
            randomized.reserve(indexed_tests.size());

            for (auto const& indexed : indexed_tests) {
                randomized.push_back(indexed.second);
            }

            return randomized;
        }
        }

        CATCH_INTERNAL_ERROR("Unknown test order value!");
    }

    bool isThrowSafe( TestCaseHandle const& testCase, IConfig const& config ) {
        return !testCase.getTestCaseInfo().throws() || config.allowThrows();
    }

    std::vector<TestCaseHandle> filterTests( std::vector<TestCaseHandle> const& testCases, TestSpec const& testSpec, IConfig const& config ) {
        std::vector<TestCaseHandle> filtered;
        filtered.reserve( testCases.size() );
        for (auto const& testCase : testCases) {
            if ((!testSpec.hasFilters() && !testCase.getTestCaseInfo().isHidden()) ||
                (testSpec.hasFilters() && matchTest(testCase, testSpec, config))) {
                filtered.push_back(testCase);
            }
        }
        // The Catch3 runner shards the run itself.
        return filtered;
    }
    std::vector<TestCaseHandle> const& getAllTestCasesSorted( IConfig const& config ) {
        return getRegistryHub().getTestCaseRegistry().getAllTestsSorted( config );
    }


    TestRegistry::TestRegistry() {
        // We pre-reserve some reasonable number of tests to avoid the
        // initial geometric growth churning during test registration.
        m_handles.reserve( kInitialTestCount );
        m_test_infos.reserve( kInitialTestCount );
        m_invokers.reserve( kInitialTestCount );
    }
    TestRegistry::~TestRegistry() = default;

    void TestRegistry::registerTest(Detail::unique_ptr<TestCaseInfo> testInfo, Detail::unique_ptr<ITestInvoker> testInvoker) {
        m_handles.emplace_back(testInfo.get(), testInvoker.get());
        m_test_infos.push_back(CATCH_MOVE(testInfo));
        m_invokers.push_back(CATCH_MOVE(testInvoker));
    }

    void TestRegistry::enableFilenameTags() {
        for (auto& info : m_test_infos) {
            info->addFilenameTag();
        }
    }

    std::vector<TestCaseHandle> const& TestRegistry::getAllTests() const {
        return m_handles;
    }
    std::vector<TestCaseHandle> const& TestRegistry::getAllTestsSorted( IConfig const& config ) const {
        if( m_sortedFunctions.empty() )
            enforceNoDuplicateTestCases( m_handles );

        if(  m_currentSortOrder != config.runOrder() || m_sortedFunctions.empty() ) {
            m_sortedFunctions = sortTests( config, m_handles );
            m_currentSortOrder = config.runOrder();
        }
        return m_sortedFunctions;
    }

} // end namespace Catch




#include <algorithm>
#include <cassert>

#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif

namespace Catch {
namespace TestCaseTracking {

    NameAndLocation::NameAndLocation( std::string&& _name, SourceLineInfo const& _location )
    :   name( CATCH_MOVE(_name) ),
        location( _location )
    {}

    ITracker::ITracker( NameAndLocation&& nameAndLoc, ITracker* parent ):
        m_nameAndLocation( CATCH_MOVE( nameAndLoc ) ), m_parent( parent ) {
        if ( m_parent ) {
            m_allTrackerDepth = m_parent->m_allTrackerDepth + 1;
            // We leave section trackers to bump themselves up, as
            // we cannot use `isSectionTracker` in constructor
            m_sectionOnlyDepth = m_parent->m_sectionOnlyDepth;
            m_filterRef = m_parent->m_filterRef;
            m_newStyleFilters = m_parent->m_newStyleFilters;
        }
    }

    ITracker::~ITracker() = default;

    void ITracker::markAsNeedingAnotherRun() {
        m_runState = NeedsAnotherRun;
    }

    void ITracker::addChild( ITrackerPtr&& child ) {
        m_children.push_back( CATCH_MOVE(child) );
    }

    ITracker* ITracker::findChild( NameAndLocationRef const& nameAndLocation ) {
        auto it = std::find_if(
            m_children.begin(),
            m_children.end(),
            [&nameAndLocation]( ITrackerPtr const& tracker ) {
                auto const& tnameAndLoc = tracker->nameAndLocation();
                if ( tnameAndLoc.location.line !=
                     nameAndLocation.location.line ) {
                    return false;
                }
                return tnameAndLoc == nameAndLocation;
            } );
        return ( it != m_children.end() ) ? it->get() : nullptr;
    }

    bool ITracker::isSectionTracker() const { return false; }
    bool ITracker::isGeneratorTracker() const { return false; }

    bool ITracker::isOpen() const {
        return m_runState != NotStarted && !isComplete();
    }

    bool ITracker::hasStarted() const { return m_runState != NotStarted; }

    void ITracker::openChild() {
        if (m_runState != ExecutingChildren) {
            m_runState = ExecutingChildren;
            if (m_parent) {
                m_parent->openChild();
            }
        }
    }

    ITracker& TrackerContext::startRun() {
        using namespace std::string_literals;
        m_rootTracker = Catch::Detail::make_unique<SectionTracker>(
            NameAndLocation( "{root}"s, CATCH_INTERNAL_LINEINFO ),
            *this,
            nullptr );
        m_currentTracker = nullptr;
        m_runState = Executing;
        return *m_rootTracker;
    }

    void TrackerContext::completeCycle() {
        m_runState = CompletedCycle;
    }

    bool TrackerContext::completedCycle() const {
        return m_runState == CompletedCycle;
    }
    void TrackerContext::setCurrentTracker( ITracker* tracker ) {
        m_currentTracker = tracker;
    }


    TrackerBase::TrackerBase( NameAndLocation&& nameAndLocation, TrackerContext& ctx, ITracker* parent ):
        ITracker(CATCH_MOVE(nameAndLocation), parent),
        m_ctx( ctx )
    {}

    bool TrackerBase::isComplete() const {
        return m_runState == CompletedSuccessfully || m_runState == Failed;
    }

    void TrackerBase::open() {
        m_runState = Executing;
        moveToThis();
        if( m_parent )
            m_parent->openChild();
    }

    void TrackerBase::close() {

        // Close any still open children (e.g. generators)
        while( &m_ctx.currentTracker() != this )
            m_ctx.currentTracker().close();

        switch( m_runState ) {
            case NeedsAnotherRun:
                break;

            case Executing:
                m_runState = CompletedSuccessfully;
                break;
            case ExecutingChildren:
                if( std::all_of(m_children.begin(), m_children.end(), [](ITrackerPtr const& t){ return t->isComplete(); }) )
                    m_runState = CompletedSuccessfully;
                break;

            case NotStarted:
            case CompletedSuccessfully:
            case Failed:
                CATCH_INTERNAL_ERROR( "Illogical state: " << m_runState );

            default:
                CATCH_INTERNAL_ERROR( "Unknown state: " << m_runState );
        }
        moveToParent();
        m_ctx.completeCycle();
    }
    void TrackerBase::fail() {
        m_runState = Failed;
        if( m_parent )
            m_parent->markAsNeedingAnotherRun();
        moveToParent();
        m_ctx.completeCycle();
    }

    void TrackerBase::moveToParent() {
        assert( m_parent );
        m_ctx.setCurrentTracker( m_parent );
    }
    void TrackerBase::moveToThis() {
        m_ctx.setCurrentTracker( this );
    }

    bool SectionTracker::isFilteredImpl() const {
        // TBD: This is currently _very_ similar to the block in `isComplete`.
        //      Is this necessarily that way, or just accident of current semantics?
        const size_t filterIndex =
            m_newStyleFilters ? m_allTrackerDepth : m_sectionOnlyDepth;

        if ( filterIndex < m_filterRef->size() ) {
            // 1) New style filter must explicitly target section
            if ( m_newStyleFilters && ( *m_filterRef )[filterIndex].type !=
                                          PathFilter::For::Section ) {
                return true;
            }
            // 2) Both style filters must match the trimmed name exactly
            if ( m_trimmed_name !=
                 StringRef( ( *m_filterRef )[filterIndex].filter ) ) {
                return true;
            }
        }

        return false;
    }

    SectionTracker::SectionTracker( NameAndLocation&& nameAndLocation, TrackerContext& ctx, ITracker* parent )
    :   TrackerBase( CATCH_MOVE(nameAndLocation), ctx, parent ),
        m_trimmed_name(trim(StringRef(ITracker::nameAndLocation().name)))
    {
        if( m_parent ) {
            ++m_sectionOnlyDepth;
        }
    }

    bool SectionTracker::isComplete() const {
        // If there are active filters AND we do not pass them,
        // the section is always "completed"
        const size_t filterIndex =
            m_newStyleFilters ? m_allTrackerDepth : m_sectionOnlyDepth;
        if ( filterIndex < m_filterRef->size() ) {
            // There is active filter, check it
            // 1) New style filter must explicitly target section
            if ( m_newStyleFilters && ( *m_filterRef )[filterIndex].type !=
                                          PathFilter::For::Section ) {
                return true;
            }
            // 2) Both style filters must match the trimmed name exactly
            if ( m_trimmed_name !=
                 StringRef( ( *m_filterRef )[filterIndex].filter ) ) {
                return true;
            }
        }

        // Otherwise we delegate to the generic processing
        return TrackerBase::isComplete();
    }

    bool SectionTracker::isSectionTracker() const { return true; }

    SectionTracker& SectionTracker::acquire( TrackerContext& ctx, NameAndLocationRef const& nameAndLocation ) {
        SectionTracker* tracker;

        ITracker& currentTracker = ctx.currentTracker();
        if ( ITracker* childTracker =
                 currentTracker.findChild( nameAndLocation ) ) {
            assert( childTracker );
            assert( childTracker->isSectionTracker() );
            tracker = static_cast<SectionTracker*>( childTracker );
        } else {
            auto newTracker = Catch::Detail::make_unique<SectionTracker>(
                NameAndLocation{ static_cast<std::string>(nameAndLocation.name),
                                 nameAndLocation.location },
                ctx,
                &currentTracker );
            tracker = newTracker.get();
            currentTracker.addChild( CATCH_MOVE( newTracker ) );
        }

        if ( !ctx.completedCycle() ) {
            tracker->tryOpen();
        }

        return *tracker;
    }

    void SectionTracker::tryOpen() {
        if( !isComplete() )
            open();
    }

    StringRef SectionTracker::trimmedName() const {
        return m_trimmed_name;
    }

} // namespace TestCaseTracking

} // namespace Catch

#if defined(__clang__)
#    pragma clang diagnostic pop
#endif




namespace Catch {

    void throw_test_failure_exception() {
#if !defined( CATCH_CONFIG_DISABLE_EXCEPTIONS )
        throw TestFailureException{}; //NOLINT(bugprone-std-exception-baseclass)
#else
        CATCH_ERROR( "Test failure requires aborting test!" );
#endif
    }

    void throw_test_skip_exception() {
#if !defined( CATCH_CONFIG_DISABLE_EXCEPTIONS )
        throw Catch::TestSkipException(); //NOLINT(bugprone-std-exception-baseclass)
#else
        CATCH_ERROR( "Explicitly skipping tests during runtime requires exceptions" );
#endif
    }

} // namespace Catch



#include <algorithm>
#include <iterator>

namespace Catch {
    void ITestInvoker::prepareTestCase() {}
    void ITestInvoker::tearDownTestCase() {}
    ITestInvoker::~ITestInvoker() = default;

    namespace {
        static StringRef extractClassName( StringRef classOrMethodName ) {
            if ( !startsWith( classOrMethodName, '&' ) ) {
                return classOrMethodName;
            }

            // Remove the leading '&' to avoid having to special case it later
            const auto methodName =
                classOrMethodName.substr( 1, classOrMethodName.size() );

            auto reverseStart = std::make_reverse_iterator( methodName.end() );
            auto reverseEnd = std::make_reverse_iterator( methodName.begin() );

            // We make a simplifying assumption that ":" is only present
            // in the input as part of "::" from C++ typenames (this is
            // relatively safe assumption because the input is generated
            // as stringification of type through preprocessor).
            auto lastColons = std::find( reverseStart, reverseEnd, ':' ) + 1;
            auto secondLastColons =
                std::find( lastColons + 1, reverseEnd, ':' );

            auto const startIdx = reverseEnd - secondLastColons;
            auto const classNameSize = secondLastColons - lastColons - 1;

            return methodName.substr(
                static_cast<std::size_t>( startIdx ),
                static_cast<std::size_t>( classNameSize ) );
        }

        class TestInvokerAsFunction final : public ITestInvoker {
            using TestType = void ( * )();
            TestType m_testAsFunction;

        public:
            constexpr TestInvokerAsFunction( TestType testAsFunction ) noexcept:
                m_testAsFunction( testAsFunction ) {}

            void invoke() const override { m_testAsFunction(); }
        };

    } // namespace

    Detail::unique_ptr<ITestInvoker> makeTestInvoker( void(*testAsFunction)() ) {
        return Detail::make_unique<TestInvokerAsFunction>( testAsFunction );
    }

    AutoReg::AutoReg( void ( *testAsFunction )(),
                      SourceLineInfo const& lineInfo,
                      StringRef classOrMethod,
                      NameAndTags const& nameAndTags ) noexcept:
        AutoReg( makeTestInvoker( testAsFunction ),
                 lineInfo,
                 classOrMethod,
                 nameAndTags ) {}

    AutoReg::AutoReg( Detail::unique_ptr<ITestInvoker> invoker, SourceLineInfo const& lineInfo, StringRef classOrMethod, NameAndTags const& nameAndTags ) noexcept {
        CATCH_TRY {
            getMutableRegistryHub()
                    .registerTest(
                        makeTestCaseInfo(
                            extractClassName( classOrMethod ),
                            nameAndTags,
                            lineInfo),
                        CATCH_MOVE(invoker)
                    );
        } CATCH_CATCH_ALL {
            // Do not throw when constructing global objects, instead register the exception to be processed later
            getMutableRegistryHub().registerStartupException();
        }
    }
}





namespace Catch {

    TestSpecParser::TestSpecParser( ITagAliasRegistry const& tagAliases ) : m_tagAliases( &tagAliases ) {}

    TestSpecParser& TestSpecParser::parse( std::string const& arg ) {
        m_mode = None;
        m_exclusion = false;
        m_arg = m_tagAliases->expandAliases( arg );
        m_escapeChars.clear();
        m_substring.reserve(m_arg.size());
        m_patternName.reserve(m_arg.size());
        m_realPatternPos = 0;

        for( m_pos = 0; m_pos < m_arg.size(); ++m_pos )
          //if visitChar fails
           if( !visitChar( m_arg[m_pos] ) ){
               m_testSpec.m_invalidSpecs.push_back(arg);
               break;
           }
        endMode();
        return *this;
    }
    TestSpec TestSpecParser::testSpec() {
        addFilter();
        return CATCH_MOVE(m_testSpec);
    }
    bool TestSpecParser::visitChar( char c ) {
        if( (m_mode != EscapedName) && (c == '\\') ) {
            escape();
            addCharToPattern(c);
            return true;
        }else if((m_mode != EscapedName) && (c == ',') )  {
            return separate();
        }

        switch( m_mode ) {
        case None:
            if( processNoneChar( c ) )
                return true;
            break;
        case Name:
            processNameChar( c );
            break;
        case EscapedName:
            endMode();
            addCharToPattern(c);
            return true;
        default:
        case Tag:
        case QuotedName:
            if( processOtherChar( c ) )
                return true;
            break;
        }

        m_substring += c;
        if( !isControlChar( c ) ) {
            m_patternName += c;
            m_realPatternPos++;
        }
        return true;
    }
    // Two of the processing methods return true to signal the caller to return
    // without adding the given character to the current pattern strings
    bool TestSpecParser::processNoneChar( char c ) {
        switch( c ) {
        case ' ':
            return true;
        case '~':
            m_exclusion = true;
            return false;
        case '[':
            startNewMode( Tag );
            return false;
        case '"':
            startNewMode( QuotedName );
            return false;
        default:
            startNewMode( Name );
            return false;
        }
    }
    void TestSpecParser::processNameChar( char c ) {
        if( c == '[' ) {
            if( m_substring == "exclude:" )
                m_exclusion = true;
            else
                endMode();
            startNewMode( Tag );
        }
    }
    bool TestSpecParser::processOtherChar( char c ) {
        if( !isControlChar( c ) )
            return false;
        m_substring += c;
        endMode();
        return true;
    }
    void TestSpecParser::startNewMode( Mode mode ) {
        m_mode = mode;
    }
    void TestSpecParser::endMode() {
        switch( m_mode ) {
        case Name:
        case QuotedName:
            return addNamePattern();
        case Tag:
            return addTagPattern();
        case EscapedName:
            revertBackToLastMode();
            return;
        case None:
        default:
            return startNewMode( None );
        }
    }
    void TestSpecParser::escape() {
        saveLastMode();
        m_mode = EscapedName;
        m_escapeChars.push_back(m_realPatternPos);
    }
    bool TestSpecParser::isControlChar( char c ) const {
        switch( m_mode ) {
            default:
                return false;
            case None:
                return c == '~';
            case Name:
                return c == '[';
            case EscapedName:
                return true;
            case QuotedName:
                return c == '"';
            case Tag:
                return c == '[' || c == ']';
        }
    }

    void TestSpecParser::addFilter() {
        if( !m_currentFilter.m_required.empty() || !m_currentFilter.m_forbidden.empty() ) {
            m_testSpec.m_filters.push_back( CATCH_MOVE(m_currentFilter) );
            m_currentFilter = TestSpec::Filter();
        }
    }

    void TestSpecParser::saveLastMode() {
      lastMode = m_mode;
    }

    void TestSpecParser::revertBackToLastMode() {
      m_mode = lastMode;
    }

    bool TestSpecParser::separate() {
      if( (m_mode==QuotedName) || (m_mode==Tag) ){
         //invalid argument, signal failure to previous scope.
         m_mode = None;
         m_pos = m_arg.size();
         m_substring.clear();
         m_patternName.clear();
         m_realPatternPos = 0;
         return false;
      }
      endMode();
      addFilter();
      return true; //success
    }

    std::string TestSpecParser::preprocessPattern() {
        std::string token = m_patternName;
        for (std::size_t i = 0; i < m_escapeChars.size(); ++i)
            token = token.substr(0, m_escapeChars[i] - i) + token.substr(m_escapeChars[i] - i + 1);
        m_escapeChars.clear();
        if (startsWith(token, "exclude:")) {
            m_exclusion = true;
            token = token.substr(8);
        }

        m_patternName.clear();
        m_realPatternPos = 0;

        return token;
    }

    void TestSpecParser::addNamePattern() {
        auto token = preprocessPattern();

        if (!token.empty()) {
            if (m_exclusion) {
                m_currentFilter.m_forbidden.emplace_back(Detail::make_unique<TestSpec::NamePattern>(token, m_substring));
            } else {
                m_currentFilter.m_required.emplace_back(Detail::make_unique<TestSpec::NamePattern>(token, m_substring));
            }
        }
        m_substring.clear();
        m_exclusion = false;
        m_mode = None;
    }

    void TestSpecParser::addTagPattern() {
        auto token = preprocessPattern();

        if (!token.empty()) {
            // If the tag pattern is the "hide and tag" shorthand (e.g. [.foo])
            // we have to create a separate hide tag and shorten the real one
            if (token.size() > 1 && token[0] == '.') {
                token.erase(token.begin());
                if (m_exclusion) {
                    m_currentFilter.m_forbidden.emplace_back(Detail::make_unique<TestSpec::TagPattern>(".", m_substring));
                } else {
                    m_currentFilter.m_required.emplace_back(Detail::make_unique<TestSpec::TagPattern>(".", m_substring));
                }
            }
            if (m_exclusion) {
                m_currentFilter.m_forbidden.emplace_back(Detail::make_unique<TestSpec::TagPattern>(token, m_substring));
            } else {
                m_currentFilter.m_required.emplace_back(Detail::make_unique<TestSpec::TagPattern>(token, m_substring));
            }
        }
        m_substring.clear();
        m_exclusion = false;
        m_mode = None;
    }

} // namespace Catch




#include <exception>

namespace Catch {
    bool uncaught_exceptions() {
#if defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
        return false;
#elif defined(CATCH_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS)
        return std::uncaught_exceptions() > 0;
#else
        return std::uncaught_exception();
#endif
  }
} // end namespace Catch



namespace Catch {

    WildcardPattern::WildcardPattern( std::string const& pattern,
                                      CaseSensitive caseSensitivity )
    :   m_caseSensitivity( caseSensitivity ),
        m_pattern( trim(pattern) )
    {
        if( startsWith( m_pattern, '*' ) ) {
            m_pattern = m_pattern.erase( 0, 1 ); // effectively pop_front
            m_wildcard = static_cast<WildcardPosition>( m_wildcard | WildcardAtStart );
        }
        if( endsWith( m_pattern, '*' ) ) {
            m_pattern.pop_back();
            m_wildcard = static_cast<WildcardPosition>( m_wildcard | WildcardAtEnd );
        }
    }

    bool WildcardPattern::matches( StringRef input ) const {
        input = trim( input );
        if ( input.size() < m_pattern.size() ) { return false; }

        auto casedMatch = [&]( StringRef lhs, StringRef rhs ) {
            if ( m_caseSensitivity == CaseSensitive::Yes ) {
                return lhs == rhs;
            } else {
                Detail::CaseInsensitiveEqualTo eq;
                return eq( lhs, rhs );
            }
        };

        switch ( m_wildcard ) {
        case NoWildcard:
            return casedMatch(input, m_pattern);
        case WildcardAtStart:
            return casedMatch(
                input.substr( input.size() - m_pattern.size(), input.size() ),
                m_pattern );
        case WildcardAtEnd:
            return casedMatch( input.substr( 0, m_pattern.size() ), m_pattern );
        case WildcardAtBothEnds: {
            // TBD: This has terrible performance, but it is also the least
            //      common use case. Once the StringContains matcher gets
            //      efficient implementation, we should reuse it here.
            const size_t lastOffset = input.size() - m_pattern.size();
            for ( size_t offset = 0; offset <= lastOffset; ++offset ) {
                if ( casedMatch( input.substr(offset, m_pattern.size()), m_pattern ) ) { return true; }
            }
            return false;
        }
        default:
            CATCH_INTERNAL_ERROR( "Unknown enum" );
        }
    }
}




namespace Catch {
namespace Matchers {

    std::string IsEmptyMatcher::describe() const {
        return "is empty";
    }

    std::string HasSizeMatcher::describe() const {
        ReusableStringStream sstr;
        sstr << "has size == " << m_target_size;
        return sstr.str();
    }

} // end namespace Matchers
} // end namespace Catch



namespace Catch {
namespace Matchers {

bool ExceptionMessageMatcher::match(std::exception const& ex) const {
    return ex.what() == m_message;
}

std::string ExceptionMessageMatcher::describe() const {
    return "exception message matches \"" + m_message + '"';
}

ExceptionMessageMatcher Message(std::string const& message) {
    return ExceptionMessageMatcher(message);
}

} // namespace Matchers
} // namespace Catch



#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <limits>


namespace Catch {
namespace {

    template <typename FP>
    bool almostEqualUlps(FP lhs, FP rhs, uint64_t maxUlpDiff) {
        // Comparison with NaN should always be false.
        // This way we can rule it out before getting into the ugly details
        if (Catch::isnan(lhs) || Catch::isnan(rhs)) {
            return false;
        }

        // This should also handle positive and negative zeros, infinities
        const auto ulpDist = ulpDistance(lhs, rhs);

        return ulpDist <= maxUlpDiff;
    }


template <typename FP>
FP step(FP start, FP direction, uint64_t steps) {
    for (uint64_t i = 0; i < steps; ++i) {
        start = Catch::nextafter(start, direction);
    }
    return start;
}

// Performs equivalent check of std::fabs(lhs - rhs) <= margin
// But without the subtraction to allow for INFINITY in comparison
bool marginComparison(double lhs, double rhs, double margin) {
    return (lhs + margin >= rhs) && (rhs + margin >= lhs);
}

template <typename FloatingPoint>
void write(std::ostream& out, FloatingPoint num) {
    out << std::scientific
        << std::setprecision(std::numeric_limits<FloatingPoint>::max_digits10 - 1)
        << num;
}

} // end anonymous namespace

namespace Matchers {
namespace Detail {

    enum class FloatingPointKind : uint8_t {
        Float,
        Double
    };

} // end namespace Detail


    WithinAbsMatcher::WithinAbsMatcher(double target, double margin)
        :m_target{ target }, m_margin{ margin } {
        CATCH_ENFORCE(margin >= 0, "Invalid margin: " << margin << '.'
            << " Margin has to be non-negative.");
    }

    // Performs equivalent check of std::fabs(lhs - rhs) <= margin
    // But without the subtraction to allow for INFINITY in comparison
    bool WithinAbsMatcher::match(double const& matchee) const {
        return (matchee + m_margin >= m_target) && (m_target + m_margin >= matchee);
    }

    std::string WithinAbsMatcher::describe() const {
        return "is within " + ::Catch::Detail::stringify(m_margin) + " of " + ::Catch::Detail::stringify(m_target);
    }


    WithinUlpsMatcher::WithinUlpsMatcher(double target, uint64_t ulps, Detail::FloatingPointKind baseType)
        :m_target{ target }, m_ulps{ ulps }, m_type{ baseType } {
        CATCH_ENFORCE(m_type == Detail::FloatingPointKind::Double
                   || m_ulps < (std::numeric_limits<uint32_t>::max)(),
            "Provided ULP is impossibly large for a float comparison.");
        CATCH_ENFORCE( std::numeric_limits<double>::is_iec559,
                       "WithinUlp matcher only supports platforms with "
                       "IEEE-754 compatible floating point representation" );
    }

#if defined(__clang__)
#pragma clang diagnostic push
// Clang <3.5 reports on the default branch in the switch below
#pragma clang diagnostic ignored "-Wunreachable-code"
#endif

    bool WithinUlpsMatcher::match(double const& matchee) const {
        switch (m_type) {
        case Detail::FloatingPointKind::Float:
            return almostEqualUlps<float>(static_cast<float>(matchee), static_cast<float>(m_target), m_ulps);
        case Detail::FloatingPointKind::Double:
            return almostEqualUlps<double>(matchee, m_target, m_ulps);
        default:
            CATCH_INTERNAL_ERROR( "Unknown Detail::FloatingPointKind value" );
        }
    }

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

    std::string WithinUlpsMatcher::describe() const {
        std::stringstream ret;

        ret << "is within " << m_ulps << " ULPs of ";

        if (m_type == Detail::FloatingPointKind::Float) {
            write(ret, static_cast<float>(m_target));
            ret << 'f';
        } else {
            write(ret, m_target);
        }

        ret << " ([";
        if (m_type == Detail::FloatingPointKind::Double) {
            write( ret,
                   step( m_target,
                         -std::numeric_limits<double>::infinity(),
                         m_ulps ) );
            ret << ", ";
            write( ret,
                   step( m_target,
                         std::numeric_limits<double>::infinity(),
                         m_ulps ) );
        } else {
            // We have to cast INFINITY to float because of MinGW, see #1782
            write( ret,
                   step( static_cast<float>( m_target ),
                         -std::numeric_limits<float>::infinity(),
                         m_ulps ) );
            ret << ", ";
            write( ret,
                   step( static_cast<float>( m_target ),
                         std::numeric_limits<float>::infinity(),
                         m_ulps ) );
        }
        ret << "])";

        return ret.str();
    }

    WithinRelMatcher::WithinRelMatcher(double target, double epsilon):
        m_target(target),
        m_epsilon(epsilon){
        CATCH_ENFORCE(m_epsilon >= 0., "Relative comparison with epsilon <  0 does not make sense.");
        CATCH_ENFORCE(m_epsilon  < 1., "Relative comparison with epsilon >= 1 does not make sense.");
    }

    bool WithinRelMatcher::match(double const& matchee) const {
        const auto relMargin = m_epsilon * (std::max)(std::fabs(matchee), std::fabs(m_target));
        return marginComparison(matchee, m_target,
                                std::isinf(relMargin)? 0 : relMargin);
    }

    std::string WithinRelMatcher::describe() const {
        Catch::ReusableStringStream sstr;
        sstr << "and " << ::Catch::Detail::stringify(m_target) << " are within " << m_epsilon * 100. << "% of each other";
        return sstr.str();
    }


WithinUlpsMatcher WithinULP(double target, uint64_t maxUlpDiff) {
    return WithinUlpsMatcher(target, maxUlpDiff, Detail::FloatingPointKind::Double);
}

WithinUlpsMatcher WithinULP(float target, uint64_t maxUlpDiff) {
    return WithinUlpsMatcher(target, maxUlpDiff, Detail::FloatingPointKind::Float);
}

WithinAbsMatcher WithinAbs(double target, double margin) {
    return WithinAbsMatcher(target, margin);
}

WithinRelMatcher WithinRel(double target, double eps) {
    return WithinRelMatcher(target, eps);
}

WithinRelMatcher WithinRel(double target) {
    return WithinRelMatcher(target, std::numeric_limits<double>::epsilon() * 100);
}

WithinRelMatcher WithinRel(float target, float eps) {
    return WithinRelMatcher(target, eps);
}

WithinRelMatcher WithinRel(float target) {
    return WithinRelMatcher(target, std::numeric_limits<float>::epsilon() * 100);
}



bool IsNaNMatcher::match( double const& matchee ) const {
    return std::isnan( matchee );
}

std::string IsNaNMatcher::describe() const {
    using namespace std::string_literals;
    return "is NaN"s;
}

IsNaNMatcher IsNaN() { return IsNaNMatcher(); }

    } // namespace Matchers
} // namespace Catch




namespace Catch {

    // This is the general overload that takes a any string matcher
    // There is another overload, in catch_assertionhandler.h/.cpp, that only takes a string and infers
    // the Equals matcher (so the header does not mention matchers)
    void handleExceptionMatchExpr( AssertionHandler& handler, StringMatcher const& matcher ) {
        std::string exceptionMessage = Catch::translateActiveException();
        MatchExpr<std::string, StringMatcher const&> expr( CATCH_MOVE(exceptionMessage), matcher );
        handler.handleExpr( expr );
    }

} // namespace Catch





namespace Catch {
namespace Matchers {

    std::string MatcherUntypedBase::toString() const {
        return describe();
    }

    std::string MatcherUntypedBase::describe() const {
        using namespace std::string_literals;
        return "Undescribed matcher"s;
    }

} // namespace Matchers
} // namespace Catch




std::string Catch::Matchers::Detail::finalizeDescription(const std::string& desc) {
    if (desc.empty()) {
        return "matches undescribed predicate";
    } else {
        return "matches predicate: \"" + desc + '"';
    }
}



namespace Catch {
    namespace Matchers {

        std::string AllTrueMatcher::describe() const { return "contains only true"; }

        std::string NoneTrueMatcher::describe() const { return "contains no true"; }

        std::string AnyTrueMatcher::describe() const { return "contains at least one true"; }

    } // namespace Matchers
} // namespace Catch



#include <regex>

namespace Catch {

    namespace {
        constexpr StringRef caseSensitivitySuffix( CaseSensitive caseSensitivity ) {
            return caseSensitivity == CaseSensitive::Yes
                       ? StringRef{}
                       : " (case insensitive)"_sr;
        }
    } // namespace

namespace Matchers {

    StringMatcherBase::StringMatcherBase( std::string target,
                                          StringRef operation,
                                          CaseSensitive caseSensitivity ):
        m_target( CATCH_MOVE( target ) ),
        m_operation( operation ),
        m_caseSensitivity( caseSensitivity ) {}


    std::string StringMatcherBase::describe() const {
        std::string description;
        description.reserve(5 + m_operation.size() + m_target.size() +
                                    caseSensitivitySuffix(m_caseSensitivity).size());
        description += m_operation;
        description += ": \"";
        description += m_target;
        description += '"';
        description += caseSensitivitySuffix(m_caseSensitivity);
        return description;
    }


    StringEqualsMatcher::StringEqualsMatcher( std::string comparator, CaseSensitive caseSensitivity ):
        StringMatcherBase( CATCH_MOVE( comparator ), "equals"_sr, caseSensitivity ) {}

    bool StringEqualsMatcher::match( std::string const& source ) const {
        if (m_caseSensitivity == CaseSensitive::Yes) {
            return m_target == source;
        }
        if (m_target.size() != source.size()) { return false; }
        Catch::Detail::CaseInsensitiveEqualTo eq;
        return eq( m_target, source );
    }


    StringContainsMatcher::StringContainsMatcher(
        std::string comparator, CaseSensitive caseSensitivity ):
        StringMatcherBase( CATCH_MOVE( comparator ), "contains"_sr, caseSensitivity ) {}

    bool StringContainsMatcher::match( std::string const& source ) const {
        if ( m_caseSensitivity == CaseSensitive::Yes ) {
            return contains( source, m_target );
        }
        if ( source.size() < m_target.size() ) { return false; }
        StringRef as_ref( source );
        // The worst case of this is O(m*n), which is terrible, BUT:
        //  * The average case is much better, the worst case only happens rarely
        //  * We can implement BMH/other better searchers later if it matters
        Catch::Detail::CaseInsensitiveEqualTo eq;
        for (size_t i = 0; i < source.size(); ++i) {
            const auto substr = as_ref.substr( i, m_target.size() );
            bool found = eq( substr, m_target );
            if ( found ) { return true; }
        }
        return false;
    }


    StartsWithMatcher::StartsWithMatcher( std::string comparator,
                                          CaseSensitive caseSensitivity ):
        StringMatcherBase( CATCH_MOVE( comparator ), "starts with"_sr, caseSensitivity ) {}

    bool StartsWithMatcher::match( std::string const& source ) const {
        if ( m_caseSensitivity == CaseSensitive::Yes ) {
            return startsWith( source, m_target );
        }
        if (source.size() < m_target.size()) { return false; }
        Catch::Detail::CaseInsensitiveEqualTo eq;
        return eq(
            StringRef( source ).substr( 0, m_target.size() ), m_target );
    }


    EndsWithMatcher::EndsWithMatcher( std::string comparator,
                                      CaseSensitive caseSensitivity ):
        StringMatcherBase( CATCH_MOVE( comparator ), "ends with"_sr, caseSensitivity ) {}

    bool EndsWithMatcher::match( std::string const& source ) const {
        if ( m_caseSensitivity == CaseSensitive::Yes ) {
            return endsWith( source, m_target );
        }
        if ( source.size() < m_target.size() ) { return false; }
        Catch::Detail::CaseInsensitiveEqualTo eq;
        const size_t start_point = source.size() - m_target.size();
        return eq( StringRef( source ).substr( start_point, m_target.size() ), m_target );
    }



    RegexMatcher::RegexMatcher(std::string regex, CaseSensitive caseSensitivity): m_regex(CATCH_MOVE(regex)), m_caseSensitivity(caseSensitivity) {}

    bool RegexMatcher::match(std::string const& matchee) const {
        auto flags = std::regex::ECMAScript; // ECMAScript is the default syntax option anyway
        if (m_caseSensitivity == CaseSensitive::No) {
            flags |= std::regex::icase;
        }
        auto reg = std::regex(m_regex, flags);
        return std::regex_match(matchee, reg);
    }

    std::string RegexMatcher::describe() const {
        return "matches " + ::Catch::Detail::stringify(m_regex) + ((m_caseSensitivity == CaseSensitive::Yes)? " case sensitively" : " case insensitively");
    }


    StringEqualsMatcher Equals( std::string str, CaseSensitive caseSensitivity ) {
        return StringEqualsMatcher( CATCH_MOVE( str ), caseSensitivity );
    }
    StringContainsMatcher ContainsSubstring( std::string str, CaseSensitive caseSensitivity ) {
        return StringContainsMatcher( CATCH_MOVE( str ), caseSensitivity );
    }
    EndsWithMatcher EndsWith( std::string str, CaseSensitive caseSensitivity ) {
        return EndsWithMatcher( CATCH_MOVE( str ), caseSensitivity );
    }
    StartsWithMatcher StartsWith( std::string str, CaseSensitive caseSensitivity ) {
        return StartsWithMatcher( CATCH_MOVE( str ), caseSensitivity );
    }

    RegexMatcher Matches(std::string regex, CaseSensitive caseSensitivity) {
        return RegexMatcher( CATCH_MOVE( regex ), caseSensitivity );
    }

} // namespace Matchers
} // namespace Catch



namespace Catch {
namespace Matchers {

    std::string MatcherGenericBase::describe() const {
        using namespace std::string_literals;
        return "Undescribed generic matcher"s;
    }

    namespace Detail {

        std::string describe_multi_matcher(StringRef combine, std::string const* descriptions_begin, std::string const* descriptions_end) {
            std::string description;
            std::size_t combined_size = 4;
            for ( auto desc = descriptions_begin; desc != descriptions_end; ++desc ) {
                combined_size += desc->size();
            }
            combined_size += static_cast<size_t>(descriptions_end - descriptions_begin - 1) * combine.size();

            description.reserve(combined_size);

            description += "( ";
            bool first = true;
            for( auto desc = descriptions_begin; desc != descriptions_end; ++desc ) {
                if( first )
                    first = false;
                else
                    description += combine;
                description += *desc;
            }
            description += " )";
            return description;
        }

    } // namespace Detail
} // namespace Matchers
} // namespace Catch




#include <cassert>
#include <stack>

namespace Catch {

    ////////////////////////////////////////////////////////////////////////////


    ScopedMessage::ScopedMessage( MessageBuilder&& builder ):
        m_messageId( builder.m_info.sequence ) {
        MessageInfo info( CATCH_MOVE( builder.m_info ) );
        info.message = builder.m_stream.str();
        Detail::pushScopedMessage( CATCH_MOVE( info ) );
    }

    ScopedMessage::ScopedMessage( ScopedMessage&& old ) noexcept:
        m_messageId( old.m_messageId ) {
        old.m_moved = true;
    }

    ScopedMessage::~ScopedMessage() {
        if ( !m_moved ) { Detail::popScopedMessage( m_messageId ); }
    }


    Capturer::Capturer( StringRef macroName,
                        SourceLineInfo const& lineInfo,
                        ResultWas::OfType resultType,
                        StringRef names,
                        bool isScoped):
        m_isScoped(isScoped) {
        auto trimmed = [&] (size_t start, size_t end) {
            while (names[start] == ',' || isspace(static_cast<unsigned char>(names[start]))) {
                ++start;
            }
            while (names[end] == ',' || isspace(static_cast<unsigned char>(names[end]))) {
                --end;
            }
            return names.substr(start, end - start + 1);
        };
        auto skipq = [&] (size_t start, char quote) {
            for (auto i = start + 1; i < names.size() ; ++i) {
                if (names[i] == quote)
                    return i;
                if (names[i] == '\\')
                    ++i;
            }
            CATCH_INTERNAL_ERROR("CAPTURE parsing encountered unmatched quote");
        };

        size_t start = 0;
        std::stack<char> openings;
        for (size_t pos = 0; pos < names.size(); ++pos) {
            char c = names[pos];
            switch (c) {
            case '[':
            case '{':
            case '(':
            // It is basically impossible to disambiguate between
            // comparison and start of template args in this context
//            case '<':
                openings.push(c);
                break;
            case ']':
            case '}':
            case ')':
//           case '>':
                openings.pop();
                break;
            case '"':
            case '\'':
                pos = skipq(pos, c);
                break;
            case ',':
                if (start != pos && openings.empty()) {
                    m_messages.emplace_back(macroName, lineInfo, resultType);
                    m_messages.back().message += trimmed(start, pos);
                    m_messages.back().message += " := "_sr;
                    start = pos;
                }
                break;
            default:; // noop
            }
        }
        assert(openings.empty() && "Mismatched openings");
        m_messages.emplace_back(macroName, lineInfo, resultType);
        m_messages.back().message += trimmed(start, names.size() - 1);
        m_messages.back().message += " := "_sr;
    }
    Capturer::~Capturer() {
        assert( m_captured == m_messages.size() );
        if ( m_isScoped ) {
            for ( auto const& message : m_messages ) {
                Detail::popScopedMessage( message.sequence );
            }
        }
    }

    void Capturer::captureValue( size_t index, std::string const& value ) {
        assert( index < m_messages.size() );
        m_messages[index].message += value;
        if ( m_isScoped ) {
            Detail::pushScopedMessage( CATCH_MOVE( m_messages[index] ) );
        } else {
            Detail::addUnscopedMessage( CATCH_MOVE( m_messages[index] ) );
        }
        m_captured++;
    }

} // end namespace Catch




#include <exception>

namespace Catch {

    namespace {

        class RegistryHub : public IRegistryHub,
                            public IMutableRegistryHub,
                            private Detail::NonCopyable {

        public: // IRegistryHub
            RegistryHub() = default;
            ReporterRegistry const& getReporterRegistry() const override {
                return m_reporterRegistry;
            }
            ITestCaseRegistry const& getTestCaseRegistry() const override {
                return m_testCaseRegistry;
            }
            IExceptionTranslatorRegistry const& getExceptionTranslatorRegistry() const override {
                return m_exceptionTranslatorRegistry;
            }
            ITagAliasRegistry const& getTagAliasRegistry() const override {
                return m_tagAliasRegistry;
            }
            StartupExceptionRegistry const& getStartupExceptionRegistry() const override {
                return m_exceptionRegistry;
            }

        public: // IMutableRegistryHub
            void registerListener( Detail::unique_ptr<EventListenerFactory> factory ) override {
                m_reporterRegistry.registerListener( CATCH_MOVE(factory) );
            }
            void registerTest( Detail::unique_ptr<TestCaseInfo>&& testInfo, Detail::unique_ptr<ITestInvoker>&& invoker ) override {
                m_testCaseRegistry.registerTest( CATCH_MOVE(testInfo), CATCH_MOVE(invoker) );
            }
            void registerTranslator( Detail::unique_ptr<IExceptionTranslator>&& translator ) override {
                m_exceptionTranslatorRegistry.registerTranslator( CATCH_MOVE(translator) );
            }
            void registerTagAlias( std::string const& alias, std::string const& tag, SourceLineInfo const& lineInfo ) override {
                m_tagAliasRegistry.add( alias, tag, lineInfo );
            }
            void registerStartupException() noexcept override {
#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
                m_exceptionRegistry.add(std::current_exception());
#else
                CATCH_INTERNAL_ERROR("Attempted to register active exception under CATCH_CONFIG_DISABLE_EXCEPTIONS!");
#endif
            }

            ITestCaseRegistry& getMutableTestCaseRegistry() override {
                return m_testCaseRegistry;
            }

        private:
            TestRegistry m_testCaseRegistry;
            ReporterRegistry m_reporterRegistry;
            ExceptionTranslatorRegistry m_exceptionTranslatorRegistry;
            TagAliasRegistry m_tagAliasRegistry;
            StartupExceptionRegistry m_exceptionRegistry;
        };
    }

    using RegistryHubSingleton = Singleton<RegistryHub, IRegistryHub, IMutableRegistryHub>;

    IRegistryHub const& getRegistryHub() {
        return RegistryHubSingleton::get();
    }
    IMutableRegistryHub& getMutableRegistryHub() {
        return RegistryHubSingleton::getMutable();
    }
    void cleanUp() {
        cleanupSingletons();
    }
    std::string translateActiveException() {
        return getRegistryHub().getExceptionTranslatorRegistry().translateActiveException();
    }


} // end namespace Catch




namespace Catch {

    void EventListenerBase::fatalErrorEncountered( StringRef ) {}

    void EventListenerBase::benchmarkPreparing( StringRef ) {}
    void EventListenerBase::benchmarkStarting( BenchmarkInfo const& ) {}
    void EventListenerBase::benchmarkEnded( BenchmarkStats<> const& ) {}
    void EventListenerBase::benchmarkFailed( StringRef ) {}

    void EventListenerBase::assertionStarting( AssertionInfo const& ) {}

    void EventListenerBase::assertionEnded( AssertionStats const& ) {}
    void EventListenerBase::noMatchingTestCases( StringRef ) {}
    void EventListenerBase::reportInvalidTestSpec( StringRef ) {}
    void EventListenerBase::testRunStarting( TestRunInfo const& ) {}
    void EventListenerBase::testCaseStarting( TestCaseInfo const& ) {}
    void EventListenerBase::testCasePartialStarting(TestCaseInfo const&, uint64_t) {}
    void EventListenerBase::sectionStarting( SectionInfo const& ) {}
    void EventListenerBase::sectionEnded( SectionStats const& ) {}
    void EventListenerBase::testCasePartialEnded(TestCaseStats const&, uint64_t) {}
    void EventListenerBase::testCaseEnded( TestCaseStats const& ) {}
    void EventListenerBase::testRunEnded( TestRunStats const& ) {}
} // namespace Catch




#include <ostream>

namespace Catch {
    void MultiReporter::updatePreferences(IEventListener const& reporterish) {
        m_preferences.shouldRedirectStdOut |=
            reporterish.getPreferences().shouldRedirectStdOut;
        m_preferences.shouldReportAllAssertions |=
            reporterish.getPreferences().shouldReportAllAssertions;
        m_preferences.shouldReportAllAssertionStarts |=
            reporterish.getPreferences().shouldReportAllAssertionStarts;
    }

    void MultiReporter::addListener( IEventListenerPtr&& listener ) {
        updatePreferences(*listener);
        m_reporterLikes.insert(m_reporterLikes.begin() + m_insertedListeners, CATCH_MOVE(listener) );
        ++m_insertedListeners;
    }

    void MultiReporter::addReporter( IEventListenerPtr&& reporter ) {
        updatePreferences(*reporter);

        // We will need to output the captured stdout if there are reporters
        // that do not want it captured.
        // We do not consider listeners, because it is generally assumed that
        // listeners are output-transparent, even though they can ask for stdout
        // capture to do something with it.
        m_haveNoncapturingReporters |= !reporter->getPreferences().shouldRedirectStdOut;

        // Reporters can always be placed to the back without breaking the
        // reporting order
        m_reporterLikes.push_back( CATCH_MOVE( reporter ) );
    }

    void MultiReporter::noMatchingTestCases( StringRef unmatchedSpec ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->noMatchingTestCases( unmatchedSpec );
        }
    }

    void MultiReporter::fatalErrorEncountered( StringRef error ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->fatalErrorEncountered( error );
        }
    }

    void MultiReporter::reportInvalidTestSpec( StringRef arg ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->reportInvalidTestSpec( arg );
        }
    }

    void MultiReporter::benchmarkPreparing( StringRef name ) {
        for (auto& reporterish : m_reporterLikes) {
            reporterish->benchmarkPreparing(name);
        }
    }
    void MultiReporter::benchmarkStarting( BenchmarkInfo const& benchmarkInfo ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->benchmarkStarting( benchmarkInfo );
        }
    }
    void MultiReporter::benchmarkEnded( BenchmarkStats<> const& benchmarkStats ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->benchmarkEnded( benchmarkStats );
        }
    }

    void MultiReporter::benchmarkFailed( StringRef error ) {
        for (auto& reporterish : m_reporterLikes) {
            reporterish->benchmarkFailed(error);
        }
    }

    void MultiReporter::testRunStarting( TestRunInfo const& testRunInfo ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->testRunStarting( testRunInfo );
        }
    }

    void MultiReporter::testCaseStarting( TestCaseInfo const& testInfo ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->testCaseStarting( testInfo );
        }
    }

    void
    MultiReporter::testCasePartialStarting( TestCaseInfo const& testInfo,
                                                uint64_t partNumber ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->testCasePartialStarting( testInfo, partNumber );
        }
    }

    void MultiReporter::sectionStarting( SectionInfo const& sectionInfo ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->sectionStarting( sectionInfo );
        }
    }

    void MultiReporter::assertionStarting( AssertionInfo const& assertionInfo ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->assertionStarting( assertionInfo );
        }
    }

    void MultiReporter::assertionEnded( AssertionStats const& assertionStats ) {
        const bool reportByDefault =
            assertionStats.assertionResult.getResultType() != ResultWas::Ok ||
            m_config->includeSuccessfulResults();

        for ( auto & reporterish : m_reporterLikes ) {
            if ( reportByDefault ||
                 reporterish->getPreferences().shouldReportAllAssertions ) {
                    reporterish->assertionEnded( assertionStats );
            }
        }
    }

    void MultiReporter::sectionEnded( SectionStats const& sectionStats ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->sectionEnded( sectionStats );
        }
    }

    void MultiReporter::testCasePartialEnded( TestCaseStats const& testStats,
                                                  uint64_t partNumber ) {
        if ( m_preferences.shouldRedirectStdOut &&
             m_haveNoncapturingReporters ) {
            if ( !testStats.stdOut.empty() ) {
                Catch::cout() << testStats.stdOut << std::flush;
            }
            if ( !testStats.stdErr.empty() ) {
                Catch::cerr() << testStats.stdErr << std::flush;
            }
        }

        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->testCasePartialEnded( testStats, partNumber );
        }
    }

    void MultiReporter::testCaseEnded( TestCaseStats const& testCaseStats ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->testCaseEnded( testCaseStats );
        }
    }

    void MultiReporter::testRunEnded( TestRunStats const& testRunStats ) {
        for ( auto& reporterish : m_reporterLikes ) {
            reporterish->testRunEnded( testRunStats );
        }
    }


} // end namespace Catch





namespace Catch {
    namespace Detail {

        void registerListenerImpl( Detail::unique_ptr<EventListenerFactory> listenerFactory ) {
            getMutableRegistryHub().registerListener( CATCH_MOVE(listenerFactory) );
        }


    } // namespace Detail
} // namespace Catch




namespace Catch {

    Session::Session() {
        static bool alreadyInstantiated = false;
        if( alreadyInstantiated ) {
            CATCH_TRY { CATCH_INTERNAL_ERROR( "Only one instance of Catch::Session can ever be used" ); }
            CATCH_CATCH_ALL { getMutableRegistryHub().registerStartupException(); }
        }
        alreadyInstantiated = true;
    }
    Session::~Session() {
        Catch::cleanUp();
    }

    void Session::showHelp() const {
        Progmasoft::Catch3::PrintHelp( m_arguments.empty() ? std::string( "tests" )
                                                           : m_arguments.front() );
    }

    int Session::applyCommandLine( int argc, char const * const * argv ) {
        m_arguments.assign( argv, argv + argc );
        return 0;
    }

#if defined(CATCH_CONFIG_WCHAR) && defined(_WIN32) && defined(UNICODE)
    int Session::applyCommandLine( int argc, wchar_t const * const * argv ) {
        m_arguments.clear();
        for ( int i = 0; i < argc; ++i ) {
            int bufSize = WideCharToMultiByte( CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr, nullptr );
            std::string utf8( static_cast<std::size_t>( bufSize ), '\0' );
            WideCharToMultiByte( CP_UTF8, 0, argv[i], -1, &utf8[0], bufSize, nullptr, nullptr );
            if ( !utf8.empty() ) { utf8.pop_back(); } // the terminator
            m_arguments.push_back( utf8 );
        }
        return 0;
    }
#endif

    int Session::run() {
        return Progmasoft::Catch3::RunTests( m_arguments );
    }

} // end namespace Catch




namespace Catch {

    RegistrarForTagAliases::RegistrarForTagAliases(char const* alias, char const* tag, SourceLineInfo const& lineInfo) {
        CATCH_TRY {
            getMutableRegistryHub().registerTagAlias(alias, tag, lineInfo);
        } CATCH_CATCH_ALL {
            // Do not throw when constructing global objects, instead register the exception to be processed later
            getMutableRegistryHub().registerStartupException();
        }
    }

}



#include <cassert>
#include <cctype>
#include <algorithm>
#include <utility>

namespace Catch {

    namespace {
        using TCP_underlying_type = uint8_t;
        static_assert(sizeof(TestCaseProperties) == sizeof(TCP_underlying_type),
                      "The size of the TestCaseProperties is different from the assumed size");

        constexpr TestCaseProperties operator|(TestCaseProperties lhs, TestCaseProperties rhs) {
            return static_cast<TestCaseProperties>(
                static_cast<TCP_underlying_type>(lhs) | static_cast<TCP_underlying_type>(rhs)
            );
        }

        constexpr TestCaseProperties& operator|=(TestCaseProperties& lhs, TestCaseProperties rhs) {
            lhs = static_cast<TestCaseProperties>(
                static_cast<TCP_underlying_type>(lhs) | static_cast<TCP_underlying_type>(rhs)
            );
            return lhs;
        }

        constexpr TestCaseProperties operator&(TestCaseProperties lhs, TestCaseProperties rhs) {
            return static_cast<TestCaseProperties>(
                static_cast<TCP_underlying_type>(lhs) & static_cast<TCP_underlying_type>(rhs)
            );
        }

        constexpr bool applies(TestCaseProperties tcp) {
            static_assert(static_cast<TCP_underlying_type>(TestCaseProperties::None) == 0,
                          "TestCaseProperties::None must be equal to 0");
            return tcp != TestCaseProperties::None;
        }

        bool isRunnerTagWithValue( StringRef tag ) {
            for ( StringRef prefix : { "!resource:"_sr, "!depends:"_sr,
                                       "!timeout:"_sr, "!retries:"_sr } ) {
                if ( tag.size() >= prefix.size() &&
                     tag.substr( 0, prefix.size() ) == prefix ) {
                    return true;
                }
            }
            return false;
        }

        TestCaseProperties propertiesFromTag( StringRef tag, SourceLineInfo const& _lineInfo ) {
            if ( tag.empty() ) { return TestCaseProperties::None; }
            // All tags starting with alphanumeric characters are normal.
            if ( std::isalnum( tag[0] ) ) { return TestCaseProperties::None; }

            // Explicitly check known special tags, and error out if it is not one of them
            if ( tag[0] == '.' ) { return TestCaseProperties::IsHidden; }
            else if ( tag == "!throws"_sr ) { return TestCaseProperties::Throws; }
            else if ( tag == "!shouldfail"_sr ) { return TestCaseProperties::ShouldFail; }
            else if ( tag == "!mayfail"_sr ) { return TestCaseProperties::MayFail; }
            else if ( tag == "!nonportable"_sr ) { return TestCaseProperties::NonPortable; }
            else if ( tag == "!benchmark"_sr ) {
                return TestCaseProperties::Benchmark |
                       TestCaseProperties::IsHidden;
            }
            // Read by the Catch3 runner: scheduling, limits and quarantine.
            else if ( tag == "!serial"_sr || tag == "!isolated"_sr ||
                      tag == "!quarantine"_sr ) {
                return TestCaseProperties::None;
            } else if ( isRunnerTagWithValue( tag ) ) {
                CATCH_ENFORCE( tag[tag.size() - 1] != ':',
                               "Tag name: [" << tag << "] needs a value after ':'\n"
                                             << _lineInfo );
                return TestCaseProperties::None;
            } else {
                CATCH_RUNTIME_ERROR(
                    "Tag name: [" << tag << "] is not allowed.\n"
                                  << "Tag names starting with non-alphanumeric "
                                     "characters are reserved.\n"
                                  << _lineInfo );
            }
        }

        std::string makeDefaultName() {
            static size_t counter = 0;
            return "Anonymous test case " + std::to_string(++counter);
        }

        constexpr StringRef extractFilenamePart(StringRef filename) {
            size_t lastDot = filename.size();
            while (lastDot > 0 && filename[lastDot - 1] != '.') {
                --lastDot;
            }
            // In theory we could have filename without any extension in it
            if ( lastDot == 0 ) { return StringRef(); }

            --lastDot;
            size_t nameStart = lastDot;
            while (nameStart > 0 && filename[nameStart - 1] != '/' && filename[nameStart - 1] != '\\') {
                --nameStart;
            }

            return filename.substr(nameStart, lastDot - nameStart);
        }

        // Returns the upper bound on size of extra tags ([#file]+[.])
        constexpr size_t sizeOfExtraTags(StringRef filepath) {
            // [.] is 3, [#] is another 3
            const size_t extras = 3 + 3;
            return extractFilenamePart(filepath).size() + extras;
        }
    } // end unnamed namespace

    bool operator<(  Tag const& lhs, Tag const& rhs ) {
        Detail::CaseInsensitiveLess cmp;
        return cmp( lhs.original, rhs.original );
    }
    bool operator==( Tag const& lhs, Tag const& rhs ) {
        Detail::CaseInsensitiveEqualTo cmp;
        return cmp( lhs.original, rhs.original );
    }

    Detail::unique_ptr<TestCaseInfo>
        makeTestCaseInfo(StringRef _className,
                         NameAndTags const& nameAndTags,
                         SourceLineInfo const& _lineInfo ) {
        return Detail::make_unique<TestCaseInfo>(_className, nameAndTags, _lineInfo);
    }

    TestCaseInfo::TestCaseInfo(StringRef _className,
                               NameAndTags const& _nameAndTags,
                               SourceLineInfo const& _lineInfo):
        name( _nameAndTags.name.empty() ? makeDefaultName() : _nameAndTags.name ),
        className( _className ),
        lineInfo( _lineInfo )
    {
        StringRef originalTags = _nameAndTags.tags;
        // We need to reserve enough space to store all of the tags
        // (including optional hidden tag and filename tag)
        auto requiredSize = originalTags.size() + sizeOfExtraTags(_lineInfo.file);
        backingTags.reserve(requiredSize);

        // Since there is usually only small number of tags on a test case,
        // the geometric growth on vector does not get time to properly
        // amortize the reallocations. But we can save a lot of time by
        // pre-allocating space for all tags.
        if ( !originalTags.empty() ) {
            size_t tagCount = 1; // 1 for filename tag
            for ( size_t i = 0; i < originalTags.size(); ++i ) {
                if ( originalTags[i] == '[' ) { ++tagCount; }
            }
            tags.reserve( tagCount );
        }

        // We cannot copy the tags directly, as we need to normalize
        // some tags, so that [.foo] is copied as [.][foo].
        size_t tagStart = 0;
        size_t tagEnd = 0;
        bool inTag = false;
        for (size_t idx = 0; idx < originalTags.size(); ++idx) {
            auto c = originalTags[idx];
            if (c == '[') {
                CATCH_ENFORCE(
                    !inTag,
                    "Found '[' inside a tag while registering test case '"
                        << _nameAndTags.name << "' at " << _lineInfo );

                inTag = true;
                tagStart = idx;
            }
            if (c == ']') {
                CATCH_ENFORCE(
                    inTag,
                    "Found unmatched ']' while registering test case '"
                        << _nameAndTags.name << "' at " << _lineInfo );

                inTag = false;
                tagEnd = idx;
                assert(tagStart < tagEnd);

                // We need to check the tag for special meanings, copy
                // it over to backing storage and actually reference the
                // backing storage in the saved tags
                StringRef tagStr = originalTags.substr(tagStart+1, tagEnd - tagStart - 1);
                CATCH_ENFORCE( !tagStr.empty(),
                               "Found an empty tag while registering test case '"
                                   << _nameAndTags.name << "' at "
                                   << _lineInfo );

                properties |= propertiesFromTag(tagStr, _lineInfo);
                // When copying a tag to the backing storage, we need to
                // check if it is a merged hide tag, such as [.foo], and
                // if it is, we need to handle it as if it was [foo].
                if (tagStr.size() > 1 && tagStr[0] == '.') {
                    tagStr = tagStr.substr(1, tagStr.size() - 1);
                }
                // We skip over dealing with the [.] tag, as we will add
                // it later unconditionally and then sort and unique all
                // the tags.
                internalAppendTag(tagStr);
            }
        }
        CATCH_ENFORCE( !inTag,
                       "Found an unclosed tag while registering test case '"
                           << _nameAndTags.name << "' at " << _lineInfo );


        // Add [.] if relevant
        if (isHidden()) {
            internalAppendTag("."_sr);
        }

        // Tests usually have small number of tags, so we explicitly handle
        // the 1 and 2 tags cases for better performance in low-opt builds.
        switch ( tags.size() ) {
        case 1:
            break;
        case 2:
            if ( tags[0] == tags[1] ) { tags.pop_back(); }
            else if ( tags[1] < tags[0] ) { std::swap( tags[0], tags[1] ); }
            break;
        default:
            std::sort( begin( tags ), end( tags ) );
            tags.erase( std::unique( begin( tags ), end( tags ) ),
                        end( tags ) );
        }

    }

    bool TestCaseInfo::isHidden() const {
        return applies( properties & TestCaseProperties::IsHidden );
    }
    bool TestCaseInfo::throws() const {
        return applies( properties & TestCaseProperties::Throws );
    }
    bool TestCaseInfo::okToFail() const {
        return applies( properties & (TestCaseProperties::ShouldFail | TestCaseProperties::MayFail ) );
    }
    bool TestCaseInfo::expectedToFail() const {
        return applies( properties & (TestCaseProperties::ShouldFail) );
    }

    void TestCaseInfo::addFilenameTag() {
        std::string combined("#");
        combined += extractFilenamePart(lineInfo.file);
        internalAppendTag(combined);
    }

    std::string TestCaseInfo::tagsAsString() const {
        std::string ret;
        // '[' and ']' per tag
        std::size_t full_size = 2 * tags.size();
        for (const auto& tag : tags) {
            full_size += tag.original.size();
        }
        ret.reserve(full_size);
        for (const auto& tag : tags) {
            ret.push_back('[');
            ret += tag.original;
            ret.push_back(']');
        }

        return ret;
    }

    void TestCaseInfo::internalAppendTag(StringRef tagStr) {
        backingTags += '[';
        const auto backingStart = backingTags.size();
        backingTags += tagStr;
        const auto backingEnd = backingTags.size();
        backingTags += ']';
        tags.emplace_back(StringRef(backingTags.c_str() + backingStart, backingEnd - backingStart));
    }

    bool operator<( TestCaseInfo const& lhs, TestCaseInfo const& rhs ) {
        // We want to avoid redoing the string comparisons multiple times,
        // so we store the result of a three-way comparison before using
        // it in the actual comparison logic.
        const auto cmpName = lhs.name.compare( rhs.name );
        if ( cmpName != 0 ) {
            return cmpName < 0;
        }
        const auto cmpClassName = lhs.className.compare( rhs.className );
        if ( cmpClassName != 0 ) {
            return cmpClassName < 0;
        }
        return lhs.tags < rhs.tags;
    }

    bool operator==( TestCaseInfo const& lhs, TestCaseInfo const& rhs ) {
        return lhs.name == rhs.name
            && lhs.className == rhs.className
            && lhs.tags == rhs.tags;
    }

} // end namespace Catch



#include <algorithm>
#include <string>
#include <vector>
#include <ostream>

namespace Catch {

    TestSpec::Pattern::Pattern( std::string const& name )
    : m_name( name )
    {}

    TestSpec::Pattern::~Pattern() = default;

    std::string const& TestSpec::Pattern::name() const {
        return m_name;
    }


    TestSpec::NamePattern::NamePattern( std::string const& name, std::string const& filterString )
    : Pattern( filterString )
    , m_wildcardPattern( name, CaseSensitive::No )
    {}

    bool TestSpec::NamePattern::matches( TestCaseInfo const& testCase ) const {
        return m_wildcardPattern.matches( testCase.name );
    }

    void TestSpec::NamePattern::serializeTo( std::ostream& out ) const {
        out << '"' << name() << '"';
    }


    TestSpec::TagPattern::TagPattern( std::string const& tag, std::string const& filterString )
    : Pattern( filterString )
    , m_tag( tag )
    {}

    bool TestSpec::TagPattern::matches( TestCaseInfo const& testCase ) const {
        return std::find( begin( testCase.tags ),
                          end( testCase.tags ),
                          Tag( m_tag ) ) != end( testCase.tags );
    }

    void TestSpec::TagPattern::serializeTo( std::ostream& out ) const {
        out << name();
    }

    bool TestSpec::Filter::matches( TestCaseInfo const& testCase ) const {
        bool should_use = !testCase.isHidden();
        for (auto const& pattern : m_required) {
            should_use = true;
            if (!pattern->matches(testCase)) {
                return false;
            }
        }
        for (auto const& pattern : m_forbidden) {
            if (pattern->matches(testCase)) {
                return false;
            }
        }
        return should_use;
    }

    void TestSpec::Filter::serializeTo( std::ostream& out ) const {
        bool first = true;
        for ( auto const& pattern : m_required ) {
            if ( !first ) {
                out << ' ';
            }
            out << *pattern;
            first = false;
        }
        for ( auto const& pattern : m_forbidden ) {
            if ( !first ) {
                out << ' ';
            }
            out << *pattern;
            first = false;
        }
    }


    std::string TestSpec::extractFilterName( Filter const& filter ) {
        Catch::ReusableStringStream sstr;
        sstr << filter;
        return sstr.str();
    }

    bool TestSpec::hasFilters() const {
        return !m_filters.empty();
    }

    bool TestSpec::matches( TestCaseInfo const& testCase ) const {
        return std::any_of( m_filters.begin(), m_filters.end(), [&]( Filter const& f ){ return f.matches( testCase ); } );
    }

    TestSpec::Matches TestSpec::matchesByFilter( std::vector<TestCaseHandle> const& testCases, IConfig const& config ) const {
        Matches matches;
        matches.reserve( m_filters.size() );
        for ( auto const& filter : m_filters ) {
            std::vector<TestCaseHandle const*> currentMatches;
            for ( auto const& test : testCases ) {
                if ( isThrowSafe( test, config ) &&
                     filter.matches( test.getTestCaseInfo() ) ) {
                    currentMatches.emplace_back( &test );
                }
            }
            matches.push_back( FilterMatch{ extractFilterName( filter ),
                                            CATCH_MOVE( currentMatches ) } );
        }
        return matches;
    }

    const TestSpec::vectorStrings& TestSpec::getInvalidSpecs() const {
        return m_invalidSpecs;
    }

    void TestSpec::serializeTo( std::ostream& out ) const {
        bool first = true;
        for ( auto const& filter : m_filters ) {
            if ( !first ) {
                out << ',';
            }
            out << filter;
            first = false;
        }
    }

}



#include <chrono>

namespace Catch {

    namespace {
        static auto getCurrentNanosecondsSinceEpoch() -> uint64_t {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        }
    } // end unnamed namespace

    void Timer::start() {
       m_nanoseconds = getCurrentNanosecondsSinceEpoch();
    }
    auto Timer::getElapsedNanoseconds() const -> uint64_t {
        return getCurrentNanosecondsSinceEpoch() - m_nanoseconds;
    }
    auto Timer::getElapsedMicroseconds() const -> uint64_t {
        return getElapsedNanoseconds()/1000;
    }
    auto Timer::getElapsedMilliseconds() const -> unsigned int {
        return static_cast<unsigned int>(getElapsedMicroseconds()/1000);
    }
    auto Timer::getElapsedSeconds() const -> double {
        return static_cast<double>(getElapsedMicroseconds())/1000000.0;
    }


} // namespace Catch




#include <iomanip>

namespace Catch {

namespace Detail {

    namespace {
        const int hexThreshold = 255;

        struct Endianness {
            enum Arch : uint8_t {
                Big,
                Little
            };

            static Arch which() {
                int one = 1;
                // If the lowest byte we read is non-zero, we can assume
                // that little endian format is used.
                auto value = *reinterpret_cast<char*>(&one);
                return value ? Little : Big;
            }
        };

        template<typename T>
        std::string fpToString(T value, int precision) {
            if (Catch::isnan(value)) {
                return "nan";
            }

            ReusableStringStream rss;
            rss << std::setprecision(precision)
                << std::fixed
                << value;
            std::string d = rss.str();
            std::size_t i = d.find_last_not_of('0');
            if (i != std::string::npos && i != d.size() - 1) {
                if (d[i] == '.')
                    i++;
                d = d.substr(0, i + 1);
            }
            return d;
        }
    } // end unnamed namespace

    std::size_t catch_strnlen( const char* str, std::size_t n ) {
        auto ret = std::char_traits<char>::find( str, n, '\0' );
        if ( ret != nullptr ) { return static_cast<std::size_t>( ret - str ); }
        return n;
    }

    std::string formatTimeT(std::time_t time) {
        std::tm timeInfo = {};
#ifdef _MSC_VER
        const auto err = gmtime_s( &timeInfo, &time );
        if ( err ) {
            return "gmtime from provided timepoint has failed. This "
                   "happens e.g. with pre-1970 dates using Microsoft libc";
        }
#elif defined( __MINGW32__ )
        if ( gmtime_s( &timeInfo, &time ) != 0 ) {
            return "gmtime from provided timepoint has failed";
        }
#elif defined( CATCH_PLATFORM_PLAYSTATION )
        gmtime_s( &time, &timeInfo );
#elif defined( __IAR_SYSTEMS_ICC__ )
        // This toolchain does not provide gmtime_r. Copy the result immediately
        // so the formatter does not retain the C library's static buffer.
        const std::tm* const converted = std::gmtime( &time );
        if ( converted == nullptr ) {
            return "gmtime from provided timepoint has failed";
        }
        timeInfo = *converted;
#else
        if ( gmtime_r( &time, &timeInfo ) == nullptr ) {
            return "gmtime from provided timepoint has failed";
        }
#endif

        auto const timeStampSize = sizeof( "2017-01-16T17:06:45Z" );
        char timeStamp[timeStampSize] = {};
        const char* const fmt = "%Y-%m-%dT%H:%M:%SZ";

        if ( std::strftime( timeStamp, timeStampSize, fmt, &timeInfo ) == 0 ) {
            return "gmtime from provided timepoint has failed";
        }
        return std::string( timeStamp, timeStampSize - 1 );
    }

    std::string convertIntoString(StringRef string, bool escapeInvisibles) {
        std::string ret;
        // This is enough for the "don't escape invisibles" case, and a good
        // lower bound on the "escape invisibles" case.
        ret.reserve( string.size() + 2 );

        if ( !escapeInvisibles ) {
            ret += '"';
            ret += string;
            ret += '"';
            return ret;
        }

        size_t last_start = 0;
        auto write_to = [&]( size_t idx ) {
            if ( last_start < idx ) {
                ret += string.substr( last_start, idx - last_start );
            }
            last_start = idx + 1;
        };

        ret += '"';
        for ( size_t i = 0; i < string.size(); ++i ) {
            const char c = string[i];
            if ( c == '\r' || c == '\n' || c == '\t' || c == '\f' ) {
                write_to( i );
                if ( c == '\r' ) { ret.append( "\\r" ); }
                if ( c == '\n' ) { ret.append( "\\n" ); }
                if ( c == '\t' ) { ret.append( "\\t" ); }
                if ( c == '\f' ) { ret.append( "\\f" ); }
            }
        }
        write_to( string.size() );
        ret += '"';

        return ret;
    }

    std::string convertIntoString(StringRef string) {
        return convertIntoString(string, getCurrentContext().getConfig()->showInvisibles());
    }

    std::string rawMemoryToString( const void *object, std::size_t size ) {
        // Reverse order for little endian architectures
        int i = 0, end = static_cast<int>( size ), inc = 1;
        if( Endianness::which() == Endianness::Little ) {
            i = end-1;
            end = inc = -1;
        }

        unsigned char const *bytes = static_cast<unsigned char const *>(object);
        ReusableStringStream rss;
        rss << "0x" << std::setfill('0') << std::hex;
        for( ; i != end; i += inc )
             rss << std::setw(2) << static_cast<unsigned>(bytes[i]);
       return rss.str();
    }

    std::string makeExceptionHappenedString() {
        return "{ stringification failed with an exception: \"" +
               translateActiveException() + "\" }";

    }

} // end Detail namespace



//// ======================================================= ////
//
//   Out-of-line defs for full specialization of StringMaker
//
//// ======================================================= ////

std::string StringMaker<std::string>::convert(const std::string& str) {
    return Detail::convertIntoString( str );
}

#ifdef CATCH_CONFIG_CPP17_STRING_VIEW
std::string StringMaker<std::string_view>::convert(std::string_view str) {
    return Detail::convertIntoString( StringRef( str.data(), str.size() ) );
}
#endif

std::string StringMaker<char const*>::convert(char const* str) {
    if (str) {
        return Detail::convertIntoString( str );
    } else {
        return{ "{null string}" };
    }
}
std::string StringMaker<char*>::convert(char* str) { // NOLINT(readability-non-const-parameter)
    if (str) {
        return Detail::convertIntoString( str );
    } else {
        return{ "{null string}" };
    }
}

#ifdef CATCH_CONFIG_WCHAR
std::string StringMaker<std::wstring>::convert(const std::wstring& wstr) {
    std::string s;
    s.reserve(wstr.size());
    for (auto c : wstr) {
        s += (c <= 0xff) ? static_cast<char>(c) : '?';
    }
    return ::Catch::Detail::stringify(s);
}

# ifdef CATCH_CONFIG_CPP17_STRING_VIEW
std::string StringMaker<std::wstring_view>::convert(std::wstring_view str) {
    return StringMaker<std::wstring>::convert(std::wstring(str));
}
# endif

std::string StringMaker<wchar_t const*>::convert(wchar_t const * str) {
    if (str) {
        return ::Catch::Detail::stringify(std::wstring{ str });
    } else {
        return{ "{null string}" };
    }
}
std::string StringMaker<wchar_t *>::convert(wchar_t * str) {
    if (str) {
        return ::Catch::Detail::stringify(std::wstring{ str });
    } else {
        return{ "{null string}" };
    }
}
#endif

#if defined(CATCH_CONFIG_CPP17_BYTE)
#include <cstddef>
std::string StringMaker<std::byte>::convert(std::byte value) {
    return ::Catch::Detail::stringify(std::to_integer<unsigned long long>(value));
}
#endif // defined(CATCH_CONFIG_CPP17_BYTE)

std::string StringMaker<int>::convert(int value) {
    return ::Catch::Detail::stringify(static_cast<long long>(value));
}
std::string StringMaker<long>::convert(long value) {
    return ::Catch::Detail::stringify(static_cast<long long>(value));
}
std::string StringMaker<long long>::convert(long long value) {
    ReusableStringStream rss;
    rss << value;
    if (value > Detail::hexThreshold) {
        rss << " (0x" << std::hex << value << ')';
    }
    return rss.str();
}

std::string StringMaker<unsigned int>::convert(unsigned int value) {
    return ::Catch::Detail::stringify(static_cast<unsigned long long>(value));
}
std::string StringMaker<unsigned long>::convert(unsigned long value) {
    return ::Catch::Detail::stringify(static_cast<unsigned long long>(value));
}
std::string StringMaker<unsigned long long>::convert(unsigned long long value) {
    ReusableStringStream rss;
    rss << value;
    if (value > Detail::hexThreshold) {
        rss << " (0x" << std::hex << value << ')';
    }
    return rss.str();
}

std::string StringMaker<signed char>::convert(signed char value) {
    if (value == '\r') {
        return "'\\r'";
    } else if (value == '\f') {
        return "'\\f'";
    } else if (value == '\n') {
        return "'\\n'";
    } else if (value == '\t') {
        return "'\\t'";
    } else if ('\0' <= value && value < ' ') {
        return ::Catch::Detail::stringify(static_cast<unsigned int>(value));
    } else {
        char chstr[] = "' '";
        chstr[1] = value;
        return chstr;
    }
}
std::string StringMaker<char>::convert(char c) {
    return ::Catch::Detail::stringify(static_cast<signed char>(c));
}
std::string StringMaker<unsigned char>::convert(unsigned char value) {
    return ::Catch::Detail::stringify(static_cast<char>(value));
}

int StringMaker<float>::precision = std::numeric_limits<float>::max_digits10;

std::string StringMaker<float>::convert(float value) {
    return Detail::fpToString(value, precision) + 'f';
}

int StringMaker<double>::precision = std::numeric_limits<double>::max_digits10;

std::string StringMaker<double>::convert(double value) {
    return Detail::fpToString(value, precision);
}

} // end namespace Catch



namespace Catch {

    Counts Counts::operator - ( Counts const& other ) const {
        Counts diff;
        diff.passed = passed - other.passed;
        diff.failed = failed - other.failed;
        diff.failedButOk = failedButOk - other.failedButOk;
        diff.skipped = skipped - other.skipped;
        return diff;
    }

    Counts& Counts::operator += ( Counts const& other ) {
        passed += other.passed;
        failed += other.failed;
        failedButOk += other.failedButOk;
        skipped += other.skipped;
        return *this;
    }

    std::uint64_t Counts::total() const {
        return passed + failed + failedButOk + skipped;
    }
    bool Counts::allPassed() const {
        return failed == 0 && failedButOk == 0 && skipped == 0;
    }
    bool Counts::allOk() const {
        return failed == 0;
    }

    Totals Totals::operator - ( Totals const& other ) const {
        Totals diff;
        diff.assertions = assertions - other.assertions;
        diff.testCases = testCases - other.testCases;
        return diff;
    }

    Totals& Totals::operator += ( Totals const& other ) {
        assertions += other.assertions;
        testCases += other.testCases;
        return *this;
    }

    Totals Totals::delta( Totals const& prevTotals ) const {
        Totals diff = *this - prevTotals;
        if( diff.assertions.failed > 0 )
            ++diff.testCases.failed;
        else if( diff.assertions.failedButOk > 0 )
            ++diff.testCases.failedButOk;
        else if ( diff.assertions.skipped > 0 )
            ++ diff.testCases.skipped;
        else
            ++diff.testCases.passed;
        return diff;
    }

}




namespace Catch {
    namespace Detail {
        void registerTranslatorImpl(
            Detail::unique_ptr<IExceptionTranslator>&& translator ) {
            getMutableRegistryHub().registerTranslator(
                CATCH_MOVE( translator ) );
        }
    } // namespace Detail
} // namespace Catch
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <algorithm>
#include <cstdio>
#include <cstdlib>

namespace Progmasoft::Catch3::Detail
{
    namespace
    {
        constexpr auto kMacro = "CATCH3_EXPECT_CALL";

        /// Whether a test case is running that a report can belong to.
        [[nodiscard]] bool
        InTestCase()
        {
            const auto *capture = Catch::getCurrentContext().getResultCapture();
            return capture != nullptr && !capture->getCurrentTestName().empty();
        }

        void
        Report(const Catch::SourceLineInfo &line,
               const std::string &mock,
               Catch::ResultWas::OfType result,
               std::string message,
               Catch::ResultDisposition::Flags disposition)
        {
            Catch::AssertionHandler handler(kMacro, line, mock, disposition);
            handler.handleMessage(result, std::move(message));
            handler.complete();
        }

        void
        WriteToStandardError(const Catch::SourceLineInfo &line,
                             const std::string &mock,
                             const std::string &message)
        {
            std::fprintf(stderr,
                         "%s:%zu: %s: %s\n",
                         line.file,
                         line.line,
                         mock.c_str(),
                         message.c_str());
        }
    } // namespace

    void
    MockSequence::Add(const std::shared_ptr<MockExpectationState> &step)
    {
        steps.push_back(step);
    }

    std::string
    MockSequence::Enter(const MockExpectationState &step)
    {
        const auto position
            = std::find_if(steps.begin() + static_cast<std::ptrdiff_t>(current),
                           steps.end(),
                           [&step](const auto &candidate) {
                               return candidate.lock().get() == &step;
                           });
        std::string undone;
        if (position == steps.end())
        {
            return undone;
        }
        const auto target = static_cast<std::size_t>(position - steps.begin());
        for (; current < target; ++current)
        {
            const auto earlier = steps[current].lock();
            if (earlier == nullptr)
            {
                continue;
            }
            if (!earlier->Satisfied())
            {
                undone += (undone.empty() ? "call out of order: " : "\n  and ")
                          + step.Description + " came before "
                          + earlier->Description;
            }
            earlier->Retired = true;
        }
        return undone;
    }

    std::unique_lock<std::recursive_mutex>
    LockMocks()
    {
        // Never destroyed: a mock of static lifetime can go after it would.
        static auto *mutex = new std::recursive_mutex;
        return std::unique_lock<std::recursive_mutex>{ *mutex };
    }

    void
    ReportMockFailure(const Catch::SourceLineInfo &line,
                      const std::string &mock,
                      std::string message)
    {
        if (!InTestCase())
        {
            WriteToStandardError(line, mock, message);
            return;
        }
        // An aborting run throws out of any failure. The run still stops, at
        // its next assertion.
        CATCH_TRY
        {
            Report(line,
                   mock,
                   Catch::ResultWas::ExplicitFailure,
                   std::move(message),
                   Catch::ResultDisposition::ContinueOnFailure);
        }
        CATCH_CATCH_ALL
        {}
    }

    void
    AbortMockCall(const Catch::SourceLineInfo &line,
                  const std::string &mock,
                  std::string message)
    {
        if (InTestCase())
        {
            Report(line,
                   mock,
                   Catch::ResultWas::ExplicitFailure,
                   std::move(message),
                   Catch::ResultDisposition::Normal);
        }
        else
        {
            WriteToStandardError(line, mock, message);
        }
        std::abort();
    }

    void
    ReportMockSuccess(const Catch::SourceLineInfo &line,
                      const std::string &mock,
                      std::string message)
    {
        if (InTestCase())
        {
            Report(line,
                   mock,
                   Catch::ResultWas::Ok,
                   std::move(message),
                   Catch::ResultDisposition::ContinueOnFailure);
        }
    }

    std::string
    DescribeCalls(std::size_t minimum, std::size_t maximum)
    {
        const auto calls = [](std::size_t count) {
            return std::to_string(count) + (count == 1 ? " call" : " calls");
        };
        if (minimum == 0 && maximum == kUnlimitedCalls)
        {
            return "any number of calls";
        }
        if (minimum == maximum)
        {
            return maximum == 0 ? "no calls" : "exactly " + calls(minimum);
        }
        if (maximum == kUnlimitedCalls)
        {
            return "at least " + calls(minimum);
        }
        if (minimum == 0)
        {
            return "at most " + calls(maximum);
        }
        return std::to_string(minimum) + " to " + calls(maximum);
    }

    std::string
    DescribeCount(std::size_t calls)
    {
        switch (calls)
        {
            case 0:
                return "never called";
            case 1:
                return "called once";
            default:
                return "called " + std::to_string(calls) + " times";
        }
    }
} // namespace Progmasoft::Catch3::Detail
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <array>
#include <string_view>
#include <utility>

namespace Progmasoft::Catch3
{
    namespace
    {
        constexpr std::array<std::pair<Outcome, std::string_view>, 9>
            kOutcomeNames = {{
                {Outcome::kPassed, "passed"},
                {Outcome::kFailed, "failed"},
                {Outcome::kFailedAsExpected, "failed-as-expected"},
                {Outcome::kSkipped, "skipped"},
                {Outcome::kCrashed, "crashed"},
                {Outcome::kTimedOut, "timed-out"},
                {Outcome::kBlocked, "blocked"},
                {Outcome::kNotRun, "not-run"},
                {Outcome::kUpToDate, "up-to-date"},
            }};

        constexpr std::array<std::pair<FailureCategory, std::string_view>, 9>
            kCategoryNames = {{
                {FailureCategory::kNone, "none"},
                {FailureCategory::kAssertion, "assertion"},
                {FailureCategory::kUnexpectedException, "unexpected-exception"},
                {FailureCategory::kMissingException, "missing-exception"},
                {FailureCategory::kExplicitFailure, "explicit-failure"},
                {FailureCategory::kCrash, "crash"},
                {FailureCategory::kTimeout, "timeout"},
                {FailureCategory::kInfrastructure, "infrastructure"},
                {FailureCategory::kDependency, "dependency"},
            }};
    } // namespace

    std::int64_t
    TestResult::DurationMicroseconds() const noexcept
    {
        std::int64_t total = 0;
        for (const auto &attempt : attempts)
        {
            total += attempt.durationMicroseconds;
        }
        return total;
    }

    bool
    TestResult::FailsRun() const noexcept
    {
        if (quarantined)
        {
            return false;
        }
        switch (outcome)
        {
            case Outcome::kFailed:
            case Outcome::kCrashed:
            case Outcome::kTimedOut:
            case Outcome::kBlocked:
                return true;
            default:
                return false;
        }
    }

    RunSummary
    Summarize(const RunResult &run)
    {
        RunSummary summary;
        summary.tests = run.tests.size();
        summary.wallMicroseconds = run.wallMicroseconds;
        for (const auto &test : run.tests)
        {
            switch (test.outcome)
            {
                case Outcome::kPassed:
                    ++summary.passed;
                    break;
                case Outcome::kFailed:
                    ++summary.failed;
                    break;
                case Outcome::kFailedAsExpected:
                    ++summary.failedAsExpected;
                    break;
                case Outcome::kSkipped:
                    ++summary.skipped;
                    break;
                case Outcome::kCrashed:
                    ++summary.crashed;
                    break;
                case Outcome::kTimedOut:
                    ++summary.timedOut;
                    break;
                case Outcome::kBlocked:
                    ++summary.blocked;
                    break;
                case Outcome::kNotRun:
                    ++summary.notRun;
                    break;
                case Outcome::kUpToDate:
                    ++summary.upToDate;
                    break;
            }
            summary.flaky += test.flaky ? 1 : 0;
            summary.quarantined += test.quarantined ? 1 : 0;
            summary.attempts += test.attempts.size();
            for (const auto &attempt : test.attempts)
            {
                summary.assertionsPassed += attempt.assertionsPassed;
                summary.assertionsFailed += attempt.assertionsFailed;
                summary.testMicroseconds += attempt.durationMicroseconds;
            }
        }
        return summary;
    }

    const char *
    OutcomeName(Outcome outcome) noexcept
    {
        for (const auto &[value, name] : kOutcomeNames)
        {
            if (value == outcome)
            {
                return name.data();
            }
        }
        return "unknown";
    }

    std::optional<Outcome>
    OutcomeFromName(const std::string &name) noexcept
    {
        for (const auto &[value, text] : kOutcomeNames)
        {
            if (text == name)
            {
                return value;
            }
        }
        return std::nullopt;
    }

    const char *
    CategoryName(FailureCategory category) noexcept
    {
        for (const auto &[value, name] : kCategoryNames)
        {
            if (value == category)
            {
                return name.data();
            }
        }
        return "unknown";
    }

    std::optional<FailureCategory>
    CategoryFromName(const std::string &name) noexcept
    {
        for (const auto &[value, text] : kCategoryNames)
        {
            if (text == name)
            {
                return value;
            }
        }
        return std::nullopt;
    }

    const char *
    HistoryName(FailureHistory history) noexcept
    {
        switch (history)
        {
            case FailureHistory::kUnknown:
                return "unknown";
            case FailureHistory::kNew:
                return "new";
            case FailureHistory::kPersistent:
                return "persistent";
            case FailureHistory::kIntermittent:
                return "intermittent";
        }
        return "unknown";
    }

    namespace
    {
        void
        AppendIndented(std::string &text, const std::string &block)
        {
            std::size_t start = 0;
            while (start <= block.size())
            {
                const auto end = block.find('\n', start);
                text += "  ";
                text += block.substr(start,
                                     end == std::string::npos ? std::string::npos
                                                              : end - start);
                text += '\n';
                if (end == std::string::npos)
                {
                    break;
                }
                start = end + 1;
            }
        }

        std::string
        JoinSections(const std::vector<std::string> &sections)
        {
            std::string text;
            for (const auto &section : sections)
            {
                text += text.empty() ? "" : " / ";
                text += section;
            }
            return text;
        }

        std::string
        Seconds(std::int64_t microseconds)
        {
            const auto tenths = (microseconds + 50000) / 100000;
            return std::to_string(tenths / 10) + "." +
                   std::to_string(tenths % 10) + " s";
        }
    } // namespace

    std::string
    DescribeFailure(const Attempt &attempt)
    {
        std::string text;
        if (attempt.outcome == Outcome::kTimedOut)
        {
            text += "timed out after " + Seconds(attempt.durationMicroseconds);
            text += "\n";
        }
        else if (attempt.outcome == Outcome::kCrashed)
        {
            text += "crashed";
            if (!attempt.exitDescription.empty())
            {
                text += " with " + attempt.exitDescription;
            }
            text += "\n";
            if (!attempt.fatalMessage.empty())
            {
                AppendIndented(text, attempt.fatalMessage);
            }
        }
        if ((attempt.outcome == Outcome::kTimedOut ||
             attempt.outcome == Outcome::kCrashed) &&
            !attempt.lastLocation.empty())
        {
            text += "last assertion started at " + attempt.lastLocation + "\n";
        }
        if ((attempt.outcome == Outcome::kTimedOut ||
             attempt.outcome == Outcome::kCrashed))
        {
            // The innermost section that had not ended is where it stopped.
            std::vector<std::string> open;
            for (const auto &section : attempt.sections)
            {
                if (section.durationMicroseconds < 0)
                {
                    open.resize(section.depth);
                    open.push_back(section.name);
                }
            }
            if (!open.empty())
            {
                text += "in section " + JoinSections(open) + "\n";
            }
        }
        for (const auto &assertion : attempt.assertions)
        {
            if (assertion.passed)
            {
                continue;
            }
            text += assertion.file + ":" + std::to_string(assertion.line) +
                    ": " + (assertion.allowed ? "FAILED (allowed)" : "FAILED") +
                    ":\n";
            if (!assertion.expression.empty())
            {
                AppendIndented(text,
                               assertion.macro.empty()
                                   ? assertion.expression
                                   : assertion.macro + "( " +
                                         assertion.expression + " )");
            }
            if (!assertion.expansion.empty() &&
                assertion.expansion != assertion.expression)
            {
                text += "with expansion:\n";
                AppendIndented(text, assertion.expansion);
            }
            if (!assertion.message.empty())
            {
                text += assertion.category ==
                                FailureCategory::kUnexpectedException
                            ? "due to unexpected exception with message:\n"
                            : "with message:\n";
                AppendIndented(text, assertion.message);
            }
            for (const auto &context : assertion.context)
            {
                text += "with context:\n";
                AppendIndented(text, context);
            }
            if (!assertion.sections.empty())
            {
                text += "in section " + JoinSections(assertion.sections) + "\n";
            }
        }
        return text;
    }

    std::string
    SummarizeFailure(const Attempt &attempt)
    {
        const std::string text = DescribeFailure(attempt);
        return text.substr(0, text.find('\n'));
    }

    bool
    IsFailure(Outcome outcome) noexcept
    {
        return outcome == Outcome::kFailed || outcome == Outcome::kCrashed ||
               outcome == Outcome::kTimedOut;
    }
} // namespace Progmasoft::Catch3
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include "catch_amalgamated_third_party.hpp"

#include <ostream>
#include <sstream>
#include <string_view>

namespace Progmasoft::Catch3
{
    namespace
    {
        struct DecodedCharacter final
        {
            char32_t value = 0;
            std::size_t length = 0;
        };

        DecodedCharacter
        DecodeUtf8(std::string_view text, std::size_t offset) noexcept
        {
            const auto first = static_cast<unsigned char>(text[offset]);
            if (first < 0x80)
            {
                return {first, 1};
            }
            std::size_t length = 0;
            char32_t value = 0;
            char32_t minimum = 0;
            if (first >= 0xC2 && first <= 0xDF)
            {
                length = 2;
                value = first & 0x1F;
                minimum = 0x80;
            }
            else if (first >= 0xE0 && first <= 0xEF)
            {
                length = 3;
                value = first & 0x0F;
                minimum = 0x800;
            }
            else if (first >= 0xF0 && first <= 0xF4)
            {
                length = 4;
                value = first & 0x07;
                minimum = 0x10000;
            }
            else
            {
                return {0xFFFD, 0};
            }
            if (offset + length > text.size())
            {
                return {0xFFFD, 0};
            }
            for (std::size_t index = 1; index < length; ++index)
            {
                const auto continuation =
                    static_cast<unsigned char>(text[offset + index]);
                if ((continuation & 0xC0) != 0x80)
                {
                    return {0xFFFD, 0};
                }
                value = static_cast<char32_t>((value << 6) |
                                              (continuation & 0x3F));
            }
            if (value < minimum || value > 0x10FFFF ||
                (value >= 0xD800 && value <= 0xDFFF))
            {
                return {0xFFFD, 0};
            }
            return {value, length};
        }

        bool
        IsXmlCharacter(char32_t value) noexcept
        {
            return value == 0x09 || value == 0x0A || value == 0x0D ||
                   (value >= 0x20 && value <= 0xD7FF) ||
                   (value >= 0xE000 && value <= 0xFFFD) ||
                   (value >= 0x10000 && value <= 0x10FFFF);
        }

        /// The text as XML can hold it: every byte that is not part of a
        /// well-formed character XML allows becomes U+FFFD, replaced and not
        /// dropped, so the reader sees that something was there. pugixml
        /// escapes the rest.
        std::string
        XmlText(std::string_view value)
        {
            std::string result;
            result.reserve(value.size());
            for (std::size_t index = 0; index < value.size();)
            {
                const DecodedCharacter character = DecodeUtf8(value, index);
                if (character.length == 0 || !IsXmlCharacter(character.value))
                {
                    result += "\xEF\xBF\xBD";
                    ++index;
                    continue;
                }
                result.append(value.data() + index, character.length);
                index += character.length;
            }
            return result;
        }

        void
        AddAttribute(pugi::xml_node node,
                     const char *name,
                     std::string_view value)
        {
            node.append_attribute(name).set_value(XmlText(value).c_str());
        }

        void
        AddText(pugi::xml_node node, std::string_view value)
        {
            node.text().set(XmlText(value).c_str());
        }

        void
        AddTextElement(pugi::xml_node parent,
                       const char *name,
                       std::string_view value)
        {
            AddText(parent.append_child(name), value);
        }

        std::string
        JUnitSeconds(std::int64_t microseconds)
        {
            const bool negative = microseconds < 0;
            const auto magnitude = negative ? -microseconds : microseconds;
            std::string fraction = std::to_string(magnitude % 1000000);
            fraction.insert(0, 6 - fraction.size(), '0');
            return (negative ? "-" : "") + std::to_string(magnitude / 1000000) +
                   "." + fraction;
        }

        std::string
        Output(const Attempt &attempt, bool errors)
        {
            if (errors)
            {
                return attempt.standardError + attempt.processOutput;
            }
            return attempt.standardOutput;
        }

        /// An earlier attempt: `<rerunFailure>` before a final failure,
        /// `<flakyFailure>` before a final pass.
        void
        AddEarlierAttempt(pugi::xml_node testCase,
                          const Attempt &attempt,
                          const char *element)
        {
            pugi::xml_node node = testCase.append_child(element);
            AddAttribute(node, "message", SummarizeFailure(attempt));
            AddAttribute(node, "type", CategoryName(attempt.category));
            AddTextElement(node, "stackTrace", DescribeFailure(attempt));
            if (!attempt.standardOutput.empty())
            {
                AddTextElement(node, "system-out", Output(attempt, false));
            }
            if (!Output(attempt, true).empty())
            {
                AddTextElement(node, "system-err", Output(attempt, true));
            }
        }

        std::string
        SkipReason(const TestResult &test)
        {
            switch (test.outcome)
            {
                case Outcome::kBlocked:
                    return test.analysis.empty() ? "a dependency did not pass"
                                                 : test.analysis.front();
                case Outcome::kNotRun:
                    return "the run stopped before this test";
                case Outcome::kUpToDate:
                    return "passed before; nothing it depends on changed";
                default:
                    return "skipped";
            }
        }

        void
        AddTestCase(pugi::xml_node suite,
                    const TestResult &test,
                    std::string_view suiteName)
        {
            pugi::xml_node node = suite.append_child("testcase");
            AddAttribute(node,
                         "classname",
                         test.className.empty() ? suiteName : test.className);
            AddAttribute(node, "name", test.name);
            AddAttribute(node, "time", JUnitSeconds(test.DurationMicroseconds()));
            if (!test.file.empty())
            {
                AddAttribute(node, "file", test.file);
                AddAttribute(node, "line", std::to_string(test.line));
            }

            const Attempt *last =
                test.attempts.empty() ? nullptr : &test.attempts.back();
            const bool failedRun = IsFailure(test.outcome);
            if (failedRun && last != nullptr)
            {
                const bool error = test.outcome != Outcome::kFailed ||
                                   last->category ==
                                       FailureCategory::kInfrastructure;
                pugi::xml_node failure =
                    node.append_child(error ? "error" : "failure");
                AddAttribute(failure, "message", SummarizeFailure(*last));
                AddAttribute(failure, "type", CategoryName(test.category));
                std::string details = DescribeFailure(*last);
                for (const auto &finding : test.analysis)
                {
                    details += "analysis: " + finding + "\n";
                }
                if (!test.replayCommand.empty())
                {
                    details += "replay: " + test.replayCommand + "\n";
                }
                AddText(failure, details);
            }
            else if (test.outcome == Outcome::kSkipped ||
                     test.outcome == Outcome::kBlocked ||
                     test.outcome == Outcome::kNotRun ||
                     test.outcome == Outcome::kUpToDate)
            {
                AddAttribute(
                    node.append_child("skipped"), "message", SkipReason(test));
            }
            // Every earlier failed attempt, so retries stay visible.
            for (std::size_t index = 0; index + 1 < test.attempts.size();
                 ++index)
            {
                const Attempt &attempt = test.attempts[index];
                if (!IsFailure(attempt.outcome))
                {
                    continue;
                }
                AddEarlierAttempt(node,
                                  attempt,
                                  failedRun ? "rerunFailure" : "flakyFailure");
            }
            if (last != nullptr && !last->standardOutput.empty())
            {
                AddTextElement(node, "system-out", Output(*last, false));
            }
            if (last != nullptr && !Output(*last, true).empty())
            {
                AddTextElement(node, "system-err", Output(*last, true));
            }
        }

        void
        AddCounts(pugi::xml_node node,
                  const RunResult &run,
                  const RunSummary &summary)
        {
            AddAttribute(node, "name", run.binary);
            AddAttribute(node, "tests", std::to_string(summary.tests));
            AddAttribute(node, "failures", std::to_string(summary.failed));
            AddAttribute(node,
                         "errors",
                         std::to_string(summary.crashed + summary.timedOut));
            AddAttribute(node,
                         "skipped",
                         std::to_string(summary.skipped + summary.blocked +
                                        summary.notRun + summary.upToDate));
            AddAttribute(node, "time", JUnitSeconds(run.wallMicroseconds));
        }
    } // namespace

    bool
    JUnitXmlWriter::Write(const RunResult &run, std::ostream &output)
    {
        const RunSummary summary = Summarize(run);
        pugi::xml_document document;
        pugi::xml_node declaration =
            document.append_child(pugi::node_declaration);
        declaration.append_attribute("version").set_value("1.0");
        declaration.append_attribute("encoding").set_value("UTF-8");

        pugi::xml_node suites = document.append_child("testsuites");
        AddCounts(suites, run, summary);
        pugi::xml_node suite = suites.append_child("testsuite");
        AddCounts(suite, run, summary);
        pugi::xml_node properties = suite.append_child("properties");
        const auto property = [&](const char *name, const std::string &value) {
            pugi::xml_node node = properties.append_child("property");
            AddAttribute(node, "name", name);
            AddAttribute(node, "value", value);
        };
        property("random-seed", std::to_string(run.seed));
        property("workers", std::to_string(run.workers));
        if (run.shardCount > 1)
        {
            property("shard",
                     std::to_string(run.shardIndex) + "/" +
                         std::to_string(run.shardCount));
        }
        property("flaky", std::to_string(summary.flaky));
        property("quarantined", std::to_string(summary.quarantined));
        for (const auto &test : run.tests)
        {
            AddTestCase(suite, test, run.binary);
        }

        document.save(output, "  ", pugi::format_indent, pugi::encoding_utf8);
        return static_cast<bool>(output);
    }

    std::string
    JUnitXmlWriter::ToString(const RunResult &run)
    {
        std::ostringstream output;
        static_cast<void>(Write(run, output));
        return output.str();
    }
} // namespace Progmasoft::Catch3
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0


#include <algorithm>
#include <cctype>
#include <map>

namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        /// Replaces what differs between two runs of the same failure:
        /// numbers, hexadecimal addresses and the temporary directory.
        std::string
        Normalise(std::string_view text)
        {
            std::string result;
            bool inNumber = false;
            for (std::size_t index = 0; index < text.size(); ++index)
            {
                const char character = text[index];
                const bool digit =
                    std::isdigit(static_cast<unsigned char>(character)) != 0;
                const bool hexLetter =
                    inNumber &&
                    std::isxdigit(static_cast<unsigned char>(character)) != 0;
                if (digit || hexLetter ||
                    (inNumber && (character == 'x' || character == '.')))
                {
                    if (!inNumber)
                    {
                        result.push_back('#');
                    }
                    inNumber = true;
                    continue;
                }
                inNumber = false;
                result.push_back(character);
            }
            return result;
        }

        const Attempt *
        FirstFailure(const TestResult &test)
        {
            for (const auto &attempt : test.attempts)
            {
                if (IsFailure(attempt.outcome))
                {
                    return &attempt;
                }
            }
            return nullptr;
        }

        std::string
        Seconds(std::int64_t microseconds)
        {
            const auto tenths = (microseconds + 50000) / 100000;
            return std::to_string(tenths / 10) + "." +
                   std::to_string(tenths % 10) + " s";
        }

        std::string
        OpenSections(const Attempt &attempt)
        {
            std::vector<std::string> open;
            for (const auto &section : attempt.sections)
            {
                if (section.durationMicroseconds < 0)
                {
                    open.resize(section.depth);
                    open.push_back(section.name);
                }
            }
            std::string text;
            for (const auto &name : open)
            {
                text += text.empty() ? "" : " / ";
                text += name;
            }
            return text;
        }

        std::string
        QuoteArgument(const std::string &argument)
        {
            if (argument.find_first_of(" \t\"'\\$`&|;<>()[]*?~!#") ==
                    std::string::npos &&
                !argument.empty())
            {
                return argument;
            }
            std::string quoted = "\"";
            for (const char character : argument)
            {
                if (character == '"' || character == '\\' || character == '$' ||
                    character == '`')
                {
                    quoted.push_back('\\');
                }
                quoted.push_back(character);
            }
            quoted.push_back('"');
            return quoted;
        }
    } // namespace

    std::string
    QuoteTestSpec(const std::string &name)
    {
        // Catch2's spec syntax gives these characters a meaning; a backslash
        // makes them literal.
        std::string spec;
        for (const char character : name)
        {
            if (character == ',' || character == '[' || character == ']' ||
                character == '*' || character == '\\' || character == '~' ||
                character == '"')
            {
                spec.push_back('\\');
            }
            spec.push_back(character);
        }
        return spec;
    }

    std::string
    FailureSignature(const Attempt &attempt)
    {
        std::string text = CategoryName(attempt.category);
        text += '|';
        bool described = false;
        for (const auto &assertion : attempt.assertions)
        {
            if (assertion.passed || assertion.allowed)
            {
                continue;
            }
            text += assertion.file + ":" + std::to_string(assertion.line) +
                    "|" + Normalise(assertion.message) + "|" +
                    assertion.expression;
            described = true;
            break;
        }
        if (!described)
        {
            text += Normalise(attempt.fatalMessage) + "|" +
                    attempt.lastLocation + "|" + OpenSections(attempt);
        }
        return "sig-" + ToHex(Fnv1a64(text)).substr(0, 12);
    }

    void
    Analyze(RunResult &run, const AnalysisInputs &inputs)
    {
        std::map<std::string, std::vector<std::size_t>> bySignature;
        for (std::size_t index = 0; index < run.tests.size(); ++index)
        {
            TestResult &test = run.tests[index];
            const Attempt *failure = FirstFailure(test);
            if (failure == nullptr)
            {
                continue;
            }
            test.signature = FailureSignature(*failure);
            bySignature[test.signature].push_back(index);
        }

        for (std::size_t index = 0; index < run.tests.size(); ++index)
        {
            TestResult &test = run.tests[index];
            const Attempt *failure = FirstFailure(test);
            if (failure == nullptr)
            {
                continue;
            }
            std::vector<std::string> findings;

            // How it relates to earlier runs.
            const TestHistory *past =
                inputs.history == nullptr ? nullptr
                                          : inputs.history->Find(test.id);
            const TestRecord *last = past == nullptr ? nullptr : past->Last();
            if (last == nullptr)
            {
                test.history = FailureHistory::kUnknown;
            }
            else if (test.flaky || past->FlakyEvents(20) > 0)
            {
                test.history = FailureHistory::kIntermittent;
                const auto events = past->FlakyEvents(20) + (test.flaky ? 1 : 0);
                findings.push_back("intermittent: " + std::to_string(events) +
                                   " flaky result(s) in the last 20 runs");
            }
            else if (IsFailure(last->outcome))
            {
                test.history = FailureHistory::kPersistent;
                std::size_t streak = 0;
                for (auto record = past->records.rbegin();
                     record != past->records.rend() &&
                     IsFailure(record->outcome);
                     ++record)
                {
                    ++streak;
                }
                std::string finding = "persistent: failed in the last " +
                                      std::to_string(streak) + " run(s)";
                if (!last->signature.empty() && last->signature != test.signature)
                {
                    finding += ", but differently last time";
                }
                findings.push_back(finding);
            }
            else
            {
                test.history = FailureHistory::kNew;
                std::string finding = "new failure after " +
                                      std::to_string(past->PassStreak()) +
                                      " passing run(s)";
                if (inputs.history != nullptr)
                {
                    for (const auto &runRecord : inputs.history->Runs())
                    {
                        if (runRecord.id == last->run && !runRecord.commit.empty())
                        {
                            finding += "; it last passed at commit " +
                                       runRecord.commit.substr(0, 12);
                        }
                    }
                }
                findings.push_back(finding);
            }

            // Other tests that fail the same way.
            const auto &group = bySignature[test.signature];
            if (group.size() > 1)
            {
                std::string others;
                std::size_t named = 0;
                for (const std::size_t other : group)
                {
                    if (other == index)
                    {
                        continue;
                    }
                    if (named < 3)
                    {
                        others += (named > 0 ? ", '" : "'") +
                                  run.tests[other].name + "'";
                    }
                    ++named;
                }
                if (named > 3)
                {
                    others += " and " + std::to_string(named - 3) + " more";
                }
                findings.push_back("fails the same way as " + others +
                                   ": probably one cause");
            }

            // Where it stopped.
            if (failure->outcome == Outcome::kCrashed)
            {
                std::string finding = "crashed";
                if (!failure->exitDescription.empty())
                {
                    finding += " (" + failure->exitDescription + ")";
                }
                if (!failure->lastLocation.empty())
                {
                    finding += " after the assertion at " + failure->lastLocation;
                }
                const std::string sections = OpenSections(*failure);
                if (!sections.empty())
                {
                    finding += ", in section " + sections;
                }
                findings.push_back(finding);
            }
            else if (failure->outcome == Outcome::kTimedOut)
            {
                std::string finding =
                    "stopped after " + Seconds(failure->durationMicroseconds);
                if (test.predictedDurationMicroseconds > 0 && last != nullptr)
                {
                    finding += "; it usually takes " +
                               Seconds(test.predictedDurationMicroseconds);
                }
                const std::string sections = OpenSections(*failure);
                if (!sections.empty())
                {
                    finding += "; it was in section " + sections;
                }
                if (!failure->lastLocation.empty())
                {
                    finding += "; last assertion started at " +
                               failure->lastLocation;
                }
                findings.push_back(finding);
            }

            // The change it follows.
            if (inputs.changes != nullptr)
            {
                std::vector<std::string> changedFiles;
                const auto changed = [&](const std::string &file) {
                    return std::any_of(inputs.changes->files.begin(),
                                       inputs.changes->files.end(),
                                       [&](const auto &path) {
                                           return SameSourceFile(path, file);
                                       });
                };
                if (changed(test.file))
                {
                    changedFiles.push_back(test.file);
                }
                for (const auto &assertion : failure->assertions)
                {
                    if (!assertion.passed && assertion.file != test.file &&
                        changed(assertion.file) &&
                        std::find(changedFiles.begin(),
                                  changedFiles.end(),
                                  assertion.file) == changedFiles.end())
                    {
                        changedFiles.push_back(assertion.file);
                    }
                }
                for (const auto &file : changedFiles)
                {
                    findings.push_back(
                        "changed since " +
                        (inputs.changesSince.empty() ? std::string{"HEAD"}
                                                     : inputs.changesSince) +
                        ": " + file);
                }
            }

            if (!failure->temporaryDirectory.empty())
            {
                findings.push_back("its temporary files are kept in " +
                                   failure->temporaryDirectory);
            }

            // Findings of the scheduler (order dependence, flakiness,
            // quarantine) come first.
            test.analysis.insert(
                test.analysis.end(), findings.begin(), findings.end());

            // An imported failure ran somewhere else.
            if (inputs.program.empty())
            {
                continue;
            }
            std::string replay = QuoteArgument(inputs.program) + " " +
                                 QuoteArgument(QuoteTestSpec(test.name)) +
                                 " --seed=" + std::to_string(failure->seed);
            if (!failure->precededBy.empty() && !test.flaky)
            {
                // The replay mode runs the same tests first, in the same
                // process, as the failure did.
                replay = QuoteArgument(inputs.program) + " --replay";
            }
            test.replayCommand = replay;
        }
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0
#include <algorithm>
#include <cmath>
#include <iterator>
#include <map>


namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        std::optional<double>
        ParseSeconds(std::string_view text)
        {
            if (text.empty())
            {
                return std::nullopt;
            }
            double value = 0.0;
            double scale = 0.0;
            for (const char character : text)
            {
                if (character == '.' && scale <= 0.0)
                {
                    scale = 1.0;
                    continue;
                }
                if (character < '0' || character > '9')
                {
                    return std::nullopt;
                }
                if (scale > 0.0)
                {
                    scale /= 10.0;
                    value += (character - '0') * scale;
                }
                else
                {
                    value = value * 10.0 + (character - '0');
                }
            }
            return value;
        }

        bool
        HasPrefix(std::string_view text, std::string_view prefix)
        {
            return text.substr(0, prefix.size()) == prefix;
        }

        Status
        ApplyRunnerTag(TestEntry &entry, const std::string &tag)
        {
            const auto where = [&] {
                return "test '" + entry.name + "' at " + entry.file + ":"
                       + std::to_string(entry.line);
            };
            if (tag == "!serial")
            {
                entry.serial = true;
            }
            else if (tag == "!isolated")
            {
                entry.isolated = true;
            }
            else if (tag == "!quarantine")
            {
                entry.quarantined = true;
            }
            else if (HasPrefix(tag, "!resource:"))
            {
                entry.resources.push_back(tag.substr(10));
            }
            else if (HasPrefix(tag, "!depends:"))
            {
                entry.dependsOn.push_back(tag.substr(9));
            }
            else if (HasPrefix(tag, "!timeout:"))
            {
                const auto seconds = ParseSeconds(tag.substr(9));
                if (!seconds || *seconds > 1e7)
                {
                    return Error{ "[" + tag + "] of " + where()
                                  + " needs a number of seconds, such as "
                                    "[!timeout:30]" };
                }
                entry.timeout = std::chrono::milliseconds{
                    static_cast<long long>(std::llround(*seconds * 1000.0))
                };
            }
            else if (HasPrefix(tag, "!retries:"))
            {
                const auto count = ParseSeconds(tag.substr(9));
                if (!count || *count > 100 || std::floor(*count) < *count)
                {
                    return Error{ "[" + tag + "] of " + where()
                                  + " needs a whole number up to 100, such as "
                                    "[!retries:3]" };
                }
                entry.retries = static_cast<unsigned>(*count);
            }
            return Status::Ok();
        }
    } // namespace

    Expected<Catalog>
    Catalog::Load()
    {
        Catalog catalog;
        const auto &handles
            = Catch::getRegistryHub().getTestCaseRegistry().getAllTests();
        catalog.handles_ = handles;
        // Two tests with the same class, name and tags cannot be told apart
        // by any filter, history record or report, so they are refused.
        std::vector<const Catch::TestCaseInfo *> sorted;
        sorted.reserve(handles.size());
        for (const auto &handle : handles)
        {
            sorted.push_back(&handle.getTestCaseInfo());
        }
        std::stable_sort(sorted.begin(),
                         sorted.end(),
                         [](const auto *lhs, const auto *rhs) {
                             return *lhs < *rhs;
                         });
        const auto duplicate
            = std::adjacent_find(sorted.begin(),
                                 sorted.end(),
                                 [](const auto *lhs, const auto *rhs) {
                                     return *lhs == *rhs;
                                 });
        if (duplicate != sorted.end())
        {
            const auto *first = *duplicate;
            const auto *again = *std::next(duplicate);
            return Error{ "test case \"" + again->name + "\", with tags \""
                          + again->tagsAsString() + "\" already defined.\n"
                          + "\tFirst seen at "
                          + std::string{ first->lineInfo.file } + ":"
                          + std::to_string(first->lineInfo.line) + "\n"
                          + "\tRedefined at "
                          + std::string{ again->lineInfo.file } + ":"
                          + std::to_string(again->lineInfo.line) };
        }
        std::map<std::string, std::size_t> nameCounts;
        for (const auto &handle : handles)
        {
            const auto &info = handle.getTestCaseInfo();
            const std::string className{ info.className.data(),
                                         info.className.size() };
            ++nameCounts[className.empty() ? info.name
                                           : className + "::" + info.name];
        }
        Sha256 identity;
        catalog.tests_.reserve(handles.size());
        for (std::size_t index = 0; index < handles.size(); ++index)
        {
            const auto &info = handles[index].getTestCaseInfo();
            TestEntry entry;
            entry.index = index;
            entry.name = info.name;
            entry.className
                = std::string{ info.className.data(), info.className.size() };
            entry.file = std::string{ info.lineInfo.file };
            entry.line = info.lineInfo.line;
            entry.hidden = info.isHidden();
            entry.mayFail = info.okToFail() && !info.expectedToFail();
            entry.shouldFail = info.expectedToFail();
            for (const auto &tag : info.tags)
            {
                entry.tags.emplace_back(tag.original.data(),
                                        tag.original.size());
            }
            entry.benchmark
                = std::find(entry.tags.begin(), entry.tags.end(), "!benchmark")
                  != entry.tags.end();
            for (const auto &tag : entry.tags)
            {
                if (!tag.empty() && tag.front() == '!')
                {
                    if (auto status = ApplyRunnerTag(entry, tag); !status)
                    {
                        return status.GetError();
                    }
                }
            }
            const std::string base = entry.className.empty()
                                         ? entry.name
                                         : entry.className + "::" + entry.name;
            entry.key = base;
            if (nameCounts[base] > 1)
            {
                entry.key += " " + info.tagsAsString();
            }
            identity.Update(entry.key);
            identity.Update("\n");
            catalog.tests_.push_back(std::move(entry));
        }
        catalog.identity_ = identity.HexDigest();
        return catalog;
    }

    Catalog
    Catalog::FromEntries(std::vector<TestEntry> entries)
    {
        Catalog catalog;
        Sha256 identity;
        for (std::size_t index = 0; index < entries.size(); ++index)
        {
            TestEntry &entry = entries[index];
            entry.index = index;
            if (entry.key.empty())
            {
                entry.key = entry.name;
            }
            identity.Update(entry.key);
            identity.Update("\n");
        }
        catalog.tests_ = std::move(entries);
        catalog.identity_ = identity.HexDigest();
        return catalog;
    }

    const TestEntry *
    Catalog::Find(const std::string &keyOrName) const
    {
        for (const auto &entry : tests_)
        {
            if (entry.key == keyOrName)
            {
                return &entry;
            }
        }
        for (const auto &entry : tests_)
        {
            if (entry.name == keyOrName)
            {
                return &entry;
            }
        }
        return nullptr;
    }

    std::vector<std::size_t>
    Catalog::Match(const Catch::IConfig &config,
                   std::vector<std::string> &unmatched) const
    {
        std::vector<std::size_t> selected;
        const auto &spec = config.testSpec();
        if (!spec.hasFilters())
        {
            for (const auto &entry : tests_)
            {
                if (!entry.hidden
                    && Catch::isThrowSafe(handles_[entry.index], config))
                {
                    selected.push_back(entry.index);
                }
            }
            return selected;
        }
        std::vector<bool> chosen(tests_.size(), false);
        for (const auto &match : spec.matchesByFilter(handles_, config))
        {
            if (match.tests.empty())
            {
                unmatched.push_back(match.name);
            }
            for (const auto *handle : match.tests)
            {
                chosen[static_cast<std::size_t>(handle - handles_.data())]
                    = true;
            }
        }
        for (std::size_t index = 0; index < chosen.size(); ++index)
        {
            if (chosen[index])
            {
                selected.push_back(index);
            }
        }
        return selected;
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0
#include <algorithm>
#include <cstdio>
#include <set>


namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        constexpr const char *kRed = "\x1b[31m";
        constexpr const char *kGreen = "\x1b[32m";
        constexpr const char *kYellow = "\x1b[33m";
        constexpr const char *kCyan = "\x1b[36m";
        constexpr const char *kGrey = "\x1b[90m";
        constexpr const char *kBold = "\x1b[1m";

        std::string
        Duration(std::int64_t microseconds)
        {
            if (microseconds < 1000)
            {
                return std::to_string(microseconds) + " us";
            }
            if (microseconds < 1000000)
            {
                return std::to_string(microseconds / 1000) + " ms";
            }
            const auto tenths = (microseconds + 50000) / 100000;
            return std::to_string(tenths / 10) + "."
                   + std::to_string(tenths % 10) + " s";
        }

        const char *
        Label(const TestResult &test)
        {
            if (test.flaky)
            {
                return "FLAKY";
            }
            switch (test.outcome)
            {
                case Outcome::kPassed:
                    return "PASS";
                case Outcome::kFailed:
                    return "FAIL";
                case Outcome::kFailedAsExpected:
                    return "XFAIL";
                case Outcome::kSkipped:
                    return "SKIP";
                case Outcome::kCrashed:
                    return "CRASH";
                case Outcome::kTimedOut:
                    return "TIMEOUT";
                case Outcome::kBlocked:
                    return "BLOCKED";
                case Outcome::kNotRun:
                    return "NOT RUN";
                case Outcome::kUpToDate:
                    return "UP TO DATE";
            }
            return "?";
        }

        void
        Write(std::FILE *output, const std::string &text)
        {
            std::fwrite(text.data(), 1, text.size(), output);
        }

        std::string
        Indent(const std::string &text, const std::string &prefix)
        {
            std::string result;
            std::size_t start = 0;
            while (start < text.size())
            {
                auto end = text.find('\n', start);
                if (end == std::string::npos)
                {
                    end = text.size();
                }
                result += prefix + text.substr(start, end - start) + "\n";
                start = end + 1;
            }
            return result;
        }

        /// One assertion on one line, then its messages: what --success
        /// shows for every assertion of a test.
        std::string
        DescribeAssertion(const AssertionRecord &assertion)
        {
            std::string text = assertion.file + ":"
                               + std::to_string(assertion.line) + ": "
                               + (assertion.passed ? "passed" : "FAILED") + ":";
            if (!assertion.expression.empty())
            {
                text += " "
                        + (assertion.macro.empty()
                               ? assertion.expression
                               : assertion.macro + "( " + assertion.expression
                                     + " )");
                if (!assertion.expansion.empty()
                    && assertion.expansion != assertion.expression)
                {
                    text += " for: " + assertion.expansion;
                }
            }
            text += "\n";
            if (!assertion.message.empty())
            {
                text += "  with message: " + assertion.message + "\n";
            }
            for (const auto &context : assertion.context)
            {
                text += "  with context: " + context + "\n";
            }
            return text;
        }

        /// The last lines of a long output.
        std::string
        Tail(const std::string &text, std::size_t lines)
        {
            std::size_t position = text.size();
            std::size_t count = 0;
            while (position > 0 && count <= lines)
            {
                position = text.rfind('\n', position - 1);
                if (position == std::string::npos)
                {
                    return text;
                }
                ++count;
            }
            return count > lines ? text.substr(position + 1) : text;
        }
    } // namespace

    ConsoleReport::ConsoleReport(const Options &options, std::FILE *output)
        : options_{ options }
        , output_{ output }
    {
        terminal_ = output == stdout && IsStandardOutputTerminal();
        colour_ = options.color == ColorMode::kAlways
                  || (options.color == ColorMode::kAuto && terminal_
                      && EnableTerminalColours());
    }

    std::string
    ConsoleReport::Paint(std::string_view text, const char *colour) const
    {
        if (!colour_)
        {
            return std::string{ text };
        }
        return std::string{ colour } + std::string{ text } + "\x1b[0m";
    }

    void
    ConsoleReport::ClearProgress()
    {
        if (progressShown_)
        {
            Write(output_, "\r\x1b[2K");
            progressShown_ = false;
        }
    }

    void
    ConsoleReport::RunStarting(std::size_t tests,
                               std::size_t workers,
                               std::uint32_t seed,
                               const std::vector<std::string> &notes)
    {
        total_ = tests;
        if (options_.verbosity == Verbosity::kQuiet)
        {
            return;
        }
        std::string text = Paint("Catch3", kBold) + ": " + std::to_string(tests)
                           + " test(s)";
        if (options_.inProcess)
        {
            text += " in this process";
        }
        else
        {
            text += ", up to " + std::to_string(workers) + " worker(s)";
        }
        text += ", seed " + std::to_string(seed) + "\n";
        for (const auto &note : notes)
        {
            text += Paint("note: ", kCyan) + note + "\n";
        }
        Write(output_, text);
        std::fflush(output_);
    }

    void
    ConsoleReport::ImportStarting(std::size_t tests, std::size_t files)
    {
        total_ = tests;
        if (options_.verbosity == Verbosity::kQuiet)
        {
            return;
        }
        Write(output_,
              Paint("Catch3", kBold) + ": " + std::to_string(tests)
                  + " test(s) imported from " + std::to_string(files)
                  + " report(s)\n");
        std::fflush(output_);
    }

    void
    ConsoleReport::Retrying(const TestResult &test, const Attempt &attempt)
    {
        if (options_.verbosity != Verbosity::kVerbose)
        {
            return;
        }
        ClearProgress();
        Write(output_,
              Paint("RETRY ", kYellow) + test.name + " after "
                  + OutcomeName(attempt.outcome) + "\n");
    }

    void
    ConsoleReport::TestFinished(const TestResult &test)
    {
        ++finished_;
        const bool failed = test.FailsRun();
        failed_ += failed ? 1 : 0;
        const bool show = options_.verbosity == Verbosity::kVerbose
                          || options_.includeSuccessful || failed || test.flaky
                          || (test.quarantined && IsFailure(test.outcome));
        if (show)
        {
            ClearProgress();
            const char *colour = failed       ? kRed
                                 : test.flaky ? kYellow
                                 : test.outcome == Outcome::kSkipped
                                         || test.outcome == Outcome::kBlocked
                                     ? kGrey
                                     : kGreen;
            std::string line
                = Paint(Label(test), colour) + " " + test.name + " "
                  + Paint("(" + Duration(test.DurationMicroseconds()) + ")",
                          kGrey);
            if (test.quarantined && IsFailure(test.outcome))
            {
                line += Paint(" [quarantined]", kYellow);
            }
            Write(output_, line + "\n");
            if (options_.includeSuccessful && !test.attempts.empty())
            {
                for (const auto &assertion : test.attempts.back().assertions)
                {
                    Write(output_,
                          Indent(DescribeAssertion(assertion), "    "));
                }
            }
            if (options_.verbosity == Verbosity::kVerbose && !failed
                && !test.attempts.empty())
            {
                // What a passing test printed; a failure shows it later.
                const Attempt &attempt = test.attempts.back();
                const std::string output = attempt.standardOutput
                                           + attempt.standardError
                                           + attempt.processOutput;
                if (!output.empty())
                {
                    Write(output_, Indent(output, "    "));
                }
            }
        }
        if (terminal_ && options_.verbosity != Verbosity::kQuiet
            && finished_ < total_)
        {
            ClearProgress();
            std::string progress = "[" + std::to_string(finished_) + "/"
                                   + std::to_string(total_) + "]";
            if (failed_ > 0)
            {
                progress
                    += " " + Paint(std::to_string(failed_) + " failed", kRed);
            }
            Write(output_, progress);
            progressShown_ = true;
        }
        std::fflush(output_);
    }

    void
    ConsoleReport::PrintFailure(const TestResult &test)
    {
        std::string text
            = "\n" + Paint(std::string{ Label(test) } + ": " + test.name, kBold)
              + "\n" + Paint(test.file + ":" + std::to_string(test.line), kGrey)
              + "\n";
        const Attempt *failure = nullptr;
        for (const auto &attempt : test.attempts)
        {
            if (IsFailure(attempt.outcome))
            {
                failure = &attempt;
                break;
            }
        }
        if (failure != nullptr)
        {
            text += Indent(DescribeFailure(*failure), "  ");
            if (!failure->standardOutput.empty())
            {
                text += "  standard output:\n"
                        + Indent(Tail(failure->standardOutput, 20), "    ");
            }
            const std::string errors
                = failure->standardError + failure->processOutput;
            if (!errors.empty())
            {
                text
                    += "  standard error:\n" + Indent(Tail(errors, 20), "    ");
            }
        }
        if (test.outcome == Outcome::kBlocked && !test.analysis.empty())
        {
            text += "  " + test.analysis.front() + "\n";
        }
        else
        {
            for (const auto &finding : test.analysis)
            {
                text += Paint("  analysis: ", kCyan) + finding + "\n";
            }
        }
        if (!test.replayCommand.empty())
        {
            text += Paint("  replay: ", kCyan) + test.replayCommand + "\n";
        }
        Write(output_, text);
    }

    void
    ConsoleReport::RunEnded(const RunResult &run)
    {
        ClearProgress();
        for (const auto &test : run.tests)
        {
            if (test.FailsRun() || test.flaky
                || (test.quarantined && IsFailure(test.outcome)))
            {
                PrintFailure(test);
            }
        }
        for (const auto &spec : run.unmatchedSpecs)
        {
            Write(output_,
                  Paint("warning: ", kYellow) + "no test matches '" + spec
                      + "'\n");
        }
        for (const auto &note : run.notes)
        {
            Write(output_, Paint("note: ", kCyan) + note + "\n");
        }
        if (options_.slowest > 0)
        {
            std::vector<const TestResult *> ordered;
            for (const auto &test : run.tests)
            {
                if (!test.attempts.empty())
                {
                    ordered.push_back(&test);
                }
            }
            std::sort(ordered.begin(),
                      ordered.end(),
                      [](const TestResult *left, const TestResult *right) {
                          return left->DurationMicroseconds()
                                 > right->DurationMicroseconds();
                      });
            ordered.resize(
                std::min<std::size_t>(ordered.size(), options_.slowest));
            Write(output_, "\nslowest tests:\n");
            for (const auto *test : ordered)
            {
                std::string duration = Duration(test->DurationMicroseconds());
                duration.insert(0,
                                duration.size() < 9 ? 9 - duration.size() : 0,
                                ' ');
                Write(output_, "  " + duration + "  " + test->name + "\n");
            }
        }

        const RunSummary summary = Summarize(run);
        std::string line = "\n" + std::to_string(summary.tests) + " test(s): ";
        std::vector<std::string> parts;
        const auto add
            = [&](std::size_t count, const char *what, const char *colour) {
                  if (count > 0)
                  {
                      parts.push_back(
                          Paint(std::to_string(count) + " " + what, colour));
                  }
              };
        add(summary.passed, "passed", kGreen);
        add(summary.failed, "failed", kRed);
        add(summary.crashed, "crashed", kRed);
        add(summary.timedOut, "timed out", kRed);
        add(summary.failedAsExpected, "failed as expected", kGreen);
        add(summary.skipped, "skipped", kGrey);
        add(summary.blocked, "blocked", kRed);
        add(summary.notRun, "not run", kGrey);
        add(summary.upToDate, "up to date", kGrey);
        add(summary.flaky, "flaky", kYellow);
        add(summary.quarantined, "in quarantine", kYellow);
        if (parts.empty())
        {
            parts.emplace_back("none ran");
        }
        for (std::size_t index = 0; index < parts.size(); ++index)
        {
            line += (index > 0 ? ", " : "") + parts[index];
        }
        line += "\n"
                + std::to_string(summary.assertionsPassed
                                 + summary.assertionsFailed)
                + " assertion(s), " + std::to_string(summary.attempts)
                + " attempt(s); " + Duration(summary.wallMicroseconds);
        if (summary.wallMicroseconds > 0 && summary.testMicroseconds > 0
            && !options_.inProcess)
        {
            const auto speedup
                = static_cast<double>(summary.testMicroseconds)
                  / static_cast<double>(summary.wallMicroseconds);
            line += " (" + Duration(summary.testMicroseconds) + " of tests";
            // Below this, worker start-up outweighs the tests and the ratio
            // says nothing about parallelism.
            if (speedup >= 1.05)
            {
                char buffer[32];
                std::snprintf(buffer, sizeof(buffer), "%.1f", speedup);
                line += std::string{ ", " } + buffer + "x in parallel";
            }
            line += ")";
        }
        Write(output_, line + "\n");
        std::fflush(output_);
    }

    std::string
    TraceJson(const RunResult &run)
    {
        JsonValue events = JsonValue::Array();
        std::set<std::size_t> workers;
        for (const auto &test : run.tests)
        {
            for (const auto &attempt : test.attempts)
            {
                workers.insert(attempt.worker);
                JsonValue slice = JsonValue::Object();
                slice.Set("name", test.name);
                slice.Set("cat", OutcomeName(attempt.outcome));
                slice.Set("ph", "X");
                slice.Set("ts", attempt.startMicroseconds);
                slice.Set(
                    "dur",
                    std::max<std::int64_t>(1, attempt.durationMicroseconds));
                slice.Set("pid", 1);
                slice.Set("tid", attempt.worker);
                JsonValue arguments = JsonValue::Object();
                arguments.Set("attempt", attempt.number);
                arguments.Set("outcome", OutcomeName(attempt.outcome));
                arguments.Set("file",
                              test.file + ":" + std::to_string(test.line));
                if (!attempt.precededBy.empty())
                {
                    arguments.Set("precededBy", attempt.precededBy.size());
                }
                slice.Set("args", std::move(arguments));
                events.Append(std::move(slice));
                for (const auto &section : attempt.sections)
                {
                    JsonValue inner = JsonValue::Object();
                    inner.Set("name", section.name);
                    inner.Set("cat", "section");
                    inner.Set("ph", "X");
                    inner.Set("ts",
                              attempt.startMicroseconds
                                  + section.startMicroseconds);
                    inner.Set("dur",
                              section.durationMicroseconds < 0
                                  ? std::max<std::int64_t>(
                                        1,
                                        attempt.durationMicroseconds
                                            - section.startMicroseconds)
                                  : std::max<std::int64_t>(
                                        1,
                                        section.durationMicroseconds));
                    inner.Set("pid", 1);
                    inner.Set("tid", attempt.worker);
                    events.Append(std::move(inner));
                }
            }
        }
        for (const std::size_t worker : workers)
        {
            JsonValue name = JsonValue::Object();
            name.Set("name", "thread_name");
            name.Set("ph", "M");
            name.Set("pid", 1);
            name.Set("tid", worker);
            JsonValue arguments = JsonValue::Object();
            arguments.Set("name",
                          worker == 0 ? std::string{ "in process" }
                                      : "worker " + std::to_string(worker));
            name.Set("args", std::move(arguments));
            events.Append(std::move(name));
        }
        JsonValue trace = JsonValue::Object();
        trace.Set("traceEvents", std::move(events));
        trace.Set("displayTimeUnit", "ms");
        JsonValue metadata = JsonValue::Object();
        metadata.Set("binary", run.binary);
        metadata.Set("seed", run.seed);
        trace.Set("otherData", std::move(metadata));
        return trace.ToString();
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <optional>
#include <sstream>


namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        using Clock = std::chrono::steady_clock;

        std::int64_t
        MicrosecondsBetween(Clock::time_point start, Clock::time_point end)
        {
            return std::chrono::duration_cast<std::chrono::microseconds>(
                       end - start)
                .count();
        }

        FailureCategory
        CategoryOf(Catch::ResultWas::OfType type)
        {
            switch (type)
            {
                case Catch::ResultWas::ExpressionFailed:
                    return FailureCategory::kAssertion;
                case Catch::ResultWas::ExplicitFailure:
                    return FailureCategory::kExplicitFailure;
                case Catch::ResultWas::ThrewException:
                case Catch::ResultWas::Exception:
                    return FailureCategory::kUnexpectedException;
                case Catch::ResultWas::DidntThrowException:
                    return FailureCategory::kMissingException;
                case Catch::ResultWas::FatalErrorCondition:
                    return FailureCategory::kCrash;
                default:
                    return FailureCategory::kNone;
            }
        }

        std::string
        Location(const Catch::SourceLineInfo &info)
        {
            return std::string{ info.file } + ":" + std::to_string(info.line);
        }

        /// Turns the events of Catch2's engine into protocol messages.
        class ProtocolListener final: public Catch::IEventListener
        {
        public:
            ProtocolListener(const Catch::IConfig *config,
                             EventSink sink,
                             bool includeSuccessful)
                : IEventListener{ config }
                , sink_{ std::move(sink) }
                , includeSuccessful_{ includeSuccessful }
            {
                // Captured output belongs to the test that printed it.
                m_preferences.shouldRedirectStdOut = true;
                m_preferences.shouldReportAllAssertions = includeSuccessful;
                // The start of every assertion, for the location of a crash
                // or a hang.
                m_preferences.shouldReportAllAssertionStarts = true;
            }

            void
            Begin(std::size_t index, std::size_t attempt)
            {
                index_ = index;
                attempt_ = attempt;
                depth_ = 0;
                sections_.clear();
                sectionStarts_.clear();
                firstCategory_ = FailureCategory::kNone;
                lastLocation_.clear();
                lastSentLocation_ = Clock::time_point{};
            }

            void
            noMatchingTestCases(Catch::StringRef) override
            {}

            void
            reportInvalidTestSpec(Catch::StringRef) override
            {}

            void
            testRunStarting(const Catch::TestRunInfo &) override
            {}

            void
            testCaseStarting(const Catch::TestCaseInfo &) override
            {
                start_ = Clock::now();
                JsonValue message = Message("start");
                message.Set("index", index_);
                message.Set("attempt", attempt_);
                sink_(message);
            }

            void
            testCasePartialStarting(const Catch::TestCaseInfo &,
                                    std::uint64_t) override
            {
                // Every run through the test case enters the sections
                // again; depth starts over.
                depth_ = 0;
                sections_.clear();
                sectionStarts_.clear();
            }

            void
            sectionStarting(const Catch::SectionInfo &info) override
            {
                // Catch2 reports the test case itself as the outermost
                // section; the runner counts real sections only.
                if (depth_++ == 0)
                {
                    return;
                }
                sections_.push_back(info.name);
                sectionStarts_.push_back(Clock::now());
                JsonValue message = Message("section");
                message.Set("begin", true);
                message.Set("name", info.name);
                message.Set("file", std::string{ info.lineInfo.file });
                message.Set("line", info.lineInfo.line);
                message.Set("depth", sections_.size() - 1);
                message.Set("at", MicrosecondsBetween(start_, Clock::now()));
                sink_(message);
            }

            void
            sectionEnded(const Catch::SectionStats &stats) override
            {
                if (stats.missingAssertions)
                {
                    // --warn=no-assertions: say why the test failed.
                    AssertionRecord record;
                    record.file
                        = std::string{ stats.sectionInfo.lineInfo.file };
                    record.line = stats.sectionInfo.lineInfo.line;
                    record.message = "no assertion ran in "
                                     + stats.sectionInfo.name
                                     + " (--warn=no-assertions)";
                    record.sections = sections_;
                    record.category = FailureCategory::kAssertion;
                    if (firstCategory_ == FailureCategory::kNone)
                    {
                        firstCategory_ = record.category;
                    }
                    JsonValue message = ToJson(record);
                    message.Set("t", "assertion");
                    sink_(message);
                }
                if (depth_ == 0)
                {
                    return;
                }
                if (--depth_ == 0 || sections_.empty())
                {
                    return;
                }
                const auto now = Clock::now();
                JsonValue message = Message("section");
                message.Set("begin", false);
                message.Set("depth", sections_.size() - 1);
                message.Set("at", MicrosecondsBetween(start_, now));
                message.Set("duration",
                            MicrosecondsBetween(sectionStarts_.back(), now));
                sink_(message);
                sections_.pop_back();
                sectionStarts_.pop_back();
            }

            void
            assertionStarting(const Catch::AssertionInfo &info) override
            {
                lastLocation_ = Location(info.lineInfo);
                // A hang is only noticed from outside, so the location goes
                // out now and then rather than with every assertion.
                const auto now = Clock::now();
                if (now - lastSentLocation_ > std::chrono::milliseconds(200))
                {
                    lastSentLocation_ = now;
                    JsonValue message = Message("at");
                    message.Set("location", lastLocation_);
                    sink_(message);
                }
            }

            void
            assertionEnded(const Catch::AssertionStats &stats) override
            {
                const auto &result = stats.assertionResult;
                const bool skipped
                    = result.getResultType() == Catch::ResultWas::ExplicitSkip;
                const bool passed = result.succeeded() || skipped;
                if (passed && !includeSuccessful_ && !skipped)
                {
                    return;
                }
                AssertionRecord record;
                record.file = std::string{ result.getSourceInfo().file };
                record.line = result.getSourceInfo().line;
                record.macro = std::string{ result.getTestMacroName() };
                if (result.hasExpression())
                {
                    record.expression = result.getExpression();
                    record.expansion = result.getExpandedExpression();
                }
                record.message = std::string{ result.getMessage() };
                for (const auto &info : stats.infoMessages)
                {
                    if (info.type == Catch::ResultWas::Info
                        || info.type == Catch::ResultWas::Warning)
                    {
                        record.context.push_back(info.message);
                    }
                }
                record.sections = sections_;
                record.passed = passed;
                record.category = skipped ? FailureCategory::kNone
                                          : CategoryOf(result.getResultType());
                record.allowed = !passed && result.isOk();
                if (!passed && !record.allowed
                    && firstCategory_ == FailureCategory::kNone)
                {
                    firstCategory_ = record.category;
                }
                JsonValue message = ToJson(record);
                message.Set("t", "assertion");
                sink_(message);
            }

            void
            benchmarkPreparing(Catch::StringRef) override
            {}

            void
            benchmarkStarting(const Catch::BenchmarkInfo &) override
            {}

            void
            benchmarkEnded(const Catch::BenchmarkStats<> &stats) override
            {
                BenchmarkRecord record;
                record.name = stats.info.name;
                record.samples = stats.samples.size();
                record.iterations
                    = static_cast<std::size_t>(stats.info.iterations);
                record.meanNanoseconds = stats.mean.point.count();
                record.lowMeanNanoseconds = stats.mean.lower_bound.count();
                record.highMeanNanoseconds = stats.mean.upper_bound.count();
                record.standardDeviationNanoseconds
                    = stats.standardDeviation.point.count();
                record.outlierVariance = stats.outlierVariance;
                JsonValue message = ToJson(record);
                message.Set("t", "benchmark");
                sink_(message);
            }

            void
            benchmarkFailed(Catch::StringRef) override
            {}

            void
            testCasePartialEnded(const Catch::TestCaseStats &,
                                 std::uint64_t) override
            {}

            void
            testCaseEnded(const Catch::TestCaseStats &stats) override
            {
                const auto &counts = stats.totals.testCases;
                Outcome outcome = Outcome::kPassed;
                if (counts.failed > 0)
                {
                    outcome = Outcome::kFailed;
                }
                else if (counts.failedButOk > 0)
                {
                    outcome = Outcome::kFailedAsExpected;
                }
                else if (counts.skipped > 0)
                {
                    outcome = Outcome::kSkipped;
                }
                FailureCategory category = firstCategory_;
                if (outcome == Outcome::kFailed
                    && category == FailureCategory::kNone)
                {
                    // A [!shouldfail] test that passed, or a test without
                    // assertions under --warn=no-assertions.
                    category = FailureCategory::kAssertion;
                }
                JsonValue message = Message("end");
                message.Set("index", index_);
                message.Set("attempt", attempt_);
                message.Set("outcome", OutcomeName(outcome));
                if (outcome == Outcome::kFailed)
                {
                    message.Set("category", CategoryName(category));
                }
                message.Set("passed", stats.totals.assertions.passed);
                // Failures [!mayfail] and [!shouldfail] allow are failures
                // too; the verdict says whether they count.
                message.Set("failed",
                            stats.totals.assertions.failed
                                + stats.totals.assertions.failedButOk);
                message.Set("duration",
                            MicrosecondsBetween(start_, Clock::now()));
                if (!stats.stdOut.empty())
                {
                    message.Set("stdout", stats.stdOut);
                }
                if (!stats.stdErr.empty())
                {
                    message.Set("stderr", stats.stdErr);
                }
                sink_(message);
            }

            void
            testRunEnded(const Catch::TestRunStats &) override
            {}

            void
            fatalErrorEncountered(Catch::StringRef error) override
            {
                JsonValue message = Message("fatal");
                message.Set("message", std::string{ error });
                message.Set("location", lastLocation_);
                sink_(message);
            }

        private:
            EventSink sink_;
            bool includeSuccessful_;
            std::size_t index_ = 0;
            std::size_t attempt_ = 0;
            std::size_t depth_ = 0;
            std::vector<std::string> sections_;
            std::vector<Clock::time_point> sectionStarts_;
            Clock::time_point start_{};
            FailureCategory firstCategory_ = FailureCategory::kNone;
            std::string lastLocation_;
            Clock::time_point lastSentLocation_{};
        };
    } // namespace

    Catch::ConfigData
    ToConfigData(const Options &options, const std::string &programName)
    {
        Catch::ConfigData data;
        data.showSuccessfulTests = options.includeSuccessful;
        data.shouldDebugBreak = options.breakIntoDebugger;
        data.noThrow = options.noThrow;
        data.filenamesAsTags = options.filenamesAsTags;
        data.allowZeroTests = options.allowNoTests;
        data.rngSeed = options.seed;
        data.rngSeedWasFixed = true;
        data.skipBenchmarks = options.skipBenchmarks;
        data.benchmarkNoAnalysis = options.benchmarkNoAnalysis;
        data.benchmarkSamples = options.benchmarkSamples;
        data.benchmarkResamples = options.benchmarkResamples;
        data.benchmarkConfidenceInterval = options.benchmarkConfidenceInterval;
        data.benchmarkWarmupTime = options.benchmarkWarmupTime.count();
        int warnings = Catch::WarnAbout::Nothing;
        if (options.warnNoAssertions)
        {
            warnings |= Catch::WarnAbout::NoAssertions;
        }
        if (options.warnUnmatchedSpecs)
        {
            warnings |= Catch::WarnAbout::UnmatchedTestSpec;
        }
        if (options.warnInfiniteGenerators)
        {
            warnings |= Catch::WarnAbout::InfiniteGenerator;
        }
        data.warnings = static_cast<Catch::WarnAbout::What>(warnings);
        data.runOrder = Catch::TestRunOrder::Declared;
        data.processName = programName;
        data.testsOrTags = options.testSpecs;
        for (const auto &section : options.sections)
        {
            data.pathFilters.emplace_back(Catch::PathFilter::For::Section,
                                          Catch::trim(section));
        }
        for (const auto &path : options.paths)
        {
            data.useNewPathFilteringBehaviour = true;
            data.pathFilters.emplace_back(
                path[0] == 'c' ? Catch::PathFilter::For::Section
                               : Catch::PathFilter::For::Generator,
                Catch::trim(path.substr(2)));
        }
        return data;
    }

    namespace
    {
        /// The executor whose test is running, for the exit handler.
        std::atomic<TestExecutor::State *> runningState{ nullptr };

        /// A test that calls `exit` never ends, and what Catch captured of
        /// its output would be lost with the process: it is written where
        /// the runner reads the process's own output.
        void
        WriteCapturedOutputAtExit();
    } // namespace

    struct TestExecutor::State final
    {
        Catch::Config config;
        const Catalog &catalog;
        ProtocolListener *listener = nullptr;
        std::optional<Catch::RunContext> context;

        State(const Catch::ConfigData &data, const Catalog &knownTests)
            : config{ data }
            , catalog{ knownTests }
        {}
    };

    TestExecutor::TestExecutor(const Options &options,
                               const Catalog &catalog,
                               const std::string &programName,
                               EventSink sink)
        : state_{ std::make_unique<State>(ToConfigData(options, programName),
                                          catalog) }
    {
        Catch::getCurrentMutableContext().setConfig(&state_->config);
        auto multi
            = Catch::Detail::make_unique<Catch::MultiReporter>(&state_->config);
        for (const auto &factory :
             Catch::getRegistryHub().getReporterRegistry().getListeners())
        {
            multi->addListener(factory->create(&state_->config));
        }
        auto listener = Catch::Detail::make_unique<ProtocolListener>(
            &state_->config,
            std::move(sink),
            options.includeSuccessful);
        state_->listener = listener.get();
        multi->addReporter(CATCH_MOVE(listener));
        Catch::seedRng(state_->config);
        state_->context.emplace(&state_->config, CATCH_MOVE(multi));
        static const bool registered
            = std::atexit(&WriteCapturedOutputAtExit) == 0;
        static_cast<void>(registered);
    }

    TestExecutor::~TestExecutor()
    {
        // The run context reports the end of the run to the listeners.
        state_->context.reset();
        Catch::getCurrentMutableContext().setConfig(nullptr);
    }

    void
    TestExecutor::Run(std::size_t index,
                      std::size_t attempt,
                      const std::string &temporaryDirectory)
    {
        if (index >= state_->catalog.Handles().size())
        {
            return;
        }
        const TemporaryDirectoryScope scope{ temporaryDirectory };
        state_->listener->Begin(index, attempt);
        runningState.store(state_.get());
        static_cast<void>(
            state_->context->runTest(state_->catalog.Handles()[index]));
        runningState.store(nullptr);
    }

    namespace
    {
        void
        WriteCapturedOutputAtExit()
        {
            TestExecutor::State *state = runningState.exchange(nullptr);
            if (state == nullptr || !state->context)
            {
                return;
            }
            const std::string output = state->context->endCapture();
            std::fflush(stdout);
            std::fflush(stderr);
            WriteStandardError(output);
        }
    } // namespace

    int
    RunWorker(const Options &options, const std::string &programName)
    {
        QuietCrashReporting();
        auto streams = TakeStandardStreams();
        if (!streams)
        {
            std::fprintf(stderr,
                         "catch3 worker: %s\n",
                         streams.GetError().Message().c_str());
            return 1;
        }
        auto catalog = Catalog::Load();
        if (!catalog)
        {
            std::fprintf(stderr,
                         "catch3 worker: %s\n",
                         catalog.GetError().Message().c_str());
            return 1;
        }
        MessageWriter writer{ streams->events };
        bool connected = true;
        const auto send = [&](const JsonValue &message) {
            if (connected && !writer.Send(message))
            {
                // The runner is gone; there is no one to report to.
                connected = false;
            }
        };
        // Listeners are created, and told of the run, before the greeting.
        // What they print then travels in the greeting: through the error
        // stream it could arrive after the runner gave this worker a test,
        // and pass for that test's output.
        std::ostringstream startup;
        auto *const previousOut = Catch::cout().rdbuf(startup.rdbuf());
        auto *const previousErr = Catch::cerr().rdbuf(startup.rdbuf());
        auto *const previousLog = Catch::clog().rdbuf(startup.rdbuf());
        TestExecutor executor{ options, *catalog, programName, send };
        Catch::cout().rdbuf(previousOut);
        Catch::cerr().rdbuf(previousErr);
        Catch::clog().rdbuf(previousLog);
        std::fflush(stdout);
        std::fflush(stderr);

        JsonValue hello = Message("hello");
        hello.Set("output", startup.str());
        hello.Set("pid", CurrentProcessId());
        hello.Set("identity", catalog->Identity());
        hello.Set("tests", catalog->Tests().size());
        hello.Set("protocol", kWorkerProtocolVersion);
        send(hello);
        LineReader commands{ streams->commands };
        while (connected)
        {
            const auto line = commands.Next();
            if (!line)
            {
                break;
            }
            const auto command = ParseJson(*line);
            if (!command)
            {
                continue;
            }
            const std::string &kind = (*command)["t"].AsString();
            if (kind == "quit")
            {
                break;
            }
            if (kind == "run")
            {
                executor.Run(
                    static_cast<std::size_t>((*command)["index"].AsUnsigned()),
                    static_cast<std::size_t>(
                        (*command)["attempt"].AsUnsigned()),
                    (*command)["temp"].AsString());
            }
        }
        return 0;
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0


#if defined(_WIN32)
#    include <fcntl.h>
#    include <io.h>
#    include <share.h>
#    include <sys/stat.h>
#else
#    include <fcntl.h>
#    include <sys/stat.h>
#    include <unistd.h>
#endif

#include <atomic>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <system_error>
#include <thread>

namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        /// Creates `path` only if it does not exist, in one atomic step.
        bool
        CreateExclusively(const std::filesystem::path &path)
        {
#if defined(_WIN32)
            int descriptor = -1;
            if (_wsopen_s(&descriptor,
                          path.wstring().c_str(),
                          _O_CREAT | _O_EXCL | _O_WRONLY,
                          _SH_DENYNO,
                          _S_IREAD | _S_IWRITE) != 0)
            {
                return false;
            }
            _close(descriptor);
            return true;
#else
            const int descriptor = open(path.c_str(),
                                        O_CREAT | O_EXCL | O_WRONLY | O_CLOEXEC,
                                        S_IRUSR | S_IWUSR);
            if (descriptor < 0)
            {
                return false;
            }
            close(descriptor);
            return true;
#endif
        }
    } // namespace

    FileLock::FileLock(const std::filesystem::path &file,
                       std::chrono::milliseconds wait,
                       std::chrono::milliseconds stale)
        : path_{file.string() + ".lock"}
    {
        std::error_code error;
        std::filesystem::create_directories(path_.parent_path(), error);
        const auto deadline = std::chrono::steady_clock::now() + wait;
        while (true)
        {
            if (CreateExclusively(path_))
            {
                held_ = true;
                return;
            }
            const auto written = std::filesystem::last_write_time(path_, error);
            if (!error &&
                std::filesystem::file_time_type::clock::now() - written > stale)
            {
                std::filesystem::remove(path_, error);
                continue;
            }
            if (std::chrono::steady_clock::now() >= deadline)
            {
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    FileLock::~FileLock()
    {
        if (held_)
        {
            std::error_code error;
            std::filesystem::remove(path_, error);
        }
    }

    Expected<std::string>
    ReadFile(const std::filesystem::path &file)
    {
        std::ifstream input(file, std::ios::binary);
        if (!input)
        {
            return Error{"cannot read '" + file.string() + "'"};
        }
        std::ostringstream contents;
        contents << input.rdbuf();
        if (input.bad())
        {
            return Error{"cannot read '" + file.string() + "'"};
        }
        return contents.str();
    }

    Status
    WriteFileAtomically(const std::filesystem::path &file,
                        std::string_view contents)
    {
        std::error_code error;
        if (file.has_parent_path())
        {
            std::filesystem::create_directories(file.parent_path(), error);
        }
        const auto temporary =
            std::filesystem::path{file.string() + "." + UniqueName() + ".tmp"};
        {
            std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
            output.write(contents.data(),
                         static_cast<std::streamsize>(contents.size()));
            if (!output)
            {
                output.close();
                std::filesystem::remove(temporary, error);
                return Error{"cannot write '" + file.string() + "'"};
            }
        }
        std::filesystem::rename(temporary, file, error);
        if (error)
        {
            std::error_code ignored;
            std::filesystem::remove(temporary, ignored);
            return Error{"cannot replace '" + file.string() +
                         "': " + error.message()};
        }
        return Status::Ok();
    }

    Status
    CreatePrivateFile(const std::filesystem::path &file)
    {
#if defined(_WIN32)
        std::error_code error;
        const auto status = std::filesystem::symlink_status(file, error);
        if (!error && std::filesystem::exists(status) &&
            !std::filesystem::is_regular_file(status))
        {
            return Error{"'" + file.string() + "' is not a regular file"};
        }
        int descriptor = -1;
        if (_wsopen_s(&descriptor,
                      file.wstring().c_str(),
                      _O_CREAT | _O_TRUNC | _O_WRONLY | _O_NOINHERIT,
                      _SH_DENYNO,
                      _S_IREAD | _S_IWRITE) != 0)
        {
            return Error{"cannot create '" + file.string() + "'"};
        }
        _close(descriptor);
        return Status::Ok();
#else
        int flags = O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC | O_NONBLOCK;
#    if defined(O_NOFOLLOW)
        flags |= O_NOFOLLOW;
#    endif
        const int descriptor = open(file.c_str(), flags, S_IRUSR | S_IWUSR);
        if (descriptor < 0)
        {
            return Error{"cannot create '" + file.string() + "'"};
        }
        struct stat information = {};
        if (fstat(descriptor, &information) != 0 ||
            !S_ISREG(information.st_mode))
        {
            close(descriptor);
            return Error{"'" + file.string() + "' is not a regular file"};
        }
        // A file that existed keeps its mode; make it private too.
        fchmod(descriptor, S_IRUSR | S_IWUSR);
        close(descriptor);
        return Status::Ok();
#endif
    }

    std::string
    UniqueName()
    {
        static std::atomic<unsigned> counter{0};
        std::ostringstream text;
        text << CurrentProcessId() << '-'
             << std::chrono::steady_clock::now().time_since_epoch().count()
             << '-' << std::this_thread::get_id() << '-' << ++counter;
        return ToHex(Fnv1a64(text.str()));
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0


#include <algorithm>
#include <cctype>
#include <system_error>

namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        Expected<std::string>
        Git(const std::filesystem::path &directory,
            std::vector<std::string> arguments)
        {
            ChildOptions options;
            options.arguments.emplace_back("git");
            options.arguments.insert(
                options.arguments.end(), arguments.begin(), arguments.end());
            options.workingDirectory = directory;
            options.searchPath = true;
            auto run = RunAndCapture(options);
            if (!run)
            {
                return run.GetError();
            }
            if (!run->status.Succeeded())
            {
                std::string message = run->errors;
                while (!message.empty() &&
                       (message.back() == '\n' || message.back() == '\r'))
                {
                    message.pop_back();
                }
                return Error{"git " + arguments.front() + " failed: " +
                             message};
            }
            return run->output;
        }

        std::vector<std::string>
        Lines(const std::string &text)
        {
            std::vector<std::string> lines;
            std::size_t start = 0;
            while (start < text.size())
            {
                auto end = text.find('\n', start);
                if (end == std::string::npos)
                {
                    end = text.size();
                }
                std::string line = text.substr(start, end - start);
                if (!line.empty() && line.back() == '\r')
                {
                    line.pop_back();
                }
                if (!line.empty())
                {
                    lines.push_back(std::move(line));
                }
                start = end + 1;
            }
            return lines;
        }

        std::string
        Normalised(const std::filesystem::path &path)
        {
            std::string text = path.lexically_normal().generic_string();
#if defined(_WIN32)
            // Windows paths compare without case.
            std::transform(text.begin(), text.end(), text.begin(), [](char c) {
                return static_cast<char>(
                    std::tolower(static_cast<unsigned char>(c)));
            });
#endif
            return text;
        }
    } // namespace

    Expected<Changes>
    ReadChanges(const std::filesystem::path &directory,
                const std::string &revision)
    {
        auto root = Git(directory, {"rev-parse", "--show-toplevel"});
        if (!root)
        {
            return root.GetError();
        }
        Changes changes;
        const auto rootLines = Lines(*root);
        if (rootLines.empty())
        {
            return Error{"git did not name the top of the checkout"};
        }
        changes.root = std::filesystem::path{rootLines.front()};
        changes.commit = CurrentCommit(directory);
        const std::string base = revision.empty() ? "HEAD" : revision;
        auto changed =
            Git(changes.root, {"diff", "--name-only", "--no-renames", base});
        if (!changed)
        {
            return changed.GetError();
        }
        auto untracked = Git(changes.root,
                             {"ls-files", "--others", "--exclude-standard"});
        for (const auto &line : Lines(*changed))
        {
            changes.files.push_back(changes.root / line);
        }
        if (untracked)
        {
            for (const auto &line : Lines(*untracked))
            {
                changes.files.push_back(changes.root / line);
            }
        }
        return changes;
    }

    std::string
    CurrentCommit(const std::filesystem::path &directory)
    {
        auto commit = Git(directory, {"rev-parse", "HEAD"});
        if (!commit)
        {
            return {};
        }
        const auto lines = Lines(*commit);
        return lines.empty() ? std::string{} : lines.front();
    }

    bool
    SameSourceFile(const std::filesystem::path &left,
                   const std::filesystem::path &right)
    {
        const std::string first = Normalised(left);
        const std::string second = Normalised(right);
        if (first == second)
        {
            return true;
        }
        const auto endsWith = [](const std::string &text,
                                 const std::string &suffix) {
            if (suffix.empty() || suffix.size() >= text.size())
            {
                return false;
            }
            return text.compare(text.size() - suffix.size(),
                                suffix.size(),
                                suffix) == 0 &&
                   text[text.size() - suffix.size() - 1] == '/';
        };
        // __FILE__ is often relative to the build directory.
        std::string relativeFirst = first;
        std::string relativeSecond = second;
        while (relativeFirst.rfind("../", 0) == 0)
        {
            relativeFirst.erase(0, 3);
        }
        while (relativeSecond.rfind("../", 0) == 0)
        {
            relativeSecond.erase(0, 3);
        }
        return (!left.is_absolute() && endsWith(second, relativeFirst)) ||
               (!right.is_absolute() && endsWith(first, relativeSecond));
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <cstring>
#include <fstream>

namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        constexpr std::array<std::uint32_t, 64> kRoundConstants = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b,
            0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
            0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7,
            0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
            0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152,
            0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
            0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
            0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
            0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08,
            0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
            0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
            0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
        };

        constexpr std::uint32_t
        RotateRight(std::uint32_t value, int count) noexcept
        {
            return (value >> count) | (value << (32 - count));
        }
    } // namespace

    Sha256::Sha256()
        : state_{0x6a09e667,
                 0xbb67ae85,
                 0x3c6ef372,
                 0xa54ff53a,
                 0x510e527f,
                 0x9b05688c,
                 0x1f83d9ab,
                 0x5be0cd19}
    {
    }

    void
    Sha256::Update(const void *data, std::size_t size)
    {
        const auto *bytes = static_cast<const std::uint8_t *>(data);
        totalSize_ += size;
        while (size > 0)
        {
            const std::size_t room = buffer_.size() - bufferSize_;
            const std::size_t take = size < room ? size : room;
            std::memcpy(buffer_.data() + bufferSize_, bytes, take);
            bufferSize_ += take;
            bytes += take;
            size -= take;
            if (bufferSize_ == buffer_.size())
            {
                Compress(buffer_.data());
                bufferSize_ = 0;
            }
        }
    }

    void
    Sha256::Compress(const std::uint8_t *block)
    {
        std::array<std::uint32_t, 64> schedule{};
        for (int index = 0; index < 16; ++index)
        {
            schedule[index] =
                (static_cast<std::uint32_t>(block[index * 4]) << 24) |
                (static_cast<std::uint32_t>(block[index * 4 + 1]) << 16) |
                (static_cast<std::uint32_t>(block[index * 4 + 2]) << 8) |
                static_cast<std::uint32_t>(block[index * 4 + 3]);
        }
        for (int index = 16; index < 64; ++index)
        {
            const std::uint32_t s0 = RotateRight(schedule[index - 15], 7) ^
                                     RotateRight(schedule[index - 15], 18) ^
                                     (schedule[index - 15] >> 3);
            const std::uint32_t s1 = RotateRight(schedule[index - 2], 17) ^
                                     RotateRight(schedule[index - 2], 19) ^
                                     (schedule[index - 2] >> 10);
            schedule[index] =
                schedule[index - 16] + s0 + schedule[index - 7] + s1;
        }
        std::uint32_t a = state_[0];
        std::uint32_t b = state_[1];
        std::uint32_t c = state_[2];
        std::uint32_t d = state_[3];
        std::uint32_t e = state_[4];
        std::uint32_t f = state_[5];
        std::uint32_t g = state_[6];
        std::uint32_t h = state_[7];
        for (int index = 0; index < 64; ++index)
        {
            const std::uint32_t s1 =
                RotateRight(e, 6) ^ RotateRight(e, 11) ^ RotateRight(e, 25);
            const std::uint32_t choice = (e & f) ^ (~e & g);
            const std::uint32_t first =
                h + s1 + choice + kRoundConstants[index] + schedule[index];
            const std::uint32_t s0 =
                RotateRight(a, 2) ^ RotateRight(a, 13) ^ RotateRight(a, 22);
            const std::uint32_t majority = (a & b) ^ (a & c) ^ (b & c);
            const std::uint32_t second = s0 + majority;
            h = g;
            g = f;
            f = e;
            e = d + first;
            d = c;
            c = b;
            b = a;
            a = first + second;
        }
        state_[0] += a;
        state_[1] += b;
        state_[2] += c;
        state_[3] += d;
        state_[4] += e;
        state_[5] += f;
        state_[6] += g;
        state_[7] += h;
    }

    std::string
    Sha256::HexDigest()
    {
        const std::uint64_t bitCount = totalSize_ * 8;
        const std::uint8_t marker = 0x80;
        Update(&marker, 1);
        const std::uint8_t zero = 0;
        while (bufferSize_ != 56)
        {
            Update(&zero, 1);
        }
        std::uint8_t length[8];
        for (int index = 0; index < 8; ++index)
        {
            length[index] =
                static_cast<std::uint8_t>(bitCount >> (56 - index * 8));
        }
        Update(length, sizeof(length));
        static constexpr char kDigits[] = "0123456789abcdef";
        std::string text;
        text.reserve(64);
        for (const std::uint32_t word : state_)
        {
            for (int shift = 28; shift >= 0; shift -= 4)
            {
                text.push_back(kDigits[(word >> shift) & 0xF]);
            }
        }
        return text;
    }

    std::string
    Sha256Hex(std::string_view text)
    {
        Sha256 hash;
        hash.Update(text);
        return hash.HexDigest();
    }

    Expected<std::string>
    Sha256File(const std::filesystem::path &path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            return Error{"cannot read '" + path.string() + "'"};
        }
        Sha256 hash;
        char buffer[1 << 16];
        while (file)
        {
            file.read(buffer, sizeof(buffer));
            const auto count = file.gcount();
            if (count > 0)
            {
                hash.Update(buffer, static_cast<std::size_t>(count));
            }
        }
        if (file.bad())
        {
            return Error{"cannot read '" + path.string() + "'"};
        }
        return hash.HexDigest();
    }

    std::uint64_t
    Fnv1a64(std::string_view text) noexcept
    {
        std::uint64_t hash = 0xcbf29ce484222325ULL;
        for (const char character : text)
        {
            hash ^= static_cast<unsigned char>(character);
            hash *= 0x100000001b3ULL;
        }
        return hash;
    }

    std::string
    ToHex(std::uint64_t value)
    {
        static constexpr char kDigits[] = "0123456789abcdef";
        std::string text(16, '0');
        for (int index = 15; index >= 0; --index)
        {
            text[static_cast<std::size_t>(index)] = kDigits[value & 0xF];
            value >>= 4;
        }
        return text;
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0
#include <algorithm>
#include <cmath>
#include <system_error>


namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        constexpr int kHistoryVersion = 1;
        constexpr std::size_t kRecordsPerTest = 50;
        constexpr std::size_t kRunsKept = 100;

        /// Runs that did not finish on their own say nothing about how long
        /// the test takes.
        bool
        HasDuration(Outcome outcome)
        {
            return outcome == Outcome::kPassed || outcome == Outcome::kFailed
                   || outcome == Outcome::kFailedAsExpected
                   || outcome == Outcome::kSkipped;
        }

        bool
        Counts(Outcome outcome)
        {
            return outcome != Outcome::kNotRun && outcome != Outcome::kBlocked
                   && outcome != Outcome::kUpToDate;
        }

        JsonValue
        RecordToJson(const TestRecord &record)
        {
            JsonValue value = JsonValue::Object();
            value.Set("run", record.run);
            value.Set("outcome", OutcomeName(record.outcome));
            if (record.category != FailureCategory::kNone)
            {
                value.Set("category", CategoryName(record.category));
            }
            value.Set("duration", record.durationMicroseconds);
            value.Set("attempts", record.attempts);
            if (record.flaky)
            {
                value.Set("flaky", true);
            }
            if (!record.signature.empty())
            {
                value.Set("signature", record.signature);
            }
            if (!record.fingerprint.empty())
            {
                value.Set("fingerprint", record.fingerprint);
            }
            return value;
        }

        TestRecord
        RecordFromJson(const JsonValue &value)
        {
            TestRecord record;
            record.run = value["run"].AsInteger();
            record.outcome = OutcomeFromName(value["outcome"].AsString())
                                 .value_or(Outcome::kNotRun);
            record.category = CategoryFromName(value["category"].AsString())
                                  .value_or(FailureCategory::kNone);
            record.durationMicroseconds = value["duration"].AsInteger();
            record.attempts
                = static_cast<std::size_t>(value["attempts"].AsUnsigned());
            record.flaky = value["flaky"].AsBoolean();
            record.signature = value["signature"].AsString();
            record.fingerprint = value["fingerprint"].AsString();
            return record;
        }
    } // namespace

    const TestRecord *
    TestHistory::Last() const noexcept
    {
        for (auto record = records.rbegin(); record != records.rend(); ++record)
        {
            if (Counts(record->outcome))
            {
                return &*record;
            }
        }
        return nullptr;
    }

    std::optional<std::int64_t>
    TestHistory::DurationPercentile(double fraction) const
    {
        std::vector<std::int64_t> durations;
        for (auto record = records.rbegin();
             record != records.rend() && durations.size() < 20;
             ++record)
        {
            if (HasDuration(record->outcome))
            {
                durations.push_back(record->durationMicroseconds);
            }
        }
        if (durations.empty())
        {
            return std::nullopt;
        }
        std::sort(durations.begin(), durations.end());
        const auto position = static_cast<std::size_t>(
            std::ceil(fraction * static_cast<double>(durations.size())));
        return durations[std::min(durations.size() - 1,
                                  position == 0 ? 0 : position - 1)];
    }

    double
    TestHistory::FailureProbability() const
    {
        // A beta prior worth one run: a test without history fails with
        // probability 0.3, a common share for new code.
        double failures = 0.3;
        double total = 1.0;
        double weight = 1.0;
        std::size_t seen = 0;
        for (auto record = records.rbegin();
             record != records.rend() && seen < 30;
             ++record)
        {
            if (!Counts(record->outcome))
            {
                continue;
            }
            ++seen;
            const bool failed = IsFailure(record->outcome) || record->flaky;
            failures += failed ? weight : 0.0;
            total += weight;
            weight *= 0.8;
        }
        double probability = failures / total;
        if (const TestRecord *last = Last();
            last != nullptr && IsFailure(last->outcome))
        {
            // What failed last time most often fails again.
            probability = std::max(probability, 0.7);
        }
        return probability;
    }

    std::size_t
    TestHistory::FlakyEvents(std::size_t window) const
    {
        std::size_t events = 0;
        std::size_t seen = 0;
        const TestRecord *newer = nullptr;
        for (auto record = records.rbegin();
             record != records.rend() && seen < window;
             ++record)
        {
            if (!Counts(record->outcome))
            {
                continue;
            }
            ++seen;
            if (record->flaky)
            {
                ++events;
            }
            else if (newer != nullptr && !newer->flaky
                     && !record->fingerprint.empty()
                     && record->fingerprint == newer->fingerprint
                     && IsFailure(record->outcome) != IsFailure(newer->outcome))
            {
                // The verdict changed while nothing the test depends on did.
                ++events;
            }
            newer = &*record;
        }
        return events;
    }

    std::size_t
    TestHistory::PassStreak() const
    {
        std::size_t streak = 0;
        for (auto record = records.rbegin(); record != records.rend(); ++record)
        {
            if (!Counts(record->outcome))
            {
                continue;
            }
            if (record->outcome != Outcome::kPassed || record->flaky)
            {
                break;
            }
            ++streak;
        }
        return streak;
    }

    std::filesystem::path
    HistoryFile(const std::filesystem::path &directory,
                const std::string &binaryName)
    {
        std::string name;
        for (const char character : binaryName)
        {
            const bool safe = (character >= 'a' && character <= 'z')
                              || (character >= 'A' && character <= 'Z')
                              || (character >= '0' && character <= '9')
                              || character == '-' || character == '_'
                              || character == '.';
            name.push_back(safe ? character : '_');
        }
        return directory / (name + ".history.json");
    }

    std::string
    History::KeyOf(const TestResult &test)
    {
        if (!test.id.empty())
        {
            return test.id;
        }
        return test.className.empty() ? test.name
                                      : test.className + "::" + test.name;
    }

    const TestHistory *
    History::Find(const std::string &key) const
    {
        const auto found = tests_.find(key);
        return found == tests_.end() ? nullptr : &found->second;
    }

    Expected<History>
    History::Load(const std::filesystem::path &file)
    {
        std::error_code error;
        if (!std::filesystem::exists(file, error))
        {
            return History{};
        }
        auto text = ReadFile(file);
        if (!text)
        {
            return text.GetError();
        }
        auto json = ParseJson(*text);
        if (!json)
        {
            return json.GetError().Context(file.string());
        }
        return FromJson(*json);
    }

    Expected<History>
    History::Commit(const std::filesystem::path &file,
                    const RunResult &run,
                    const RunRecord &record,
                    const std::map<std::string, std::string> &fingerprints,
                    const QuarantinePolicy &policy,
                    std::chrono::milliseconds lockWait)
    {
        const FileLock lock{ file, lockWait };
        if (!lock.Held())
        {
            // Writing without the lock could replace a run that another
            // process is adding at the same time.
            return Error{ "another run holds the lock of " + file.string() };
        }
        auto history = Load(file);
        if (!history)
        {
            // A damaged file is set aside rather than lost.
            std::error_code error;
            std::filesystem::rename(file,
                                    file.string() + ".damaged-" + UniqueName(),
                                    error);
            history = History{};
        }
        history->Apply(run, record, fingerprints, policy);
        if (auto status
            = WriteFileAtomically(file, history->ToJson().ToPrettyString());
            !status)
        {
            return status.GetError();
        }
        return history;
    }

    void
    History::Apply(const RunResult &run,
                   RunRecord record,
                   const std::map<std::string, std::string> &fingerprints,
                   const QuarantinePolicy &policy)
    {
        record.id = nextRun_++;
        runs_.push_back(record);
        if (runs_.size() > kRunsKept)
        {
            runs_.erase(
                runs_.begin(),
                runs_.begin()
                    + static_cast<std::ptrdiff_t>(runs_.size() - kRunsKept));
        }
        for (const auto &test : run.tests)
        {
            if (test.outcome == Outcome::kNotRun)
            {
                continue;
            }
            const std::string key = KeyOf(test);
            TestHistory &history = tests_[key];
            TestRecord result;
            result.run = record.id;
            result.outcome = test.outcome;
            result.category = test.category;
            result.durationMicroseconds
                = test.attempts.empty()
                      ? 0
                      : test.attempts.back().durationMicroseconds;
            result.attempts = test.attempts.size();
            result.flaky = test.flaky;
            result.signature = test.signature;
            if (const auto found = fingerprints.find(key);
                found != fingerprints.end())
            {
                result.fingerprint = found->second;
            }
            history.records.push_back(result);
            if (history.records.size() > kRecordsPerTest)
            {
                history.records.erase(history.records.begin());
            }
            const auto failed
                = std::find_if(test.attempts.begin(),
                               test.attempts.end(),
                               [](const Attempt &attempt) {
                                   return IsFailure(attempt.outcome);
                               });
            if ((IsFailure(test.outcome) || test.flaky)
                && failed != test.attempts.end())
            {
                // The first failure, as it happened: a flaky failure is
                // worth replaying too.
                ReplayRecord replay;
                replay.run = record.id;
                replay.seed = failed->seed;
                replay.precededBy = failed->precededBy;
                replay.freshProcess = failed->freshProcess;
                history.lastFailure = replay;
            }
            else if (test.outcome == Outcome::kPassed)
            {
                history.lastFailure.reset();
            }
            // Quarantine: in after repeated flakiness, out after a streak of
            // clean passes.
            if (!history.quarantine
                && history.FlakyEvents(policy.window)
                       >= policy.flakyEventsToEnter)
            {
                history.quarantine = QuarantineRecord{
                    record.id,
                    std::to_string(history.FlakyEvents(policy.window))
                        + " flaky results in the last "
                        + std::to_string(policy.window) + " runs"
                };
            }
            else if (history.quarantine
                     && history.PassStreak() >= policy.passesToLeave)
            {
                history.quarantine.reset();
            }
        }
    }

    JsonValue
    History::ToJson() const
    {
        JsonValue value = JsonValue::Object();
        value.Set("format", "catch3-history");
        value.Set("version", kHistoryVersion);
        value.Set("nextRun", nextRun_);
        JsonValue runs = JsonValue::Array();
        for (const auto &run : runs_)
        {
            JsonValue item = JsonValue::Object();
            item.Set("id", run.id);
            item.Set("time", run.unixMilliseconds);
            item.Set("commit", run.commit);
            item.Set("binary", run.binaryFingerprint);
            item.Set("seed", run.seed);
            item.Set("tests", run.tests);
            item.Set("failed", run.failed);
            item.Set("wall", run.wallMicroseconds);
            runs.Append(std::move(item));
        }
        value.Set("runs", std::move(runs));
        JsonValue tests = JsonValue::Object();
        for (const auto &[key, history] : tests_)
        {
            JsonValue item = JsonValue::Object();
            JsonValue records = JsonValue::Array();
            for (const auto &record : history.records)
            {
                records.Append(RecordToJson(record));
            }
            item.Set("results", std::move(records));
            if (history.quarantine)
            {
                JsonValue quarantine = JsonValue::Object();
                quarantine.Set("since", history.quarantine->since);
                quarantine.Set("reason", history.quarantine->reason);
                item.Set("quarantine", std::move(quarantine));
            }
            if (history.lastFailure)
            {
                JsonValue replay = JsonValue::Object();
                replay.Set("run", history.lastFailure->run);
                replay.Set("seed", history.lastFailure->seed);
                JsonValue preceding = JsonValue::Array();
                for (const auto &name : history.lastFailure->precededBy)
                {
                    preceding.Append(name);
                }
                replay.Set("precededBy", std::move(preceding));
                replay.Set("freshProcess", history.lastFailure->freshProcess);
                item.Set("lastFailure", std::move(replay));
            }
            tests.Set(key, std::move(item));
        }
        value.Set("tests", std::move(tests));
        return value;
    }

    Expected<History>
    History::FromJson(const JsonValue &value)
    {
        if (value["format"].AsString() != "catch3-history")
        {
            return Error{ "not a Catch3 history" };
        }
        if (value["version"].AsInteger() > kHistoryVersion)
        {
            return Error{ "the history was written by a newer Catch3" };
        }
        History history;
        history.nextRun_
            = std::max<std::int64_t>(1, value["nextRun"].AsInteger());
        for (const auto &item : value["runs"].Items())
        {
            RunRecord run;
            run.id = item["id"].AsInteger();
            run.unixMilliseconds = item["time"].AsInteger();
            run.commit = item["commit"].AsString();
            run.binaryFingerprint = item["binary"].AsString();
            run.seed = static_cast<std::uint32_t>(item["seed"].AsUnsigned());
            run.tests = static_cast<std::size_t>(item["tests"].AsUnsigned());
            run.failed = static_cast<std::size_t>(item["failed"].AsUnsigned());
            run.wallMicroseconds = item["wall"].AsInteger();
            history.runs_.push_back(run);
        }
        for (const auto &[key, item] : value["tests"].Members())
        {
            TestHistory test;
            for (const auto &record : item["results"].Items())
            {
                test.records.push_back(RecordFromJson(record));
            }
            if (item["quarantine"].IsObject())
            {
                test.quarantine = QuarantineRecord{
                    item["quarantine"]["since"].AsInteger(),
                    item["quarantine"]["reason"].AsString()
                };
            }
            if (item["lastFailure"].IsObject())
            {
                const JsonValue &failure = item["lastFailure"];
                ReplayRecord replay;
                replay.run = failure["run"].AsInteger();
                replay.seed
                    = static_cast<std::uint32_t>(failure["seed"].AsUnsigned());
                for (const auto &name : failure["precededBy"].Items())
                {
                    replay.precededBy.push_back(name.AsString());
                }
                replay.freshProcess = failure["freshProcess"].AsBoolean();
                test.lastFailure = replay;
            }
            history.tests_.emplace(key, std::move(test));
        }
        return history;
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0


#include <cmath>
#include <cstdlib>

namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        // The empty values that accessors return for a value of another
        // kind. They are never destroyed, so that no destructor runs at exit
        // while a worker may still be reading a message.
        const std::string &
        EmptyString()
        {
            static const auto *value = new std::string();
            return *value;
        }

        const std::vector<JsonValue> &
        EmptyItems()
        {
            static const auto *value = new std::vector<JsonValue>();
            return *value;
        }

        const JsonMembers &
        EmptyMembers()
        {
            static const auto *value = new JsonMembers();
            return *value;
        }

        const JsonValue &
        NullValue()
        {
            static const auto *value = new JsonValue();
            return *value;
        }

        /// Documents nested deeper than this are refused, so a hostile or
        /// broken file cannot exhaust the stack while it is converted.
        constexpr int kMaximumDepth = 256;

        /// Strings are bytes: test names and output need not be UTF-8, and
        /// they come back as they were written.
        constexpr yyjson_read_flag kReadFlags =
            YYJSON_READ_ALLOW_INVALID_UNICODE;

        /// JSON has no infinities; null is the conventional stand-in.
        constexpr yyjson_write_flag kWriteFlags =
            YYJSON_WRITE_ALLOW_INVALID_UNICODE |
            YYJSON_WRITE_INF_AND_NAN_AS_NULL;

        /// Converts a value of yyjson's tree; false when it is nested too
        /// deeply.
        bool
        FromYyjson(yyjson_val *value, int depth, JsonValue &result)
        {
            if (depth > kMaximumDepth)
            {
                return false;
            }
            switch (yyjson_get_type(value))
            {
                case YYJSON_TYPE_BOOL:
                    result = JsonValue{yyjson_get_bool(value)};
                    return true;
                case YYJSON_TYPE_NUM:
                    result = JsonValue{yyjson_get_num(value)};
                    return true;
                case YYJSON_TYPE_STR:
                    result = JsonValue{std::string_view{
                        yyjson_get_str(value), yyjson_get_len(value)}};
                    return true;
                case YYJSON_TYPE_ARR:
                {
                    JsonValue array = JsonValue::Array();
                    auto &items = array.MutableItems();
                    items.reserve(yyjson_arr_size(value));
                    yyjson_arr_iter iterator = yyjson_arr_iter_with(value);
                    while (yyjson_val *item = yyjson_arr_iter_next(&iterator))
                    {
                        items.emplace_back();
                        if (!FromYyjson(item, depth + 1, items.back()))
                        {
                            return false;
                        }
                    }
                    result = std::move(array);
                    return true;
                }
                case YYJSON_TYPE_OBJ:
                {
                    JsonValue object = JsonValue::Object();
                    auto &members = object.MutableMembers();
                    members.reserve(yyjson_obj_size(value));
                    yyjson_obj_iter iterator = yyjson_obj_iter_with(value);
                    while (yyjson_val *key = yyjson_obj_iter_next(&iterator))
                    {
                        members.emplace_back(
                            std::string{yyjson_get_str(key),
                                        yyjson_get_len(key)},
                            JsonValue{});
                        if (!FromYyjson(yyjson_obj_iter_get_val(key),
                                        depth + 1,
                                        members.back().second))
                        {
                            return false;
                        }
                    }
                    result = std::move(object);
                    return true;
                }
                default:
                    result = JsonValue{};
                    return true;
            }
        }

        /// A whole number within the exact range of a double is written as
        /// an integer, anything else as the shortest text that reads back as
        /// the same double.
        yyjson_mut_val *
        ToYyjsonNumber(yyjson_mut_doc *document, double value)
        {
            if (std::isfinite(value) &&
                std::fabs(value) < 9.007199254740992e15 &&
                value == std::trunc(value))
            {
                return yyjson_mut_sint(document,
                                       static_cast<std::int64_t>(value));
            }
            return yyjson_mut_real(document, value);
        }

        /// Builds yyjson's tree of a value. Strings are referenced, not
        /// copied: the tree is written before `value` can change.
        yyjson_mut_val *
        ToYyjson(yyjson_mut_doc *document, const JsonValue &value)
        {
            switch (value.GetKind())
            {
                case JsonValue::Kind::kNull:
                    return yyjson_mut_null(document);
                case JsonValue::Kind::kBoolean:
                    return yyjson_mut_bool(document, value.AsBoolean());
                case JsonValue::Kind::kNumber:
                    return ToYyjsonNumber(document, value.AsNumber());
                case JsonValue::Kind::kString:
                {
                    const std::string &text = value.AsString();
                    return yyjson_mut_strn(document, text.data(), text.size());
                }
                case JsonValue::Kind::kArray:
                {
                    yyjson_mut_val *array = yyjson_mut_arr(document);
                    for (const auto &item : value.Items())
                    {
                        yyjson_mut_arr_append(array, ToYyjson(document, item));
                    }
                    return array;
                }
                case JsonValue::Kind::kObject:
                {
                    yyjson_mut_val *object = yyjson_mut_obj(document);
                    for (const auto &[name, member] : value.Members())
                    {
                        yyjson_mut_obj_add(
                            object,
                            yyjson_mut_strn(document, name.data(), name.size()),
                            ToYyjson(document, member));
                    }
                    return object;
                }
            }
            return yyjson_mut_null(document);
        }

        std::string
        WriteJson(const JsonValue &value, yyjson_write_flag flags)
        {
            yyjson_mut_doc *document = yyjson_mut_doc_new(nullptr);
            if (document == nullptr)
            {
                return {};
            }
            yyjson_mut_doc_set_root(document, ToYyjson(document, value));
            std::size_t length = 0;
            char *text = yyjson_mut_write(document, flags, &length);
            std::string result =
                text == nullptr ? std::string{} : std::string{text, length};
            std::free(text);
            yyjson_mut_doc_free(document);
            return result;
        }
    } // namespace

    JsonValue::JsonValue(bool value)
        : kind_{Kind::kBoolean}
        , boolean_{value}
    {
    }

    JsonValue::JsonValue(double value)
        : kind_{Kind::kNumber}
        , number_{value}
    {
    }

    JsonValue::JsonValue(int value)
        : JsonValue{static_cast<double>(value)}
    {
    }

    JsonValue::JsonValue(unsigned value)
        : JsonValue{static_cast<double>(value)}
    {
    }

    JsonValue::JsonValue(long value)
        : JsonValue{static_cast<double>(value)}
    {
    }

    JsonValue::JsonValue(unsigned long value)
        : JsonValue{static_cast<double>(value)}
    {
    }

    JsonValue::JsonValue(long long value)
        : JsonValue{static_cast<double>(value)}
    {
    }

    JsonValue::JsonValue(unsigned long long value)
        : JsonValue{static_cast<double>(value)}
    {
    }

    JsonValue::JsonValue(const char *value)
        : kind_{Kind::kString}
        , string_{value == nullptr ? "" : value}
    {
    }

    JsonValue::JsonValue(std::string value)
        : kind_{Kind::kString}
        , string_{std::move(value)}
    {
    }

    JsonValue::JsonValue(std::string_view value)
        : kind_{Kind::kString}
        , string_{value}
    {
    }

    JsonValue::JsonValue(const JsonValue &other)
    {
        CopyFrom(other);
    }

    JsonValue::JsonValue(JsonValue &&other) noexcept
    {
        MoveFrom(std::move(other));
    }

    JsonValue &
    JsonValue::operator=(const JsonValue &other)
    {
        if (this != &other)
        {
            JsonValue copy{other};
            Destroy();
            MoveFrom(std::move(copy));
        }
        return *this;
    }

    JsonValue &
    JsonValue::operator=(JsonValue &&other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            MoveFrom(std::move(other));
        }
        return *this;
    }

    JsonValue::~JsonValue() = default;

    void
    JsonValue::Destroy() noexcept
    {
        kind_ = Kind::kNull;
        boolean_ = false;
        number_ = 0.0;
        string_.clear();
        items_.reset();
        members_.reset();
    }

    void
    JsonValue::CopyFrom(const JsonValue &other)
    {
        kind_ = other.kind_;
        boolean_ = other.boolean_;
        number_ = other.number_;
        string_ = other.string_;
        if (other.items_)
        {
            items_ = std::make_unique<std::vector<JsonValue>>(*other.items_);
        }
        if (other.members_)
        {
            members_ = std::make_unique<JsonMembers>(*other.members_);
        }
    }

    void
    JsonValue::MoveFrom(JsonValue &&other) noexcept
    {
        kind_ = other.kind_;
        boolean_ = other.boolean_;
        number_ = other.number_;
        string_ = std::move(other.string_);
        items_ = std::move(other.items_);
        members_ = std::move(other.members_);
        other.kind_ = Kind::kNull;
    }

    JsonValue
    JsonValue::Array(std::vector<JsonValue> items)
    {
        JsonValue value;
        value.kind_ = Kind::kArray;
        value.items_ = std::make_unique<std::vector<JsonValue>>(std::move(items));
        return value;
    }

    JsonValue
    JsonValue::Object(JsonMembers members)
    {
        JsonValue value;
        value.kind_ = Kind::kObject;
        value.members_ = std::make_unique<JsonMembers>(std::move(members));
        return value;
    }

    bool
    JsonValue::AsBoolean(bool fallback) const noexcept
    {
        return kind_ == Kind::kBoolean ? boolean_ : fallback;
    }

    double
    JsonValue::AsNumber(double fallback) const noexcept
    {
        return kind_ == Kind::kNumber ? number_ : fallback;
    }

    std::int64_t
    JsonValue::AsInteger(std::int64_t fallback) const noexcept
    {
        if (kind_ != Kind::kNumber || !std::isfinite(number_) ||
            number_ < -9.2e18 || number_ > 9.2e18)
        {
            return fallback;
        }
        return static_cast<std::int64_t>(number_);
    }

    std::uint64_t
    JsonValue::AsUnsigned(std::uint64_t fallback) const noexcept
    {
        if (kind_ != Kind::kNumber || !std::isfinite(number_) ||
            number_ < 0.0 || number_ > 1.8e19)
        {
            return fallback;
        }
        return static_cast<std::uint64_t>(number_);
    }

    const std::string &
    JsonValue::AsString() const noexcept
    {
        return kind_ == Kind::kString ? string_ : EmptyString();
    }

    const std::vector<JsonValue> &
    JsonValue::Items() const noexcept
    {
        return kind_ == Kind::kArray && items_ ? *items_ : EmptyItems();
    }

    std::vector<JsonValue> &
    JsonValue::MutableItems()
    {
        if (kind_ != Kind::kArray || !items_)
        {
            *this = Array();
        }
        return *items_;
    }

    const JsonMembers &
    JsonValue::Members() const noexcept
    {
        return kind_ == Kind::kObject && members_ ? *members_ : EmptyMembers();
    }

    JsonMembers &
    JsonValue::MutableMembers()
    {
        if (kind_ != Kind::kObject || !members_)
        {
            *this = Object();
        }
        return *members_;
    }

    const JsonValue &
    JsonValue::operator[](std::string_view key) const noexcept
    {
        for (const auto &[name, value] : Members())
        {
            if (name == key)
            {
                return value;
            }
        }
        return NullValue();
    }

    JsonValue &
    JsonValue::Set(std::string_view key, JsonValue value)
    {
        auto &members = MutableMembers();
        for (auto &[name, existing] : members)
        {
            if (name == key)
            {
                existing = std::move(value);
                return existing;
            }
        }
        members.emplace_back(std::string{key}, std::move(value));
        return members.back().second;
    }

    void
    JsonValue::Append(JsonValue value)
    {
        MutableItems().push_back(std::move(value));
    }

    std::string
    JsonValue::ToString() const
    {
        return WriteJson(*this, kWriteFlags);
    }

    std::string
    JsonValue::ToPrettyString() const
    {
        std::string output =
            WriteJson(*this, kWriteFlags | YYJSON_WRITE_PRETTY_TWO_SPACES);
        output.push_back('\n');
        return output;
    }

    Expected<JsonValue>
    ParseJson(std::string_view text)
    {
        yyjson_read_err error{};
        // Without YYJSON_READ_INSITU the text is only read.
        yyjson_doc *document = yyjson_read_opts(const_cast<char *>(text.data()),
                                                text.size(),
                                                kReadFlags,
                                                nullptr,
                                                &error);
        if (document == nullptr)
        {
            return Error{"invalid JSON at offset " + std::to_string(error.pos) +
                         ": " + (error.msg == nullptr ? "" : error.msg)};
        }
        JsonValue result;
        const bool converted =
            FromYyjson(yyjson_doc_get_root(document), 0, result);
        yyjson_doc_free(document);
        if (!converted)
        {
            return Error{"invalid JSON: nested deeper than " +
                         std::to_string(kMaximumDepth) + " levels"};
        }
        return result;
    }

    void
    AppendJsonString(std::string &output, std::string_view text)
    {
        output += WriteJson(JsonValue{text}, kWriteFlags);
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

#include <algorithm>
#include <clocale>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <limits>
#include <string_view>

namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        enum class Takes : std::uint8_t
        {
            kNothing,
            kValue,
            kOptionalValue,
        };

        using Apply =
            std::function<Status(Options &, const std::optional<std::string> &)>;

        /// One option: its spellings, what it takes and what it does.
        struct OptionSpec final
        {
            std::string_view name;
            char shortName = '\0';
            Takes takes = Takes::kNothing;
            std::string_view valueName;
            std::string_view group;
            std::string_view help;
            Apply apply;
        };

        std::optional<unsigned long long>
        ParseUnsigned(std::string_view text)
        {
            if (text.empty() || text.size() > 19)
            {
                return std::nullopt;
            }
            unsigned long long value = 0;
            for (const char character : text)
            {
                if (character < '0' || character > '9')
                {
                    return std::nullopt;
                }
                value = value * 10 + static_cast<unsigned>(character - '0');
            }
            return value;
        }

        std::optional<double>
        ParseDecimal(std::string_view text)
        {
            // Digits with at most one point: the format the runner writes.
            bool point = false;
            bool digit = false;
            for (const char character : text)
            {
                if (character == '.' && !point)
                {
                    point = true;
                    continue;
                }
                if (character < '0' || character > '9')
                {
                    return std::nullopt;
                }
                digit = true;
            }
            if (!digit || text.size() > 64)
            {
                return std::nullopt;
            }
            // strtod rounds correctly, in the decimal separator of the
            // current locale, which a global constructor may have changed.
            std::string copy{text};
            const auto at = copy.find('.');
            if (at != std::string::npos)
            {
                const char *separator = std::localeconv()->decimal_point;
                copy.replace(at,
                             1,
                             separator == nullptr || *separator == '\0'
                                 ? "."
                                 : separator);
            }
            return std::strtod(copy.c_str(), nullptr);
        }

        Status
        ReadCount(const std::optional<std::string> &value,
                  std::string_view option,
                  unsigned &target,
                  unsigned minimum = 0)
        {
            const auto parsed = ParseUnsigned(value.value_or(""));
            if (!parsed || *parsed > std::numeric_limits<unsigned>::max() ||
                *parsed < minimum)
            {
                return Error{"--" + std::string{option} +
                             " takes a whole number" +
                             (minimum > 0 ? " of at least " +
                                                std::to_string(minimum)
                                          : std::string{}) +
                             ", not '" + value.value_or("") + "'"};
            }
            target = static_cast<unsigned>(*parsed);
            return Status::Ok();
        }

        template<typename Value>
        Status
        ReadChoice(const std::optional<std::string> &value,
                   std::string_view option,
                   std::initializer_list<std::pair<std::string_view, Value>>
                       choices,
                   Value &target)
        {
            std::string names;
            for (const auto &[name, choice] : choices)
            {
                if (value && *value == name)
                {
                    target = choice;
                    return Status::Ok();
                }
                names += names.empty() ? "" : ", ";
                names += name;
            }
            return Error{"--" + std::string{option} + " takes one of " +
                         names + ", not '" + value.value_or("") + "'"};
        }

        const std::vector<OptionSpec> &
        Specs()
        {
            static const auto *specs = new std::vector<OptionSpec>{
                // Selecting tests.
                {"section",
                 'c',
                 Takes::kValue,
                 "NAME",
                 "Selecting tests",
                 "run only this section; repeat for nested sections",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.sections.push_back(*value);
                     return Status::Ok();
                 }},
                {"generator-index",
                 'g',
                 Takes::kValue,
                 "INDEX",
                 "Selecting tests",
                 "run one value of the next generator, or * for all; the same "
                 "as --path-filter=g:INDEX",
                 [](Options &options, const std::optional<std::string> &value) {
                     if (*value != "*" && !ParseUnsigned(*value))
                     {
                         return Status{Error{"--generator-index takes an index "
                                             "or *, not '" +
                                             *value + "'"}};
                     }
                     options.paths.push_back("g:" + *value);
                     return Status::Ok();
                 }},
                {"path-filter",
                 'p',
                 Takes::kValue,
                 "c:SECTION|g:INDEX",
                 "Selecting tests",
                 "run one path through sections and generators; repeat for "
                 "each level",
                 [](Options &options, const std::optional<std::string> &value) {
                     const bool section = value->rfind("c:", 0) == 0;
                     const bool generator = value->rfind("g:", 0) == 0;
                     if (value->size() < 3 || (!section && !generator))
                     {
                         return Status{Error{
                             "--path-filter takes c:SECTION or g:INDEX, not '" +
                             *value + "'"}};
                     }
                     if (generator && value->substr(2) != "*" &&
                         !ParseUnsigned(value->substr(2)))
                     {
                         return Status{Error{"--path-filter g: takes a generator "
                                             "index or *, not '" +
                                             value->substr(2) + "'"}};
                     }
                     options.paths.push_back(*value);
                     return Status::Ok();
                 }},
                {"input-file",
                 'f',
                 Takes::kValue,
                 "FILE",
                 "Selecting tests",
                 "run the tests named in FILE, one exact name per line; "
                 "lines starting with # are comments",
                 [](Options &options, const std::optional<std::string> &value) {
                     std::ifstream file(*value);
                     if (!file)
                     {
                         return Status{Error{"cannot read '" + *value + "'"}};
                     }
                     std::string line;
                     bool first = true;
                     while (std::getline(file, line))
                     {
                         while (!line.empty() &&
                                (line.back() == '\r' || line.back() == ' ' ||
                                 line.back() == '\t'))
                         {
                             line.pop_back();
                         }
                         const auto start = line.find_first_not_of(" \t");
                         line.erase(0, start == std::string::npos ? line.size()
                                                                  : start);
                         if (line.empty() || line.front() == '#')
                         {
                             continue;
                         }
                         // Quoted, a line is one exact test name; a comma
                         // between specs means "or".
                         if (line.front() != '"')
                         {
                             line = '"' + line + '"';
                         }
                         if (!first)
                         {
                             options.testSpecs.emplace_back(",");
                         }
                         options.testSpecs.push_back(line);
                         first = false;
                     }
                     return Status::Ok();
                 }},
                {"select",
                 '\0',
                 Takes::kValue,
                 "WHICH",
                 "Selecting tests",
                 "all (default), failed, changed, new or smart",
                 [](Options &options, const std::optional<std::string> &value) {
                     return ReadChoice<Selection>(
                         value,
                         "select",
                         {{"all", Selection::kAll},
                          {"failed", Selection::kFailed},
                          {"changed", Selection::kChanged},
                          {"new", Selection::kNew},
                          {"smart", Selection::kSmart}},
                         options.selection);
                 }},
                {"changes-since",
                 '\0',
                 Takes::kValue,
                 "REVISION",
                 "Selecting tests",
                 "the Git revision changes are counted from (default HEAD)",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.changesSince = *value;
                     return Status::Ok();
                 }},
                {"incremental",
                 '\0',
                 Takes::kNothing,
                 "",
                 "Selecting tests",
                 "skip tests that passed last time and whose inputs are "
                 "unchanged",
                 [](Options &options, const std::optional<std::string> &) {
                     options.incremental = true;
                     return Status::Ok();
                 }},
                {"shard",
                 '\0',
                 Takes::kValue,
                 "INDEX/COUNT",
                 "Selecting tests",
                 "run one of COUNT shards, counting from 0",
                 [](Options &options, const std::optional<std::string> &value) {
                     const auto slash = value->find('/');
                     const auto index =
                         ParseUnsigned(std::string_view{*value}.substr(
                             0,
                             slash == std::string::npos ? 0 : slash));
                     const auto count = slash == std::string::npos
                                            ? std::nullopt
                                            : ParseUnsigned(
                                                  std::string_view{*value}
                                                      .substr(slash + 1));
                     if (!index || !count || *count == 0 || *index >= *count ||
                         *count > std::numeric_limits<unsigned>::max())
                     {
                         return Status{Error{
                             "--shard takes INDEX/COUNT with INDEX below "
                             "COUNT, such as 0/4, not '" +
                             *value + "'"}};
                     }
                     options.shard.index = static_cast<unsigned>(*index);
                     options.shard.count = static_cast<unsigned>(*count);
                     return Status::Ok();
                 }},
                {"shard-durations",
                 '\0',
                 Takes::kValue,
                 "FILE",
                 "Selecting tests",
                 "balance shards by the durations in a JSON report or history",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.shardDurations = *value;
                     return Status::Ok();
                 }},
                {"filenames-as-tags",
                 '#',
                 Takes::kNothing,
                 "",
                 "Selecting tests",
                 "tag every test with the name of its file",
                 [](Options &options, const std::optional<std::string> &) {
                     options.filenamesAsTags = true;
                     return Status::Ok();
                 }},
                // Running.
                {"jobs",
                 'j',
                 Takes::kValue,
                 "N",
                 "Running",
                 "worker processes; 0, the default, uses one per hardware "
                 "thread",
                 [](Options &options, const std::optional<std::string> &value) {
                     return ReadCount(value, "jobs", options.jobs);
                 }},
                {"in-process",
                 '\0',
                 Takes::kNothing,
                 "",
                 "Running",
                 "run the tests in this process, one after another",
                 [](Options &options, const std::optional<std::string> &) {
                     options.inProcess = true;
                     return Status::Ok();
                 }},
                {"isolate",
                 '\0',
                 Takes::kNothing,
                 "",
                 "Running",
                 "start a fresh process for every test",
                 [](Options &options, const std::optional<std::string> &) {
                     options.isolate = true;
                     return Status::Ok();
                 }},
                {"pass-env",
                 '\0',
                 Takes::kValue,
                 "NAME",
                 "Running",
                 "give the tests the variable NAME, which they do not get "
                 "because its name looks like a secret (repeatable)",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.passedEnvironment.push_back(*value);
                     return Status::Ok();
                 }},
                {"timeout",
                 '\0',
                 Takes::kValue,
                 "SECONDS",
                 "Running",
                 "the time limit of one test; 0 for none (default: learned "
                 "from history, 300 for an unknown test)",
                 [](Options &options, const std::optional<std::string> &value) {
                     const auto seconds = ParseDecimal(*value);
                     if (!seconds || *seconds > 1e7)
                     {
                         return Status{Error{"--timeout takes seconds, such "
                                             "as 30 or 0.5, not '" +
                                             *value + "'"}};
                     }
                     options.timeout = std::chrono::milliseconds{
                         static_cast<long long>(std::llround(*seconds * 1000))};
                     options.timeoutGiven = true;
                     return Status::Ok();
                 }},
                {"no-adaptive-timeout",
                 '\0',
                 Takes::kNothing,
                 "",
                 "Running",
                 "use --timeout for every test instead of learning limits",
                 [](Options &options, const std::optional<std::string> &) {
                     options.adaptiveTimeout = false;
                     return Status::Ok();
                 }},
                {"retries",
                 '\0',
                 Takes::kValue,
                 "N|auto|off",
                 "Running",
                 "retry failures: auto (default) retries up to 2 times what "
                 "may pass again, N retries every failure",
                 [](Options &options, const std::optional<std::string> &value) {
                     if (*value == "auto")
                     {
                         options.retryMode = RetryMode::kAuto;
                         return Status::Ok();
                     }
                     if (*value == "off")
                     {
                         options.retryMode = RetryMode::kOff;
                         options.retries = 0;
                         return Status::Ok();
                     }
                     options.retryMode = RetryMode::kAlways;
                     return ReadCount(value, "retries", options.retries);
                 }},
                {"repeat",
                 '\0',
                 Takes::kValue,
                 "N",
                 "Running",
                 "run every test N times, to find flaky tests",
                 [](Options &options, const std::optional<std::string> &value) {
                     return ReadCount(value, "repeat", options.repeat, 1);
                 }},
                {"fail-fast",
                 'a',
                 Takes::kOptionalValue,
                 "N",
                 "Running",
                 "stop starting tests after N failed tests (default 1)",
                 [](Options &options, const std::optional<std::string> &value) {
                     if (!value)
                     {
                         options.failFast = 1;
                         return Status::Ok();
                     }
                     return ReadCount(value, "fail-fast", options.failFast, 1);
                 }},
                {"order",
                 '\0',
                 Takes::kValue,
                 "ORDER",
                 "Running",
                 "priority (default), declared, name or random",
                 [](Options &options, const std::optional<std::string> &value) {
                     return ReadChoice<Order>(
                         value,
                         "order",
                         {{"priority", Order::kPriority},
                          {"declared", Order::kDeclared},
                          {"name", Order::kName},
                          {"random", Order::kRandom}},
                         options.order);
                 }},
                {"seed",
                 '\0',
                 Takes::kValue,
                 "N",
                 "Running",
                 "the random seed of the run (default: a new one each run)",
                 [](Options &options, const std::optional<std::string> &value) {
                     if (*value == "random")
                     {
                         options.seedGiven = false;
                         return Status::Ok();
                     }
                     const auto seed = ParseUnsigned(*value);
                     if (!seed || *seed > 0xFFFFFFFFULL)
                     {
                         return Status{Error{
                             "--seed takes a number below 2^32 or 'random', "
                             "not '" +
                             *value + "'"}};
                     }
                     options.seed = static_cast<std::uint32_t>(*seed);
                     options.seedGiven = true;
                     return Status::Ok();
                 }},
                {"no-bisect",
                 '\0',
                 Takes::kNothing,
                 "",
                 "Running",
                 "do not search for the test that leaves state behind when a "
                 "failure depends on the order",
                 [](Options &options, const std::optional<std::string> &) {
                     options.bisectOrder = false;
                     return Status::Ok();
                 }},
                // How tests behave.
                {"success",
                 's',
                 Takes::kNothing,
                 "",
                 "Test behaviour",
                 "report passing assertions too",
                 [](Options &options, const std::optional<std::string> &) {
                     options.includeSuccessful = true;
                     return Status::Ok();
                 }},
                {"break",
                 'b',
                 Takes::kNothing,
                 "",
                 "Test behaviour",
                 "break into the debugger on a failure; runs in process",
                 [](Options &options, const std::optional<std::string> &) {
                     options.breakIntoDebugger = true;
                     options.inProcess = true;
                     return Status::Ok();
                 }},
                {"no-throw",
                 'e',
                 Takes::kNothing,
                 "",
                 "Test behaviour",
                 "skip the assertions that expect exceptions",
                 [](Options &options, const std::optional<std::string> &) {
                     options.noThrow = true;
                     return Status::Ok();
                 }},
                {"warn",
                 'w',
                 Takes::kValue,
                 "WHAT",
                 "Test behaviour",
                 "fail on: no-assertions, unmatched-specs or "
                 "infinite-generators",
                 [](Options &options, const std::optional<std::string> &value) {
                     if (*value == "no-assertions" || *value == "NoAssertions")
                     {
                         options.warnNoAssertions = true;
                     }
                     else if (*value == "unmatched-specs" ||
                              *value == "UnmatchedTestSpec")
                     {
                         options.warnUnmatchedSpecs = true;
                     }
                     else if (*value == "infinite-generators" ||
                              *value == "InfiniteGenerators")
                     {
                         options.warnInfiniteGenerators = true;
                     }
                     else
                     {
                         return Status{Error{
                             "--warn takes no-assertions, unmatched-specs or "
                             "infinite-generators, not '" +
                             *value + "'"}};
                     }
                     return Status::Ok();
                 }},
                {"allow-no-tests",
                 '\0',
                 Takes::kNothing,
                 "",
                 "Test behaviour",
                 "succeed when no test runs",
                 [](Options &options, const std::optional<std::string> &) {
                     options.allowNoTests = true;
                     return Status::Ok();
                 }},
                {"skip-benchmarks",
                 '\0',
                 Takes::kNothing,
                 "",
                 "Test behaviour",
                 "do not run BENCHMARK blocks",
                 [](Options &options, const std::optional<std::string> &) {
                     options.skipBenchmarks = true;
                     return Status::Ok();
                 }},
                {"benchmark-samples",
                 '\0',
                 Takes::kValue,
                 "N",
                 "Test behaviour",
                 "samples per benchmark (default 100)",
                 [](Options &options, const std::optional<std::string> &value) {
                     return ReadCount(
                         value, "benchmark-samples", options.benchmarkSamples, 1);
                 }},
                {"benchmark-resamples",
                 '\0',
                 Takes::kValue,
                 "N",
                 "Test behaviour",
                 "bootstrap resamples (default 100000)",
                 [](Options &options, const std::optional<std::string> &value) {
                     return ReadCount(value,
                                      "benchmark-resamples",
                                      options.benchmarkResamples,
                                      1);
                 }},
                {"benchmark-confidence-interval",
                 '\0',
                 Takes::kValue,
                 "X",
                 "Test behaviour",
                 "between 0 and 1 (default 0.95)",
                 [](Options &options, const std::optional<std::string> &value) {
                     const auto interval = ParseDecimal(*value);
                     if (!interval || *interval <= 0.0 || *interval >= 1.0)
                     {
                         return Status{
                             Error{"--benchmark-confidence-interval takes a "
                                   "number between 0 and 1, not '" +
                                   *value + "'"}};
                     }
                     options.benchmarkConfidenceInterval = *interval;
                     return Status::Ok();
                 }},
                {"benchmark-no-analysis",
                 '\0',
                 Takes::kNothing,
                 "",
                 "Test behaviour",
                 "report the mean of the samples without statistics",
                 [](Options &options, const std::optional<std::string> &) {
                     options.benchmarkNoAnalysis = true;
                     return Status::Ok();
                 }},
                {"benchmark-warmup-time",
                 '\0',
                 Takes::kValue,
                 "MILLISECONDS",
                 "Test behaviour",
                 "warm-up time per benchmark (default 100)",
                 [](Options &options, const std::optional<std::string> &value) {
                     unsigned milliseconds = 0;
                     auto status =
                         ReadCount(value, "benchmark-warmup-time", milliseconds);
                     options.benchmarkWarmupTime =
                         std::chrono::milliseconds{milliseconds};
                     return status;
                 }},
                // History and analysis.
                {"history",
                 '\0',
                 Takes::kValue,
                 "DIRECTORY",
                 "History and analysis",
                 "where results are remembered (default: .catch3 beside the "
                 "binary, or CATCH3_HISTORY)",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.historyDirectory = *value;
                     options.historyEnabled = true;
                     return Status::Ok();
                 }},
                {"no-history",
                 '\0',
                 Takes::kNothing,
                 "",
                 "History and analysis",
                 "neither read nor write the history",
                 [](Options &options, const std::optional<std::string> &) {
                     options.historyEnabled = false;
                     return Status::Ok();
                 }},
                {"quarantine",
                 '\0',
                 Takes::kValue,
                 "MODE",
                 "History and analysis",
                 "auto (default), report or off",
                 [](Options &options, const std::optional<std::string> &value) {
                     return ReadChoice<QuarantineMode>(
                         value,
                         "quarantine",
                         {{"auto", QuarantineMode::kAuto},
                          {"report", QuarantineMode::kReport},
                          {"off", QuarantineMode::kOff}},
                         options.quarantine);
                 }},
                {"replay",
                 '\0',
                 Takes::kNothing,
                 "",
                 "History and analysis",
                 "run last run's failures again, each in the order and with "
                 "the seed it failed with",
                 [](Options &options, const std::optional<std::string> &) {
                     options.replayFailures = true;
                     return Status::Ok();
                 }},
                {"import",
                 '\0',
                 Takes::kValue,
                 "FILE",
                 "Importing results",
                 "read a report of another framework instead of running tests "
                 "(repeatable)",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.mode = Mode::kImport;
                     options.importFiles.push_back(*value);
                     return Status::Ok();
                 }},
                {"import-format",
                 '\0',
                 Takes::kValue,
                 "FORMAT",
                 "Importing results",
                 "auto (default), catch3, junit, xunit, trx, tap or ctrf",
                 [](Options &options, const std::optional<std::string> &value) {
                     return ReadChoice<ImportFormat>(
                         value,
                         "import-format",
                         {{"auto", ImportFormat::kAuto},
                          {"catch3", ImportFormat::kCatch3},
                          {"junit", ImportFormat::kJUnit},
                          {"xunit", ImportFormat::kXUnit},
                          {"trx", ImportFormat::kTrx},
                          {"tap", ImportFormat::kTap},
                          {"ctrf", ImportFormat::kCtrf}},
                         options.importFormat);
                 }},
                {"import-name",
                 '\0',
                 Takes::kValue,
                 "NAME",
                 "Importing results",
                 "the history imported results go to (default: imported); "
                 "--analytics reads it too",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.importName = *value;
                     return Status::Ok();
                 }},
                {"import-revision",
                 '\0',
                 Takes::kValue,
                 "REVISION",
                 "Importing results",
                 "what the reports tested; a verdict that changes on one "
                 "revision counts as flaky",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.importRevision = *value;
                     return Status::Ok();
                 }},
                {"analytics",
                 '\0',
                 Takes::kOptionalValue,
                 "json",
                 "History and analysis",
                 "print what the history says about the tests and exit",
                 [](Options &options, const std::optional<std::string> &value) {
                     if (value && *value != "json")
                     {
                         return Status{Error{
                             "--analytics takes no value or 'json', not '" +
                             *value + "'"}};
                     }
                     options.mode = Mode::kAnalytics;
                     options.analyticsJson = value.has_value();
                     return Status::Ok();
                 }},
                // Output.
                {"verbose",
                 'v',
                 Takes::kNothing,
                 "",
                 "Output",
                 "print every test as it ends",
                 [](Options &options, const std::optional<std::string> &) {
                     options.verbosity = Verbosity::kVerbose;
                     return Status::Ok();
                 }},
                {"quiet",
                 'q',
                 Takes::kNothing,
                 "",
                 "Output",
                 "print only failures and the summary",
                 [](Options &options, const std::optional<std::string> &) {
                     options.verbosity = Verbosity::kQuiet;
                     return Status::Ok();
                 }},
                {"color",
                 '\0',
                 Takes::kValue,
                 "WHEN",
                 "Output",
                 "auto (default), always or never",
                 [](Options &options, const std::optional<std::string> &value) {
                     return ReadChoice<ColorMode>(
                         value,
                         "color",
                         {{"auto", ColorMode::kAuto},
                          {"always", ColorMode::kAlways},
                          {"never", ColorMode::kNever}},
                         options.color);
                 }},
                {"junit",
                 '\0',
                 Takes::kValue,
                 "FILE",
                 "Output",
                 "write a JUnit XML report",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.junitPath = *value;
                     return Status::Ok();
                 }},
                {"json",
                 '\0',
                 Takes::kValue,
                 "FILE",
                 "Output",
                 "write a JSON report with every attempt and its analysis",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.jsonPath = *value;
                     return Status::Ok();
                 }},
                {"trace",
                 '\0',
                 Takes::kValue,
                 "FILE",
                 "Output",
                 "write a trace of the run for chrome://tracing or Perfetto",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.tracePath = *value;
                     return Status::Ok();
                 }},
                {"premature-exit-guard-file",
                 '\0',
                 Takes::kValue,
                 "FILE",
                 "Output",
                 "create FILE when the run starts and remove it when the run "
                 "ends normally",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.exitGuardPath = *value;
                     return Status::Ok();
                 }},
                {"slowest",
                 '\0',
                 Takes::kOptionalValue,
                 "N",
                 "Output",
                 "list the N slowest tests (default 10)",
                 [](Options &options, const std::optional<std::string> &value) {
                     if (!value)
                     {
                         options.slowest = 10;
                         return Status::Ok();
                     }
                     return ReadCount(value, "slowest", options.slowest, 1);
                 }},
                // Configuration.
                {"config",
                 '\0',
                 Takes::kValue,
                 "FILE",
                 "Configuration",
                 "read this catch3.json instead of searching for one",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.configPath = *value;
                     options.noConfig = false;
                     return Status::Ok();
                 }},
                {"no-config",
                 '\0',
                 Takes::kNothing,
                 "",
                 "Configuration",
                 "read no catch3.json",
                 [](Options &options, const std::optional<std::string> &) {
                     options.noConfig = true;
                     options.configPath.clear();
                     return Status::Ok();
                 }},
                // Information.
                {"list",
                 'l',
                 Takes::kOptionalValue,
                 "FORMAT",
                 "Information",
                 "list the matching tests: text (default), names or json",
                 [](Options &options, const std::optional<std::string> &value) {
                     options.mode = Mode::kListTests;
                     if (!value)
                     {
                         options.listFormat = ListFormat::kText;
                         return Status::Ok();
                     }
                     return ReadChoice<ListFormat>(
                         value,
                         "list",
                         {{"text", ListFormat::kText},
                          {"names", ListFormat::kNames},
                          {"json", ListFormat::kJson}},
                         options.listFormat);
                 }},
                {"list-tags",
                 't',
                 Takes::kNothing,
                 "",
                 "Information",
                 "list the tags of the matching tests",
                 [](Options &options, const std::optional<std::string> &) {
                     options.mode = Mode::kListTags;
                     return Status::Ok();
                 }},
                {"help",
                 'h',
                 Takes::kNothing,
                 "",
                 "Information",
                 "print this help",
                 [](Options &options, const std::optional<std::string> &) {
                     options.mode = Mode::kHelp;
                     return Status::Ok();
                 }},
                {"version",
                 '\0',
                 Takes::kNothing,
                 "",
                 "Information",
                 "print the version of Catch3",
                 [](Options &options, const std::optional<std::string> &) {
                     options.mode = Mode::kVersion;
                     return Status::Ok();
                 }},
                // Internal.
                {"catch3-worker",
                 '\0',
                 Takes::kNothing,
                 "",
                 "",
                 "",
                 [](Options &options, const std::optional<std::string> &) {
                     options.mode = Mode::kWorker;
                     return Status::Ok();
                 }},
            };
            return *specs;
        }

        const OptionSpec *
        FindLong(std::string_view name)
        {
            for (const auto &spec : Specs())
            {
                if (spec.name == name)
                {
                    return &spec;
                }
            }
            return nullptr;
        }

        const OptionSpec *
        FindShort(char name)
        {
            for (const auto &spec : Specs())
            {
                if (spec.shortName != '\0' && spec.shortName == name)
                {
                    return &spec;
                }
            }
            return nullptr;
        }

        std::string
        UnknownOption(std::string_view written)
        {
            std::string message =
                "'" + std::string{written} + "' is not an option";
            std::string_view bare = written;
            while (!bare.empty() && bare.front() == '-')
            {
                bare.remove_prefix(1);
            }
            bare = bare.substr(0, bare.find('='));
            const OptionSpec *closest = nullptr;
            std::size_t best = 3;
            for (const auto &spec : Specs())
            {
                if (spec.group.empty())
                {
                    continue;
                }
                const std::size_t distance = EditDistance(bare, spec.name);
                if (distance < best)
                {
                    best = distance;
                    closest = &spec;
                }
            }
            if (closest != nullptr)
            {
                message += "; did you mean --" + std::string{closest->name} +
                           "?";
            }
            else
            {
                message += "; --help lists them";
            }
            return message;
        }
    } // namespace

    std::size_t
    EditDistance(std::string_view left, std::string_view right)
    {
        std::vector<std::size_t> previous(right.size() + 1);
        std::vector<std::size_t> current(right.size() + 1);
        for (std::size_t index = 0; index <= right.size(); ++index)
        {
            previous[index] = index;
        }
        for (std::size_t row = 1; row <= left.size(); ++row)
        {
            current[0] = row;
            for (std::size_t column = 1; column <= right.size(); ++column)
            {
                const std::size_t substitution =
                    previous[column - 1] +
                    (left[row - 1] == right[column - 1] ? 0 : 1);
                current[column] = std::min({previous[column] + 1,
                                            current[column - 1] + 1,
                                            substitution});
            }
            std::swap(previous, current);
        }
        return previous[right.size()];
    }

    Expected<Options>
    ParseOptions(const std::vector<std::string> &arguments, Options options)
    {
        bool onlySpecs = false;
        for (std::size_t index = 0; index < arguments.size(); ++index)
        {
            const std::string &argument = arguments[index];
            if (onlySpecs || argument.empty() || argument == "-" ||
                argument.front() != '-' ||
                (argument.size() > 1 && argument[1] == '[') ||
                (argument.size() > 1 && argument[1] == '~'))
            {
                options.testSpecs.push_back(argument);
                continue;
            }
            if (argument == "--")
            {
                onlySpecs = true;
                continue;
            }
            const OptionSpec *spec = nullptr;
            std::optional<std::string> value;
            std::string spelled;
            if (argument.rfind("--", 0) == 0)
            {
                const auto equals = argument.find('=');
                const std::string name = argument.substr(
                    2,
                    equals == std::string::npos ? std::string::npos
                                                : equals - 2);
                spec = FindLong(name);
                spelled = "--" + name;
                if (equals != std::string::npos)
                {
                    value = argument.substr(equals + 1);
                }
            }
            else
            {
                spec = FindShort(argument[1]);
                spelled = argument.substr(0, 2);
                if (argument.size() > 2)
                {
                    value = argument.substr(argument[2] == '=' ? 3 : 2);
                }
            }
            if (spec == nullptr)
            {
                return Error{UnknownOption(argument)};
            }
            if (spec->takes == Takes::kNothing && value)
            {
                return Error{spelled + " takes no value"};
            }
            if (spec->takes == Takes::kValue && !value)
            {
                // The value may also be the next argument: --jobs 4, -j 4.
                if (index + 1 >= arguments.size())
                {
                    return Error{spelled + " needs a value: " + spelled + "=" +
                                 std::string{spec->valueName}};
                }
                value = arguments[++index];
            }
            if (auto status = spec->apply(options, value); !status)
            {
                return status.GetError();
            }
        }
        return options;
    }

    std::string
    HelpText(const std::string &programName)
    {
        std::string text = "usage: " + programName +
                           " [options] [test specs...]\n\n"
                           "Test specs select tests by name, with * as a "
                           "wildcard, or by [tag];\n"
                           "~ excludes, a comma means \"or\". Hidden tests "
                           "run only when named.\n";
        std::string_view group;
        for (const auto &spec : Specs())
        {
            if (spec.group.empty())
            {
                continue;
            }
            if (spec.group != group)
            {
                group = spec.group;
                text += "\n" + std::string{group} + ":\n";
            }
            std::string left = "  ";
            if (spec.shortName != '\0')
            {
                left += "-" + std::string(1, spec.shortName) + ", ";
            }
            else
            {
                left += "    ";
            }
            left += "--" + std::string{spec.name};
            if (spec.takes == Takes::kValue)
            {
                left += "=" + std::string{spec.valueName};
            }
            else if (spec.takes == Takes::kOptionalValue)
            {
                left += "[=" + std::string{spec.valueName} + "]";
            }
            if (left.size() < 34)
            {
                left.append(34 - left.size(), ' ');
            }
            else
            {
                left += "\n" + std::string(34, ' ');
            }
            text += left + std::string{spec.help} + "\n";
        }
        return text;
    }

    std::vector<std::string>
    WorkerArguments(const Options &options)
    {
        std::vector<std::string> arguments{"--catch3-worker"};
        for (const auto &section : options.sections)
        {
            arguments.push_back("--section=" + section);
        }
        for (const auto &path : options.paths)
        {
            arguments.push_back("--path-filter=" + path);
        }
        if (options.filenamesAsTags)
        {
            arguments.emplace_back("--filenames-as-tags");
        }
        if (options.includeSuccessful)
        {
            arguments.emplace_back("--success");
        }
        if (options.noThrow)
        {
            arguments.emplace_back("--no-throw");
        }
        if (options.warnNoAssertions)
        {
            arguments.emplace_back("--warn=no-assertions");
        }
        if (options.warnInfiniteGenerators)
        {
            arguments.emplace_back("--warn=infinite-generators");
        }
        if (options.skipBenchmarks)
        {
            arguments.emplace_back("--skip-benchmarks");
        }
        if (options.benchmarkNoAnalysis)
        {
            arguments.emplace_back("--benchmark-no-analysis");
        }
        arguments.push_back("--benchmark-samples=" +
                            std::to_string(options.benchmarkSamples));
        arguments.push_back("--benchmark-resamples=" +
                            std::to_string(options.benchmarkResamples));
        arguments.push_back(
            "--benchmark-warmup-time=" +
            std::to_string(options.benchmarkWarmupTime.count()));
        // Written with enough digits to come back unchanged.
        char interval[64];
        std::snprintf(interval,
                      sizeof(interval),
                      "%.17f",
                      options.benchmarkConfidenceInterval);
        for (char &character : interval)
        {
            // The decimal separator of the locale a test may have set.
            if (character == ',')
            {
                character = '.';
            }
        }
        arguments.push_back(std::string{"--benchmark-confidence-interval="} +
                            interval);
        arguments.push_back("--seed=" + std::to_string(options.seed));
        return arguments;
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0


#include <algorithm>
#include <cstdio>
#include <functional>
#include <numeric>
#include <random>

namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        using std::chrono::milliseconds;

        /// The duration assumed for a test never seen before.
        constexpr std::int64_t kUnknownDurationMicroseconds = 500000;

        /// Durations by test key, from a history or from a JSON report, for
        /// balancing shards.
        Expected<std::map<std::string, std::int64_t>>
        LoadDurations(const std::string &path)
        {
            auto text = ReadFile(path);
            if (!text)
            {
                return text.GetError();
            }
            auto json = ParseJson(*text);
            if (!json)
            {
                return json.GetError().Context(path);
            }
            std::map<std::string, std::int64_t> durations;
            const std::string &format = (*json)["format"].AsString();
            if (format == "catch3-history")
            {
                auto history = History::FromJson(*json);
                if (!history)
                {
                    return history.GetError().Context(path);
                }
                for (const auto &[key, test] : history->Tests())
                {
                    if (const auto median = test.DurationPercentile(0.5))
                    {
                        durations[key] = *median;
                    }
                }
            }
            else if (format == "catch3-run")
            {
                for (const auto &test : (*json)["tests"].Items())
                {
                    const std::string key = test["id"].AsString().empty()
                                                ? test["name"].AsString()
                                                : test["id"].AsString();
                    durations[key] =
                        test["durationMicroseconds"].AsInteger();
                }
            }
            else
            {
                return Error{path + " is neither a Catch3 history nor a "
                                    "Catch3 JSON report"};
            }
            return durations;
        }

        std::string
        Percent(double probability)
        {
            char buffer[16];
            std::snprintf(buffer,
                          sizeof(buffer),
                          "%.0f%%",
                          probability * 100.0);
            return buffer;
        }

        /// Finds the root of a set in a union-find forest.
        std::size_t
        Root(std::vector<std::size_t> &parents, std::size_t index)
        {
            while (parents[index] != index)
            {
                parents[index] = parents[parents[index]];
                index = parents[index];
            }
            return index;
        }
    } // namespace

    unsigned
    StableShard(const std::string &key, unsigned count) noexcept
    {
        if (count <= 1)
        {
            return 0;
        }
        return static_cast<unsigned>(Fnv1a64(key) % count);
    }

    Expected<Plan>
    MakePlan(const Options &options, const PlanInputs &inputs)
    {
        const Catalog &catalog = *inputs.catalog;
        const auto &tests = catalog.Tests();
        const History *history = inputs.history;
        Plan plan;
        plan.unmatchedSpecs = inputs.unmatchedSpecs;
        plan.candidates = inputs.matched.size();

        const auto historyOf = [&](const TestEntry &entry) -> const TestHistory * {
            return history == nullptr ? nullptr : history->Find(entry.key);
        };

        std::vector<bool> selected(tests.size(), false);
        std::vector<std::string> reasons(tests.size());
        std::vector<std::optional<std::vector<std::string>>> replayKeys(
            tests.size());
        std::vector<std::optional<std::uint32_t>> replaySeeds(tests.size());

        // 1. Selection.
        if (options.replayFailures)
        {
            if (history != nullptr)
            {
                for (const auto &[key, test] : history->Tests())
                {
                    const TestEntry *entry = catalog.Find(key);
                    if (entry == nullptr || !test.lastFailure)
                    {
                        continue;
                    }
                    selected[entry->index] = true;
                    reasons[entry->index] =
                        "failed in run " + std::to_string(test.lastFailure->run);
                    replayKeys[entry->index] = test.lastFailure->precededBy;
                    replaySeeds[entry->index] = test.lastFailure->seed;
                }
            }
            if (std::none_of(selected.begin(), selected.end(), [](bool b) {
                    return b;
                }))
            {
                plan.notes.emplace_back(
                    "the history holds no failure to replay");
            }
        }
        else
        {
            const bool needsChanges =
                options.selection == Selection::kChanged ||
                options.selection == Selection::kSmart;
            if (needsChanges && inputs.changes == nullptr)
            {
                plan.notes.emplace_back(
                    "Git could not list the changed files, so no test is "
                    "selected for a change");
            }
            for (const std::size_t index : inputs.matched)
            {
                const TestEntry &entry = tests[index];
                const TestHistory *past = historyOf(entry);
                const TestRecord *last = past == nullptr ? nullptr : past->Last();
                const bool failedLast =
                    last != nullptr && (IsFailure(last->outcome) || last->flaky);
                const bool isNew = last == nullptr;
                bool changed = false;
                if (inputs.changes != nullptr)
                {
                    for (const auto &file : inputs.changes->files)
                    {
                        if (SameSourceFile(file, entry.file))
                        {
                            changed = true;
                            break;
                        }
                    }
                }
                const double probability =
                    past == nullptr ? 0.3 : past->FailureProbability();
                switch (options.selection)
                {
                    case Selection::kAll:
                        selected[index] = true;
                        break;
                    case Selection::kFailed:
                        if (failedLast)
                        {
                            selected[index] = true;
                            reasons[index] = last->flaky ? "flaky last run"
                                                         : "failed last run";
                        }
                        break;
                    case Selection::kNew:
                        if (isNew)
                        {
                            selected[index] = true;
                            reasons[index] = "no earlier result";
                        }
                        break;
                    case Selection::kChanged:
                        if (changed)
                        {
                            selected[index] = true;
                            reasons[index] = "its source file changed";
                        }
                        break;
                    case Selection::kSmart:
                        if (changed)
                        {
                            reasons[index] = "its source file changed";
                        }
                        else if (failedLast)
                        {
                            reasons[index] = "failed last run";
                        }
                        else if (isNew)
                        {
                            reasons[index] = "no earlier result";
                        }
                        else if (probability >= 0.5)
                        {
                            reasons[index] = "likely to fail (" +
                                             Percent(probability) + ")";
                        }
                        else if (past != nullptr && past->quarantine)
                        {
                            reasons[index] = "in quarantine";
                        }
                        selected[index] = !reasons[index].empty();
                        break;
                }
            }
        }

        // 2. Dependencies come along, whatever selected the test.
        std::vector<std::vector<std::size_t>> dependencies(tests.size());
        std::vector<std::size_t> pending;
        for (std::size_t index = 0; index < tests.size(); ++index)
        {
            if (selected[index])
            {
                pending.push_back(index);
            }
        }
        while (!pending.empty())
        {
            const std::size_t index = pending.back();
            pending.pop_back();
            for (const auto &name : tests[index].dependsOn)
            {
                const TestEntry *dependency = catalog.Find(name);
                if (dependency == nullptr)
                {
                    return Error{"test '" + tests[index].name +
                                 "' depends on '" + name +
                                 "', which is not a test of this binary"};
                }
                dependencies[index].push_back(dependency->index);
                if (!selected[dependency->index])
                {
                    selected[dependency->index] = true;
                    reasons[dependency->index] =
                        "needed by '" + tests[index].name + "'";
                    pending.push_back(dependency->index);
                }
            }
        }

        // 3. A cycle can never run.
        {
            std::vector<int> state(tests.size(), 0);
            std::vector<std::size_t> path;
            std::string cycle;
            const std::function<bool(std::size_t)> visit =
                [&](std::size_t index) {
                    state[index] = 1;
                    path.push_back(index);
                    for (const std::size_t next : dependencies[index])
                    {
                        if (state[next] == 1)
                        {
                            const auto start =
                                std::find(path.begin(), path.end(), next);
                            for (auto at = start; at != path.end(); ++at)
                            {
                                cycle += "'" + tests[*at].name + "' -> ";
                            }
                            cycle += "'" + tests[next].name + "'";
                            return true;
                        }
                        if (state[next] == 0 && visit(next))
                        {
                            return true;
                        }
                    }
                    path.pop_back();
                    state[index] = 2;
                    return false;
                };
            for (std::size_t index = 0; index < tests.size(); ++index)
            {
                if (selected[index] && state[index] == 0 && visit(index))
                {
                    return Error{"the tests depend on each other in a "
                                 "cycle: " +
                                 cycle};
                }
            }
        }

        // 4. Sharding. Tests linked by dependencies stay in one shard.
        if (options.shard.count > 1)
        {
            std::vector<std::size_t> parents(tests.size());
            std::iota(parents.begin(), parents.end(), std::size_t{0});
            for (std::size_t index = 0; index < tests.size(); ++index)
            {
                for (const std::size_t dependency : dependencies[index])
                {
                    parents[Root(parents, index)] = Root(parents, dependency);
                }
            }
            // A group is named by its first key, in registration order.
            std::map<std::size_t, std::vector<std::size_t>> groups;
            for (std::size_t index = 0; index < tests.size(); ++index)
            {
                if (selected[index])
                {
                    groups[Root(parents, index)].push_back(index);
                }
            }
            std::vector<unsigned> shardOf(tests.size(), 0);
            if (!options.shardDurations.empty())
            {
                auto durations = LoadDurations(options.shardDurations);
                if (!durations)
                {
                    return durations.GetError();
                }
                struct Group
                {
                    std::int64_t duration = 0;
                    std::string key;
                    const std::vector<std::size_t> *members = nullptr;
                };
                std::vector<Group> ordered;
                for (const auto &[root, members] : groups)
                {
                    Group group;
                    group.members = &members;
                    group.key = tests[members.front()].key;
                    for (const std::size_t member : members)
                    {
                        const auto found = durations->find(tests[member].key);
                        group.duration += found == durations->end()
                                              ? kUnknownDurationMicroseconds
                                              : found->second;
                    }
                    ordered.push_back(group);
                }
                // Longest first into the least loaded shard balances the
                // shards; the order is total, so every machine agrees.
                std::sort(ordered.begin(),
                          ordered.end(),
                          [](const Group &left, const Group &right) {
                              return left.duration != right.duration
                                         ? left.duration > right.duration
                                         : left.key < right.key;
                          });
                std::vector<std::int64_t> load(options.shard.count, 0);
                for (const auto &group : ordered)
                {
                    const auto least = static_cast<unsigned>(
                        std::min_element(load.begin(), load.end()) -
                        load.begin());
                    load[least] += group.duration;
                    for (const std::size_t member : *group.members)
                    {
                        shardOf[member] = least;
                    }
                }
            }
            else
            {
                for (const auto &[root, members] : groups)
                {
                    const unsigned shard = StableShard(
                        tests[members.front()].key, options.shard.count);
                    for (const std::size_t member : members)
                    {
                        shardOf[member] = shard;
                    }
                }
            }
            for (std::size_t index = 0; index < tests.size(); ++index)
            {
                if (selected[index] && shardOf[index] != options.shard.index)
                {
                    selected[index] = false;
                }
            }
        }

        // 5. Fingerprints, and what an incremental run may keep.
        std::vector<bool> kept(tests.size(), false);
        for (std::size_t index = 0; index < tests.size(); ++index)
        {
            if (!selected[index])
            {
                continue;
            }
            const TestEntry &entry = tests[index];
            const std::string fingerprint =
                Sha256Hex(inputs.binaryFingerprint + "\n" + entry.key);
            plan.fingerprints[entry.key] = fingerprint;
            if (!options.incremental || options.replayFailures)
            {
                continue;
            }
            const TestHistory *past = historyOf(entry);
            const TestRecord *last = past == nullptr ? nullptr : past->Last();
            if (last != nullptr && last->outcome == Outcome::kPassed &&
                !last->flaky && last->fingerprint == fingerprint &&
                !(past->quarantine && options.quarantine != QuarantineMode::kOff))
            {
                kept[index] = true;
                TestResult result;
                result.id = entry.key;
                result.name = entry.name;
                result.className = entry.className;
                result.file = entry.file;
                result.line = entry.line;
                result.tags = entry.tags;
                result.outcome = Outcome::kUpToDate;
                result.selectionReason = reasons[index];
                plan.kept.push_back(std::move(result));
            }
        }

        // 6. The tests to run, with their limits and predictions.
        std::vector<std::size_t> order;
        for (std::size_t index = 0; index < tests.size(); ++index)
        {
            if (selected[index] && !kept[index])
            {
                order.push_back(index);
            }
        }
        std::map<std::size_t, PlannedTest> planned;
        for (const std::size_t index : order)
        {
            const TestEntry &entry = tests[index];
            const TestHistory *past = historyOf(entry);
            PlannedTest test;
            test.entry = &entry;
            test.selectionReason = reasons[index];
            test.fingerprint = plan.fingerprints[entry.key];
            test.failureProbability =
                past == nullptr ? 0.3 : past->FailureProbability();
            test.flakyHistory = past != nullptr && past->FlakyEvents(20) > 0;
            const auto median =
                past == nullptr ? std::nullopt : past->DurationPercentile(0.5);
            test.predictedMicroseconds =
                median ? *median : kUnknownDurationMicroseconds;

            const milliseconds cap =
                options.timeoutGiven ? options.timeout : milliseconds{300000};
            if (entry.timeout)
            {
                test.timeout = *entry.timeout;
            }
            else if (options.adaptiveTimeout && !entry.benchmark && past &&
                     median)
            {
                // Generous enough for a slow machine, tight enough that a
                // hang is noticed long before the global limit.
                const auto p95 = past->DurationPercentile(0.95).value_or(*median);
                const auto learned = std::max(p95 * 4, *median * 10) / 1000 + 2000;
                test.timeout = milliseconds{std::max<std::int64_t>(5000, learned)};
                if (cap.count() > 0 && test.timeout > cap)
                {
                    test.timeout = cap;
                }
                test.timeoutLearned = true;
            }
            else
            {
                test.timeout = cap;
            }

            if (entry.retries)
            {
                test.retryLimit = *entry.retries;
            }
            else
            {
                test.retryLimit = options.retryMode == RetryMode::kOff
                                      ? 0
                                      : options.retries;
            }
            if (options.quarantine != QuarantineMode::kOff)
            {
                if (entry.quarantined)
                {
                    test.quarantined = true;
                    test.quarantineReason = "tagged [!quarantine]";
                }
                else if (past != nullptr && past->quarantine)
                {
                    test.quarantined = true;
                    test.quarantineReason = past->quarantine->reason;
                }
            }
            if (replaySeeds[index])
            {
                test.replaySeed = replaySeeds[index];
            }
            planned.emplace(index, std::move(test));
        }

        // 7. The order the scheduler prefers.
        switch (options.order)
        {
            case Order::kDeclared:
                break;
            case Order::kName:
                std::stable_sort(order.begin(),
                                 order.end(),
                                 [&](std::size_t left, std::size_t right) {
                                     return tests[left].name < tests[right].name;
                                 });
                break;
            case Order::kRandom:
            {
                std::mt19937 generator{options.seed};
                std::shuffle(order.begin(), order.end(), generator);
                break;
            }
            case Order::kPriority:
            {
                // Tests likely to fail go first, the most failures per second
                // first, so a failing run fails early. The rest go longest
                // first, which keeps the workers busy until the end.
                const auto risky = [&](std::size_t index) {
                    return planned.at(index).failureProbability >= 0.5;
                };
                const auto rate = [&](std::size_t index) {
                    const auto &test = planned.at(index);
                    const double seconds = std::max(
                        0.05,
                        static_cast<double>(test.predictedMicroseconds) / 1e6);
                    return test.failureProbability / seconds;
                };
                std::stable_sort(
                    order.begin(),
                    order.end(),
                    [&](std::size_t left, std::size_t right) {
                        if (risky(left) != risky(right))
                        {
                            return risky(left);
                        }
                        if (risky(left))
                        {
                            return rate(left) > rate(right);
                        }
                        return planned.at(left).predictedMicroseconds >
                               planned.at(right).predictedMicroseconds;
                    });
                break;
            }
        }
        std::map<std::size_t, std::size_t> position;
        for (std::size_t at = 0; at < order.size(); ++at)
        {
            position[order[at]] = at;
        }
        std::map<std::string, std::size_t> indexByKey;
        for (const auto &entry : tests)
        {
            indexByKey[entry.key] = entry.index;
        }
        for (const std::size_t index : order)
        {
            PlannedTest test = std::move(planned.at(index));
            for (const std::size_t dependency : dependencies[index])
            {
                // A dependency kept as up to date has already passed.
                if (const auto found = position.find(dependency);
                    found != position.end())
                {
                    test.dependencies.push_back(found->second);
                }
            }
            if (replayKeys[index])
            {
                std::vector<std::size_t> sequence;
                for (const auto &key : *replayKeys[index])
                {
                    if (const auto found = indexByKey.find(key);
                        found != indexByKey.end())
                    {
                        sequence.push_back(found->second);
                    }
                }
                test.replaySequence = std::move(sequence);
            }
            plan.tests.push_back(std::move(test));
        }
        return plan;
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string_view>
#include <thread>


#if defined(_WIN32)
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
#    include <fcntl.h>
#    include <io.h>
#    include <share.h>
#    include <sys/stat.h>
#    include <windows.h>
#else
#    include <cerrno>
#    include <csignal>
#    include <fcntl.h>
#    include <spawn.h>
#    include <sys/types.h>
#    include <sys/wait.h>
#    include <unistd.h>
#    if defined(__APPLE__)
#        include <crt_externs.h>
#        include <mach-o/dyld.h>
#    endif
#    if !defined(__APPLE__) && !(defined(__GLIBC__) && defined(_GNU_SOURCE))
extern char **environ;
#    endif
#endif

namespace Progmasoft::Catch3::Runner
{
    bool
    IsSecretVariable(std::string_view name)
    {
        std::string upper;
        upper.reserve(name.size());
        for (const char character : name)
        {
            upper.push_back(character >= 'a' && character <= 'z'
                                ? static_cast<char>(character - 'a' + 'A')
                                : character);
        }
        constexpr std::string_view kSecretWords[] = {
            "TOKEN",   "SECRET", "PASSWORD",    "PASSWD",     "CREDENTIAL",
            "API_KEY", "APIKEY", "PRIVATE_KEY", "ACCESS_KEY",
        };
        return std::any_of(std::begin(kSecretWords),
                           std::end(kSecretWords),
                           [&](std::string_view word) {
                               return upper.find(word) != std::string::npos;
                           });
    }

    namespace
    {
        /// Whether a variable of this process stays out of a child: a secret
        /// that the options do not keep. Windows spells names in any case.
        bool
        Withheld(std::string_view name, const std::vector<std::string> &kept)
        {
            if (!IsSecretVariable(name))
            {
                return false;
            }
            return std::none_of(
                kept.begin(),
                kept.end(),
                [&](const std::string &keptName) {
                    if (keptName.size() != name.size())
                    {
                        return false;
                    }
#if defined(_WIN32)
                    return std::equal(
                        keptName.begin(),
                        keptName.end(),
                        name.begin(),
                        [](char left, char right) {
                            return std::toupper(
                                       static_cast<unsigned char>(left))
                                   == std::toupper(
                                       static_cast<unsigned char>(right));
                        });
#else
                    return keptName == name;
#endif
                });
        }

#if defined(_WIN32)
        HANDLE
        AsHandle(std::intptr_t value) noexcept
        {
            return reinterpret_cast<HANDLE>(value);
        }

        std::intptr_t
        FromHandle(HANDLE handle) noexcept
        {
            return reinterpret_cast<std::intptr_t>(handle);
        }

        std::wstring
        Widen(std::string_view text)
        {
            if (text.empty())
            {
                return {};
            }
            const int size = MultiByteToWideChar(CP_UTF8,
                                                 0,
                                                 text.data(),
                                                 static_cast<int>(text.size()),
                                                 nullptr,
                                                 0);
            std::wstring wide(static_cast<std::size_t>(size), L'\0');
            MultiByteToWideChar(CP_UTF8,
                                0,
                                text.data(),
                                static_cast<int>(text.size()),
                                wide.data(),
                                size);
            return wide;
        }

        std::string
        Narrow(std::wstring_view text)
        {
            if (text.empty())
            {
                return {};
            }
            const int size = WideCharToMultiByte(CP_UTF8,
                                                 0,
                                                 text.data(),
                                                 static_cast<int>(text.size()),
                                                 nullptr,
                                                 0,
                                                 nullptr,
                                                 nullptr);
            std::string narrow(static_cast<std::size_t>(size), '\0');
            WideCharToMultiByte(CP_UTF8,
                                0,
                                text.data(),
                                static_cast<int>(text.size()),
                                narrow.data(),
                                size,
                                nullptr,
                                nullptr);
            return narrow;
        }

        std::string
        LastErrorMessage(const char *what)
        {
            const DWORD code = GetLastError();
            wchar_t *buffer = nullptr;
            const DWORD length = FormatMessageW(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                    | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                code,
                0,
                reinterpret_cast<wchar_t *>(&buffer),
                0,
                nullptr);
            std::string message
                = length > 0 ? Narrow(std::wstring_view{ buffer, length })
                             : std::string{};
            if (buffer != nullptr)
            {
                LocalFree(buffer);
            }
            while (!message.empty()
                   && (message.back() == '\n' || message.back() == '\r'
                       || message.back() == ' ' || message.back() == '.'))
            {
                message.pop_back();
            }
            return std::string{ what } + ": " + message + " (error "
                   + std::to_string(code) + ")";
        }

        /// Quotes one argument the way the Microsoft C runtime splits a
        /// command line back into `argv`.
        void
        AppendQuotedArgument(std::wstring &commandLine,
                             const std::wstring &argument)
        {
            const bool needsQuotes
                = argument.empty()
                  || argument.find_first_of(L" \t\n\v\"") != std::wstring::npos;
            if (!needsQuotes)
            {
                commandLine += argument;
                return;
            }
            commandLine.push_back(L'"');
            std::size_t backslashes = 0;
            for (const wchar_t character : argument)
            {
                if (character == L'\\')
                {
                    ++backslashes;
                    continue;
                }
                if (character == L'"')
                {
                    commandLine.append(backslashes * 2 + 1, L'\\');
                }
                else
                {
                    commandLine.append(backslashes, L'\\');
                }
                backslashes = 0;
                commandLine.push_back(character);
            }
            commandLine.append(backslashes * 2, L'\\');
            commandLine.push_back(L'"');
        }

        std::wstring
        EnvironmentBlock(
            const std::vector<std::pair<std::string, std::string>> &additions,
            const std::vector<std::string> &kept)
        {
            std::vector<std::wstring> entries;
            wchar_t *current = GetEnvironmentStringsW();
            if (current != nullptr)
            {
                for (const wchar_t *entry = current; *entry != L'\0';
                     entry += wcslen(entry) + 1)
                {
                    // A leading '=' marks the hidden per-drive variables.
                    const std::wstring_view text{ entry };
                    const auto equals = text.find(L'=', 1);
                    if (Withheld(Narrow(text.substr(0, equals)), kept))
                    {
                        continue;
                    }
                    entries.emplace_back(entry);
                }
                FreeEnvironmentStringsW(current);
            }
            for (const auto &[name, value] : additions)
            {
                const std::wstring wideName = Widen(name);
                const auto removed = std::remove_if(
                    entries.begin(),
                    entries.end(),
                    [&](const std::wstring &entry) {
                        // A leading '=' marks the hidden per-drive variables.
                        const auto equals = entry.find(L'=', 1);
                        return equals == wideName.size()
                               && CompareStringOrdinal(
                                      entry.data(),
                                      static_cast<int>(equals),
                                      wideName.data(),
                                      static_cast<int>(wideName.size()),
                                      TRUE)
                                      == CSTR_EQUAL;
                    });
                entries.erase(removed, entries.end());
                entries.push_back(wideName + L"=" + Widen(value));
            }
            std::wstring block;
            for (const auto &entry : entries)
            {
                block += entry;
                block.push_back(L'\0');
            }
            // A block ends in an empty entry; one without variables is two
            // terminators.
            if (entries.empty())
            {
                block.push_back(L'\0');
            }
            block.push_back(L'\0');
            return block;
        }
#else
        int
        Descriptor(std::intptr_t value) noexcept
        {
            return static_cast<int>(value);
        }

        char **
        CurrentEnvironment() noexcept
        {
#    if defined(__APPLE__)
            return *_NSGetEnviron();
#    else
            return environ;
#    endif
        }

        std::string
        ErrnoMessage(const char *what, int error)
        {
            return std::string{ what } + ": " + std::strerror(error);
        }

        bool
        MakePipe(int (&descriptors)[2])
        {
#    if defined(__linux__)
            return pipe2(descriptors, O_CLOEXEC) == 0;
#    else
            // Only the scheduler thread starts processes, so no other
            // thread can fork between creating the pipe and marking it.
            if (pipe(descriptors) != 0)
            {
                return false;
            }
            fcntl(descriptors[0], F_SETFD, FD_CLOEXEC);
            fcntl(descriptors[1], F_SETFD, FD_CLOEXEC);
            return true;
#    endif
        }

        const char *
        SignalName(int signal) noexcept
        {
            switch (signal)
            {
                case SIGSEGV:
                    return "SIGSEGV";
                case SIGABRT:
                    return "SIGABRT";
                case SIGBUS:
                    return "SIGBUS";
                case SIGFPE:
                    return "SIGFPE";
                case SIGILL:
                    return "SIGILL";
                case SIGKILL:
                    return "SIGKILL";
                case SIGTERM:
                    return "SIGTERM";
                case SIGPIPE:
                    return "SIGPIPE";
                case SIGINT:
                    return "SIGINT";
                default:
                    return nullptr;
            }
        }
#endif

#if defined(_WIN32)
        const char *
        ExceptionName(std::int64_t code) noexcept
        {
            switch (static_cast<DWORD>(code))
            {
                case 0xC0000005:
                    return "access violation";
                case 0xC00000FD:
                    return "stack overflow";
                case 0xC0000094:
                    return "integer division by zero";
                case 0xC000008E:
                    return "floating-point division by zero";
                case 0xC000001D:
                    return "illegal instruction";
                case 0xC0000409:
                    return "stack buffer overrun or fast fail";
                case 0xC0000374:
                    return "heap corruption";
                case 0x80000003:
                    return "breakpoint";
                case 0xE06D7363:
                    return "unhandled C++ exception";
                default:
                    return nullptr;
            }
        }
#endif
    } // namespace

    std::string
    ExitStatus::Describe() const
    {
        switch (kind)
        {
            case Kind::kExited:
                return "exit code " + std::to_string(code);
            case Kind::kSignaled:
            {
                std::string text = "signal " + std::to_string(code);
#if !defined(_WIN32)
                if (const char *name = SignalName(static_cast<int>(code)))
                {
                    text += std::string{ " (" } + name + ")";
                }
#endif
                return text;
            }
            case Kind::kException:
            {
                char buffer[32];
                std::snprintf(buffer,
                              sizeof(buffer),
                              "0x%08llX",
                              static_cast<unsigned long long>(code));
                std::string text = std::string{ "exception " } + buffer;
#if defined(_WIN32)
                if (const char *name = ExceptionName(code))
                {
                    text += std::string{ " (" } + name + ")";
                }
#endif
                return text;
            }
        }
        return "unknown status";
    }

    PipeReader::PipeReader(PipeReader &&other) noexcept
        : handle_{ other.handle_ }
    {
        other.handle_ = -1;
    }

    PipeReader &
    PipeReader::operator=(PipeReader &&other) noexcept
    {
        if (this != &other)
        {
            Close();
            handle_ = other.handle_;
            other.handle_ = -1;
        }
        return *this;
    }

    PipeReader::~PipeReader()
    {
        Close();
    }

    bool
    PipeReader::IsOpen() const noexcept
    {
        return handle_ != -1;
    }

    void
    PipeReader::Close() noexcept
    {
        if (handle_ == -1)
        {
            return;
        }
#if defined(_WIN32)
        CloseHandle(AsHandle(handle_));
#else
        close(Descriptor(handle_));
#endif
        handle_ = -1;
    }

    std::int64_t
    PipeReader::Read(char *buffer, std::size_t size) noexcept
    {
        if (handle_ == -1)
        {
            return -1;
        }
#if defined(_WIN32)
        DWORD read = 0;
        const DWORD request
            = size > 0x10000000 ? 0x10000000 : static_cast<DWORD>(size);
        if (!::ReadFile(AsHandle(handle_), buffer, request, &read, nullptr))
        {
            const DWORD error = GetLastError();
            return error == ERROR_BROKEN_PIPE || error == ERROR_NO_DATA ? 0
                                                                        : -1;
        }
        return static_cast<std::int64_t>(read);
#else
        while (true)
        {
            const auto count = read(Descriptor(handle_), buffer, size);
            if (count >= 0)
            {
                return static_cast<std::int64_t>(count);
            }
            if (errno != EINTR)
            {
                return -1;
            }
        }
#endif
    }

    PipeWriter::PipeWriter(PipeWriter &&other) noexcept
        : handle_{ other.handle_ }
    {
        other.handle_ = -1;
    }

    PipeWriter &
    PipeWriter::operator=(PipeWriter &&other) noexcept
    {
        if (this != &other)
        {
            Close();
            handle_ = other.handle_;
            other.handle_ = -1;
        }
        return *this;
    }

    PipeWriter::~PipeWriter()
    {
        Close();
    }

    bool
    PipeWriter::IsOpen() const noexcept
    {
        return handle_ != -1;
    }

    void
    PipeWriter::Close() noexcept
    {
        if (handle_ == -1)
        {
            return;
        }
#if defined(_WIN32)
        CloseHandle(AsHandle(handle_));
#else
        close(Descriptor(handle_));
#endif
        handle_ = -1;
    }

    Status
    PipeWriter::Write(std::string_view data) noexcept
    {
        if (handle_ == -1)
        {
            return Error{ "the pipe is closed" };
        }
        while (!data.empty())
        {
#if defined(_WIN32)
            DWORD written = 0;
            const DWORD request = data.size() > 0x10000000
                                      ? 0x10000000
                                      : static_cast<DWORD>(data.size());
            if (!::WriteFile(AsHandle(handle_),
                             data.data(),
                             request,
                             &written,
                             nullptr))
            {
                return Error{ "the other end of the pipe is gone" };
            }
            data.remove_prefix(written);
#else
            const auto count
                = write(Descriptor(handle_), data.data(), data.size());
            if (count < 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }
                return Error{ ErrnoMessage("cannot write to the pipe", errno) };
            }
            data.remove_prefix(static_cast<std::size_t>(count));
#endif
        }
        return Status::Ok();
    }

    ChildProcess::~ChildProcess()
    {
        if (!exit_)
        {
            Kill();
            static_cast<void>(Wait());
        }
#if defined(_WIN32)
        if (process_ != -1)
        {
            CloseHandle(AsHandle(process_));
        }
        if (group_ != -1)
        {
            CloseHandle(AsHandle(group_));
        }
#endif
    }

#if defined(_WIN32)
    Expected<std::unique_ptr<ChildProcess>>
    ChildProcess::Start(const ChildOptions &options)
    {
        if (options.arguments.empty())
        {
            return Error{ "no program to start" };
        }
        SECURITY_ATTRIBUTES inheritable{};
        inheritable.nLength = sizeof(inheritable);
        inheritable.bInheritHandle = TRUE;

        HANDLE pipes[3][2] = {};
        for (auto &pipe : pipes)
        {
            if (!CreatePipe(&pipe[0], &pipe[1], &inheritable, 0))
            {
                const std::string message
                    = LastErrorMessage("cannot create a pipe");
                for (auto &created : pipes)
                {
                    for (HANDLE handle : created)
                    {
                        if (handle != nullptr)
                        {
                            CloseHandle(handle);
                        }
                    }
                }
                return Error{ message };
            }
        }
        // The child gets the read end of its input and the write ends of its
        // output; the ends this process keeps are not inheritable.
        HANDLE childInput = pipes[0][0];
        HANDLE parentInput = pipes[0][1];
        HANDLE parentOutput = pipes[1][0];
        HANDLE childOutput = pipes[1][1];
        HANDLE parentErrors = pipes[2][0];
        HANDLE childErrors = pipes[2][1];
        SetHandleInformation(parentInput, HANDLE_FLAG_INHERIT, 0);
        SetHandleInformation(parentOutput, HANDLE_FLAG_INHERIT, 0);
        SetHandleInformation(parentErrors, HANDLE_FLAG_INHERIT, 0);

        auto closeChildEnds = [&] {
            CloseHandle(childInput);
            CloseHandle(childOutput);
            CloseHandle(childErrors);
        };

        std::unique_ptr<ChildProcess> child{ new ChildProcess{} };
        child->input_ = PipeWriter{ FromHandle(parentInput) };
        child->output_ = PipeReader{ FromHandle(parentOutput) };
        child->errors_ = PipeReader{ FromHandle(parentErrors) };

        // Only these three handles are inherited, so a worker never holds
        // the pipes of another one open.
        HANDLE inherited[3] = { childInput, childOutput, childErrors };
        SIZE_T attributeSize = 0;
        InitializeProcThreadAttributeList(nullptr, 1, 0, &attributeSize);
        std::vector<unsigned char> attributeStorage(attributeSize);
        auto *attributes = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(
            attributeStorage.data());
        if (!InitializeProcThreadAttributeList(attributes, 1, 0, &attributeSize)
            || !UpdateProcThreadAttribute(attributes,
                                          0,
                                          PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
                                          inherited,
                                          sizeof(inherited),
                                          nullptr,
                                          nullptr))
        {
            const std::string message
                = LastErrorMessage("cannot prepare the process attributes");
            closeChildEnds();
            return Error{ message };
        }

        STARTUPINFOEXW startup{};
        startup.StartupInfo.cb = sizeof(startup);
        startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
        startup.StartupInfo.hStdInput = childInput;
        startup.StartupInfo.hStdOutput = childOutput;
        startup.StartupInfo.hStdError = childErrors;
        startup.lpAttributeList = attributes;

        std::wstring program = Widen(options.arguments.front());
        std::wstring commandLine;
        for (std::size_t index = 0; index < options.arguments.size(); ++index)
        {
            if (index > 0)
            {
                commandLine.push_back(L' ');
            }
            AppendQuotedArgument(commandLine, Widen(options.arguments[index]));
        }
        std::wstring environment
            = EnvironmentBlock(options.environment, options.keptEnvironment);
        const std::wstring directory = options.workingDirectory.wstring();

        PROCESS_INFORMATION information{};
        const BOOL created
            = CreateProcessW(options.searchPath ? nullptr : program.c_str(),
                             commandLine.data(),
                             nullptr,
                             nullptr,
                             TRUE,
                             CREATE_UNICODE_ENVIRONMENT | CREATE_SUSPENDED
                                 | EXTENDED_STARTUPINFO_PRESENT,
                             environment.data(),
                             directory.empty() ? nullptr : directory.c_str(),
                             &startup.StartupInfo,
                             &information);
        DeleteProcThreadAttributeList(attributes);
        if (!created)
        {
            const std::string message = LastErrorMessage(
                ("cannot start '" + options.arguments.front() + "'").c_str());
            closeChildEnds();
            return Error{ message };
        }
        closeChildEnds();

        // A job ends the whole tree at once, and its limit keeps a crash from
        // waiting on the error-reporting dialog.
        HANDLE job = CreateJobObjectW(nullptr, nullptr);
        if (job != nullptr)
        {
            JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
            limits.BasicLimitInformation.LimitFlags
                = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE
                  | JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION;
            SetInformationJobObject(job,
                                    JobObjectExtendedLimitInformation,
                                    &limits,
                                    sizeof(limits));
            if (!AssignProcessToJobObject(job, information.hProcess))
            {
                CloseHandle(job);
                job = nullptr;
            }
        }
        ResumeThread(information.hThread);
        CloseHandle(information.hThread);

        child->id_ = static_cast<std::int64_t>(information.dwProcessId);
        child->process_ = FromHandle(information.hProcess);
        child->group_ = job == nullptr ? -1 : FromHandle(job);
        return child;
    }

    void
    ChildProcess::Kill() noexcept
    {
        if (exit_)
        {
            return;
        }
        if (group_ != -1)
        {
            TerminateJobObject(AsHandle(group_), 0xC000013A);
        }
        else if (process_ != -1)
        {
            TerminateProcess(AsHandle(process_), 0xC000013A);
        }
    }

    ExitStatus
    ChildProcess::Wait() noexcept
    {
        if (exit_)
        {
            return *exit_;
        }
        ExitStatus status;
        if (process_ != -1)
        {
            WaitForSingleObject(AsHandle(process_), INFINITE);
            DWORD code = 0;
            GetExitCodeProcess(AsHandle(process_), &code);
            status.code = static_cast<std::int64_t>(code);
            // NTSTATUS error codes have both top bits set.
            status.kind
                = (code & 0xC0000000U) == 0xC0000000U || code == 0x80000003U
                      ? ExitStatus::Kind::kException
                      : ExitStatus::Kind::kExited;
            if (status.kind == ExitStatus::Kind::kExited && code > 0x7FFFFFFFU)
            {
                status.code = static_cast<std::int32_t>(code);
            }
        }
        exit_ = status;
        return status;
    }

    std::optional<ExitStatus>
    ChildProcess::WaitFor(std::chrono::milliseconds timeout) noexcept
    {
        if (exit_ || process_ == -1)
        {
            return Wait();
        }
        const auto waited = WaitForSingleObject(
            AsHandle(process_),
            static_cast<DWORD>(std::max<long long>(0, timeout.count())));
        if (waited == WAIT_TIMEOUT)
        {
            return std::nullopt;
        }
        return Wait();
    }

    Expected<WorkerStreams>
    TakeStandardStreams()
    {
        HANDLE process = GetCurrentProcess();
        HANDLE input = nullptr;
        HANDLE output = nullptr;
        if (!DuplicateHandle(process,
                             GetStdHandle(STD_INPUT_HANDLE),
                             process,
                             &input,
                             0,
                             FALSE,
                             DUPLICATE_SAME_ACCESS)
            || !DuplicateHandle(process,
                                GetStdHandle(STD_OUTPUT_HANDLE),
                                process,
                                &output,
                                0,
                                FALSE,
                                DUPLICATE_SAME_ACCESS))
        {
            return Error{ LastErrorMessage(
                "cannot take the standard streams") };
        }
        std::fflush(stdout);
        int nullDescriptor = -1;
        _sopen_s(&nullDescriptor,
                 "NUL",
                 _O_RDONLY | _O_BINARY,
                 _SH_DENYNO,
                 _S_IREAD);
        if (nullDescriptor >= 0)
        {
            _dup2(nullDescriptor, 0);
            _close(nullDescriptor);
        }
        _dup2(2, 1);
        // The runner writes what the worker prints through its own text-mode
        // streams; translating line ends here as well would double them.
        _setmode(1, _O_BINARY);
        _setmode(2, _O_BINARY);
        SetStdHandle(STD_INPUT_HANDLE,
                     reinterpret_cast<HANDLE>(_get_osfhandle(0)));
        SetStdHandle(STD_OUTPUT_HANDLE, GetStdHandle(STD_ERROR_HANDLE));
        WorkerStreams streams;
        streams.commands = PipeReader{ FromHandle(input) };
        streams.events = PipeWriter{ FromHandle(output) };
        return streams;
    }

    std::filesystem::path
    CurrentExecutable(const char *argumentZero)
    {
        std::wstring buffer(MAX_PATH, L'\0');
        while (true)
        {
            const DWORD length
                = GetModuleFileNameW(nullptr,
                                     buffer.data(),
                                     static_cast<DWORD>(buffer.size()));
            if (length == 0)
            {
                return argumentZero == nullptr
                           ? std::filesystem::path{}
                           : std::filesystem::path{ argumentZero };
            }
            if (length < buffer.size())
            {
                buffer.resize(length);
                return std::filesystem::path{ buffer };
            }
            buffer.resize(buffer.size() * 2);
        }
    }

    std::string
    ReadEnvironment(const char *name)
    {
        const std::wstring wideName = Widen(name);
        const DWORD size
            = GetEnvironmentVariableW(wideName.c_str(), nullptr, 0);
        if (size == 0)
        {
            return {};
        }
        std::wstring value(size, L'\0');
        const DWORD length
            = GetEnvironmentVariableW(wideName.c_str(), value.data(), size);
        value.resize(length);
        return Narrow(value);
    }

    bool
    HasEnvironment(const char *name)
    {
        const std::wstring wideName = Widen(name);
        return GetEnvironmentVariableW(wideName.c_str(), nullptr, 0) > 0
               || GetLastError() != ERROR_ENVVAR_NOT_FOUND;
    }

    bool
    IsStandardOutputTerminal() noexcept
    {
        DWORD mode = 0;
        return GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode) != 0;
    }

    bool
    EnableTerminalColours() noexcept
    {
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        if (!GetConsoleMode(output, &mode))
        {
            return false;
        }
        return SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)
               != 0;
    }

    void
    QuietCrashReporting() noexcept
    {
        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX
                     | SEM_NOOPENFILEERRORBOX);
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    }

    std::int64_t
    CurrentProcessId() noexcept
    {
        return static_cast<std::int64_t>(GetCurrentProcessId());
    }

    void
    WriteStandardError(std::string_view text) noexcept
    {
        while (!text.empty())
        {
            const unsigned chunk = static_cast<unsigned>(
                std::min<std::size_t>(text.size(), 1U << 20U));
            const int written = _write(2, text.data(), chunk);
            if (written <= 0)
            {
                return;
            }
            text.remove_prefix(static_cast<std::size_t>(written));
        }
    }
#else
    Expected<std::unique_ptr<ChildProcess>>
    ChildProcess::Start(const ChildOptions &options)
    {
        if (options.arguments.empty())
        {
            return Error{ "no program to start" };
        }
        int input[2] = { -1, -1 };
        int output[2] = { -1, -1 };
        int errors[2] = { -1, -1 };
        auto closeAll = [&] {
            for (int *pipe : { input, output, errors })
            {
                for (int index = 0; index < 2; ++index)
                {
                    if (pipe[index] != -1)
                    {
                        close(pipe[index]);
                        pipe[index] = -1;
                    }
                }
            }
        };
        if (!MakePipe(input) || !MakePipe(output) || !MakePipe(errors))
        {
            const int error = errno;
            closeAll();
            return Error{ ErrnoMessage("cannot create a pipe", error) };
        }

        posix_spawn_file_actions_t actions;
        posix_spawn_file_actions_init(&actions);
        posix_spawn_file_actions_adddup2(&actions, input[0], 0);
        posix_spawn_file_actions_adddup2(&actions, output[1], 1);
        posix_spawn_file_actions_adddup2(&actions, errors[1], 2);

        posix_spawnattr_t attributes;
        posix_spawnattr_init(&attributes);
        sigset_t defaults;
        sigemptyset(&defaults);
        // The runner ignores SIGPIPE; a test must see the default.
        sigaddset(&defaults, SIGPIPE);
        posix_spawnattr_setsigdefault(&attributes, &defaults);
        sigset_t mask;
        sigemptyset(&mask);
        posix_spawnattr_setsigmask(&attributes, &mask);
        posix_spawnattr_setpgroup(&attributes, 0);
        posix_spawnattr_setflags(&attributes,
                                 static_cast<short>(POSIX_SPAWN_SETPGROUP
                                                    | POSIX_SPAWN_SETSIGDEF
                                                    | POSIX_SPAWN_SETSIGMASK));

        std::vector<std::string> environmentStorage;
        for (char **entry = CurrentEnvironment();
             entry != nullptr && *entry != nullptr;
             ++entry)
        {
            const std::string_view text{ *entry };
            if (Withheld(text.substr(0, text.find('=')),
                         options.keptEnvironment))
            {
                continue;
            }
            environmentStorage.emplace_back(*entry);
        }
        for (const auto &[name, value] : options.environment)
        {
            // A lambda cannot capture a structured binding in C++17.
            const std::string &variable = name;
            const auto removed = std::remove_if(
                environmentStorage.begin(),
                environmentStorage.end(),
                [&](const std::string &entry) {
                    return entry.size() > variable.size()
                           && entry.compare(0, variable.size(), variable) == 0
                           && entry[variable.size()] == '=';
                });
            environmentStorage.erase(removed, environmentStorage.end());
            environmentStorage.push_back(name + "=" + value);
        }
        std::vector<char *> environment;
        environment.reserve(environmentStorage.size() + 1);
        for (auto &entry : environmentStorage)
        {
            environment.push_back(entry.data());
        }
        environment.push_back(nullptr);

        std::vector<std::string> argumentStorage = options.arguments;
        std::vector<char *> arguments;
        arguments.reserve(argumentStorage.size() + 1);
        for (auto &argument : argumentStorage)
        {
            arguments.push_back(argument.data());
        }
        arguments.push_back(nullptr);

        // posix_spawn has no working-directory action everywhere, so the
        // directory changes around the call; only this thread spawns.
        std::filesystem::path previousDirectory;
        if (!options.workingDirectory.empty())
        {
            std::error_code error;
            previousDirectory = std::filesystem::current_path(error);
            std::filesystem::current_path(options.workingDirectory, error);
            if (error)
            {
                posix_spawn_file_actions_destroy(&actions);
                posix_spawnattr_destroy(&attributes);
                closeAll();
                return Error{ "cannot enter '"
                              + options.workingDirectory.string() + "'" };
            }
        }
        pid_t pid = 0;
        const int result = options.searchPath ? posix_spawnp(&pid,
                                                             arguments.front(),
                                                             &actions,
                                                             &attributes,
                                                             arguments.data(),
                                                             environment.data())
                                              : posix_spawn(&pid,
                                                            arguments.front(),
                                                            &actions,
                                                            &attributes,
                                                            arguments.data(),
                                                            environment.data());
        if (!previousDirectory.empty())
        {
            std::error_code error;
            std::filesystem::current_path(previousDirectory, error);
        }
        posix_spawn_file_actions_destroy(&actions);
        posix_spawnattr_destroy(&attributes);
        close(input[0]);
        close(output[1]);
        close(errors[1]);
        input[0] = output[1] = errors[1] = -1;
        if (result != 0)
        {
            closeAll();
            return Error{ ErrnoMessage(
                ("cannot start '" + options.arguments.front() + "'").c_str(),
                result) };
        }

        std::unique_ptr<ChildProcess> child{ new ChildProcess{} };
        child->input_ = PipeWriter{ input[1] };
        child->output_ = PipeReader{ output[0] };
        child->errors_ = PipeReader{ errors[0] };
        child->id_ = pid;
        child->process_ = pid;
        child->group_ = pid;
        return child;
    }

    void
    ChildProcess::Kill() noexcept
    {
        if (exit_ || process_ == -1)
        {
            return;
        }
        // The whole group: the worker and whatever its test started.
        if (kill(-static_cast<pid_t>(group_), SIGKILL) != 0)
        {
            kill(static_cast<pid_t>(process_), SIGKILL);
        }
    }

    ExitStatus
    ChildProcess::Wait() noexcept
    {
        if (exit_)
        {
            return *exit_;
        }
        ExitStatus status;
        if (process_ != -1)
        {
            int raw = 0;
            while (waitpid(static_cast<pid_t>(process_), &raw, 0) < 0)
            {
                if (errno != EINTR)
                {
                    break;
                }
            }
            if (WIFSIGNALED(raw))
            {
                status.kind = ExitStatus::Kind::kSignaled;
                status.code = WTERMSIG(raw);
            }
            else if (WIFEXITED(raw))
            {
                status.code = WEXITSTATUS(raw);
            }
            // Whatever the test left running in the group goes too.
            kill(-static_cast<pid_t>(group_), SIGKILL);
        }
        exit_ = status;
        return status;
    }

    std::optional<ExitStatus>
    ChildProcess::WaitFor(std::chrono::milliseconds timeout) noexcept
    {
        if (exit_ || process_ == -1)
        {
            return Wait();
        }
        const auto deadline = std::chrono::steady_clock::now() + timeout;
        while (true)
        {
            int raw = 0;
            const pid_t result
                = waitpid(static_cast<pid_t>(process_), &raw, WNOHANG);
            if (result == static_cast<pid_t>(process_))
            {
                ExitStatus status;
                if (WIFSIGNALED(raw))
                {
                    status.kind = ExitStatus::Kind::kSignaled;
                    status.code = WTERMSIG(raw);
                }
                else if (WIFEXITED(raw))
                {
                    status.code = WEXITSTATUS(raw);
                }
                kill(-static_cast<pid_t>(group_), SIGKILL);
                exit_ = status;
                return status;
            }
            if (result < 0 && errno != EINTR)
            {
                return Wait();
            }
            if (std::chrono::steady_clock::now() >= deadline)
            {
                return std::nullopt;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    Expected<WorkerStreams>
    TakeStandardStreams()
    {
        const int input = fcntl(0, F_DUPFD_CLOEXEC, 3);
        const int output = fcntl(1, F_DUPFD_CLOEXEC, 3);
        if (input < 0 || output < 0)
        {
            const int error = errno;
            if (input >= 0)
            {
                close(input);
            }
            return Error{ ErrnoMessage("cannot take the standard streams",
                                       error) };
        }
        std::fflush(stdout);
        const int nullDescriptor = open("/dev/null", O_RDONLY | O_CLOEXEC);
        if (nullDescriptor >= 0)
        {
            dup2(nullDescriptor, 0);
            close(nullDescriptor);
        }
        dup2(2, 1);
        WorkerStreams streams;
        streams.commands = PipeReader{ input };
        streams.events = PipeWriter{ output };
        return streams;
    }

    std::filesystem::path
    CurrentExecutable(const char *argumentZero)
    {
        std::error_code error;
#    if defined(__linux__)
        auto path = std::filesystem::read_symlink("/proc/self/exe", error);
        if (!error)
        {
            return path;
        }
#    elif defined(__APPLE__)
        std::uint32_t size = 0;
        _NSGetExecutablePath(nullptr, &size);
        std::string buffer(size, '\0');
        if (_NSGetExecutablePath(buffer.data(), &size) == 0)
        {
            buffer.resize(std::strlen(buffer.c_str()));
            auto canonical = std::filesystem::weakly_canonical(buffer, error);
            return error ? std::filesystem::path{ buffer } : canonical;
        }
#    endif
        if (argumentZero == nullptr)
        {
            return {};
        }
        auto absolute = std::filesystem::absolute(argumentZero, error);
        return error ? std::filesystem::path{ argumentZero } : absolute;
    }

    std::string
    ReadEnvironment(const char *name)
    {
        const char *value = std::getenv(name);
        return value == nullptr ? std::string{} : std::string{ value };
    }

    bool
    HasEnvironment(const char *name)
    {
        return std::getenv(name) != nullptr;
    }

    bool
    IsStandardOutputTerminal() noexcept
    {
        return isatty(1) != 0;
    }

    bool
    EnableTerminalColours() noexcept
    {
        const char *terminal = std::getenv("TERM");
        return terminal == nullptr || std::string_view{ terminal } != "dumb";
    }

    void
    QuietCrashReporting() noexcept
    {}

    std::int64_t
    CurrentProcessId() noexcept
    {
        return static_cast<std::int64_t>(getpid());
    }

    void
    WriteStandardError(std::string_view text) noexcept
    {
        while (!text.empty())
        {
            const ssize_t written = write(2, text.data(), text.size());
            if (written < 0 && errno == EINTR)
            {
                continue;
            }
            if (written <= 0)
            {
                return;
            }
            text.remove_prefix(static_cast<std::size_t>(written));
        }
    }
#endif

    unsigned
    HardwareThreads() noexcept
    {
        const unsigned count = std::thread::hardware_concurrency();
        return count == 0 ? 1 : count;
    }

    Expected<CapturedRun>
    RunAndCapture(const ChildOptions &options)
    {
        auto child = ChildProcess::Start(options);
        if (!child)
        {
            return child.GetError();
        }
        (*child)->Input().Close();
        CapturedRun run;
        std::thread errorReader([&] {
            char buffer[4096];
            while (true)
            {
                const auto count
                    = (*child)->Errors().Read(buffer, sizeof(buffer));
                if (count <= 0)
                {
                    return;
                }
                run.errors.append(buffer, static_cast<std::size_t>(count));
            }
        });
        char buffer[4096];
        while (true)
        {
            const auto count = (*child)->Output().Read(buffer, sizeof(buffer));
            if (count <= 0)
            {
                break;
            }
            run.output.append(buffer, static_cast<std::size_t>(count));
        }
        errorReader.join();
        run.status = (*child)->Wait();
        return run;
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <set>
#include <system_error>
#include <utility>


namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        /// The version of the file format this runner reads.
        constexpr std::uint64_t kConfigVersion = 1;

        /// What the value of a key is, and how it becomes arguments.
        enum class ValueKind : std::uint8_t
        {
            /// true gives the option; false, the default, gives nothing.
            kFlag,
            /// false gives the negating option; true is the default.
            kNegated,
            /// A string, given as the value of the option.
            kWord,
            /// A file or directory, relative to the file's directory.
            kPath,
            /// A whole number.
            kCount,
            /// A whole number or a word, such as "auto".
            kCountOrWord,
            /// A number with a fraction, such as seconds.
            kNumber,
            /// An array of strings, each given to the option.
            kWords,
            /// A directory, or false for none.
            kPathOrFalse,
            /// "quiet", "normal" or "verbose".
            kVerbosity,
            /// An array of test specs.
            kTests,
        };

        struct ConfigKey final
        {
            std::string_view name;
            ValueKind kind;
            /// The option it stands for, when it is not spelled like the key.
            std::string_view option = {};
        };

        const std::vector<ConfigKey> &
        Keys()
        {
            static const auto *keys = new std::vector<ConfigKey>{
                { "tests", ValueKind::kTests },
                // Selecting tests.
                { "select", ValueKind::kWord },
                { "changes-since", ValueKind::kWord },
                { "incremental", ValueKind::kFlag },
                { "shard-durations", ValueKind::kPath },
                { "filenames-as-tags", ValueKind::kFlag },
                // Running.
                { "jobs", ValueKind::kCount },
                { "in-process", ValueKind::kFlag },
                { "isolate", ValueKind::kFlag },
                { "pass-env", ValueKind::kWords },
                { "timeout", ValueKind::kNumber },
                { "adaptive-timeout",
                  ValueKind::kNegated,
                  "no-adaptive-timeout" },
                { "retries", ValueKind::kCountOrWord },
                { "repeat", ValueKind::kCount },
                { "fail-fast", ValueKind::kCount },
                { "order", ValueKind::kWord },
                { "seed", ValueKind::kCountOrWord },
                { "bisect", ValueKind::kNegated, "no-bisect" },
                // How tests behave.
                { "success", ValueKind::kFlag },
                { "no-throw", ValueKind::kFlag },
                { "warn", ValueKind::kWords },
                { "allow-no-tests", ValueKind::kFlag },
                { "skip-benchmarks", ValueKind::kFlag },
                { "benchmark-samples", ValueKind::kCount },
                { "benchmark-resamples", ValueKind::kCount },
                { "benchmark-confidence-interval", ValueKind::kNumber },
                { "benchmark-no-analysis", ValueKind::kFlag },
                { "benchmark-warmup-time", ValueKind::kCount },
                // History and analysis.
                { "history", ValueKind::kPathOrFalse },
                { "quarantine", ValueKind::kWord },
                // Output.
                { "verbosity", ValueKind::kVerbosity },
                { "color", ValueKind::kWord },
                { "junit", ValueKind::kPath },
                { "json", ValueKind::kPath },
                { "trace", ValueKind::kPath },
                { "slowest", ValueKind::kCount },
            };
            return *keys;
        }

        /// Options that describe one run rather than a project.
        constexpr std::string_view kCommandLineOnly[] = {
            "section",
            "generator-index",
            "path-filter",
            "input-file",
            "shard",
            "break",
            "replay",
            "analytics",
            "import",
            "import-format",
            "import-name",
            "import-revision",
            "verbose",
            "quiet",
            "premature-exit-guard-file",
            "list",
            "list-tags",
            "help",
            "config",
            "no-config",
            "catch3-worker",
        };

        std::string
        Quoted(std::string_view key)
        {
            return "\"" + std::string{ key } + "\"";
        }

        std::string
        UnknownKey(std::string_view key)
        {
            for (const std::string_view option : kCommandLineOnly)
            {
                if (key == option)
                {
                    return Quoted(key)
                           + " belongs on the command line, not in a "
                             "catch3.json";
                }
            }
            std::string message = Quoted(key) + " is not a key of catch3.json";
            std::string_view closest;
            std::size_t best = 3;
            const auto consider = [&](std::string_view name) {
                const std::size_t distance = EditDistance(key, name);
                if (distance < best)
                {
                    best = distance;
                    closest = name;
                }
            };
            consider("version");
            for (const auto &entry : Keys())
            {
                consider(entry.name);
            }
            if (!closest.empty())
            {
                message += "; did you mean " + Quoted(closest) + "?";
            }
            return message;
        }

        /// A whole number JSON can say exactly, or nothing.
        std::optional<std::uint64_t>
        WholeNumber(const JsonValue &value)
        {
            if (!value.IsNumber())
            {
                return std::nullopt;
            }
            const double number = value.AsNumber();
            if (!std::isfinite(number) || number < 0
                || number > std::floor(number) || number > 9007199254740992.0)
            {
                return std::nullopt;
            }
            return static_cast<std::uint64_t>(number);
        }

        /// A number in the form the command line reads: digits and a point.
        std::string
        DecimalText(double number)
        {
            char text[400];
            std::snprintf(text, sizeof(text), "%.17f", number);
            std::string result{ text };
            for (char &character : result)
            {
                // The decimal separator of the locale a test may have set.
                if (character == ',')
                {
                    character = '.';
                }
            }
            return result;
        }

        std::string
        PathText(const std::string &value,
                 const std::filesystem::path &directory)
        {
            const std::filesystem::path path{ value };
            if (path.is_absolute() || directory.empty())
            {
                return path.lexically_normal().string();
            }
            return (directory / path).lexically_normal().string();
        }

        /// The arguments that one key and its value stand for.
        Expected<std::vector<std::string>>
        KeyArguments(const ConfigKey &key,
                     const JsonValue &value,
                     const std::filesystem::path &directory)
        {
            const std::string option
                = "--"
                  + std::string{ key.option.empty() ? key.name : key.option };
            const auto wrong = [&](std::string_view expected) {
                return Error{ Quoted(key.name) + " takes "
                              + std::string{ expected } + ", not "
                              + value.ToString() };
            };
            switch (key.kind)
            {
                case ValueKind::kFlag:
                    if (value.GetKind() != JsonValue::Kind::kBoolean)
                    {
                        return wrong("true or false");
                    }
                    if (value.AsBoolean())
                    {
                        return std::vector<std::string>{ option };
                    }
                    return std::vector<std::string>{};
                case ValueKind::kNegated:
                    if (value.GetKind() != JsonValue::Kind::kBoolean)
                    {
                        return wrong("true or false");
                    }
                    if (!value.AsBoolean())
                    {
                        return std::vector<std::string>{ option };
                    }
                    return std::vector<std::string>{};
                case ValueKind::kWord:
                    if (!value.IsString())
                    {
                        return wrong("a string");
                    }
                    return std::vector<std::string>{ option + "="
                                                     + value.AsString() };
                case ValueKind::kPath:
                    if (!value.IsString() || value.AsString().empty())
                    {
                        return wrong("a path");
                    }
                    return std::vector<std::string>{
                        option + "=" + PathText(value.AsString(), directory)
                    };
                case ValueKind::kCount:
                    if (const auto number = WholeNumber(value))
                    {
                        return std::vector<std::string>{
                            option + "=" + std::to_string(*number)
                        };
                    }
                    return wrong("a whole number");
                case ValueKind::kCountOrWord:
                    if (value.IsString())
                    {
                        return std::vector<std::string>{ option + "="
                                                         + value.AsString() };
                    }
                    if (const auto number = WholeNumber(value))
                    {
                        return std::vector<std::string>{
                            option + "=" + std::to_string(*number)
                        };
                    }
                    return wrong("a whole number or a string");
                case ValueKind::kNumber:
                    if (!value.IsNumber() || !std::isfinite(value.AsNumber())
                        || value.AsNumber() < 0)
                    {
                        return wrong("a number of at least 0");
                    }
                    return std::vector<std::string>{
                        option + "=" + DecimalText(value.AsNumber())
                    };
                case ValueKind::kWords:
                {
                    if (!value.IsArray())
                    {
                        return wrong("an array of strings");
                    }
                    std::vector<std::string> arguments;
                    for (const auto &item : value.Items())
                    {
                        if (!item.IsString())
                        {
                            return wrong("an array of strings");
                        }
                        arguments.push_back(option + "=" + item.AsString());
                    }
                    return arguments;
                }
                case ValueKind::kPathOrFalse:
                    if (value.GetKind() == JsonValue::Kind::kBoolean
                        && !value.AsBoolean())
                    {
                        return std::vector<std::string>{
                            "--no-" + std::string{ key.name }
                        };
                    }
                    if (!value.IsString() || value.AsString().empty())
                    {
                        return wrong("a path or false");
                    }
                    return std::vector<std::string>{
                        option + "=" + PathText(value.AsString(), directory)
                    };
                case ValueKind::kVerbosity:
                    if (value.IsString())
                    {
                        if (value.AsString() == "quiet")
                        {
                            return std::vector<std::string>{ "--quiet" };
                        }
                        if (value.AsString() == "verbose")
                        {
                            return std::vector<std::string>{ "--verbose" };
                        }
                        if (value.AsString() == "normal")
                        {
                            return std::vector<std::string>{};
                        }
                    }
                    return wrong("\"quiet\", \"normal\" or \"verbose\"");
                case ValueKind::kTests:
                {
                    if (!value.IsArray())
                    {
                        return wrong("an array of test specs");
                    }
                    // After --, every argument is a test spec, even one that
                    // starts with a dash.
                    std::vector<std::string> arguments{ "--" };
                    for (const auto &item : value.Items())
                    {
                        if (!item.IsString() || item.AsString().empty())
                        {
                            return wrong("an array of test specs");
                        }
                        arguments.push_back(item.AsString());
                    }
                    return arguments;
                }
            }
            return wrong("a known kind of value");
        }
    } // namespace

    std::optional<std::filesystem::path>
    FindProjectConfig(const std::filesystem::path &start)
    {
        std::error_code error;
        std::filesystem::path directory
            = std::filesystem::absolute(start, error).lexically_normal();
        if (error)
        {
            return std::nullopt;
        }
        if (directory.filename().empty() && directory.has_relative_path())
        {
            directory = directory.parent_path();
        }
        std::vector<std::filesystem::path> candidates;
        for (std::filesystem::path current = directory;;
             current = current.parent_path())
        {
            candidates.push_back(current);
            if (std::filesystem::exists(current / ".git", error))
            {
                break;
            }
            if (!current.has_relative_path())
            {
                // No repository holds the directory.
                candidates = { directory };
                break;
            }
        }
        for (const auto &candidate : candidates)
        {
            const auto file = candidate / kProjectConfigName;
            if (std::filesystem::is_regular_file(file, error))
            {
                return file;
            }
        }
        return std::nullopt;
    }

    Expected<Options>
    ApplyProjectConfig(std::string_view text,
                       const std::filesystem::path &directory,
                       std::string_view file,
                       Options options)
    {
        const std::string prefix = std::string{ file } + ": ";
        auto document = ParseJson(text);
        if (!document)
        {
            return Error{ prefix + document.GetError().Message() };
        }
        if (!document->IsObject())
        {
            return Error{ prefix + "the file holds an object, not "
                          + document->ToString() };
        }
        std::set<std::string_view> seen;
        bool versioned = false;
        for (const auto &[name, value] : document->Members())
        {
            if (!seen.insert(name).second)
            {
                return Error{ prefix + Quoted(name) + " appears twice" };
            }
            if (name == "version")
            {
                if (WholeNumber(value) != kConfigVersion)
                {
                    return Error{
                        prefix + "version " + value.ToString()
                        + " is not known; this Catch3 reads version 1"
                    };
                }
                versioned = true;
                continue;
            }
            if (name == "$schema")
            {
                // For editors; the runner knows its own format.
                continue;
            }
            const ConfigKey *key = nullptr;
            for (const auto &entry : Keys())
            {
                if (entry.name == name)
                {
                    key = &entry;
                    break;
                }
            }
            if (key == nullptr)
            {
                return Error{ prefix + UnknownKey(name) };
            }
            auto arguments = KeyArguments(*key, value, directory);
            if (!arguments)
            {
                return Error{ prefix + arguments.GetError().Message() };
            }
            auto applied = ParseOptions(*arguments, std::move(options));
            if (!applied)
            {
                return Error{ prefix + Quoted(name) + ": "
                              + applied.GetError().Message() };
            }
            options = std::move(*applied);
        }
        if (!versioned)
        {
            return Error{ prefix + "\"version\": 1 is missing" };
        }
        return options;
    }

    Expected<Options>
    ParseCommandLine(const std::vector<std::string> &arguments,
                     const std::filesystem::path &workingDirectory)
    {
        auto commandLine = ParseOptions(arguments);
        if (!commandLine)
        {
            return commandLine;
        }
        if (commandLine->noConfig || commandLine->mode == Mode::kWorker
            || commandLine->mode == Mode::kHelp
            || commandLine->mode == Mode::kVersion)
        {
            // A worker gets everything it needs from its runner.
            return commandLine;
        }
        std::filesystem::path file;
        if (!commandLine->configPath.empty())
        {
            file = commandLine->configPath;
            if (file.is_relative())
            {
                file = workingDirectory / file;
            }
        }
        else if (auto found = FindProjectConfig(workingDirectory))
        {
            file = std::move(*found);
        }
        else
        {
            return commandLine;
        }
        file = file.lexically_normal();
        auto text = ReadFile(file);
        if (!text)
        {
            return Error{ "cannot read the configuration " + file.string()
                          + ": " + text.GetError().Message() };
        }
        auto configured
            = ApplyProjectConfig(*text, file.parent_path(), file.string());
        if (!configured)
        {
            return configured;
        }
        Options base = std::move(*configured);
        // Specs on the command line replace those of the file rather than
        // widening them with "or".
        std::vector<std::string> fileSpecs = std::move(base.testSpecs);
        base.testSpecs.clear();
        auto parsed = ParseOptions(arguments, std::move(base));
        if (!parsed)
        {
            return parsed;
        }
        if (parsed->testSpecs.empty())
        {
            parsed->testSpecs = std::move(fileSpecs);
        }
        parsed->configPath = file.string();
        return parsed;
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

namespace Progmasoft::Catch3::Runner
{
    std::optional<std::string>
    LineReader::Next()
    {
        while (true)
        {
            const auto end = buffer_.find('\n', scanned_);
            if (end != std::string::npos)
            {
                std::string line = buffer_.substr(0, end);
                buffer_.erase(0, end + 1);
                scanned_ = 0;
                if (!line.empty() && line.back() == '\r')
                {
                    line.pop_back();
                }
                return line;
            }
            scanned_ = buffer_.size();
            if (ended_)
            {
                if (buffer_.empty())
                {
                    return std::nullopt;
                }
                // A last line without its end still counts.
                std::string line = std::move(buffer_);
                buffer_.clear();
                scanned_ = 0;
                return line;
            }
            char chunk[8192];
            const auto count = pipe_.Read(chunk, sizeof(chunk));
            if (count <= 0)
            {
                ended_ = true;
                continue;
            }
            buffer_.append(chunk, static_cast<std::size_t>(count));
        }
    }

    Status
    MessageWriter::Send(const JsonValue &message)
    {
        std::string line = message.ToString();
        line.push_back('\n');
        const std::lock_guard lock{mutex_};
        return pipe_.Write(line);
    }

    JsonValue
    Message(std::string_view kind)
    {
        JsonValue message = JsonValue::Object();
        message.Set("t", kind);
        return message;
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iterator>
#include <map>
#include <utility>

namespace Progmasoft::Catch3::Runner
{
    std::string_view
    ImportTrim(std::string_view text)
    {
        const auto blank = [](char character) {
            return std::isspace(static_cast<unsigned char>(character)) != 0;
        };
        while (!text.empty() && blank(text.front()))
        {
            text.remove_prefix(1);
        }
        while (!text.empty() && blank(text.back()))
        {
            text.remove_suffix(1);
        }
        return text;
    }

    bool
    ImportStartsWith(std::string_view text, std::string_view prefix)
    {
        return text.substr(0, prefix.size()) == prefix;
    }

    std::vector<std::string_view>
    ImportLines(std::string_view text)
    {
        std::vector<std::string_view> lines;
        while (!text.empty())
        {
            const auto end = text.find('\n');
            auto line = text.substr(0, end);
            if (!line.empty() && line.back() == '\r')
            {
                line.remove_suffix(1);
            }
            lines.push_back(line);
            if (end == std::string_view::npos)
            {
                break;
            }
            text.remove_prefix(end + 1);
        }
        return lines;
    }

    namespace
    {
        namespace ImportText
        {
            /// Without a byte order mark and the white space before the
            /// first character that counts.
            std::string_view
            Contents(std::string_view text)
            {
                if (ImportStartsWith(text, "\xEF\xBB\xBF"))
                {
                    text.remove_prefix(3);
                }
                return ImportTrim(text);
            }

            /// The name of the root element of an XML document.
            std::string_view
            RootElement(std::string_view text)
            {
                while (!text.empty())
                {
                    const auto open = text.find('<');
                    if (open == std::string_view::npos
                        || open + 1 >= text.size())
                    {
                        return {};
                    }
                    text.remove_prefix(open + 1);
                    if (text.front() == '?' || text.front() == '!')
                    {
                        const auto close = text.find('>');
                        if (close == std::string_view::npos)
                        {
                            return {};
                        }
                        text.remove_prefix(close + 1);
                        continue;
                    }
                    return text.substr(0, text.find_first_of(" \t\r\n/>"));
                }
                return {};
            }
        } // namespace ImportText
    } // namespace

    std::optional<ImportFormat>
    DetectImportFormat(std::string_view text)
    {
        const std::string_view contents = ImportText::Contents(text);
        if (contents.empty())
        {
            return std::nullopt;
        }
        if (contents.front() == '<')
        {
            const std::string_view root = ImportText::RootElement(contents);
            if (root == "testsuites" || root == "testsuite")
            {
                return ImportFormat::kJUnit;
            }
            if (root == "assemblies" || root == "assembly")
            {
                return ImportFormat::kXUnit;
            }
            if (root == "TestRun")
            {
                return ImportFormat::kTrx;
            }
            return std::nullopt;
        }
        if (contents.front() == '{')
        {
            auto whole = ParseJson(contents);
            if (!whole)
            {
                // JSON Lines: the first line says what the others are.
                whole = ParseJson(contents.substr(0, contents.find('\n')));
            }
            return whole ? ImportJsonFormat(*whole) : std::nullopt;
        }
        for (const auto raw : ImportLines(contents))
        {
            const std::string_view line = ImportTrim(raw);
            if (line.empty() || line.front() == '#')
            {
                continue;
            }
            if (ImportStartsWith(line, "TAP version")
                || ImportStartsWith(line, "1..") || IsTapTestLine(line))
            {
                return ImportFormat::kTap;
            }
            break;
        }
        return std::nullopt;
    }

    Expected<std::vector<TestResult>>
    ParseImport(std::string_view text,
                ImportFormat format,
                const std::string &source)
    {
        const std::string_view contents = ImportText::Contents(text);
        switch (format)
        {
            case ImportFormat::kAuto:
                break;
            case ImportFormat::kCatch3:
                return ParseCatch3Report(contents);
            case ImportFormat::kJUnit:
                return ParseJUnit(contents);
            case ImportFormat::kXUnit:
                return ParseXUnit(contents);
            case ImportFormat::kTrx:
                return ParseTrx(contents);
            case ImportFormat::kTap:
                return ParseTap(contents, source);
            case ImportFormat::kCtrf:
                return ParseCtrf(contents);
        }
        const auto detected = DetectImportFormat(contents);
        if (!detected)
        {
            return Error{ "its format is not one Catch3 knows; "
                          "--import-format names it" };
        }
        return ParseImport(contents, *detected, source);
    }

    std::vector<TestResult>
    MergeImported(std::vector<TestResult> results)
    {
        std::vector<TestResult> tests;
        std::map<std::string, std::size_t> positions;
        for (auto &result : results)
        {
            if (result.id.empty())
            {
                result.id = History::KeyOf(result);
            }
            const auto [position, added]
                = positions.emplace(result.id, tests.size());
            if (added)
            {
                tests.push_back(std::move(result));
                continue;
            }
            TestResult &test = tests[position->second];
            test.flaky = test.flaky || result.flaky;
            std::move(result.attempts.begin(),
                      result.attempts.end(),
                      std::back_inserter(test.attempts));
        }
        for (auto &test : tests)
        {
            std::size_t passes = 0;
            std::size_t failures = 0;
            for (std::size_t index = 0; index < test.attempts.size(); ++index)
            {
                Attempt &attempt = test.attempts[index];
                attempt.number = index + 1;
                passes += attempt.outcome == Outcome::kPassed ? 1 : 0;
                failures += IsFailure(attempt.outcome) ? 1 : 0;
            }
            if (test.attempts.empty())
            {
                continue;
            }
            // A report of a skip says nothing about an earlier failure.
            const auto verdict
                = std::find_if(test.attempts.rbegin(),
                               test.attempts.rend(),
                               [](const Attempt &attempt) {
                                   return IsFailure(attempt.outcome);
                               });
            const Attempt &last = verdict != test.attempts.rend()
                                      ? *verdict
                                      : test.attempts.back();
            test.outcome = last.outcome;
            test.category = last.category;
            if (passes > 0 && failures > 0)
            {
                test.outcome = Outcome::kPassed;
                test.category = FailureCategory::kNone;
                test.flaky = true;
                test.analysis.push_back(
                    "flaky: failed " + std::to_string(failures) + " of "
                    + std::to_string(test.attempts.size()) + " attempts");
            }
        }
        return tests;
    }

    TestResult
    ImportedTest(std::string name, std::string className)
    {
        TestResult test;
        test.name = std::move(name);
        test.className = std::move(className);
        test.id = History::KeyOf(test);
        return test;
    }

    Attempt
    ImportedAttempt(Outcome outcome,
                    std::int64_t durationMicroseconds,
                    std::string message,
                    std::string details,
                    FailureCategory category)
    {
        Attempt attempt;
        attempt.outcome = outcome;
        attempt.durationMicroseconds
            = std::max<std::int64_t>(durationMicroseconds, 0);
        if (outcome == Outcome::kSkipped)
        {
            attempt.standardOutput = std::move(message);
            return attempt;
        }
        if (outcome != Outcome::kFailedAsExpected && !IsFailure(outcome))
        {
            return attempt;
        }
        if (category == FailureCategory::kNone)
        {
            category = outcome == Outcome::kCrashed ? FailureCategory::kCrash
                       : outcome == Outcome::kTimedOut
                           ? FailureCategory::kTimeout
                           : FailureCategory::kAssertion;
        }
        if (message.empty())
        {
            message = details.substr(0, details.find('\n'));
        }
        attempt.category = category;
        attempt.assertionsFailed = 1;
        AssertionRecord assertion;
        assertion.message = std::move(message);
        assertion.expansion = std::move(details);
        assertion.category = category;
        assertion.allowed = outcome == Outcome::kFailedAsExpected;
        attempt.assertions.push_back(std::move(assertion));
        return attempt;
    }

    std::int64_t
    ImportedMicroseconds(double seconds)
    {
        // About thirty years; a longer duration is not a duration.
        constexpr double kLongest = 1e9;
        if (!std::isfinite(seconds) || seconds <= 0.0 || seconds > kLongest)
        {
            return 0;
        }
        return static_cast<std::int64_t>(std::llround(seconds * 1e6));
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

// The JSON reports: Catch3's own and CTRF.
#include <utility>

namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        namespace ImportJson
        {
            /// A format of Catch3's own or CTRF, from the parsed document.
            std::optional<ImportFormat>
            FormatOf(const JsonValue &value)
            {
                if (value["format"].AsString() == "catch3-run"
                    || value["format"].AsString() == "catch3-results")
                {
                    return ImportFormat::kCatch3;
                }
                if (value["results"]["tests"].IsArray())
                {
                    return ImportFormat::kCtrf;
                }
                return std::nullopt;
            }

            /// What a test owes the verdict of an earlier run is worked out
            /// again here, from this run's point of view.
            void
            Forget(TestResult &test)
            {
                test.quarantined = false;
                test.history = FailureHistory::kUnknown;
                test.selectionReason.clear();
                test.signature.clear();
                test.analysis.clear();
                test.replayCommand.clear();
                test.predictedDurationMicroseconds = 0;
                test.predictedFailureProbability = 0.0;
            }

            Expected<TestResult>
            ReadLine(const JsonValue &value)
            {
                TestResult test = TestResultFromJson(value);
                Forget(test);
                if (test.name.empty())
                {
                    test.name = test.id;
                }
                if (test.name.empty())
                {
                    return Error{ "a test needs an 'id' or a 'name'" };
                }
                const std::string &outcome = value["outcome"].AsString();
                if (!OutcomeFromName(outcome))
                {
                    return Error{ "'" + test.name + "' has the outcome '"
                                  + outcome + "'" };
                }
                if (test.attempts.empty())
                {
                    Attempt attempt = ImportedAttempt(
                        test.outcome,
                        value["durationMicroseconds"].AsInteger(),
                        value["message"].AsString(),
                        value["details"].AsString(),
                        test.category);
                    attempt.standardOutput = value["standardOutput"].AsString();
                    attempt.standardError = value["standardError"].AsString();
                    test.attempts.push_back(std::move(attempt));
                }
                return test;
            }

            Expected<std::vector<TestResult>>
            ParseLines(std::string_view text)
            {
                std::vector<TestResult> tests;
                std::size_t number = 0;
                bool started = false;
                for (const auto line : ImportLines(text))
                {
                    ++number;
                    if (ImportTrim(line).empty())
                    {
                        continue;
                    }
                    const std::string where = "line " + std::to_string(number);
                    auto value = ParseJson(line);
                    if (!value)
                    {
                        return value.GetError().Context(where);
                    }
                    if (!started)
                    {
                        started = true;
                        if ((*value)["format"].AsString() != "catch3-results")
                        {
                            return Error{ "the first line must name the format "
                                          "catch3-results" };
                        }
                        if ((*value)["version"].AsInteger()
                            > kResultsFormatVersion)
                        {
                            return Error{ "the format is newer than this "
                                          "version of Catch3 reads" };
                        }
                        continue;
                    }
                    auto test = ReadLine(*value);
                    if (!test)
                    {
                        return test.GetError().Context(where);
                    }
                    tests.push_back(std::move(*test));
                }
                return tests;
            }

            Expected<std::vector<TestResult>>
            ParseCatch3(std::string_view text)
            {
                auto whole = ParseJson(text);
                if (!whole)
                {
                    return ParseLines(text);
                }
                if ((*whole)["format"].AsString() != "catch3-run")
                {
                    return ParseLines(text);
                }
                auto run = RunResultFromJson(*whole);
                if (!run)
                {
                    return run.GetError();
                }
                for (auto &test : run->tests)
                {
                    Forget(test);
                }
                return std::move(run->tests);
            }

            std::string
            TextOf(const JsonValue &value)
            {
                if (!value.IsArray())
                {
                    return value.AsString();
                }
                std::string text;
                for (const auto &item : value.Items())
                {
                    text += item.AsString() + "\n";
                }
                return text;
            }

            Outcome
            CtrfOutcome(const std::string &status)
            {
                if (status == "passed")
                {
                    return Outcome::kPassed;
                }
                if (status == "failed")
                {
                    return Outcome::kFailed;
                }
                if (status == "skipped" || status == "pending")
                {
                    return Outcome::kSkipped;
                }
                return Outcome::kNotRun;
            }

            /// A test, or one retry of it: the two share these fields.
            Attempt
            CtrfAttempt(const JsonValue &value)
            {
                Attempt attempt = ImportedAttempt(
                    CtrfOutcome(value["status"].AsString()),
                    ImportedMicroseconds(value["duration"].AsNumber() / 1e3),
                    value["message"].AsString(),
                    value["trace"].AsString());
                attempt.standardOutput = TextOf(value["stdout"]);
                attempt.standardError = TextOf(value["stderr"]);
                return attempt;
            }

            std::string
            CtrfSuite(const JsonValue &suite)
            {
                if (!suite.IsArray())
                {
                    return suite.AsString();
                }
                std::string text;
                for (const auto &item : suite.Items())
                {
                    text += (text.empty() ? "" : " > ") + item.AsString();
                }
                return text;
            }

            Expected<std::vector<TestResult>>
            ParseCtrf(std::string_view text)
            {
                auto document = ParseJson(text);
                if (!document)
                {
                    return document.GetError();
                }
                const JsonValue &tests = (*document)["results"]["tests"];
                if (!tests.IsArray())
                {
                    return Error{ "a CTRF report has results.tests" };
                }
                std::vector<TestResult> results;
                for (const auto &item : tests.Items())
                {
                    const std::string &name = item["name"].AsString();
                    if (name.empty())
                    {
                        return Error{ "a CTRF test has no name" };
                    }
                    TestResult test
                        = ImportedTest(name, CtrfSuite(item["suite"]));
                    test.file = item["filePath"].AsString();
                    test.line
                        = static_cast<std::size_t>(item["line"].AsUnsigned());
                    test.flaky = item["flaky"].AsBoolean();
                    for (const auto &tag : item["tags"].Items())
                    {
                        test.tags.push_back(tag.AsString());
                    }
                    for (const auto &retry : item["retryAttempts"].Items())
                    {
                        test.attempts.push_back(CtrfAttempt(retry));
                    }
                    // The retries end before the last attempt, which is the
                    // test itself.
                    test.attempts.push_back(CtrfAttempt(item));
                    results.push_back(std::move(test));
                }
                return results;
            }
        } // namespace ImportJson
    } // namespace

    std::optional<ImportFormat>
    ImportJsonFormat(const JsonValue &value)
    {
        return ImportJson::FormatOf(value);
    }

    Expected<std::vector<TestResult>>
    ParseCatch3Report(std::string_view text)
    {
        return ImportJson::ParseCatch3(text);
    }

    Expected<std::vector<TestResult>>
    ParseCtrf(std::string_view text)
    {
        return ImportJson::ParseCtrf(text);
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

// The Test Anything Protocol, versions 12 to 14.
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <map>
#include <utility>

namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        namespace ImportTap
        {
            std::string
            Lowered(std::string_view text)
            {
                std::string lowered{ text };
                std::transform(lowered.begin(),
                               lowered.end(),
                               lowered.begin(),
                               [](unsigned char character) {
                                   return static_cast<char>(
                                       std::tolower(character));
                               });
                return lowered;
            }

            struct Line final
            {
                bool ok = false;
                std::string description;
                std::string directive;
                std::string reason;
            };

            /// Where the `#` that starts a directive is: `\#` is part of the
            /// description.
            std::size_t
            DirectiveStart(std::string_view text)
            {
                for (std::size_t index = 0; index < text.size(); ++index)
                {
                    if (text[index] == '\\')
                    {
                        ++index;
                    }
                    else if (text[index] == '#')
                    {
                        return index;
                    }
                }
                return std::string_view::npos;
            }

            std::string
            Unescaped(std::string_view text)
            {
                std::string result;
                for (std::size_t index = 0; index < text.size(); ++index)
                {
                    if (text[index] == '\\' && index + 1 < text.size())
                    {
                        ++index;
                    }
                    result.push_back(text[index]);
                }
                return result;
            }

            /// Reads a test line; nothing when `text` is not one.
            std::optional<Line>
            ReadLine(std::string_view text)
            {
                Line line;
                if (ImportStartsWith(text, "not ok"))
                {
                    text.remove_prefix(6);
                }
                else if (ImportStartsWith(text, "ok"))
                {
                    line.ok = true;
                    text.remove_prefix(2);
                }
                else
                {
                    return std::nullopt;
                }
                if (!text.empty() && text.front() != ' '
                    && text.front() != '\t')
                {
                    return std::nullopt;
                }
                text = ImportTrim(text);
                while (!text.empty()
                       && std::isdigit(static_cast<unsigned char>(text.front()))
                              != 0)
                {
                    text.remove_prefix(1);
                }
                text = ImportTrim(text);
                if (ImportStartsWith(text, "- "))
                {
                    text.remove_prefix(2);
                }
                const auto directive = DirectiveStart(text);
                line.description
                    = Unescaped(ImportTrim(text.substr(0, directive)));
                if (directive != std::string_view::npos)
                {
                    const std::string_view rest
                        = ImportTrim(text.substr(directive + 1));
                    const auto end = rest.find_first_of(" \t:");
                    const std::string word = Lowered(rest.substr(0, end));
                    // Harnesses take the longer spellings of SKIP, and only
                    // the word TODO.
                    if (word == "skip" || word == "skipped"
                        || word == "skipping" || word == "todo")
                    {
                        line.directive = word.substr(0, 4);
                        if (end != std::string_view::npos)
                        {
                            std::string_view reason = rest.substr(end);
                            reason.remove_prefix(reason.front() == ':' ? 1 : 0);
                            line.reason = std::string{ ImportTrim(reason) };
                        }
                    }
                }
                return line;
            }

            Outcome
            OutcomeOf(const Line &line)
            {
                if (line.directive == "skip")
                {
                    return Outcome::kSkipped;
                }
                if (line.ok)
                {
                    return Outcome::kPassed;
                }
                return line.directive == "todo" ? Outcome::kFailedAsExpected
                                                : Outcome::kFailed;
            }

            std::string
            Unquoted(std::string_view text)
            {
                text = ImportTrim(text);
                if (text.size() >= 2 && text.front() == text.back()
                    && (text.front() == '"' || text.front() == '\''))
                {
                    text.remove_prefix(1);
                    text.remove_suffix(1);
                }
                return std::string{ text };
            }

            /// A YAML block after a test: its text goes into the failure, and
            /// the two keys that tools agree on are read.
            void
            ApplyDiagnostics(TestResult &test, const std::string &block)
            {
                Attempt &attempt = test.attempts.back();
                for (const auto raw : ImportLines(block))
                {
                    const std::string_view line = ImportTrim(raw);
                    if (ImportStartsWith(line, "duration_ms:"))
                    {
                        const std::string value{ line.substr(12) };
                        attempt.durationMicroseconds = ImportedMicroseconds(
                            std::strtod(value.c_str(), nullptr) / 1e3);
                    }
                    else if (ImportStartsWith(line, "message:")
                             && !attempt.assertions.empty())
                    {
                        attempt.assertions.front().message
                            = Unquoted(line.substr(8));
                    }
                }
                if (!attempt.assertions.empty())
                {
                    attempt.assertions.front().expansion += block;
                }
            }

            Expected<std::vector<TestResult>>
            Parse(std::string_view text, const std::string &source)
            {
                std::vector<TestResult> tests;
                std::vector<std::string> subtests;
                std::map<std::string, std::size_t> seen;
                std::optional<std::size_t> planned;
                std::size_t topLevel = 0;
                std::string bailOut;
                bool anything = false;
                std::optional<std::string> block;
                for (const auto raw : ImportLines(text))
                {
                    const std::string_view line = ImportTrim(raw);
                    if (block)
                    {
                        if (line == "...")
                        {
                            if (!tests.empty())
                            {
                                ApplyDiagnostics(tests.back(), *block);
                            }
                            block.reset();
                        }
                        else
                        {
                            block->append(raw).push_back('\n');
                        }
                        continue;
                    }
                    if (line == "---")
                    {
                        block.emplace();
                        continue;
                    }
                    std::size_t indent = 0;
                    for (const char character : raw)
                    {
                        if (character != ' ' && character != '\t')
                        {
                            break;
                        }
                        indent += character == '\t' ? 4 : 1;
                    }
                    const std::size_t depth = indent / 4;
                    if (ImportStartsWith(line, "# Subtest:"))
                    {
                        subtests.resize(depth);
                        subtests.emplace_back(ImportTrim(line.substr(10)));
                        continue;
                    }
                    if (ImportStartsWith(line, "Bail out!"))
                    {
                        anything = true;
                        bailOut = std::string{ line };
                        break;
                    }
                    if (ImportStartsWith(line, "1..") && depth == 0)
                    {
                        anything = true;
                        const std::string count{ line.substr(3) };
                        planned = static_cast<std::size_t>(
                            std::strtoull(count.c_str(), nullptr, 10));
                        continue;
                    }
                    if (line.size() > 1 && line.front() == '#' && !tests.empty()
                        && !tests.back().attempts.back().assertions.empty())
                    {
                        // Comments after a failure usually explain it.
                        auto &failure
                            = tests.back().attempts.back().assertions.front();
                        failure.expansion
                            += std::string{ ImportTrim(line.substr(1)) } + "\n";
                        continue;
                    }
                    const auto parsed = ReadLine(line);
                    if (!parsed)
                    {
                        continue;
                    }
                    anything = true;
                    topLevel += depth == 0 ? 1 : 0;
                    std::string parent = source;
                    for (std::size_t level = 0;
                         level < std::min(depth, subtests.size());
                         ++level)
                    {
                        parent
                            += (parent.empty() ? "" : " > ") + subtests[level];
                    }
                    std::string name = parsed->description;
                    if (name.empty())
                    {
                        name = "test "
                               + std::to_string(++seen["unnamed\n" + parent]);
                    }
                    // TAP has no retries: a name that comes back in one
                    // stream is another test.
                    const std::size_t repeat = ++seen[parent + "\n" + name];
                    if (repeat > 1)
                    {
                        name += " #" + std::to_string(repeat);
                    }
                    TestResult test
                        = ImportedTest(std::move(name), std::move(parent));
                    test.attempts.push_back(
                        ImportedAttempt(OutcomeOf(*parsed), 0, parsed->reason));
                    tests.push_back(std::move(test));
                }
                if (!anything)
                {
                    return Error{ "there are no TAP results in it" };
                }
                // A stream that ends early must not read as a run that passed.
                std::string incomplete;
                if (!bailOut.empty())
                {
                    incomplete = bailOut;
                }
                else if (planned && *planned != topLevel)
                {
                    incomplete = "planned " + std::to_string(*planned)
                                 + " test(s), reported "
                                 + std::to_string(topLevel);
                }
                if (!incomplete.empty())
                {
                    TestResult test = ImportedTest("the TAP stream", source);
                    test.attempts.push_back(
                        ImportedAttempt(Outcome::kFailed,
                                        0,
                                        std::move(incomplete),
                                        {},
                                        FailureCategory::kInfrastructure));
                    tests.push_back(std::move(test));
                }
                return tests;
            }
        } // namespace ImportTap
    } // namespace

    bool
    IsTapTestLine(std::string_view line)
    {
        return ImportTap::ReadLine(line).has_value();
    }

    Expected<std::vector<TestResult>>
    ParseTap(std::string_view text, const std::string &source)
    {
        return ImportTap::Parse(text, source);
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

// The XML reports: JUnit, xUnit.net v2 and Visual Studio's TRX.
#include <cstdlib>
#include <map>
#include <string_view>
#include <utility>


namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        namespace ImportXml
        {
            Expected<bool>
            Load(pugi::xml_document &document, std::string_view text)
            {
                const pugi::xml_parse_result parsed
                    = document.load_buffer(text.data(), text.size());
                if (!parsed)
                {
                    return Error{ std::string{ "it is not valid XML: " }
                                  + parsed.description() + " at byte "
                                  + std::to_string(parsed.offset) };
                }
                return true;
            }

            double
            NumberOf(const char *text)
            {
                std::string digits;
                for (; *text != '\0'; ++text)
                {
                    if (*text != ',')
                    {
                        digits.push_back(*text);
                    }
                }
                return std::strtod(digits.c_str(), nullptr);
            }

            std::string
            TextOf(pugi::xml_node node)
            {
                return node.text().get();
            }

            /// `error` and `failure` of JUnit and what Surefire writes for
            /// retries, which have the same shape.
            Attempt
            JUnitFailure(pugi::xml_node node, FailureCategory category)
            {
                std::string details = TextOf(node);
                if (const auto trace = node.child("stackTrace"))
                {
                    details = TextOf(trace);
                }
                Attempt attempt
                    = ImportedAttempt(Outcome::kFailed,
                                      ImportedMicroseconds(NumberOf(
                                          node.attribute("time").value())),
                                      node.attribute("message").value(),
                                      std::move(details),
                                      category);
                attempt.assertions.front().macro
                    = node.attribute("type").value();
                attempt.standardOutput = TextOf(node.child("system-out"));
                attempt.standardError = TextOf(node.child("system-err"));
                return attempt;
            }

            bool
            NameIs(pugi::xml_node node, std::string_view name)
            {
                return name == node.name();
            }

            bool
            NameEndsWithError(pugi::xml_node node)
            {
                return NameIs(node, "flakyError") || NameIs(node, "rerunError");
            }

            bool
            NotRunStatus(pugi::xml_node testCase)
            {
                const std::string_view status
                    = testCase.attribute("status").value();
                const std::string_view result
                    = testCase.attribute("result").value();
                return status == "notrun" || status == "skipped"
                       || status == "disabled" || result == "skipped"
                       || result == "suppressed";
            }

            void
            ReadJUnitCase(pugi::xml_node testCase,
                          pugi::xml_node suite,
                          std::vector<TestResult> &tests)
            {
                TestResult test
                    = ImportedTest(testCase.attribute("name").value(),
                                   testCase.attribute("classname").value());
                test.file = testCase.attribute("file").value();
                for (auto outer = suite; test.file.empty() && outer;
                     outer = outer.parent())
                {
                    test.file = outer.attribute("file").value();
                }
                test.line = testCase.attribute("line").as_ullong();

                std::vector<Attempt> failures;
                std::optional<Attempt> skipped;
                for (const auto child : testCase.children())
                {
                    if (NameIs(child, "flakyFailure")
                        || NameIs(child, "rerunFailure")
                        || NameEndsWithError(child))
                    {
                        test.attempts.push_back(JUnitFailure(
                            child,
                            NameEndsWithError(child)
                                ? FailureCategory::kUnexpectedException
                                : FailureCategory::kAssertion));
                    }
                    else if (NameIs(child, "failure"))
                    {
                        failures.push_back(
                            JUnitFailure(child, FailureCategory::kAssertion));
                    }
                    else if (NameIs(child, "error"))
                    {
                        failures.push_back(JUnitFailure(
                            child,
                            FailureCategory::kUnexpectedException));
                    }
                    else if (NameIs(child, "skipped"))
                    {
                        std::string reason = child.attribute("message").value();
                        skipped = ImportedAttempt(
                            Outcome::kSkipped,
                            0,
                            reason.empty() ? TextOf(child) : std::move(reason));
                    }
                }

                const std::int64_t duration = ImportedMicroseconds(
                    NumberOf(testCase.attribute("time").value()));
                Attempt verdict;
                if (!failures.empty())
                {
                    // Several failures are one attempt with several findings.
                    verdict = std::move(failures.front());
                    for (std::size_t index = 1; index < failures.size();
                         ++index)
                    {
                        auto &record = failures[index].assertions.front();
                        verdict.assertions.push_back(std::move(record));
                        ++verdict.assertionsFailed;
                    }
                    verdict.durationMicroseconds = duration;
                }
                else if (skipped)
                {
                    verdict = std::move(*skipped);
                }
                else if (NotRunStatus(testCase))
                {
                    verdict = ImportedAttempt(Outcome::kSkipped, 0);
                }
                else
                {
                    verdict = ImportedAttempt(Outcome::kPassed, duration);
                }
                if (verdict.standardOutput.empty())
                {
                    verdict.standardOutput
                        = TextOf(testCase.child("system-out"));
                }
                if (verdict.standardError.empty())
                {
                    verdict.standardError
                        = TextOf(testCase.child("system-err"));
                }
                test.attempts.push_back(std::move(verdict));
                tests.push_back(std::move(test));
            }

            void
            ReadJUnitSuite(pugi::xml_node suite, std::vector<TestResult> &tests)
            {
                for (const auto child : suite.children())
                {
                    if (NameIs(child, "testcase"))
                    {
                        ReadJUnitCase(child, suite, tests);
                    }
                    else if (NameIs(child, "testsuite"))
                    {
                        ReadJUnitSuite(child, tests);
                    }
                }
            }

            void
            ReadXUnitTest(pugi::xml_node node, std::vector<TestResult> &tests)
            {
                TestResult test = ImportedTest(node.attribute("name").value(),
                                               node.attribute("type").value());
                test.file = node.attribute("source-file").value();
                test.line = node.attribute("source-line").as_ullong();
                for (const auto trait : node.child("traits").children("trait"))
                {
                    test.tags.push_back(
                        std::string{ trait.attribute("name").value() } + "="
                        + trait.attribute("value").value());
                }
                const std::string_view result
                    = node.attribute("result").value();
                const std::int64_t duration = ImportedMicroseconds(
                    NumberOf(node.attribute("time").value()));
                Attempt attempt;
                if (result == "Pass")
                {
                    attempt = ImportedAttempt(Outcome::kPassed, duration);
                }
                else if (result == "Fail")
                {
                    const auto failure = node.child("failure");
                    attempt
                        = ImportedAttempt(Outcome::kFailed,
                                          duration,
                                          TextOf(failure.child("message")),
                                          TextOf(failure.child("stack-trace")));
                    attempt.assertions.front().macro
                        = failure.attribute("exception-type").value();
                }
                else if (result == "Skip")
                {
                    attempt = ImportedAttempt(Outcome::kSkipped,
                                              0,
                                              TextOf(node.child("reason")));
                }
                else
                {
                    attempt = ImportedAttempt(Outcome::kNotRun, 0);
                }
                if (attempt.standardOutput.empty())
                {
                    attempt.standardOutput = TextOf(node.child("output"));
                }
                test.attempts.push_back(std::move(attempt));
                tests.push_back(std::move(test));
            }

            /// A failure outside any test, such as in a fixture.
            void
            ReadXUnitErrors(pugi::xml_node assembly,
                            std::vector<TestResult> &tests)
            {
                for (const auto error :
                     assembly.child("errors").children("error"))
                {
                    const auto failure = error.child("failure");
                    TestResult test = ImportedTest(
                        std::string{ error.attribute("type").value() } + " "
                            + error.attribute("name").value(),
                        assembly.attribute("name").value());
                    test.attempts.push_back(
                        ImportedAttempt(Outcome::kFailed,
                                        0,
                                        TextOf(failure.child("message")),
                                        TextOf(failure.child("stack-trace")),
                                        FailureCategory::kInfrastructure));
                    tests.push_back(std::move(test));
                }
            }

            void
            ReadXUnitAssembly(pugi::xml_node assembly,
                              std::vector<TestResult> &tests)
            {
                ReadXUnitErrors(assembly, tests);
                for (const auto child : assembly.children())
                {
                    if (NameIs(child, "test"))
                    {
                        ReadXUnitTest(child, tests);
                    }
                    else if (NameIs(child, "collection"))
                    {
                        for (const auto test : child.children("test"))
                        {
                            ReadXUnitTest(test, tests);
                        }
                    }
                }
            }

            /// `[days.]hours:minutes:seconds[.fraction]`.
            std::int64_t
            TrxDuration(std::string_view text)
            {
                double seconds = 0.0;
                std::string field;
                const auto addField = [&] {
                    seconds = seconds * 60.0 + NumberOf(field.c_str());
                    field.clear();
                };
                double days = 0.0;
                for (std::size_t index = 0; index < text.size(); ++index)
                {
                    if (text[index] == ':')
                    {
                        addField();
                    }
                    else if (text[index] == '.'
                             && text.find(':', index) != std::string_view::npos)
                    {
                        days = NumberOf(field.c_str());
                        field.clear();
                    }
                    else
                    {
                        field.push_back(text[index]);
                    }
                }
                addField();
                return ImportedMicroseconds(seconds + days * 86400.0);
            }

            struct TrxOutcome final
            {
                Outcome outcome = Outcome::kNotRun;
                FailureCategory category = FailureCategory::kNone;
            };

            TrxOutcome
            TrxOutcomeOf(std::string_view outcome)
            {
                if (outcome == "Passed" || outcome == "PassedButRunAborted"
                    || outcome == "Warning" || outcome == "Completed")
                {
                    return { Outcome::kPassed, FailureCategory::kNone };
                }
                if (outcome == "Failed")
                {
                    return { Outcome::kFailed, FailureCategory::kAssertion };
                }
                if (outcome == "Error")
                {
                    return { Outcome::kFailed,
                             FailureCategory::kUnexpectedException };
                }
                if (outcome == "Timeout")
                {
                    return { Outcome::kTimedOut, FailureCategory::kTimeout };
                }
                if (outcome == "Aborted")
                {
                    return { Outcome::kCrashed, FailureCategory::kCrash };
                }
                if (outcome == "Disconnected")
                {
                    return { Outcome::kCrashed,
                             FailureCategory::kInfrastructure };
                }
                if (outcome == "NotExecuted" || outcome == "Inconclusive"
                    || outcome == "NotRunnable")
                {
                    return { Outcome::kSkipped, FailureCategory::kNone };
                }
                return {};
            }

            /// The class of a test: the type without its assembly.
            std::string
            TrxClass(std::string_view className)
            {
                return std::string{ className.substr(0, className.find(',')) };
            }

            void
            ReadTrxResult(pugi::xml_node result,
                          const std::map<std::string, std::string> &classes,
                          std::vector<TestResult> &tests)
            {
                // A data-driven test: the rows are what ran.
                if (const auto inner = result.child("InnerResults"))
                {
                    for (const auto row : inner.children())
                    {
                        ReadTrxResult(row, classes, tests);
                    }
                    return;
                }
                const auto found
                    = classes.find(result.attribute("testId").value());
                TestResult test = ImportedTest(
                    result.attribute("testName").value(),
                    found == classes.end() ? std::string{} : found->second);
                const auto [outcome, category]
                    = TrxOutcomeOf(result.attribute("outcome").value());
                const auto output = result.child("Output");
                const auto error = output.child("ErrorInfo");
                std::string message = TextOf(error.child("Message"));
                Attempt attempt = ImportedAttempt(
                    outcome,
                    TrxDuration(result.attribute("duration").value()),
                    outcome == Outcome::kSkipped
                        ? TextOf(output.child("StdOut"))
                        : std::move(message),
                    TextOf(error.child("StackTrace")),
                    category);
                if (outcome != Outcome::kSkipped)
                {
                    attempt.standardOutput = TextOf(output.child("StdOut"));
                }
                attempt.standardError = TextOf(output.child("StdErr"));
                test.attempts.push_back(std::move(attempt));
                tests.push_back(std::move(test));
            }
        } // namespace ImportXml
    } // namespace

    Expected<std::vector<TestResult>>
    ParseJUnit(std::string_view text)
    {
        pugi::xml_document document;
        if (auto loaded = ImportXml::Load(document, text); !loaded)
        {
            return loaded.GetError();
        }
        const pugi::xml_node root = document.document_element();
        std::vector<TestResult> tests;
        if (ImportXml::NameIs(root, "testsuites")
            || ImportXml::NameIs(root, "testsuite"))
        {
            ImportXml::ReadJUnitSuite(root, tests);
            return tests;
        }
        return Error{ "a JUnit report has testsuites or testsuite as its root, "
                      "not "
                      + std::string{ root.name() } };
    }

    Expected<std::vector<TestResult>>
    ParseXUnit(std::string_view text)
    {
        pugi::xml_document document;
        if (auto loaded = ImportXml::Load(document, text); !loaded)
        {
            return loaded.GetError();
        }
        const pugi::xml_node root = document.document_element();
        std::vector<TestResult> tests;
        if (ImportXml::NameIs(root, "assembly"))
        {
            ImportXml::ReadXUnitAssembly(root, tests);
            return tests;
        }
        if (ImportXml::NameIs(root, "assemblies"))
        {
            for (const auto assembly : root.children("assembly"))
            {
                ImportXml::ReadXUnitAssembly(assembly, tests);
            }
            return tests;
        }
        return Error{ "an xUnit report has assemblies or assembly as its root, "
                      "not "
                      + std::string{ root.name() } };
    }

    Expected<std::vector<TestResult>>
    ParseTrx(std::string_view text)
    {
        pugi::xml_document document;
        if (auto loaded = ImportXml::Load(document, text); !loaded)
        {
            return loaded.GetError();
        }
        const pugi::xml_node root = document.document_element();
        if (!ImportXml::NameIs(root, "TestRun"))
        {
            return Error{ "a TRX report has TestRun as its root, not "
                          + std::string{ root.name() } };
        }
        std::map<std::string, std::string> classes;
        for (const auto definition : root.child("TestDefinitions").children())
        {
            classes[definition.attribute("id").value()] = ImportXml::TrxClass(
                definition.child("TestMethod").attribute("className").value());
        }
        std::vector<TestResult> tests;
        for (const auto result : root.child("Results").children())
        {
            ImportXml::ReadTrxResult(result, classes, tests);
        }
        return tests;
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        JsonValue
        Strings(const std::vector<std::string> &values)
        {
            JsonValue array = JsonValue::Array();
            for (const auto &value : values)
            {
                array.Append(value);
            }
            return array;
        }

        std::vector<std::string>
        StringsFrom(const JsonValue &value)
        {
            std::vector<std::string> strings;
            for (const auto &item : value.Items())
            {
                strings.push_back(item.AsString());
            }
            return strings;
        }

        std::size_t
        Size(const JsonValue &value)
        {
            return static_cast<std::size_t>(value.AsUnsigned());
        }

        /// Adds a member only when it says something, so reports and
        /// protocol messages stay small.
        void
        SetText(JsonValue &object, std::string_view key, const std::string &text)
        {
            if (!text.empty())
            {
                object.Set(key, text);
            }
        }
    } // namespace

    JsonValue
    ToJson(const AssertionRecord &assertion)
    {
        JsonValue value = JsonValue::Object();
        value.Set("file", assertion.file);
        value.Set("line", assertion.line);
        SetText(value, "macro", assertion.macro);
        SetText(value, "expression", assertion.expression);
        SetText(value, "expansion", assertion.expansion);
        SetText(value, "message", assertion.message);
        if (!assertion.context.empty())
        {
            value.Set("context", Strings(assertion.context));
        }
        if (!assertion.sections.empty())
        {
            value.Set("sections", Strings(assertion.sections));
        }
        value.Set("category", CategoryName(assertion.category));
        value.Set("passed", assertion.passed);
        if (assertion.allowed)
        {
            value.Set("allowed", true);
        }
        return value;
    }

    AssertionRecord
    AssertionFromJson(const JsonValue &value)
    {
        AssertionRecord assertion;
        assertion.file = value["file"].AsString();
        assertion.line = Size(value["line"]);
        assertion.macro = value["macro"].AsString();
        assertion.expression = value["expression"].AsString();
        assertion.expansion = value["expansion"].AsString();
        assertion.message = value["message"].AsString();
        assertion.context = StringsFrom(value["context"]);
        assertion.sections = StringsFrom(value["sections"]);
        assertion.category = CategoryFromName(value["category"].AsString())
                                 .value_or(FailureCategory::kAssertion);
        assertion.passed = value["passed"].AsBoolean();
        assertion.allowed = value["allowed"].AsBoolean();
        return assertion;
    }

    JsonValue
    ToJson(const BenchmarkRecord &benchmark)
    {
        JsonValue value = JsonValue::Object();
        value.Set("name", benchmark.name);
        value.Set("samples", benchmark.samples);
        value.Set("iterations", benchmark.iterations);
        value.Set("meanNanoseconds", benchmark.meanNanoseconds);
        value.Set("lowMeanNanoseconds", benchmark.lowMeanNanoseconds);
        value.Set("highMeanNanoseconds", benchmark.highMeanNanoseconds);
        value.Set("standardDeviationNanoseconds",
                  benchmark.standardDeviationNanoseconds);
        value.Set("outlierVariance", benchmark.outlierVariance);
        return value;
    }

    BenchmarkRecord
    BenchmarkFromJson(const JsonValue &value)
    {
        BenchmarkRecord benchmark;
        benchmark.name = value["name"].AsString();
        benchmark.samples = Size(value["samples"]);
        benchmark.iterations = Size(value["iterations"]);
        benchmark.meanNanoseconds = value["meanNanoseconds"].AsNumber();
        benchmark.lowMeanNanoseconds = value["lowMeanNanoseconds"].AsNumber();
        benchmark.highMeanNanoseconds = value["highMeanNanoseconds"].AsNumber();
        benchmark.standardDeviationNanoseconds =
            value["standardDeviationNanoseconds"].AsNumber();
        benchmark.outlierVariance = value["outlierVariance"].AsNumber();
        return benchmark;
    }

    JsonValue
    ToJson(const SectionRecord &section)
    {
        JsonValue value = JsonValue::Object();
        value.Set("name", section.name);
        value.Set("file", section.file);
        value.Set("line", section.line);
        value.Set("depth", section.depth);
        value.Set("startMicroseconds", section.startMicroseconds);
        value.Set("durationMicroseconds", section.durationMicroseconds);
        return value;
    }

    SectionRecord
    SectionFromJson(const JsonValue &value)
    {
        SectionRecord section;
        section.name = value["name"].AsString();
        section.file = value["file"].AsString();
        section.line = Size(value["line"]);
        section.depth = Size(value["depth"]);
        section.startMicroseconds = value["startMicroseconds"].AsInteger();
        section.durationMicroseconds =
            value["durationMicroseconds"].AsInteger();
        return section;
    }

    JsonValue
    ToJson(const Attempt &attempt)
    {
        JsonValue value = JsonValue::Object();
        value.Set("number", attempt.number);
        value.Set("outcome", OutcomeName(attempt.outcome));
        if (attempt.category != FailureCategory::kNone)
        {
            value.Set("category", CategoryName(attempt.category));
        }
        value.Set("worker", attempt.worker);
        value.Set("freshProcess", attempt.freshProcess);
        value.Set("seed", attempt.seed);
        value.Set("startMicroseconds", attempt.startMicroseconds);
        value.Set("durationMicroseconds", attempt.durationMicroseconds);
        value.Set("assertionsPassed", attempt.assertionsPassed);
        value.Set("assertionsFailed", attempt.assertionsFailed);
        if (!attempt.precededBy.empty())
        {
            value.Set("precededBy", Strings(attempt.precededBy));
        }
        JsonValue assertions = JsonValue::Array();
        for (const auto &assertion : attempt.assertions)
        {
            assertions.Append(ToJson(assertion));
        }
        value.Set("assertions", std::move(assertions));
        if (!attempt.benchmarks.empty())
        {
            JsonValue benchmarks = JsonValue::Array();
            for (const auto &benchmark : attempt.benchmarks)
            {
                benchmarks.Append(ToJson(benchmark));
            }
            value.Set("benchmarks", std::move(benchmarks));
        }
        if (!attempt.sections.empty())
        {
            JsonValue sections = JsonValue::Array();
            for (const auto &section : attempt.sections)
            {
                sections.Append(ToJson(section));
            }
            value.Set("sections", std::move(sections));
        }
        SetText(value, "standardOutput", attempt.standardOutput);
        SetText(value, "standardError", attempt.standardError);
        SetText(value, "processOutput", attempt.processOutput);
        SetText(value, "fatalMessage", attempt.fatalMessage);
        SetText(value, "exitDescription", attempt.exitDescription);
        SetText(value, "lastLocation", attempt.lastLocation);
        SetText(value, "temporaryDirectory", attempt.temporaryDirectory);
        return value;
    }

    Attempt
    AttemptFromJson(const JsonValue &value)
    {
        Attempt attempt;
        attempt.number = Size(value["number"]);
        attempt.outcome =
            OutcomeFromName(value["outcome"].AsString()).value_or(Outcome::kNotRun);
        attempt.category = CategoryFromName(value["category"].AsString())
                               .value_or(FailureCategory::kNone);
        attempt.worker = Size(value["worker"]);
        attempt.freshProcess = value["freshProcess"].AsBoolean();
        attempt.seed = static_cast<std::uint32_t>(value["seed"].AsUnsigned());
        attempt.startMicroseconds = value["startMicroseconds"].AsInteger();
        attempt.durationMicroseconds = value["durationMicroseconds"].AsInteger();
        attempt.assertionsPassed = Size(value["assertionsPassed"]);
        attempt.assertionsFailed = Size(value["assertionsFailed"]);
        attempt.precededBy = StringsFrom(value["precededBy"]);
        for (const auto &item : value["assertions"].Items())
        {
            attempt.assertions.push_back(AssertionFromJson(item));
        }
        for (const auto &item : value["benchmarks"].Items())
        {
            attempt.benchmarks.push_back(BenchmarkFromJson(item));
        }
        for (const auto &item : value["sections"].Items())
        {
            attempt.sections.push_back(SectionFromJson(item));
        }
        attempt.standardOutput = value["standardOutput"].AsString();
        attempt.standardError = value["standardError"].AsString();
        attempt.processOutput = value["processOutput"].AsString();
        attempt.fatalMessage = value["fatalMessage"].AsString();
        attempt.exitDescription = value["exitDescription"].AsString();
        attempt.lastLocation = value["lastLocation"].AsString();
        attempt.temporaryDirectory = value["temporaryDirectory"].AsString();
        return attempt;
    }

    JsonValue
    ToJson(const TestResult &test)
    {
        JsonValue value = JsonValue::Object();
        value.Set("id", test.id);
        value.Set("name", test.name);
        SetText(value, "className", test.className);
        value.Set("file", test.file);
        value.Set("line", test.line);
        value.Set("tags", Strings(test.tags));
        value.Set("outcome", OutcomeName(test.outcome));
        if (test.category != FailureCategory::kNone)
        {
            value.Set("category", CategoryName(test.category));
        }
        value.Set("history", HistoryName(test.history));
        value.Set("flaky", test.flaky);
        value.Set("quarantined", test.quarantined);
        value.Set("durationMicroseconds", test.DurationMicroseconds());
        SetText(value, "selectionReason", test.selectionReason);
        SetText(value, "signature", test.signature);
        if (!test.analysis.empty())
        {
            value.Set("analysis", Strings(test.analysis));
        }
        SetText(value, "replayCommand", test.replayCommand);
        value.Set("predictedDurationMicroseconds",
                  test.predictedDurationMicroseconds);
        value.Set("predictedFailureProbability",
                  test.predictedFailureProbability);
        JsonValue attempts = JsonValue::Array();
        for (const auto &attempt : test.attempts)
        {
            attempts.Append(ToJson(attempt));
        }
        value.Set("attempts", std::move(attempts));
        return value;
    }

    TestResult
    TestResultFromJson(const JsonValue &value)
    {
        TestResult test;
        test.id = value["id"].AsString();
        test.name = value["name"].AsString();
        test.className = value["className"].AsString();
        test.file = value["file"].AsString();
        test.line = Size(value["line"]);
        test.tags = StringsFrom(value["tags"]);
        test.outcome =
            OutcomeFromName(value["outcome"].AsString()).value_or(Outcome::kNotRun);
        test.category = CategoryFromName(value["category"].AsString())
                            .value_or(FailureCategory::kNone);
        const std::string &history = value["history"].AsString();
        test.history = history == "new"            ? FailureHistory::kNew
                       : history == "persistent"   ? FailureHistory::kPersistent
                       : history == "intermittent" ? FailureHistory::kIntermittent
                                                   : FailureHistory::kUnknown;
        test.flaky = value["flaky"].AsBoolean();
        test.quarantined = value["quarantined"].AsBoolean();
        test.selectionReason = value["selectionReason"].AsString();
        test.signature = value["signature"].AsString();
        test.analysis = StringsFrom(value["analysis"]);
        test.replayCommand = value["replayCommand"].AsString();
        test.predictedDurationMicroseconds =
            value["predictedDurationMicroseconds"].AsInteger();
        test.predictedFailureProbability =
            value["predictedFailureProbability"].AsNumber();
        for (const auto &item : value["attempts"].Items())
        {
            test.attempts.push_back(AttemptFromJson(item));
        }
        return test;
    }

    JsonValue
    ToJson(const RunResult &run)
    {
        const RunSummary summary = Summarize(run);
        JsonValue value = JsonValue::Object();
        value.Set("format", "catch3-run");
        value.Set("version", kResultsFormatVersion);
        value.Set("binary", run.binary);
        value.Set("seed", run.seed);
        value.Set("workers", run.workers);
        value.Set("shardIndex", run.shardIndex);
        value.Set("shardCount", run.shardCount);
        value.Set("candidates", run.candidates);
        value.Set("startUnixMilliseconds", run.startUnixMilliseconds);
        value.Set("wallMicroseconds", run.wallMicroseconds);
        JsonValue totals = JsonValue::Object();
        totals.Set("tests", summary.tests);
        totals.Set("passed", summary.passed);
        totals.Set("failed", summary.failed);
        totals.Set("failedAsExpected", summary.failedAsExpected);
        totals.Set("skipped", summary.skipped);
        totals.Set("crashed", summary.crashed);
        totals.Set("timedOut", summary.timedOut);
        totals.Set("blocked", summary.blocked);
        totals.Set("notRun", summary.notRun);
        totals.Set("upToDate", summary.upToDate);
        totals.Set("flaky", summary.flaky);
        totals.Set("quarantined", summary.quarantined);
        totals.Set("attempts", summary.attempts);
        totals.Set("assertionsPassed", summary.assertionsPassed);
        totals.Set("assertionsFailed", summary.assertionsFailed);
        totals.Set("testMicroseconds", summary.testMicroseconds);
        value.Set("summary", std::move(totals));
        value.Set("unmatchedSpecs", Strings(run.unmatchedSpecs));
        value.Set("notes", Strings(run.notes));
        JsonValue tests = JsonValue::Array();
        for (const auto &test : run.tests)
        {
            tests.Append(ToJson(test));
        }
        value.Set("tests", std::move(tests));
        return value;
    }

    Expected<RunResult>
    RunResultFromJson(const JsonValue &value)
    {
        if (value["format"].AsString() != "catch3-run")
        {
            return Error{"not a Catch3 run report"};
        }
        const auto version = value["version"].AsInteger();
        if (version < 1 || version > kResultsFormatVersion)
        {
            return Error{"report version " + std::to_string(version) +
                         " is newer than this Catch3 reads (" +
                         std::to_string(kResultsFormatVersion) + ")"};
        }
        RunResult run;
        run.binary = value["binary"].AsString();
        run.seed = static_cast<std::uint32_t>(value["seed"].AsUnsigned());
        run.workers = Size(value["workers"]);
        run.shardIndex = static_cast<unsigned>(value["shardIndex"].AsUnsigned());
        run.shardCount =
            static_cast<unsigned>(value["shardCount"].AsUnsigned(1));
        run.candidates = Size(value["candidates"]);
        run.startUnixMilliseconds = value["startUnixMilliseconds"].AsInteger();
        run.wallMicroseconds = value["wallMicroseconds"].AsInteger();
        run.unmatchedSpecs = StringsFrom(value["unmatchedSpecs"]);
        run.notes = StringsFrom(value["notes"]);
        for (const auto &item : value["tests"].Items())
        {
            run.tests.push_back(TestResultFromJson(item));
        }
        return run;
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <iterator>
#include <map>
#include <random>
#include <system_error>


namespace Progmasoft::Catch3
{
    namespace
    {
        using Runner::JsonValue;

        void
        PrintError(const std::string &message)
        {
            std::fprintf(stderr, "error: %s\n", message.c_str());
        }

        /// Problems found while the tests registered, such as two tests of
        /// the same name; reported before anything runs.
        bool
        ReportStartupErrors()
        {
#if !defined(CATCH_CONFIG_DISABLE_EXCEPTIONS)
            const auto &exceptions = Catch::getRegistryHub()
                                         .getStartupExceptionRegistry()
                                         .getExceptions();
            if (exceptions.empty())
            {
                return false;
            }
            std::fprintf(stderr, "error: the tests could not be registered:\n");
            for (const auto &exception : exceptions)
            {
                try
                {
                    std::rethrow_exception(exception);
                }
                catch (const std::exception &error)
                {
                    std::fprintf(stderr, "  %s\n", error.what());
                }
                catch (...)
                {
                    std::fprintf(stderr, "  an unknown exception\n");
                }
            }
            return true;
#else
            return false;
#endif
        }

        bool
        BazelTest()
        {
#if defined(CATCH_CONFIG_BAZEL_SUPPORT)
            return true;
#else
            return Runner::HasEnvironment("BAZEL_TEST");
#endif
        }

        /// Bazel passes what it wants through the environment; it wins over
        /// the command line, as `bazel test --test_filter` expects.
        void
        ApplyBazel(Runner::Options &options, std::vector<std::string> &notes)
        {
            const std::string xml = Runner::ReadEnvironment("XML_OUTPUT_FILE");
            if (!xml.empty() && options.junitPath.empty())
            {
                options.junitPath = xml;
            }
            const std::string filter
                = Runner::ReadEnvironment("TESTBRIDGE_TEST_ONLY");
            if (!filter.empty())
            {
                options.testSpecs = { filter };
            }
            const std::string index
                = Runner::ReadEnvironment("TEST_SHARD_INDEX");
            const std::string total
                = Runner::ReadEnvironment("TEST_TOTAL_SHARDS");
            const std::string status
                = Runner::ReadEnvironment("TEST_SHARD_STATUS_FILE");
            if (!index.empty() && !total.empty())
            {
                auto parsed = Runner::ParseOptions(
                    { "--shard=" + index + "/" + total });
                if (parsed)
                {
                    options.shard = parsed->shard;
                    if (!status.empty())
                    {
                        // Bazel needs to see that the shard was read.
                        static_cast<void>(
                            Runner::WriteFileAtomically(status, ""));
                    }
                }
                else
                {
                    notes.push_back("ignoring Bazel's shard " + index + "/"
                                    + total + ": "
                                    + parsed.GetError().Message());
                }
            }
            const std::string seed
                = Runner::ReadEnvironment("TEST_RANDOM_SEED");
            if (!seed.empty())
            {
                auto parsed = Runner::ParseOptions({ "--seed=" + seed });
                if (parsed)
                {
                    options.seed = parsed->seed;
                    options.seedGiven = true;
                }
            }
            const std::string outputs
                = Runner::ReadEnvironment("TEST_UNDECLARED_OUTPUTS_DIR");
            if (!outputs.empty() && options.jsonPath.empty())
            {
                options.jsonPath
                    = (std::filesystem::path{ outputs } / "catch3.json")
                          .string();
            }
            if (options.historyDirectory.empty()
                && !Runner::HasEnvironment("CATCH3_HISTORY"))
            {
                // A sandboxed test has nowhere lasting to keep history.
                options.historyEnabled = false;
            }
        }

        std::filesystem::path
        HistoryDirectory(const Runner::Options &options,
                         const std::filesystem::path &program)
        {
            if (!options.historyDirectory.empty())
            {
                return options.historyDirectory;
            }
            const std::string fromEnvironment
                = Runner::ReadEnvironment("CATCH3_HISTORY");
            if (!fromEnvironment.empty())
            {
                return fromEnvironment;
            }
            return program.parent_path() / ".catch3";
        }

        JsonValue
        Strings(const std::vector<std::string> &values)
        {
            JsonValue array = JsonValue::Array();
            for (const auto &value : values)
            {
                array.Append(value);
            }
            return array;
        }

        /// The discovery protocol: what `--list=json` prints. Its shape is
        /// fixed for a given version; IDE adapters and CMake read it.
        std::string
        ListJson(const Runner::Catalog &catalog,
                 const std::vector<std::size_t> &matched,
                 const std::string &programName)
        {
            JsonValue list = JsonValue::Object();
            list.Set("format", "catch3-tests");
            list.Set("version", 1);
            list.Set("binary", programName);
            list.Set("catch3", kVersion);
            JsonValue tests = JsonValue::Array();
            for (const std::size_t index : matched)
            {
                const auto &entry = catalog.Tests()[index];
                JsonValue test = JsonValue::Object();
                test.Set("id", entry.key);
                test.Set("name", entry.name);
                test.Set("className", entry.className);
                test.Set("file", entry.file);
                test.Set("line", entry.line);
                test.Set("tags", Strings(entry.tags));
                test.Set("hidden", entry.hidden);
                test.Set("serial", entry.serial);
                test.Set("isolated", entry.isolated);
                test.Set("quarantined", entry.quarantined);
                test.Set("resources", Strings(entry.resources));
                test.Set("dependsOn", Strings(entry.dependsOn));
                if (entry.timeout)
                {
                    test.Set("timeoutSeconds",
                             static_cast<double>(entry.timeout->count())
                                 / 1000.0);
                }
                if (entry.retries)
                {
                    test.Set("retries", *entry.retries);
                }
                tests.Append(std::move(test));
            }
            list.Set("tests", std::move(tests));
            return list.ToPrettyString();
        }

        int
        List(const Runner::Options &options,
             const Runner::Catalog &catalog,
             const std::vector<std::size_t> &matched,
             const std::string &programName)
        {
            if (options.mode == Runner::Mode::kListTags)
            {
                std::map<std::string, std::size_t> counts;
                for (const std::size_t index : matched)
                {
                    for (const auto &tag : catalog.Tests()[index].tags)
                    {
                        ++counts[tag];
                    }
                }
                for (const auto &[tag, count] : counts)
                {
                    std::printf("%6zu  [%s]\n", count, tag.c_str());
                }
                std::printf("%zu tag(s)\n", counts.size());
                return 0;
            }
            switch (options.listFormat)
            {
                case Runner::ListFormat::kJson:
                    std::fputs(ListJson(catalog, matched, programName).c_str(),
                               stdout);
                    break;
                case Runner::ListFormat::kNames:
                    for (const std::size_t index : matched)
                    {
                        std::printf("%s\n",
                                    catalog.Tests()[index].name.c_str());
                    }
                    break;
                case Runner::ListFormat::kText:
                    for (const std::size_t index : matched)
                    {
                        const auto &entry = catalog.Tests()[index];
                        std::string tags;
                        for (const auto &tag : entry.tags)
                        {
                            tags += "[" + tag + "]";
                        }
                        std::printf("%s\n    %s:%zu\n",
                                    entry.name.c_str(),
                                    entry.file.c_str(),
                                    entry.line);
                        if (!tags.empty())
                        {
                            std::printf("    %s\n", tags.c_str());
                        }
                    }
                    std::printf("%zu matching test(s)\n", matched.size());
                    break;
            }
            return 0;
        }

        std::string
        FormatMicroseconds(std::int64_t microseconds)
        {
            char buffer[32];
            std::snprintf(buffer,
                          sizeof(buffer),
                          "%.3f s",
                          static_cast<double>(microseconds) / 1e6);
            return buffer;
        }

        /// What the history says about the tests: `--analytics`.
        int
        Analytics(const Runner::Options &options,
                  const Runner::History &history,
                  const std::filesystem::path &file)
        {
            struct Row
            {
                std::string key;
                double failureRate = 0.0;
                std::size_t runs = 0;
                std::size_t flakyEvents = 0;
                std::int64_t median = 0;
                std::int64_t recentMedian = 0;
                std::int64_t olderMedian = 0;
                std::string quarantine;
            };
            std::vector<Row> rows;
            for (const auto &[key, test] : history.Tests())
            {
                Row row;
                row.key = key;
                std::size_t failures = 0;
                std::vector<std::int64_t> recent;
                std::vector<std::int64_t> older;
                std::size_t position = 0;
                for (auto record = test.records.rbegin();
                     record != test.records.rend();
                     ++record, ++position)
                {
                    ++row.runs;
                    failures += IsFailure(record->outcome) ? 1 : 0;
                    if (record->outcome == Outcome::kPassed)
                    {
                        (position < 5 ? recent : older)
                            .push_back(record->durationMicroseconds);
                    }
                }
                const auto median = [](std::vector<std::int64_t> values) {
                    if (values.empty())
                    {
                        return std::int64_t{ 0 };
                    }
                    std::sort(values.begin(), values.end());
                    return values[values.size() / 2];
                };
                row.failureRate = row.runs == 0
                                      ? 0.0
                                      : static_cast<double>(failures)
                                            / static_cast<double>(row.runs);
                row.flakyEvents = test.FlakyEvents(20);
                row.median = test.DurationPercentile(0.5).value_or(0);
                row.recentMedian = median(recent);
                row.olderMedian = median(older);
                if (test.quarantine)
                {
                    row.quarantine = test.quarantine->reason;
                }
                rows.push_back(std::move(row));
            }
            const auto top = [&](auto key, std::size_t count) {
                std::vector<const Row *> ordered;
                for (const auto &row : rows)
                {
                    ordered.push_back(&row);
                }
                std::stable_sort(ordered.begin(),
                                 ordered.end(),
                                 [&](const Row *left, const Row *right) {
                                     return key(*left) > key(*right);
                                 });
                ordered.resize(std::min(count, ordered.size()));
                return ordered;
            };
            const auto flaky = top(
                [](const Row &row) {
                    return static_cast<double>(row.flakyEvents);
                },
                10);
            const auto failing = top(
                [](const Row &row) {
                    return row.failureRate;
                },
                10);
            const auto slowest = top(
                [](const Row &row) {
                    return static_cast<double>(row.median);
                },
                10);
            const auto slower = top(
                [](const Row &row) {
                    return row.olderMedian > 0
                               ? static_cast<double>(row.recentMedian)
                                     / static_cast<double>(row.olderMedian)
                               : 0.0;
                },
                10);

            if (options.analyticsJson)
            {
                JsonValue report = JsonValue::Object();
                report.Set("format", "catch3-analytics");
                report.Set("version", 1);
                report.Set("runs", history.Runs().size());
                report.Set("tests", rows.size());
                JsonValue tests = JsonValue::Array();
                for (const auto &row : rows)
                {
                    JsonValue item = JsonValue::Object();
                    item.Set("id", row.key);
                    item.Set("runs", row.runs);
                    item.Set("failureRate", row.failureRate);
                    item.Set("flakyEvents", row.flakyEvents);
                    item.Set("medianMicroseconds", row.median);
                    item.Set("recentMedianMicroseconds", row.recentMedian);
                    item.Set("olderMedianMicroseconds", row.olderMedian);
                    if (!row.quarantine.empty())
                    {
                        item.Set("quarantine", row.quarantine);
                    }
                    tests.Append(std::move(item));
                }
                report.Set("tests", std::move(tests));
                std::fputs(report.ToPrettyString().c_str(), stdout);
                return 0;
            }
            std::printf("history: %s\n%zu run(s), %zu test(s)\n",
                        file.string().c_str(),
                        history.Runs().size(),
                        rows.size());
            std::printf("\nflaky (last 20 runs):\n");
            for (const Row *row : flaky)
            {
                if (row->flakyEvents > 0)
                {
                    std::printf("  %3zu  %s\n",
                                row->flakyEvents,
                                row->key.c_str());
                }
            }
            std::printf("\nin quarantine:\n");
            for (const auto &row : rows)
            {
                if (!row.quarantine.empty())
                {
                    std::printf("  %s: %s\n",
                                row.key.c_str(),
                                row.quarantine.c_str());
                }
            }
            std::printf("\nfailing most often:\n");
            for (const Row *row : failing)
            {
                if (row->failureRate > 0.0)
                {
                    std::printf("  %3.0f%%  %s\n",
                                row->failureRate * 100.0,
                                row->key.c_str());
                }
            }
            std::printf("\nslowest (median):\n");
            for (const Row *row : slowest)
            {
                std::printf("  %10s  %s\n",
                            FormatMicroseconds(row->median).c_str(),
                            row->key.c_str());
            }
            std::printf(
                "\ngetting slower (last 5 runs against the 15 before):\n");
            for (const Row *row : slower)
            {
                if (row->olderMedian > 0
                    && row->recentMedian > row->olderMedian * 3 / 2)
                {
                    std::printf("  %10s -> %-10s  %s\n",
                                FormatMicroseconds(row->olderMedian).c_str(),
                                FormatMicroseconds(row->recentMedian).c_str(),
                                row->key.c_str());
                }
            }
            return 0;
        }

        std::filesystem::path
        SourceDirectory(const Runner::Catalog &catalog)
        {
            for (const auto &entry : catalog.Tests())
            {
                const std::filesystem::path file{ entry.file };
                if (file.is_absolute())
                {
                    return file.parent_path();
                }
            }
            std::error_code error;
            return std::filesystem::current_path(error);
        }

        std::int64_t
        UnixMilliseconds()
        {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                .count();
        }

        /// Writes the reports that were asked for. One that is missing would
        /// let a build server miss the run, so it fails the run.
        bool
        WriteReports(const Runner::Options &options, const RunResult &run)
        {
            bool written = true;
            const auto write = [&](const std::string &path,
                                   const std::string &contents) {
                if (auto status = Runner::WriteFileAtomically(path, contents);
                    !status)
                {
                    PrintError(status.GetError().Message());
                    written = false;
                }
            };
            if (!options.junitPath.empty())
            {
                write(options.junitPath, JUnitXmlWriter::ToString(run));
            }
            if (!options.jsonPath.empty())
            {
                write(options.jsonPath, Runner::ToJson(run).ToPrettyString());
            }
            if (!options.tracePath.empty())
            {
                write(options.tracePath, Runner::TraceJson(run));
            }
            return written;
        }

        void
        SaveHistory(const std::filesystem::path &file,
                    const RunResult &run,
                    const std::string &commit,
                    const std::string &binaryFingerprint,
                    const std::map<std::string, std::string> &fingerprints)
        {
            if (run.tests.empty())
            {
                return;
            }
            Runner::RunRecord record;
            record.unixMilliseconds = run.startUnixMilliseconds;
            record.commit = commit;
            record.binaryFingerprint = binaryFingerprint;
            record.seed = run.seed;
            record.tests = run.tests.size();
            record.wallMicroseconds = run.wallMicroseconds;
            for (const auto &test : run.tests)
            {
                record.failed += test.FailsRun() ? 1 : 0;
            }
            auto committed = Runner::History::Commit(file,
                                                     run,
                                                     record,
                                                     fingerprints,
                                                     {});
            if (!committed)
            {
                std::fprintf(stderr,
                             "warning: the history was not saved: %s\n",
                             committed.GetError().Message().c_str());
            }
        }

        int
        ExitCodeOf(const Runner::Options &options,
                   const RunResult &run,
                   bool reportMissing)
        {
            const bool quarantineCounts
                = options.quarantine == Runner::QuarantineMode::kReport;
            bool failed = false;
            std::size_t ran = 0;
            std::size_t skipped = 0;
            for (const auto &test : run.tests)
            {
                if (test.outcome != Outcome::kUpToDate)
                {
                    ++ran;
                }
                skipped += test.outcome == Outcome::kSkipped ? 1 : 0;
                failed = failed || test.FailsRun()
                         || (quarantineCounts && test.quarantined
                             && IsFailure(test.outcome));
            }
            if (failed)
            {
                return Catch::TestFailureExitCode;
            }
            if (reportMissing)
            {
                return Catch::UnspecifiedErrorExitCode;
            }
            if (run.tests.empty() && !options.allowNoTests)
            {
                return Catch::NoTestsRunExitCode;
            }
            if (ran > 0 && skipped == ran && !options.allowNoTests)
            {
                return Catch::AllTestsSkippedExitCode;
            }
            return 0;
        }

        /// The file results are remembered in.
        std::filesystem::path
        HistoryFileOf(const Runner::Options &options,
                      const std::filesystem::path &program,
                      const std::string &programName)
        {
            std::string name = programName;
            if (options.mode == Runner::Mode::kImport
                || (options.mode == Runner::Mode::kAnalytics
                    && !options.importName.empty()))
            {
                name = options.importName.empty() ? "imported"
                                                  : options.importName;
            }
            return Runner::HistoryFile(HistoryDirectory(options, program),
                                       name);
        }

        /// Reports of other frameworks: `--import`. What they say goes
        /// through the analysis, the history and the reports of a run.
        int
        Import(const Runner::Options &options,
               const std::filesystem::path &program,
               const std::string &programName)
        {
            std::vector<TestResult> imported;
            for (const auto &file : options.importFiles)
            {
                auto text = Runner::ReadFile(file);
                if (!text)
                {
                    PrintError(text.GetError().Message());
                    return Catch::UnspecifiedErrorExitCode;
                }
                auto tests = Runner::ParseImport(
                    *text,
                    options.importFormat,
                    std::filesystem::path{ file }.generic_string());
                if (!tests)
                {
                    PrintError(tests.GetError().Context(file).Message());
                    return Catch::UnspecifiedErrorExitCode;
                }
                imported.insert(imported.end(),
                                std::make_move_iterator(tests->begin()),
                                std::make_move_iterator(tests->end()));
            }

            const std::filesystem::path historyFile
                = HistoryFileOf(options, program, programName);
            Runner::History history;
            RunResult run;
            run.binary = options.importName.empty() ? "imported"
                                                    : options.importName;
            if (options.historyEnabled)
            {
                auto loaded = Runner::History::Load(historyFile);
                if (loaded)
                {
                    history = std::move(*loaded);
                }
                else
                {
                    run.notes.push_back("the history could not be read: "
                                        + loaded.GetError().Message());
                }
            }

            run.tests = Runner::MergeImported(std::move(imported));
            run.candidates = run.tests.size();
            run.startUnixMilliseconds = UnixMilliseconds();
            std::error_code here;
            const std::string revision
                = options.importRevision.empty()
                      ? Runner::CurrentCommit(
                            std::filesystem::current_path(here))
                      : options.importRevision;
            // Only the person importing knows that two reports tested the
            // same thing: a checkout can have edits, or be another one.
            std::map<std::string, std::string> fingerprints;
            for (auto &test : run.tests)
            {
                run.wallMicroseconds += test.DurationMicroseconds();
                if (!options.importRevision.empty())
                {
                    fingerprints[test.id] = options.importRevision;
                }
                const Runner::TestHistory *past = history.Find(test.id);
                test.quarantined
                    = options.quarantine != Runner::QuarantineMode::kOff
                      && past != nullptr && past->quarantine.has_value();
            }

            Runner::AnalysisInputs analysis;
            analysis.history = options.historyEnabled ? &history : nullptr;
            Runner::Analyze(run, analysis);

            Runner::ConsoleReport console{ options, stdout };
            console.ImportStarting(run.tests.size(),
                                   options.importFiles.size());
            for (const auto &test : run.tests)
            {
                console.TestFinished(test);
            }
            console.RunEnded(run);

            const bool reportMissing = !WriteReports(options, run);
            if (options.historyEnabled)
            {
                SaveHistory(historyFile, run, revision, {}, fingerprints);
            }
            return ExitCodeOf(options, run, reportMissing);
        }

        int
        Run(const std::vector<std::string> &arguments)
        {
            const std::filesystem::path program = Runner::CurrentExecutable(
                arguments.empty() ? nullptr : arguments.front().c_str());
            const std::string programName
                = program.empty() ? "tests" : program.stem().string();
            std::vector<std::string> rest;
            if (arguments.size() > 1)
            {
                rest.assign(arguments.begin() + 1, arguments.end());
            }
            std::error_code here;
            auto parsed = Runner::ParseCommandLine(
                rest, std::filesystem::current_path(here));
            if (!parsed)
            {
                PrintError(parsed.GetError().Message());
                return Catch::UnspecifiedErrorExitCode;
            }
            Runner::Options options = std::move(*parsed);
            options.programName = programName;

            if (options.mode == Runner::Mode::kHelp)
            {
                PrintHelp(programName);
                return 0;
            }
            if (options.mode == Runner::Mode::kVersion)
            {
                std::printf("Catch3 %s\n", kVersion);
                return 0;
            }
            // Importing does not touch the tests of this binary.
            if (options.mode == Runner::Mode::kImport)
            {
                return Import(options, program, programName);
            }
            if (ReportStartupErrors())
            {
                return Catch::UnspecifiedErrorExitCode;
            }
            if (options.filenamesAsTags)
            {
                Catch::getMutableRegistryHub()
                    .getMutableTestCaseRegistry()
                    .enableFilenameTags();
            }
            if (options.mode == Runner::Mode::kWorker)
            {
                return Runner::RunWorker(options, programName);
            }

            std::vector<std::string> notes;
            if (!options.configPath.empty()
                && options.verbosity == Runner::Verbosity::kVerbose)
            {
                notes.push_back("configuration from " + options.configPath);
            }
            if (BazelTest())
            {
                ApplyBazel(options, notes);
            }
            if (!options.seedGiven)
            {
                std::random_device device;
                options.seed = device();
            }

            auto catalog = Runner::Catalog::Load();
            if (!catalog)
            {
                PrintError(catalog.GetError().Message());
                return Catch::UnspecifiedErrorExitCode;
            }
            const Catch::Config config{ Runner::ToConfigData(options,
                                                             programName) };
            const auto &invalid = config.testSpec().getInvalidSpecs();
            if (!invalid.empty())
            {
                for (const auto &spec : invalid)
                {
                    PrintError("invalid test spec: " + spec);
                }
                return Catch::InvalidTestSpecExitCode;
            }
            std::vector<std::string> unmatched;
            auto matched = catalog->Match(config, unmatched);
            if (options.mode == Runner::Mode::kListTests
                || options.mode == Runner::Mode::kListTags)
            {
                if (options.shard.count > 1)
                {
                    // A shard lists the tests it would run, dependencies
                    // included, in registration order.
                    Runner::PlanInputs inputs;
                    inputs.catalog = &*catalog;
                    inputs.matched = matched;
                    Runner::Options listing = options;
                    listing.order = Runner::Order::kDeclared;
                    auto plan = Runner::MakePlan(listing, inputs);
                    if (!plan)
                    {
                        PrintError(plan.GetError().Message());
                        return Catch::UnspecifiedErrorExitCode;
                    }
                    matched.clear();
                    for (const auto &test : plan->tests)
                    {
                        matched.push_back(test.entry->index);
                    }
                    std::sort(matched.begin(), matched.end());
                }
                return List(options, *catalog, matched, programName);
            }

            const std::filesystem::path historyFile
                = HistoryFileOf(options, program, programName);
            Runner::History history;
            if (options.historyEnabled
                || options.mode == Runner::Mode::kAnalytics)
            {
                auto loaded = Runner::History::Load(historyFile);
                if (loaded)
                {
                    history = std::move(*loaded);
                }
                else
                {
                    notes.push_back("the history could not be read: "
                                    + loaded.GetError().Message());
                }
            }
            if (options.mode == Runner::Mode::kAnalytics)
            {
                return Analytics(options, history, historyFile);
            }

            std::string binaryFingerprint;
            if (options.historyEnabled || options.incremental)
            {
                auto digest = Runner::Sha256File(program);
                binaryFingerprint = digest ? *digest : std::string{};
            }

            std::optional<Runner::Changes> changes;
            const auto readChanges = [&] {
                if (changes)
                {
                    return;
                }
                auto read = Runner::ReadChanges(SourceDirectory(*catalog),
                                                options.changesSince);
                if (read)
                {
                    changes = std::move(*read);
                }
                else if (options.selection == Runner::Selection::kChanged
                         || options.selection == Runner::Selection::kSmart)
                {
                    notes.push_back(read.GetError().Message());
                }
            };
            if (options.selection == Runner::Selection::kChanged
                || options.selection == Runner::Selection::kSmart)
            {
                readChanges();
            }

            Runner::PlanInputs inputs;
            inputs.catalog = &*catalog;
            inputs.matched = matched;
            inputs.unmatchedSpecs = unmatched;
            inputs.history = options.historyEnabled ? &history : nullptr;
            inputs.changes = changes ? &*changes : nullptr;
            inputs.binaryFingerprint = binaryFingerprint;
            auto plan = Runner::MakePlan(options, inputs);
            if (!plan)
            {
                PrintError(plan.GetError().Message());
                return Catch::UnspecifiedErrorExitCode;
            }
            notes.insert(notes.end(), plan->notes.begin(), plan->notes.end());

            Runner::ConsoleReport console{ options, stdout };
            const std::size_t workers
                = options.inProcess
                      ? 1
                      : std::max<std::size_t>(
                            1,
                            std::min<std::size_t>(
                                options.jobs == 0 ? Runner::HardwareThreads()
                                                  : options.jobs,
                                plan->tests.size()));
            console.RunStarting(plan->tests.size(),
                                workers,
                                options.seed,
                                notes);

            std::string guard = options.exitGuardPath;
            if (BazelTest() && guard.empty())
            {
                guard = Runner::ReadEnvironment("TEST_PREMATURE_EXIT_FILE");
            }
            if (!guard.empty())
            {
                if (auto status = Runner::CreatePrivateFile(guard); !status)
                {
                    PrintError("the exit guard file: "
                               + status.GetError().Message());
                    return Catch::UnspecifiedErrorExitCode;
                }
            }

            const auto started = std::chrono::steady_clock::now();
            RunResult run;
            run.binary = programName;
            run.seed = options.seed;
            run.shardIndex = options.shard.index;
            run.shardCount = options.shard.count;
            run.candidates = plan->candidates;
            run.unmatchedSpecs = plan->unmatchedSpecs;
            run.startUnixMilliseconds = UnixMilliseconds();
            Runner::SchedulerEvents events;
            events.testFinished = [&](const TestResult &test) {
                console.TestFinished(test);
            };
            events.retrying
                = [&](const TestResult &test, const Attempt &attempt) {
                      console.Retrying(test, attempt);
                  };
            auto outcome = Runner::RunPlan(options,
                                           *plan,
                                           *catalog,
                                           program.string(),
                                           programName,
                                           events);
            run.wallMicroseconds
                = std::chrono::duration_cast<std::chrono::microseconds>(
                      std::chrono::steady_clock::now() - started)
                      .count();
            run.workers = outcome.workers;
            run.tests = std::move(outcome.tests);
            for (auto &kept : plan->kept)
            {
                run.tests.push_back(std::move(kept));
            }
            run.notes = std::move(outcome.notes);

            const bool anyFailure
                = std::any_of(run.tests.begin(),
                              run.tests.end(),
                              [](const TestResult &test) {
                                  return IsFailure(test.outcome) || test.flaky;
                              });
            if (anyFailure)
            {
                readChanges();
            }
            Runner::AnalysisInputs analysis;
            analysis.history = options.historyEnabled ? &history : nullptr;
            analysis.changes = changes ? &*changes : nullptr;
            analysis.plan = &*plan;
            analysis.program = program.string();
            analysis.changesSince = options.changesSince;
            Runner::Analyze(run, analysis);
            console.RunEnded(run);

            const bool reportMissing = !WriteReports(options, run);
            if (options.historyEnabled)
            {
                SaveHistory(historyFile,
                            run,
                            changes ? changes->commit : std::string{},
                            binaryFingerprint,
                            plan->fingerprints);
            }
            if (!guard.empty())
            {
                std::error_code error;
                std::filesystem::remove(guard, error);
            }

            if (outcome.infrastructureFailed)
            {
                return Catch::UnspecifiedErrorExitCode;
            }
            if (!run.unmatchedSpecs.empty() && options.warnUnmatchedSpecs)
            {
                return Catch::UnmatchedTestSpecExitCode;
            }
            return ExitCodeOf(options, run, reportMissing);
        }
    } // namespace

    int
    RunTests(const std::vector<std::string> &arguments)
    {
        return Run(arguments);
    }

    void
    PrintHelp(const std::string &programName)
    {
        std::fputs(Runner::HelpText(programName).c_str(), stdout);
    }
} // namespace Progmasoft::Catch3
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <deque>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <thread>


namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        using Scheduling::AppendBounded;
        using Scheduling::Bisection;
        using Scheduling::Clock;
        using Scheduling::Event;
        using Scheduling::Job;
        using Scheduling::kMaximumProbes;
        using Scheduling::kMaximumProcessOutput;
        using Scheduling::kMaximumRecordedAssertions;
        using Scheduling::kStartupFailuresToGiveUp;
        using Scheduling::kStartupTimeout;
        using Scheduling::Microseconds;
        using Scheduling::Passing;
        using Scheduling::Running;
        using Scheduling::Satisfies;
        using Scheduling::TestState;
        using Scheduling::Worker;
        using std::chrono::milliseconds;

        class Scheduler final
        {
        public:
            Scheduler(const Options &options,
                      const Plan &plan,
                      const Catalog &catalog,
                      std::string program,
                      std::string programName,
                      const SchedulerEvents &events)
                : options_{ options }
                , plan_{ plan }
                , catalog_{ catalog }
                , program_{ std::move(program) }
                , programName_{ std::move(programName) }
                , events_{ events }
                , runStart_{ Clock::now() }
            {
                std::error_code error;
                temporaryRoot_ = std::filesystem::temp_directory_path(error)
                                 / ("catch3-" + UniqueName());
                const unsigned hardware = HardwareThreads();
                maximumWorkers_ = options.jobs == 0 ? hardware : options.jobs;
                maximumWorkers_ = std::max<std::size_t>(
                    1,
                    std::min(maximumWorkers_, plan.tests.size()));
                retryBudget_ = std::max<std::size_t>(5, plan.tests.size() / 10);
                states_.resize(plan.tests.size());
                for (std::size_t position = 0; position < plan.tests.size();
                     ++position)
                {
                    const PlannedTest &planned = plan.tests[position];
                    const TestEntry &entry = *planned.entry;
                    TestResult &result = states_[position].result;
                    result.id = entry.key;
                    result.name = entry.name;
                    result.className = entry.className;
                    result.file = entry.file;
                    result.line = entry.line;
                    result.tags = entry.tags;
                    result.selectionReason = planned.selectionReason;
                    result.quarantined = planned.quarantined;
                    result.predictedDurationMicroseconds
                        = planned.predictedMicroseconds;
                    result.predictedFailureProbability
                        = planned.failureProbability;
                    if (planned.quarantined)
                    {
                        result.analysis.push_back("in quarantine: "
                                                  + planned.quarantineReason);
                    }
                    states_[position].repeatsLeft
                        = options.repeat > 0 ? options.repeat - 1 : 0;
                }
            }

            ScheduleOutcome
            Run()
            {
                for (std::size_t position = 0; position < plan_.tests.size();
                     ++position)
                {
                    Job job;
                    job.test = position;
                    const PlannedTest &planned = plan_.tests[position];
                    if (planned.replaySequence)
                    {
                        job.before = *planned.replaySequence;
                        job.fresh = true;
                        job.seed = planned.replaySeed;
                    }
                    Enqueue(std::move(job));
                }
                if (options_.inProcess)
                {
                    RunInProcess();
                }
                else
                {
                    RunInWorkers();
                }
                ScheduleOutcome outcome;
                for (auto &state : states_)
                {
                    if (!state.finished)
                    {
                        state.result.outcome = Outcome::kNotRun;
                        if (infrastructureFailed_)
                        {
                            state.result.category
                                = FailureCategory::kInfrastructure;
                        }
                    }
                    outcome.tests.push_back(std::move(state.result));
                }
                outcome.workers = workersStarted_;
                outcome.notes = std::move(notes_);
                outcome.infrastructureFailed = infrastructureFailed_;
                std::error_code error;
                // Only an empty root goes; kept directories hold failures.
                std::filesystem::remove(temporaryRoot_, error);
                return outcome;
            }

        private:
            // Queue and dispatch.

            void
            Enqueue(Job job)
            {
                ++states_[job.test].pendingJobs;
                states_[job.test].queued = true;
                if (job.kind == Job::Kind::kProbe || !job.before.empty()
                    || states_[job.test].result.attempts.size() > 0)
                {
                    // Retries and probes go ahead of first attempts: they
                    // finish a verdict that is already overdue.
                    queue_.push_front(std::move(job));
                }
                else
                {
                    queue_.push_back(std::move(job));
                }
            }

            /// Whether the dependencies of a test have their verdicts:
            /// returns false while one is still open.
            bool
            DependenciesSettled(std::size_t position) const
            {
                for (const std::size_t dependency :
                     plan_.tests[position].dependencies)
                {
                    if (!states_[dependency].finished)
                    {
                        return false;
                    }
                }
                return true;
            }

            bool
            ResourcesFree(std::size_t position) const
            {
                for (const auto &resource :
                     plan_.tests[position].entry->resources)
                {
                    if (heldResources_.count(resource) > 0)
                    {
                        return false;
                    }
                }
                return true;
            }

            std::size_t
            RunningCount() const
            {
                std::size_t count = 0;
                for (const auto &worker : workers_)
                {
                    count += worker->job.has_value() ? 1 : 0;
                }
                return count;
            }

            /// The next job that may start now, removed from the queue.
            std::optional<Job>
            TakeRunnable()
            {
                for (auto job = queue_.begin(); job != queue_.end(); ++job)
                {
                    const std::size_t position = job->test;
                    if (job->kind == Job::Kind::kAttempt
                        && !DependenciesSettled(position))
                    {
                        continue;
                    }
                    const bool serial = plan_.tests[position].entry->serial;
                    if (serial && RunningCount() > 0)
                    {
                        // Nothing else starts until the serial test has run.
                        return std::nullopt;
                    }
                    if (serialRunning_)
                    {
                        return std::nullopt;
                    }
                    if (!ResourcesFree(position))
                    {
                        continue;
                    }
                    Job taken = std::move(*job);
                    queue_.erase(job);
                    return taken;
                }
                return std::nullopt;
            }

            /// Settles tests whose dependencies failed, and those left when
            /// the run stops early.
            void
            SettleBlocked()
            {
                bool changed = true;
                while (changed)
                {
                    changed = false;
                    for (auto job = queue_.begin(); job != queue_.end();)
                    {
                        const std::size_t position = job->test;
                        std::string reason;
                        if (job->kind == Job::Kind::kAttempt)
                        {
                            for (const std::size_t dependency :
                                 plan_.tests[position].dependencies)
                            {
                                const auto &state = states_[dependency];
                                if (state.finished
                                    && !Satisfies(state.result.outcome))
                                {
                                    reason
                                        = "depends on '" + state.result.name
                                          + "', which "
                                          + (state.result.outcome
                                                     == Outcome::kBlocked
                                                 ? std::string{ "was blocked" }
                                                 : std::string{ OutcomeName(
                                                       state.result.outcome) });
                                    break;
                                }
                            }
                        }
                        if (reason.empty())
                        {
                            ++job;
                            continue;
                        }
                        auto &state = states_[position];
                        job = queue_.erase(job);
                        --state.pendingJobs;
                        if (state.pendingJobs == 0 && !state.finished
                            && state.result.attempts.empty())
                        {
                            state.result.outcome = Outcome::kBlocked;
                            state.result.category
                                = FailureCategory::kDependency;
                            state.result.analysis.insert(
                                state.result.analysis.begin(),
                                reason);
                            Finish(position);
                            changed = true;
                        }
                    }
                }
            }

            std::string
            TemporaryFor(std::size_t catalogIndex, std::size_t attempt)
            {
                return (temporaryRoot_
                        / ("t" + std::to_string(catalogIndex) + "-a"
                           + std::to_string(attempt) + "-"
                           + std::to_string(++temporaryCounter_)))
                    .string();
            }

            /// Starts the next step of a worker's job.
            void
            StartStep(Worker &worker)
            {
                const Job &job = *worker.job;
                const bool target = worker.step >= job.before.size();
                const std::size_t catalogIndex
                    = target ? plan_.tests[job.test].entry->index
                             : job.before[worker.step];
                Running running;
                running.catalogIndex = catalogIndex;
                running.target = target;
                running.start = Clock::now();
                Attempt &attempt = running.attempt;
                attempt.worker = worker.id;
                attempt.freshProcess = worker.history.empty();
                attempt.seed = worker.seed.value_or(options_.seed);
                attempt.startMicroseconds
                    = Microseconds(running.start - runStart_);
                for (const std::size_t before : worker.history)
                {
                    attempt.precededBy.push_back(catalog_.Tests()[before].key);
                }
                if (target)
                {
                    const PlannedTest &planned = plan_.tests[job.test];
                    attempt.number
                        = job.kind == Job::Kind::kAttempt
                              ? states_[job.test].result.attempts.size() + 1
                              : 0;
                    if (planned.timeout.count() > 0 && !options_.inProcess)
                    {
                        running.deadline = running.start + planned.timeout;
                    }
                }
                attempt.temporaryDirectory
                    = TemporaryFor(catalogIndex, attempt.number);
                worker.running = std::move(running);
                if (options_.inProcess)
                {
                    return;
                }
                JsonValue command = Message("run");
                command.Set("index", catalogIndex);
                command.Set("attempt", worker.running->attempt.number);
                command.Set("temp", worker.running->attempt.temporaryDirectory);
                if (!worker.process->Input().Write(command.ToString() + "\n"))
                {
                    // The worker died; its closed output reports it.
                }
            }

            void
            Assign(Worker &worker, Job job)
            {
                const PlannedTest &planned = plan_.tests[job.test];
                for (const auto &resource : planned.entry->resources)
                {
                    heldResources_.insert(resource);
                }
                if (planned.entry->serial)
                {
                    serialRunning_ = true;
                }
                worker.job = std::move(job);
                worker.step = 0;
                StartStep(worker);
            }

            void
            ReleaseJob(Worker &worker)
            {
                if (!worker.job)
                {
                    return;
                }
                const PlannedTest &planned = plan_.tests[worker.job->test];
                for (const auto &resource : planned.entry->resources)
                {
                    heldResources_.erase(resource);
                }
                if (planned.entry->serial)
                {
                    serialRunning_ = false;
                }
                worker.job.reset();
                worker.running.reset();
            }

            // Results.

            /// An attempt of the job's target ended.
            void
            Complete(Worker &worker, Attempt attempt)
            {
                Job job = std::move(*worker.job);
                ReleaseJob(worker);
                auto &state = states_[job.test];
                --state.pendingJobs;
                std::error_code error;
                if (Passing(attempt.outcome)
                    && !attempt.temporaryDirectory.empty())
                {
                    std::filesystem::remove_all(attempt.temporaryDirectory,
                                                error);
                    attempt.temporaryDirectory.clear();
                }
                else if (!attempt.temporaryDirectory.empty()
                         && std::filesystem::is_empty(
                             attempt.temporaryDirectory,
                             error))
                {
                    std::filesystem::remove(attempt.temporaryDirectory, error);
                    attempt.temporaryDirectory.clear();
                }
                if (job.kind == Job::Kind::kProbe)
                {
                    ProbeEnded(job, IsFailure(attempt.outcome));
                    return;
                }
                state.result.attempts.push_back(std::move(attempt));
                Decide(job.test);
            }

            /// What follows an attempt: a repeat, a retry, a bisection, or
            /// the verdict.
            void
            Decide(std::size_t position)
            {
                auto &state = states_[position];
                const auto &attempts = state.result.attempts;
                const Attempt &last = attempts.back();
                const PlannedTest &planned = plan_.tests[position];
                if (stopping_)
                {
                    Finish(position);
                    return;
                }
                if (state.repeatsLeft > 0)
                {
                    --state.repeatsLeft;
                    NotifyRetry(position);
                    Job job;
                    job.test = position;
                    Enqueue(std::move(job));
                    return;
                }
                const bool failed = IsFailure(last.outcome);
                const bool everFailed
                    = std::any_of(attempts.begin(),
                                  attempts.end(),
                                  [](const Attempt &attempt) {
                                      return IsFailure(attempt.outcome);
                                  });
                if (failed && state.retriesUsed < planned.retryLimit
                    && retryBudget_ > 0)
                {
                    bool retry = options_.retryMode == RetryMode::kAlways;
                    if (options_.retryMode == RetryMode::kAuto)
                    {
                        // One retry, alone in a fresh process, tells a
                        // deterministic failure from a flaky one and from
                        // one caused by a test before it. Beyond that only
                        // what is known to be unstable is worth more tries.
                        const bool unstable
                            = last.outcome == Outcome::kCrashed
                              || last.outcome == Outcome::kTimedOut
                              || planned.quarantined || planned.flakyHistory;
                        retry = state.retriesUsed == 0 || unstable;
                    }
                    if (retry)
                    {
                        ++state.retriesUsed;
                        --retryBudget_;
                        NotifyRetry(position);
                        Job job;
                        job.test = position;
                        job.fresh = true;
                        Enqueue(std::move(job));
                        return;
                    }
                }
                // A pass alone after a failure in company: flaky, or broken
                // by a test before it. A probe tells which.
                if (!failed && everFailed && options_.bisectOrder
                    && state.bisection.stage == Bisection::Stage::kNone)
                {
                    const Attempt *sharedFailure = nullptr;
                    for (const auto &attempt : attempts)
                    {
                        if (IsFailure(attempt.outcome)
                            && !attempt.precededBy.empty()
                            && attempt.outcome != Outcome::kTimedOut)
                        {
                            sharedFailure = &attempt;
                            break;
                        }
                    }
                    if (sharedFailure != nullptr && last.freshProcess)
                    {
                        std::vector<std::size_t> sequence;
                        for (const auto &key : sharedFailure->precededBy)
                        {
                            if (const TestEntry *entry = catalog_.Find(key))
                            {
                                sequence.push_back(entry->index);
                            }
                        }
                        if (!sequence.empty())
                        {
                            state.bisection.stage
                                = Bisection::Stage::kConfirming;
                            state.bisection.candidates = sequence;
                            StartProbe(position, sequence);
                            return;
                        }
                    }
                }
                Finish(position);
            }

            void
            StartProbe(std::size_t position, std::vector<std::size_t> sequence)
            {
                auto &bisection = states_[position].bisection;
                ++bisection.probes;
                bisection.probing = sequence;
                Job job;
                job.kind = Job::Kind::kProbe;
                job.test = position;
                job.before = sequence;
                job.fresh = true;
                job.probeSequence = std::move(sequence);
                Enqueue(std::move(job));
            }

            void
            ProbeEnded(const Job &job, bool failed)
            {
                const std::size_t position = job.test;
                auto &bisection = states_[position].bisection;
                if (bisection.stage == Bisection::Stage::kConfirming)
                {
                    if (!failed)
                    {
                        // It does not fail again after the same tests: flaky.
                        bisection.stage = Bisection::Stage::kDone;
                        Finish(position);
                        return;
                    }
                    bisection.reproduced = true;
                    bisection.stage = Bisection::Stage::kSearching;
                }
                else if (bisection.stage == Bisection::Stage::kSearching)
                {
                    if (failed)
                    {
                        bisection.candidates = bisection.probing;
                    }
                    else
                    {
                        std::vector<std::size_t> rest;
                        for (const std::size_t candidate : bisection.candidates)
                        {
                            if (std::find(bisection.probing.begin(),
                                          bisection.probing.end(),
                                          candidate)
                                == bisection.probing.end())
                            {
                                rest.push_back(candidate);
                            }
                        }
                        bisection.candidates = std::move(rest);
                    }
                }
                if (bisection.candidates.size() <= 1
                    || bisection.probes >= kMaximumProbes)
                {
                    if (bisection.candidates.size() == 1)
                    {
                        bisection.culprit = bisection.candidates.front();
                    }
                    bisection.stage = Bisection::Stage::kDone;
                    Finish(position);
                    return;
                }
                const auto half = bisection.candidates.size() / 2;
                StartProbe(position,
                           std::vector<std::size_t>(
                               bisection.candidates.begin(),
                               bisection.candidates.begin()
                                   + static_cast<std::ptrdiff_t>(half)));
            }

            void
            NotifyRetry(std::size_t position)
            {
                const auto &result = states_[position].result;
                if (events_.retrying && !result.attempts.empty())
                {
                    events_.retrying(result, result.attempts.back());
                }
            }

            /// Gives a test its verdict.
            void
            Finish(std::size_t position)
            {
                auto &state = states_[position];
                if (state.finished)
                {
                    return;
                }
                state.finished = true;
                TestResult &result = state.result;
                const auto &attempts = result.attempts;
                if (!attempts.empty())
                {
                    const Attempt *firstFailure = nullptr;
                    const Attempt *lastPass = nullptr;
                    for (const auto &attempt : attempts)
                    {
                        if (IsFailure(attempt.outcome)
                            && firstFailure == nullptr)
                        {
                            firstFailure = &attempt;
                        }
                        if (Passing(attempt.outcome))
                        {
                            lastPass = &attempt;
                        }
                    }
                    const auto &bisection = state.bisection;
                    if (bisection.reproduced && firstFailure != nullptr)
                    {
                        // Broken by state another test leaves behind: a real
                        // defect that a retry must not hide.
                        result.outcome = firstFailure->outcome;
                        result.category = firstFailure->category;
                        std::string finding
                            = "fails only after other tests in the same "
                              "process and passes alone";
                        if (bisection.culprit)
                        {
                            finding
                                += "; it fails after '"
                                   + catalog_.Tests()[*bisection.culprit].name
                                   + "', which leaves state behind";
                        }
                        result.analysis.insert(result.analysis.begin(),
                                               finding);
                    }
                    else if (firstFailure != nullptr && lastPass != nullptr)
                    {
                        result.flaky = true;
                        result.outcome = lastPass->outcome;
                        result.category = FailureCategory::kNone;
                        result.analysis.insert(
                            result.analysis.begin(),
                            "flaky: failed with "
                                + std::string{ CategoryName(
                                    firstFailure->category) }
                                + " and passed on attempt "
                                + std::to_string(lastPass->number));
                    }
                    else
                    {
                        result.outcome = attempts.back().outcome;
                        result.category = attempts.back().category;
                    }
                }
                if (result.FailsRun())
                {
                    ++failedTests_;
                    if (options_.failFast > 0
                        && failedTests_ >= options_.failFast && !stopping_)
                    {
                        stopping_ = true;
                        notes_.push_back("stopped after "
                                         + std::to_string(failedTests_)
                                         + " failed test(s) (--fail-fast)");
                    }
                }
                if (events_.testFinished)
                {
                    events_.testFinished(result);
                }
            }

            // Messages.

            void
            HandleLine(Worker &worker, const std::string &line)
            {
                auto parsed = ParseJson(line);
                if (!parsed)
                {
                    // Not a message: something wrote to the protocol stream.
                    if (worker.running)
                    {
                        AppendBounded(worker.running->attempt.processOutput,
                                      line + "\n");
                    }
                    return;
                }
                const JsonValue &message = *parsed;
                const std::string &kind = message["t"].AsString();
                if (kind == "hello")
                {
                    if (message["identity"].AsString() != catalog_.Identity())
                    {
                        notes_.push_back(
                            "a worker saw different tests than the runner; "
                            "the binary may have changed during the run");
                        KillWorker(worker);
                        return;
                    }
                    worker.ready = true;
                    startupFailures_ = 0;
                    // What the listeners printed when the worker started.
                    const std::string &output = message["output"].AsString();
                    if (!output.empty())
                    {
                        AppendBounded(worker.startupOutput, output);
                        std::fwrite(output.data(), 1, output.size(), stderr);
                        std::fflush(stderr);
                    }
                    return;
                }
                if (!worker.running)
                {
                    return;
                }
                Running &running = *worker.running;
                Attempt &attempt = running.attempt;
                if (kind == "section")
                {
                    if (message["begin"].AsBoolean())
                    {
                        SectionRecord section;
                        section.name = message["name"].AsString();
                        section.file = message["file"].AsString();
                        section.line = static_cast<std::size_t>(
                            message["line"].AsUnsigned());
                        section.depth = static_cast<std::size_t>(
                            message["depth"].AsUnsigned());
                        section.startMicroseconds = message["at"].AsInteger();
                        section.durationMicroseconds = -1;
                        if (attempt.sections.size() < 1000)
                        {
                            attempt.sections.push_back(section);
                        }
                    }
                    else if (!running.fatal)
                    {
                        // After a fatal signal Catch2 closes the open
                        // sections itself; they stay open here, because the
                        // crash happened inside them.
                        const auto depth = message["depth"].AsUnsigned();
                        for (auto section = attempt.sections.rbegin();
                             section != attempt.sections.rend();
                             ++section)
                        {
                            if (section->depth == depth
                                && section->durationMicroseconds < 0)
                            {
                                section->durationMicroseconds
                                    = message["duration"].AsInteger();
                                break;
                            }
                        }
                    }
                }
                else if (kind == "at")
                {
                    attempt.lastLocation = message["location"].AsString();
                }
                else if (kind == "assertion")
                {
                    // Newer than any location sent before it, which is sent
                    // only now and then.
                    attempt.lastLocation
                        = message["file"].AsString() + ":"
                          + std::to_string(message["line"].AsUnsigned());
                    if (attempt.assertions.size() < kMaximumRecordedAssertions)
                    {
                        attempt.assertions.push_back(
                            AssertionFromJson(message));
                    }
                }
                else if (kind == "benchmark")
                {
                    attempt.benchmarks.push_back(BenchmarkFromJson(message));
                }
                else if (kind == "fatal")
                {
                    running.fatal = true;
                    attempt.fatalMessage = message["message"].AsString();
                    if (!message["location"].AsString().empty())
                    {
                        attempt.lastLocation = message["location"].AsString();
                    }
                }
                else if (kind == "end")
                {
                    attempt.outcome
                        = OutcomeFromName(message["outcome"].AsString())
                              .value_or(Outcome::kFailed);
                    attempt.category
                        = CategoryFromName(message["category"].AsString())
                              .value_or(FailureCategory::kNone);
                    if (running.fatal)
                    {
                        attempt.outcome = Outcome::kCrashed;
                        attempt.category = FailureCategory::kCrash;
                    }
                    attempt.assertionsPassed = static_cast<std::size_t>(
                        message["passed"].AsUnsigned());
                    attempt.assertionsFailed = static_cast<std::size_t>(
                        message["failed"].AsUnsigned());
                    attempt.durationMicroseconds
                        = message["duration"].AsInteger();
                    attempt.standardOutput = message["stdout"].AsString();
                    attempt.standardError = message["stderr"].AsString();
                    if (running.fatal)
                    {
                        // The worker dies after a fatal condition; its exit
                        // completes the attempt.
                        return;
                    }
                    StepEnded(worker);
                }
            }

            /// The current step of a worker's job ended normally.
            void
            StepEnded(Worker &worker)
            {
                Running running = std::move(*worker.running);
                worker.running.reset();
                worker.history.push_back(running.catalogIndex);
                if (!running.target)
                {
                    // A test run only to set up a probe or a replay. If it
                    // could not run, the job cannot either.
                    std::error_code error;
                    std::filesystem::remove_all(
                        running.attempt.temporaryDirectory,
                        error);
                    ++worker.step;
                    StartStep(worker);
                    return;
                }
                Complete(worker, std::move(running.attempt));
                if (worker.dedicated)
                {
                    Quit(worker);
                }
            }

            void
            Quit(Worker &worker)
            {
                if (worker.quitting || options_.inProcess)
                {
                    return;
                }
                worker.quitting = true;
                static_cast<void>(worker.process->Input().Write(
                    Message("quit").ToString() + "\n"));
                worker.process->Input().Close();
            }

            void
            KillWorker(Worker &worker)
            {
                worker.quitting = true;
                if (worker.process)
                {
                    worker.process->Kill();
                }
            }

            /// A worker's output closed: it ended, on purpose or not.
            void
            WorkerEnded(Worker &worker)
            {
                auto status = worker.process->WaitFor(milliseconds{ 5000 });
                if (!status)
                {
                    worker.process->Kill();
                    status = worker.process->Wait();
                }
                // A worker that never said hello failed to start, unless the
                // run was already ending.
                if (!worker.ready && !stopping_ && !infrastructureFailed_)
                {
                    ++startupFailures_;
                    lastStartupOutput_ = worker.startupOutput;
                    if (startupFailures_ >= kStartupFailuresToGiveUp)
                    {
                        infrastructureFailed_ = true;
                        std::string note = "workers could not start ("
                                           + status->Describe() + ")";
                        if (!lastStartupOutput_.empty())
                        {
                            note += ": " + lastStartupOutput_;
                        }
                        notes_.push_back(note);
                    }
                }
                if (worker.running)
                {
                    Running running = std::move(*worker.running);
                    worker.running.reset();
                    Attempt &attempt = running.attempt;
                    if (worker.killedForTimeout)
                    {
                        attempt.outcome = Outcome::kTimedOut;
                        attempt.category = FailureCategory::kTimeout;
                    }
                    else
                    {
                        attempt.outcome = Outcome::kCrashed;
                        attempt.category = FailureCategory::kCrash;
                        attempt.exitDescription = status->Describe();
                        attempt.durationMicroseconds
                            = Microseconds(Clock::now() - running.start);
                    }
                    if (running.target)
                    {
                        Complete(worker, std::move(attempt));
                    }
                    else if (worker.job)
                    {
                        // A test before the target died: the job has to start
                        // over in another process.
                        Job job = std::move(*worker.job);
                        ReleaseJob(worker);
                        --states_[job.test].pendingJobs;
                        if (job.kind == Job::Kind::kProbe)
                        {
                            ProbeEnded(job, false);
                        }
                        else
                        {
                            Enqueue(std::move(job));
                        }
                    }
                }
                else if (worker.job)
                {
                    Job job = std::move(*worker.job);
                    ReleaseJob(worker);
                    --states_[job.test].pendingJobs;
                    if (!infrastructureFailed_)
                    {
                        Enqueue(std::move(job));
                    }
                }
            }

            // Worker processes.

            Worker *
            SpawnWorker(bool dedicated, std::optional<std::uint32_t> seed)
            {
                if (infrastructureFailed_)
                {
                    return nullptr;
                }
                Options workerOptions = options_;
                if (seed)
                {
                    workerOptions.seed = *seed;
                }
                ChildOptions child;
                child.arguments.push_back(program_);
                for (auto &argument : WorkerArguments(workerOptions))
                {
                    child.arguments.push_back(std::move(argument));
                }
                const std::size_t id = ++workersStarted_;
                child.environment.emplace_back("CATCH3_WORKER",
                                               std::to_string(id));
                child.keptEnvironment = options_.passedEnvironment;
                auto process = ChildProcess::Start(child);
                if (!process)
                {
                    ++startupFailures_;
                    notes_.push_back(process.GetError().Message());
                    if (startupFailures_ >= kStartupFailuresToGiveUp)
                    {
                        infrastructureFailed_ = true;
                    }
                    return nullptr;
                }
                auto worker = std::make_unique<Worker>();
                worker->id = id;
                worker->dedicated = dedicated;
                worker->seed = seed;
                worker->started = Clock::now();
                worker->process = std::move(*process);
                Worker *raw = worker.get();
                worker->outputReader = std::thread([this, raw] {
                    LineReader reader{ raw->process->Output() };
                    while (auto line = reader.Next())
                    {
                        Push({ raw->id, Event::Kind::kLine, std::move(*line) });
                    }
                    Push({ raw->id, Event::Kind::kOutputClosed, {} });
                });
                worker->errorReader = std::thread([this, raw] {
                    char buffer[4096];
                    while (true)
                    {
                        const auto count
                            = raw->process->Errors().Read(buffer,
                                                          sizeof(buffer));
                        if (count <= 0)
                        {
                            break;
                        }
                        Push({ raw->id,
                               Event::Kind::kErrors,
                               std::string(buffer,
                                           static_cast<std::size_t>(count)) });
                    }
                    Push({ raw->id, Event::Kind::kErrorsClosed, {} });
                });
                workers_.push_back(std::move(worker));
                return raw;
            }

            void
            Push(Event event)
            {
                {
                    const std::lock_guard lock{ eventMutex_ };
                    eventQueue_.push_back(std::move(event));
                }
                eventReady_.notify_one();
            }

            Worker *
            FindWorker(std::size_t id)
            {
                for (auto &worker : workers_)
                {
                    if (worker->id == id)
                    {
                        return worker.get();
                    }
                }
                return nullptr;
            }

            void
            Dispatch()
            {
                if (stopping_)
                {
                    return;
                }
                while (true)
                {
                    // An idle, ready, shared worker takes the next job that
                    // may share a process; otherwise a new worker starts if
                    // there is room.
                    std::size_t live = 0;
                    Worker *idle = nullptr;
                    for (auto &worker : workers_)
                    {
                        if (worker->quitting)
                        {
                            continue;
                        }
                        ++live;
                        if (worker->ready && !worker->job && !worker->dedicated
                            && idle == nullptr)
                        {
                            idle = worker.get();
                        }
                    }
                    auto job = TakeRunnable();
                    if (!job)
                    {
                        return;
                    }
                    const PlannedTest &planned = plan_.tests[job->test];
                    const bool fresh = job->fresh || options_.isolate
                                       || planned.entry->isolated;
                    if (!fresh && idle != nullptr)
                    {
                        Assign(*idle, std::move(*job));
                        continue;
                    }
                    if (live >= maximumWorkers_)
                    {
                        if (fresh && idle != nullptr)
                        {
                            // Make room: an idle shared worker gives way.
                            Quit(*idle);
                        }
                        queue_.push_front(std::move(*job));
                        return;
                    }
                    Worker *worker = SpawnWorker(fresh, job->seed);
                    if (worker == nullptr)
                    {
                        queue_.push_front(std::move(*job));
                        return;
                    }
                    // The job waits until the worker says hello.
                    worker->job = std::move(*job);
                    worker->step = 0;
                    pendingStart_.push_back(worker->id);
                    const PlannedTest &assigned
                        = plan_.tests[worker->job->test];
                    for (const auto &resource : assigned.entry->resources)
                    {
                        heldResources_.insert(resource);
                    }
                    if (assigned.entry->serial)
                    {
                        serialRunning_ = true;
                    }
                }
            }

            void
            StartPending()
            {
                for (auto id = pendingStart_.begin();
                     id != pendingStart_.end();)
                {
                    Worker *worker = FindWorker(*id);
                    if (worker == nullptr || !worker->job)
                    {
                        id = pendingStart_.erase(id);
                        continue;
                    }
                    if (worker->ready)
                    {
                        StartStep(*worker);
                        id = pendingStart_.erase(id);
                        continue;
                    }
                    ++id;
                }
            }

            void
            CheckDeadlines()
            {
                const auto now = Clock::now();
                for (auto &worker : workers_)
                {
                    if (worker->running && !worker->killedForTimeout
                        && now >= worker->running->deadline)
                    {
                        worker->killedForTimeout = true;
                        Attempt &attempt = worker->running->attempt;
                        attempt.durationMicroseconds
                            = Microseconds(now - worker->running->start);
                        KillWorker(*worker);
                    }
                    else if (!worker->ready && !worker->quitting
                             && now - worker->started > kStartupTimeout)
                    {
                        worker->startupOutput
                            += "\n(no answer within 60 seconds)";
                        KillWorker(*worker);
                    }
                }
            }

            Clock::time_point
            NextDeadline() const
            {
                auto deadline = Clock::now() + milliseconds{ 500 };
                for (const auto &worker : workers_)
                {
                    if (worker->running)
                    {
                        deadline
                            = std::min(deadline, worker->running->deadline);
                    }
                }
                return deadline;
            }

            bool
            Done() const
            {
                if (!workers_.empty())
                {
                    return false;
                }
                if (infrastructureFailed_ || stopping_)
                {
                    return true;
                }
                return queue_.empty();
            }

            void
            RunInWorkers()
            {
                while (true)
                {
                    SettleBlocked();
                    if (stopping_ || infrastructureFailed_)
                    {
                        // Queued work will not start; running work finishes.
                        for (const auto &job : queue_)
                        {
                            --states_[job.test].pendingJobs;
                        }
                        queue_.clear();
                        for (auto &worker : workers_)
                        {
                            if (!worker->job)
                            {
                                Quit(*worker);
                            }
                        }
                    }
                    Dispatch();
                    StartPending();
                    // Idle workers with nothing left to do go home.
                    if (queue_.empty())
                    {
                        for (auto &worker : workers_)
                        {
                            if (worker->ready && !worker->job)
                            {
                                Quit(*worker);
                            }
                        }
                    }
                    if (Done())
                    {
                        break;
                    }
                    std::deque<Event> events;
                    {
                        std::unique_lock lock{ eventMutex_ };
                        eventReady_.wait_until(lock, NextDeadline(), [&] {
                            return !eventQueue_.empty();
                        });
                        events.swap(eventQueue_);
                    }
                    for (auto &event : events)
                    {
                        Handle(event);
                    }
                    CheckDeadlines();
                    Reap();
                    if (queue_.empty() && workers_.empty())
                    {
                        // Probes and retries may have been queued by the
                        // last events; the loop picks them up.
                        SettleBlocked();
                        if (queue_.empty())
                        {
                            break;
                        }
                    }
                }
            }

            void
            Handle(Event &event)
            {
                Worker *worker = FindWorker(event.worker);
                if (worker == nullptr)
                {
                    return;
                }
                switch (event.kind)
                {
                    case Event::Kind::kLine:
                        HandleLine(*worker, event.text);
                        break;
                    case Event::Kind::kErrors:
                        if (worker->running)
                        {
                            AppendBounded(
                                worker->running->attempt.processOutput,
                                event.text);
                        }
                        else
                        {
                            // Output of no test: listeners and global set-up
                            // of the worker. It is passed on, not lost.
                            AppendBounded(worker->startupOutput, event.text);
                            std::fwrite(event.text.data(),
                                        1,
                                        event.text.size(),
                                        stderr);
                            std::fflush(stderr);
                        }
                        break;
                    case Event::Kind::kOutputClosed:
                        worker->outputClosed = true;
                        break;
                    case Event::Kind::kErrorsClosed:
                        worker->errorsClosed = true;
                        break;
                }
            }

            /// Collects workers whose streams have both closed.
            void
            Reap()
            {
                for (auto worker = workers_.begin(); worker != workers_.end();)
                {
                    if (!(*worker)->outputClosed || !(*worker)->errorsClosed)
                    {
                        ++worker;
                        continue;
                    }
                    (*worker)->outputReader.join();
                    (*worker)->errorReader.join();
                    WorkerEnded(**worker);
                    worker = workers_.erase(worker);
                }
            }

            void
            RunInProcess()
            {
                Worker local;
                local.id = 0;
                local.ready = true;
                workersStarted_ = 1;
                TestExecutor executor{ options_,
                                       catalog_,
                                       programName_,
                                       [&](const JsonValue &message) {
                                           HandleLine(local,
                                                      message.ToString());
                                       } };
                while (true)
                {
                    SettleBlocked();
                    if (stopping_)
                    {
                        break;
                    }
                    auto job = TakeRunnable();
                    if (!job)
                    {
                        break;
                    }
                    Assign(local, std::move(*job));
                    while (local.running)
                    {
                        const auto index = local.running->catalogIndex;
                        const auto number = local.running->attempt.number;
                        const auto temporary
                            = local.running->attempt.temporaryDirectory;
                        executor.Run(index, number, temporary);
                        if (local.running
                            && local.running->catalogIndex == index
                            && local.running->attempt.temporaryDirectory
                                   == temporary)
                        {
                            // The test reported no end: count it as failed
                            // rather than wait for it forever.
                            local.running->attempt.outcome = Outcome::kFailed;
                            StepEnded(local);
                        }
                    }
                }
                for (const auto &job : queue_)
                {
                    --states_[job.test].pendingJobs;
                }
                queue_.clear();
            }

            const Options &options_;
            const Plan &plan_;
            const Catalog &catalog_;
            std::string program_;
            std::string programName_;
            const SchedulerEvents &events_;
            Clock::time_point runStart_;
            std::filesystem::path temporaryRoot_;
            std::size_t temporaryCounter_ = 0;
            std::size_t maximumWorkers_ = 1;
            std::size_t retryBudget_ = 0;
            std::vector<TestState> states_;
            std::deque<Job> queue_;
            std::vector<std::unique_ptr<Worker>> workers_;
            std::vector<std::size_t> pendingStart_;
            std::set<std::string> heldResources_;
            bool serialRunning_ = false;
            std::size_t workersStarted_ = 0;
            std::size_t startupFailures_ = 0;
            std::string lastStartupOutput_;
            bool infrastructureFailed_ = false;
            bool stopping_ = false;
            std::size_t failedTests_ = 0;
            std::vector<std::string> notes_;
            std::mutex eventMutex_;
            std::condition_variable eventReady_;
            std::deque<Event> eventQueue_;
        };
    } // namespace

    ScheduleOutcome
    RunPlan(const Options &options,
            const Plan &plan,
            const Catalog &catalog,
            const std::string &program,
            const std::string &programName,
            const SchedulerEvents &events)
    {
        Scheduler scheduler{ options, plan,        catalog,
                             program, programName, events };
        return scheduler.Run();
    }
} // namespace Progmasoft::Catch3::Runner
// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0



#include <cstdlib>
#include <filesystem>
#include <mutex>
#include <system_error>

namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        constexpr std::array<const char *, 3> kVariables = {
            "TMPDIR", "TMP", "TEMP"};

        std::mutex &
        CurrentMutex()
        {
            static auto *mutex = new std::mutex();
            return *mutex;
        }

        std::string &
        Current()
        {
            static auto *directory = new std::string();
            return *directory;
        }

        void
        SetVariable(const char *name, const std::optional<std::string> &value)
        {
#if defined(_WIN32)
            // An empty value removes the variable.
            _putenv_s(name, value ? value->c_str() : "");
#else
            if (value)
            {
                setenv(name, value->c_str(), 1);
            }
            else
            {
                unsetenv(name);
            }
#endif
        }
    } // namespace

    TemporaryDirectoryScope::TemporaryDirectoryScope(
        const std::string &directory)
    {
        if (directory.empty())
        {
            return;
        }
        std::error_code error;
        std::filesystem::create_directories(directory, error);
        if (error)
        {
            return;
        }
        active_ = true;
        for (std::size_t index = 0; index < kVariables.size(); ++index)
        {
            if (HasEnvironment(kVariables[index]))
            {
                previous_[index] = ReadEnvironment(kVariables[index]);
            }
            SetVariable(kVariables[index], directory);
        }
        const std::lock_guard lock{CurrentMutex()};
        Current() = directory;
    }

    TemporaryDirectoryScope::~TemporaryDirectoryScope()
    {
        if (!active_)
        {
            return;
        }
        for (std::size_t index = 0; index < kVariables.size(); ++index)
        {
            SetVariable(kVariables[index], previous_[index]);
        }
        const std::lock_guard lock{CurrentMutex()};
        Current().clear();
    }
} // namespace Progmasoft::Catch3::Runner

namespace Progmasoft::Catch3
{
    std::filesystem::path
    TestTemporaryDirectory()
    {
        {
            const std::lock_guard lock{Runner::CurrentMutex()};
            if (!Runner::Current().empty())
            {
                return std::filesystem::path{Runner::Current()};
            }
        }
        std::error_code error;
        auto directory = std::filesystem::temp_directory_path(error);
        return error ? std::filesystem::path{"."} : directory;
    }
} // namespace Progmasoft::Catch3
