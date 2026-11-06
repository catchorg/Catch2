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

#include "Execution.hpp"
#include "Files.hpp"
#include "Process.hpp"
#include "Protocol.hpp"
#include "ResultsJson.hpp"
#include "Scheduler.hpp"
#include "SchedulerModel.hpp"

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
