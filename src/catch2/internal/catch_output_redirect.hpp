
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_OUTPUT_REDIRECT_HPP_INCLUDED
#define CATCH_OUTPUT_REDIRECT_HPP_INCLUDED

#include <catch2/internal/catch_platform.hpp>
#include <catch2/internal/catch_reusable_string_stream.hpp>
#include <catch2/internal/catch_compiler_capabilities.hpp>

#include <cstdio>
#include <iosfwd>
#include <string>

#if defined(CATCH_CONFIG_NEW_CAPTURE)
#include <thread>
#endif

namespace Catch {

    class RedirectedStream {
        std::ostream& m_originalStream;
        std::ostream& m_redirectionStream;
        std::streambuf* m_prevBuf;

    public:
        RedirectedStream( std::ostream& originalStream, std::ostream& redirectionStream );
        ~RedirectedStream();
    };

    class RedirectedStdOut {
        ReusableStringStream m_rss;
        RedirectedStream m_cout;
    public:
        RedirectedStdOut();
        auto str() const -> std::string;
    };

    // StdErr has two constituent streams in C++, std::cerr and std::clog
    // This means that we need to redirect 2 streams into 1 to keep proper
    // order of writes
    class RedirectedStdErr {
        ReusableStringStream m_rss;
        RedirectedStream m_cerr;
        RedirectedStream m_clog;
    public:
        RedirectedStdErr();
        auto str() const -> std::string;
    };

    class RedirectedStreams {
    public:
        RedirectedStreams(RedirectedStreams const&) = delete;
        RedirectedStreams& operator=(RedirectedStreams const&) = delete;
        RedirectedStreams(RedirectedStreams&&) = delete;
        RedirectedStreams& operator=(RedirectedStreams&&) = delete;

        RedirectedStreams(std::string& redirectedCout, std::string& redirectedCerr);
        ~RedirectedStreams();
    private:
        std::string& m_redirectedCout;
        std::string& m_redirectedCerr;
        RedirectedStdOut m_redirectedStdOut;
        RedirectedStdErr m_redirectedStdErr;
    };

#if defined(CATCH_CONFIG_NEW_CAPTURE)

#if defined(CATCH_CPP20_OR_GREATER)
using jthread = std::jthread;
#else
// Just enough of std::jthread from C++20 for the code below.
struct jthread final
{
    jthread() noexcept;

    template <typename F, typename... Args>
    jthread(F&& f, Args&&... args);

    jthread(jthread const&) = delete;
    jthread(jthread&&) noexcept = default;

    jthread& operator=(jthread const&) noexcept = delete;

    // Not exactly like std::jthread, but close enough for the code below.
    jthread& operator=(jthread&&) noexcept = default;

    // Not exactly like std::jthread, but close enough for the code below.
    ~jthread() noexcept;

private:
    std::thread m_thread;
};
#endif

struct UniqueFileDescriptor final
{
    constexpr UniqueFileDescriptor() noexcept;
    explicit UniqueFileDescriptor(int value) noexcept;

    UniqueFileDescriptor(UniqueFileDescriptor const&) = delete;
    constexpr UniqueFileDescriptor(UniqueFileDescriptor&& other) noexcept;

    ~UniqueFileDescriptor() noexcept;

    UniqueFileDescriptor& operator=(UniqueFileDescriptor const&) = delete;
    UniqueFileDescriptor& operator=(UniqueFileDescriptor&& other) noexcept;

    constexpr int get();

private:
    int m_value;
};

struct OutputFileRedirector final
{
    explicit OutputFileRedirector(std::FILE* file, std::string& result);

    OutputFileRedirector(OutputFileRedirector const&) = delete;
    OutputFileRedirector(OutputFileRedirector&&) = delete;

    ~OutputFileRedirector() noexcept;

    OutputFileRedirector& operator=(OutputFileRedirector const&) = delete;
    OutputFileRedirector& operator=(OutputFileRedirector&&) = delete;

private:
    std::FILE* m_file;
    int m_fd;
    UniqueFileDescriptor m_previous;
    jthread m_readThread;
};

struct OutputRedirect final
{
    OutputRedirect(std::string& output, std::string& error);

private:
    OutputFileRedirector m_output;
    OutputFileRedirector m_error;
};

#endif

} // end namespace Catch

#endif // CATCH_OUTPUT_REDIRECT_HPP_INCLUDED
