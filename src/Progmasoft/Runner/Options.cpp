// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0
#include "Options.hpp"

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
