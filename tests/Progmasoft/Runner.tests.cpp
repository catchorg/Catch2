// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0

// Starts the runner fixture and checks what the runner made of its tests.

#include "RunnerFixtureClient.hpp"

#include <Progmasoft/Runner/Files.hpp>
#include <Progmasoft/Runner/Json.hpp>
#include <Progmasoft/Runner/Process.hpp>

#include <Progmasoft/Catch3/TestMacros.hpp>
#include <Progmasoft/Catch3/Matchers/String.hpp>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <set>
#include <string>
#include <vector>

namespace
{
    namespace Runner = Progmasoft::Catch3::Runner;
    using Catch::Matchers::ContainsSubstring;
    using FixtureClient::ExitCode;
    using FixtureClient::Fixture;
    using FixtureClient::RunFixture;
    using FixtureClient::Scratch;
} // namespace

TEST_CASE("Passing and failing tests get their verdicts", "[progmasoft][runner]")
{
    const Scratch scratch;
    const auto run = RunFixture(scratch, {"--no-history", "[pass],[fail]"});
    INFO(run.output);
    CHECK(ExitCode(run) == 42);
    CHECK(run.report["format"].AsString() == "catch3-run");
    CHECK(run.Test("fixture passes")["outcome"].AsString() == "passed");

    const auto &fails = run.Test("fixture fails");
    CHECK(fails["outcome"].AsString() == "failed");
    CHECK(fails["category"].AsString() == "assertion");
    CHECK_FALSE(fails["flaky"].AsBoolean());
    // A failure in a shared process is tried once more alone.
    CHECK(fails["attempts"].Items().size() == 2);
    const auto &assertion = fails["attempts"].Items().front()["assertions"].Items().front();
    CHECK(assertion["expansion"].AsString() == "2 == 3");
    CHECK(assertion["sections"].Items().front().AsString() == "in a section");

    CHECK(run.Test("fixture throws")["category"].AsString() ==
          "unexpected-exception");

    const auto &prints = run.Test("fixture prints");
    CHECK(prints["outcome"].AsString() == "passed");
    CHECK_THAT(prints["attempts"].Items().front()["standardOutput"].AsString(),
               ContainsSubstring("said through std::cout"));
    CHECK_THAT(run.output, ContainsSubstring("FAIL fixture fails"));
}

TEST_CASE("A crashing test does not take the run with it", "[progmasoft][runner]")
{
    const Scratch scratch;
    const auto run =
        RunFixture(scratch, {"--no-history", "[crash],[pass]", "--jobs=1"});
    INFO(run.output);
    CHECK(ExitCode(run) == 42);
    const auto &crash = run.Test("fixture crashes");
    CHECK(crash["outcome"].AsString() == "crashed");
    CHECK(crash["category"].AsString() == "crash");
    CHECK_THAT(run.Analysis("fixture crashes"),
               ContainsSubstring("in section deep down"));
    // The worker that crashed is replaced; the other tests still ran.
    CHECK(run.Test("fixture passes")["outcome"].AsString() == "passed");
    CHECK(run.Test("fixture prints")["outcome"].AsString() == "passed");
}

TEST_CASE("A hanging test is stopped at its time limit", "[progmasoft][runner]")
{
    const Scratch scratch;
    const auto run =
        RunFixture(scratch, {"--no-history", "--retries=off", "[hang]"});
    INFO(run.output);
    CHECK(ExitCode(run) == 42);
    const auto &hang = run.Test("fixture hangs");
    CHECK(hang["outcome"].AsString() == "timed-out");
    CHECK(hang["category"].AsString() == "timeout");
    CHECK_THAT(hang["attempts"].Items().front()["lastLocation"].AsString(),
               ContainsSubstring("RunnerFixture.cpp"));
    // Well below the 60 s the test sleeps; a slow, busy disk can stretch
    // the runner's own start-up and Git query to half a minute.
    CHECK(run.wall < std::chrono::seconds(50));
}

TEST_CASE("A test that passes on a retry is flaky", "[progmasoft][runner]")
{
    const Scratch scratch;
    const auto run = RunFixture(scratch, {"--no-history", "[flaky]"});
    INFO(run.output);
    CHECK(ExitCode(run) == 0);
    const auto &flaky = run.Test("fixture is flaky");
    CHECK(flaky["outcome"].AsString() == "passed");
    CHECK(flaky["flaky"].AsBoolean());
    CHECK(flaky["attempts"].Items().size() == 2);
    CHECK_THAT(run.output, ContainsSubstring("FLAKY fixture is flaky"));
}

TEST_CASE("The test that leaves state behind is found", "[progmasoft][runner]")
{
    const Scratch scratch;
    const auto run = RunFixture(
        scratch, {"--no-history", "--jobs=1", "--order=declared", "[order]"});
    INFO(run.output);
    CHECK(ExitCode(run) == 42);
    const auto &victim = run.Test("fixture victim");
    CHECK(victim["outcome"].AsString() == "failed");
    CHECK_FALSE(victim["flaky"].AsBoolean());
    CHECK_THAT(run.Analysis("fixture victim"),
               ContainsSubstring("it fails after 'fixture polluter'"));
}

TEST_CASE("A test whose dependency fails is blocked", "[progmasoft][runner]")
{
    const Scratch scratch;
    const auto run = RunFixture(scratch, {"--no-history", "fixture dependent"});
    INFO(run.output);
    CHECK(ExitCode(run) == 42);
    const auto &base = run.Test("fixture base");
    CHECK(base["selectionReason"].AsString() ==
          "needed by 'fixture dependent'");
    const auto &dependent = run.Test("fixture dependent");
    CHECK(dependent["outcome"].AsString() == "blocked");
    CHECK(dependent["attempts"].Items().empty());
    CHECK_THAT(run.Analysis("fixture dependent"),
               ContainsSubstring("depends on 'fixture base', which failed"));
}

TEST_CASE("Every test gets a temporary directory of its own",
          "[progmasoft][runner]")
{
    const Scratch scratch;
    const auto passing = RunFixture(scratch, {"--no-history", "[temp]"});
    INFO(passing.output);
    CHECK(ExitCode(passing) == 0);

    const auto failing = RunFixture(scratch,
                                    {"--no-history", "--retries=off", "[temp]"},
                                    {{"CATCH3_FIXTURE_FAIL_TEMP", "1"}});
    INFO(failing.output);
    CHECK(ExitCode(failing) == 42);
    const std::string kept =
        failing.Test("fixture temporary directory")["attempts"]
            .Items()
            .front()["temporaryDirectory"]
            .AsString();
    REQUIRE_FALSE(kept.empty());
    CHECK(std::filesystem::exists(std::filesystem::path{kept} / "scratch.txt"));
    std::error_code error;
    std::filesystem::remove_all(kept, error);
}

TEST_CASE("Tests run in parallel workers", "[progmasoft][runner]")
{
    const Scratch scratch;
    const auto run = RunFixture(scratch, {"--no-history", "--jobs=4", "[slow]"});
    INFO(run.output);
    CHECK(ExitCode(run) == 0);
    std::set<std::int64_t> workers;
    for (const auto &test : run.report["tests"].Items())
    {
        workers.insert(test["attempts"].Items().front()["worker"].AsInteger());
    }
    CHECK(workers.size() >= 2);
    CHECK(run.report["summary"]["testMicroseconds"].AsInteger() >
          run.report["wallMicroseconds"].AsInteger() / 2);
}

TEST_CASE("Shards split the tests without overlap", "[progmasoft][runner]")
{
    const Scratch scratch;
    std::multiset<std::string> seen;
    for (const char *shard : {"--shard=0/2", "--shard=1/2"})
    {
        const auto run = RunFixture(
            scratch, {"--no-history", "--allow-no-tests", shard, "[slow],[pass]"});
        INFO(run.output);
        CHECK(ExitCode(run) == 0);
        for (const auto &test : run.report["tests"].Items())
        {
            seen.insert(test["name"].AsString());
        }
    }
    CHECK(seen.size() == 6);
    CHECK(std::set<std::string>(seen.begin(), seen.end()).size() == 6);
}

TEST_CASE("History drives quarantine, incremental runs and replay",
          "[progmasoft][runner]")
{
    const Scratch scratch;
    const std::string history = "--history=" + (scratch.Path() / "history").string();

    // Flaky twice in a row: into quarantine.
    for (int run = 0; run < 2; ++run)
    {
        const auto result = RunFixture(scratch, {history, "[alternate]"});
        INFO(result.output);
        CHECK(result.Test("fixture alternates")["flaky"].AsBoolean());
    }
    const auto quarantined = RunFixture(scratch, {history, "[alternate]"});
    INFO(quarantined.output);
    CHECK(quarantined.Test("fixture alternates")["quarantined"].AsBoolean());

    // Passed with the same binary: kept as up to date.
    const auto first = RunFixture(scratch, {history, "--incremental", "[pass]"});
    CHECK(ExitCode(first) == 0);
    const auto second = RunFixture(scratch, {history, "--incremental", "[pass]"});
    INFO(second.output);
    CHECK(ExitCode(second) == 0);
    CHECK(second.Test("fixture passes")["outcome"].AsString() == "up-to-date");

    // Without the search for its cause, a failure that does not repeat
    // alone counts as flaky; the replay runs it in the order it happened,
    // where it fails again, and the search finds the cause.
    const auto failed = RunFixture(
        scratch, {history, "--jobs=1", "--order=declared", "--no-bisect", "[order]"});
    CHECK(ExitCode(failed) == 0);
    CHECK(failed.Test("fixture victim")["flaky"].AsBoolean());
    const auto replay = RunFixture(scratch, {history, "--replay"});
    INFO(replay.output);
    CHECK(ExitCode(replay) == 42);
    CHECK(replay.Test("fixture victim")["outcome"].AsString() == "failed");

    // Analytics read the same history.
    Runner::ChildOptions analytics;
    analytics.arguments = {Fixture(), history, "--analytics=json"};
    auto printed = Runner::RunAndCapture(analytics);
    REQUIRE(printed);
    auto json = Runner::ParseJson(printed->output);
    REQUIRE(json);
    CHECK((*json)["format"].AsString() == "catch3-analytics");
    CHECK((*json)["runs"].AsInteger() >= 6);
}

TEST_CASE("The discovery protocol lists tests with their runner tags",
          "[progmasoft][runner]")
{
    Runner::ChildOptions options;
    options.arguments = {Fixture(), "--list=json", "[deps],[hang]"};
    auto listed = Runner::RunAndCapture(options);
    REQUIRE(listed);
    INFO(listed->output);
    auto json = Runner::ParseJson(listed->output);
    REQUIRE(json);
    CHECK((*json)["format"].AsString() == "catch3-tests");
    CHECK((*json)["version"].AsInteger() == 1);
    REQUIRE((*json)["tests"].Items().size() == 3);
    bool sawDependency = false;
    bool sawTimeout = false;
    for (const auto &test : (*json)["tests"].Items())
    {
        CHECK_THAT(test["file"].AsString(), ContainsSubstring("RunnerFixture.cpp"));
        CHECK(test["line"].AsInteger() > 0);
        sawDependency = sawDependency || (!test["dependsOn"].Items().empty() &&
                                          test["dependsOn"].Items().front().AsString() ==
                                              "fixture base");
        sawTimeout =
            sawTimeout || test["timeoutSeconds"].AsNumber() > 0.5;
    }
    CHECK(sawDependency);
    CHECK(sawTimeout);
}

TEST_CASE("JUnit and trace outputs are written", "[progmasoft][runner]")
{
    const Scratch scratch;
    const auto junit = scratch.Path() / "junit.xml";
    const auto trace = scratch.Path() / "trace.json";
    const auto run = RunFixture(scratch,
                                {"--no-history",
                                 "[pass],[skip]",
                                 "--junit=" + junit.string(),
                                 "--trace=" + trace.string()});
    INFO(run.output);
    CHECK(ExitCode(run) == 0);
    auto xml = Runner::ReadFile(junit);
    REQUIRE(xml);
    CHECK_THAT(*xml, ContainsSubstring("<skipped message=\"skipped\" />"));
    auto traceText = Runner::ReadFile(trace);
    REQUIRE(traceText);
    auto traceJson = Runner::ParseJson(*traceText);
    REQUIRE(traceJson);
    CHECK((*traceJson)["traceEvents"].Items().size() >= 3);
}

TEST_CASE("A child does not inherit the secrets of the runner",
          "[progmasoft][runner]")
{
    CHECK(Runner::IsSecretVariable("GITHUB_TOKEN"));
    CHECK(Runner::IsSecretVariable("aws_secret_access_key"));
    CHECK(Runner::IsSecretVariable("DB_PASSWORD"));
    CHECK(Runner::IsSecretVariable("NPM_ApiKey"));
    CHECK_FALSE(Runner::IsSecretVariable("PATH"));
    CHECK_FALSE(Runner::IsSecretVariable("XAUTHORITY"));

    Runner::ChildOptions options;
#if defined(_WIN32)
    REQUIRE(_putenv_s("CATCH3_TEST_TOKEN", "withheld") == 0);
    REQUIRE(_putenv_s("CATCH3_TEST_PLAIN", "kept") == 0);
    options.arguments = { "cmd.exe", "/d", "/c", "set" };
#else
    REQUIRE(setenv("CATCH3_TEST_TOKEN", "withheld", 1) == 0);
    REQUIRE(setenv("CATCH3_TEST_PLAIN", "kept", 1) == 0);
    options.arguments = { "env" };
#endif
    options.searchPath = true;
    auto printed = Runner::RunAndCapture(options);
    REQUIRE(printed);
    CHECK(printed->output.find("CATCH3_TEST_TOKEN") == std::string::npos);
    CHECK_THAT(printed->output, ContainsSubstring("CATCH3_TEST_PLAIN=kept"));

    options.keptEnvironment = { "CATCH3_TEST_TOKEN" };
    printed = Runner::RunAndCapture(options);
    REQUIRE(printed);
    CHECK_THAT(printed->output,
               ContainsSubstring("CATCH3_TEST_TOKEN=withheld"));
#if defined(_WIN32)
    CHECK(_putenv_s("CATCH3_TEST_TOKEN", "") == 0);
    CHECK(_putenv_s("CATCH3_TEST_PLAIN", "") == 0);
#else
    CHECK(unsetenv("CATCH3_TEST_TOKEN") == 0);
    CHECK(unsetenv("CATCH3_TEST_PLAIN") == 0);
#endif
}
