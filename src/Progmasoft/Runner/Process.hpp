// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0
#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Expected.hpp"

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
