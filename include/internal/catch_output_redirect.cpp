/*
 *  Created by Martin on 28/04/2018.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_output_redirect.h"



#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>

#if defined(_MSC_VER)
#include <io.h>      //_dup and _dup2
#define dup _dup
#define dup2 _dup2
#define fileno _fileno
#else
#include <unistd.h>  // dup and dup2
#endif

namespace Catch {

    RedirectedStream::RedirectedStream( std::ostream& originalStream, std::ostream& redirectionStream )
    :   m_originalStream( originalStream ),
        m_redirectionStream( redirectionStream ),
        m_prevBuf( m_originalStream.rdbuf() )
    {
        m_originalStream.rdbuf( m_redirectionStream.rdbuf() );
    }

    RedirectedStream::~RedirectedStream() {
        m_originalStream.rdbuf( m_prevBuf );
    }

    RedirectedStdOut::RedirectedStdOut() : m_cout( Catch::cout(), m_rss.get() ) {}
    auto RedirectedStdOut::str() const -> std::string { return m_rss.str(); }

    RedirectedStdErr::RedirectedStdErr()
    :   m_cerr( Catch::cerr(), m_rss.get() ),
        m_clog( Catch::clog(), m_rss.get() )
    {}
    auto RedirectedStdErr::str() const -> std::string { return m_rss.str(); }



#if defined(CATCH_PLATFORM_WINDOWS)
    TempFile::TempFile() {
        if (tmpnam_s(m_buffer)) {
            throw std::runtime_error("Could not get a temp filename");
        }
        if (fopen_s(&m_file, m_buffer, "w")) {
            char buffer[100];
            if (strerror_s(buffer, errno)) {
                throw std::runtime_error("Could not translate errno to string");
            }
// strerror_s always null terminates the buffer when successful
// m_buffer is zero initialized
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:6054) // String 'buffer' might not be zero-terminated
#endif
            throw std::runtime_error("Could not open the temp file: " + std::string(m_buffer) + buffer);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
        }
    }
#else
    TempFile::TempFile() {
        m_file = std::tmpfile();
        if (!m_file) {
            throw std::runtime_error("Could not create a temp file.");
        }
    }

#endif

    TempFile::~TempFile() {
         // TBD: What to do about errors here?
         std::fclose(m_file);
         // We manually create the file on Windows only, on Linux
         // it will be autodeleted
#if defined(CATCH_PLATFORM_WINDOWS)
         std::remove(m_buffer);
#endif
    }


    FILE* TempFile::getFile() {
        return m_file;
    }

    std::string TempFile::getContents() {
        std::stringstream sstr;
        char buffer[100] = {};
        std::rewind(m_file);
        while (std::fgets(buffer, sizeof(buffer), m_file)) {
            sstr << buffer;
        }
        return sstr.str();
    }

    OutputRedirect::OutputRedirect(std::string& stdout_dest, std::string& stderr_dest) :
        m_originalStdout(dup(1)),
        m_originalStderr(dup(2)),
        m_stdoutDest(stdout_dest),
        m_stderrDest(stderr_dest) {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:6031) // Return value ignored: '_dup2'
#endif
        dup2(fileno(m_stdoutFile.getFile()), 1);
        dup2(fileno(m_stderrFile.getFile()), 2);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    }

    OutputRedirect::~OutputRedirect() {
        Catch::cout() << std::flush;
        fflush(stdout);
        // Since we support overriding these streams, we flush cerr
        // even though std::cerr is unbuffered
        Catch::cerr() << std::flush;
        Catch::clog() << std::flush;
        fflush(stderr);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:6031) // Return value ignored: '_dup2'
#endif
        dup2(m_originalStdout, 1);
        dup2(m_originalStderr, 2);
#ifdef _MSC_VER
#pragma warning(pop)
#endif

        m_stdoutDest += m_stdoutFile.getContents();
        m_stderrDest += m_stderrFile.getContents();
    }


} // namespace Catch

#if defined(CATCH_PLATFORM_WINDOWS)
#undef dup
#undef dup2
#undef fileno
#endif
