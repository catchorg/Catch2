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

#include "Process.hpp"

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
