
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/internal/catch_output_redirect.hpp>
#include <catch2/internal/catch_stdstreams.hpp>
#include <cstdio>
#include <cstring>
#include <sstream>

#if defined( CATCH_CONFIG_NEW_CAPTURE )
#    if defined( CATCH_INTERNAL_CONFIG_USE_ASYNC )
#        include <system_error>
#        if defined( _MSC_VER )
#            include <fcntl.h> // _O_TEXT
#            include <io.h>    // _close, _dup, _dup2, _fileno, _pipe and _read
#            define close _close
#            define dup _dup
#            define dup2 _dup2
#            define fileno _fileno
#        else
#            include <unistd.h> // close, dup, dup2, fileno, pipe and read
#        endif
#    else
#        if defined( _MSC_VER )
#            include <io.h> //_dup and _dup2
#            define dup _dup
#            define dup2 _dup2
#            define fileno _fileno
#        else
#            include <unistd.h> // dup and dup2
#        endif
#    endif
#endif

namespace Catch {

    RedirectedStream::RedirectedStream( std::ostream& originalStream,
                                        std::ostream& redirectionStream ):
        m_originalStream( originalStream ),
        m_redirectionStream( redirectionStream ),
        m_prevBuf( m_originalStream.rdbuf() ) {
        m_originalStream.rdbuf( m_redirectionStream.rdbuf() );
    }

    RedirectedStream::~RedirectedStream() {
        m_originalStream.rdbuf( m_prevBuf );
    }

    RedirectedStdOut::RedirectedStdOut():
        m_cout( Catch::cout(), m_rss.get() ) {}
    auto RedirectedStdOut::str() const -> std::string { return m_rss.str(); }

    RedirectedStdErr::RedirectedStdErr():
        m_cerr( Catch::cerr(), m_rss.get() ),
        m_clog( Catch::clog(), m_rss.get() ) {}
    auto RedirectedStdErr::str() const -> std::string { return m_rss.str(); }

    RedirectedStreams::RedirectedStreams( std::string& redirectedCout,
                                          std::string& redirectedCerr ):
        m_redirectedCout( redirectedCout ),
        m_redirectedCerr( redirectedCerr ) {}

    RedirectedStreams::~RedirectedStreams() {
        m_redirectedCout += m_redirectedStdOut.str();
        m_redirectedCerr += m_redirectedStdErr.str();
    }

#if defined( CATCH_CONFIG_NEW_CAPTURE )

#    if defined( CATCH_INTERNAL_CONFIG_USE_ASYNC )

    static inline void close_or_throw( int descriptor ) {
        if ( close( descriptor ) ) {
            CATCH_SYSTEM_ERROR( errno, std::generic_category() );
        }
    }

    static inline int dup_or_throw( int descriptor ) {
        int result{ dup( descriptor ) };

        if ( result == -1 ) {
            CATCH_SYSTEM_ERROR( errno, std::generic_category() );
        }

        return result;
    }

    static inline int dup2_or_throw( int sourceDescriptor,
                                     int destinationDescriptor ) {
        int result{ dup2( sourceDescriptor, destinationDescriptor ) };

        if ( result == -1 ) {
            CATCH_SYSTEM_ERROR( errno, std::generic_category() );
        }

        return result;
    }

    static inline int fileno_or_throw( std::FILE* file ) {
        int result{ fileno( file ) };

        if ( result == -1 ) {
            CATCH_SYSTEM_ERROR( errno, std::generic_category() );
        }

        return result;
    }

    static inline void pipe_or_throw( int descriptors[2] ) {
#        if defined( _MSC_VER )
        constexpr int defaultPipeSize{ 0 };

        int result{ _pipe( descriptors, defaultPipeSize, _O_TEXT ) };
#        else
        int result{ pipe( descriptors ) };
#        endif

        if ( result ) {
            CATCH_SYSTEM_ERROR( errno, std::generic_category() );
        }
    }

    static inline size_t
    read_or_throw( int descriptor, void* buffer, size_t size ) {
#        if defined( _MSC_VER )
        int result{
            _read( descriptor, buffer, static_cast<unsigned>( size ) ) };
#        else
        ssize_t result{ read( descriptor, buffer, size ) };
#        endif

        if ( result == -1 ) {
            CATCH_SYSTEM_ERROR( errno, std::generic_category() );
        }

        return static_cast<size_t>( result );
    }

    static inline void fflush_or_throw( std::FILE* file ) {
        if ( std::fflush( file ) ) {
            CATCH_SYSTEM_ERROR( errno, std::generic_category() );
        }
    }

    constexpr UniqueFileDescriptor::UniqueFileDescriptor() noexcept:
        m_value{} {}

    UniqueFileDescriptor::UniqueFileDescriptor( int value ) noexcept:
        m_value{ value } {}

    constexpr UniqueFileDescriptor::UniqueFileDescriptor(
        UniqueFileDescriptor&& other ) noexcept:
        m_value{ other.m_value } {
        other.m_value = 0;
    }

    UniqueFileDescriptor::~UniqueFileDescriptor() noexcept {
        if ( m_value == 0 ) {
            return;
        }

        close_or_throw(
            m_value ); // std::terminate on failure (due to noexcept)
    }

    UniqueFileDescriptor&
    UniqueFileDescriptor::operator=( UniqueFileDescriptor&& other ) noexcept {
        std::swap( m_value, other.m_value );
        return *this;
    }

    constexpr int UniqueFileDescriptor::get() { return m_value; }

    static inline void create_pipe( UniqueFileDescriptor& readDescriptor,
                                    UniqueFileDescriptor& writeDescriptor ) {
        readDescriptor = {};
        writeDescriptor = {};

        int descriptors[2];
        pipe_or_throw( descriptors );

        readDescriptor = UniqueFileDescriptor{ descriptors[0] };
        writeDescriptor = UniqueFileDescriptor{ descriptors[1] };
    }

    static inline void read_thread( UniqueFileDescriptor&& file,
                                    std::string& result ) {
        std::string buffer{};
        constexpr size_t bufferSize{ 4096 };
        buffer.resize( bufferSize );
        size_t sizeRead{};

        while ( ( sizeRead = read_or_throw(
                      file.get(), &buffer[0], bufferSize ) ) != 0 ) {
            result.append( buffer.data(), sizeRead );
        }
    }

    OutputFileRedirector::OutputFileRedirector( FILE* file,
                                                std::string& result ):
        m_file{ file },
        m_fd{ fileno_or_throw( m_file ) },
        m_previous{ dup_or_throw( m_fd ) } {
        fflush_or_throw( m_file );

        UniqueFileDescriptor readDescriptor{};
        UniqueFileDescriptor writeDescriptor{};
        create_pipe( readDescriptor, writeDescriptor );

        // Anonymous pipes have a limited buffer and require an active reader to
        // ensure the writer does not become blocked. Use a separate thread to
        // ensure the buffer does not get stuck full.
        m_readThread =
            std::thread{ [readDescriptor{ CATCH_MOVE( readDescriptor ) },
                          &result]() mutable {
                read_thread( CATCH_MOVE( readDescriptor ), result );
            } };

        // Replace the stdout or stderr file descriptor with the write end of
        // the pipe.
        dup2_or_throw( writeDescriptor.get(), m_fd );
    }

    OutputFileRedirector::~OutputFileRedirector() noexcept {
        fflush_or_throw(
            m_file ); // std::terminate on failure (due to noexcept)

        // Restore the original stdout or stderr file descriptor.
        dup2_or_throw( m_previous.get(),
                       m_fd ); // std::terminate on failure (due to noexcept)

        if ( m_readThread.joinable() ) {
            m_readThread.join();
        }
    }

    OutputRedirect::OutputRedirect( std::string& output, std::string& error ):
        m_output{ stdout, output }, m_error{ stderr, error } {}

#    else // !CATCH_INTERNAL_CONFIG_USE_ASYNC

#        if defined( _MSC_VER )
    TempFile::TempFile() {
        if ( tmpnam_s( m_buffer ) ) {
            CATCH_RUNTIME_ERROR( "Could not get a temp filename" );
        }
        if ( fopen_s( &m_file, m_buffer, "w+" ) ) {
            char buffer[100];
            if ( strerror_s( buffer, errno ) ) {
                CATCH_RUNTIME_ERROR( "Could not translate errno to a string" );
            }
            CATCH_RUNTIME_ERROR( "Could not open the temp file: '"
                                 << m_buffer << "' because: " << buffer );
        }
    }
#        else
    TempFile::TempFile() {
        m_file = std::tmpfile();
        if ( !m_file ) {
            CATCH_RUNTIME_ERROR( "Could not create a temp file." );
        }
    }

#        endif

    TempFile::~TempFile() {
        // TBD: What to do about errors here?
        std::fclose( m_file );
        // We manually create the file on Windows only, on Linux
        // it will be autodeleted
#        if defined( _MSC_VER )
        std::remove( m_buffer );
#        endif
    }

    FILE* TempFile::getFile() { return m_file; }

    std::string TempFile::getContents() {
        std::stringstream sstr;
        char buffer[100] = {};
        std::rewind( m_file );
        while ( std::fgets( buffer, sizeof( buffer ), m_file ) ) {
            sstr << buffer;
        }
        return sstr.str();
    }

    OutputRedirect::OutputRedirect( std::string& stdout_dest,
                                    std::string& stderr_dest ):
        m_originalStdout( dup( 1 ) ),
        m_originalStderr( dup( 2 ) ),
        m_stdoutDest( stdout_dest ),
        m_stderrDest( stderr_dest ) {
        dup2( fileno( m_stdoutFile.getFile() ), 1 );
        dup2( fileno( m_stderrFile.getFile() ), 2 );
    }

    OutputRedirect::~OutputRedirect() {
        Catch::cout() << std::flush;
        fflush( stdout );
        // Since we support overriding these streams, we flush cerr
        // even though std::cerr is unbuffered
        Catch::cerr() << std::flush;
        Catch::clog() << std::flush;
        fflush( stderr );

        dup2( m_originalStdout, 1 );
        dup2( m_originalStderr, 2 );

        m_stdoutDest += m_stdoutFile.getContents();
        m_stderrDest += m_stderrFile.getContents();
    }

#    endif // CATCH_INTERNAL_CONFIG_USE_ASYNC

#endif // CATCH_CONFIG_NEW_CAPTURE

} // namespace Catch

#if defined( CATCH_CONFIG_NEW_CAPTURE )
#    if defined( _MSC_VER )
#        undef close
#        undef dup
#        undef dup2
#        undef fileno
#    endif
#endif
