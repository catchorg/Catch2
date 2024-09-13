
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_compiler_capabilities.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_output_redirect.hpp>
#include <catch2/internal/catch_platform.hpp>
#include <catch2/internal/catch_reusable_string_stream.hpp>
#include <catch2/internal/catch_stdstreams.hpp>

#include <cstdio>
#include <cstring>
#include <iosfwd>
#include <sstream>

#if defined(CATCH_CONFIG_USE_ASYNC)
#include <thread>
#endif

#if defined( CATCH_CONFIG_NEW_CAPTURE ) || defined(CATCH_CONFIG_USE_ASYNC)
#    if defined( _MSC_VER )
#        include <io.h> //_dup and _dup2
#        include <fcntl.h> // _O_BINARY
#        define dup _dup
#        define dup2 _dup2
#        define fileno _fileno
#        define close _close
#    else
#        include <unistd.h> // dup and dup2
#    endif
#endif

namespace Catch {

    namespace {
        //! A no-op implementation, used if no reporter wants output
        //! redirection.
        class NoopRedirect : public OutputRedirect {
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
         * users.
         */
        class RedirectedStream {
            std::ostream& m_originalStream;
            std::ostream& m_redirectionStream;
            std::streambuf* m_prevBuf;

        public:
            RedirectedStream( std::ostream& originalStream,
                                 std::ostream& redirectionStream ):
                m_originalStream( originalStream ),
                m_redirectionStream( redirectionStream ),
                m_prevBuf( m_originalStream.rdbuf() ) {}

            void startRedirect() {
                m_originalStream.rdbuf( m_redirectionStream.rdbuf() );
            }
            void stopRedirect() { m_originalStream.rdbuf( m_prevBuf ); }
        };

        /**
         * Redirects the `std::cout`, `std::cerr`, `std::clog` streams,
         * but does not touch the actual `stdout`/`stderr` file descriptors.
         */
        class StreamRedirect : public OutputRedirect {
            ReusableStringStream m_redirectedOut, m_redirectedErr;
            RedirectedStream m_cout, m_cerr, m_clog;

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
        class FileRedirect : public OutputRedirect {
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


#if defined( CATCH_CONFIG_USE_ASYNC )

        struct UniqueFileDescriptor final {
            constexpr UniqueFileDescriptor() noexcept;
            explicit UniqueFileDescriptor( int value ) noexcept;

            UniqueFileDescriptor( UniqueFileDescriptor const& ) = delete;
            constexpr UniqueFileDescriptor(
                UniqueFileDescriptor&& other ) noexcept;

            ~UniqueFileDescriptor() noexcept;

            UniqueFileDescriptor&
            operator=( UniqueFileDescriptor const& ) = delete;
            UniqueFileDescriptor&
            operator=( UniqueFileDescriptor&& other ) noexcept;

            constexpr int get();

        private:
            int m_value;
        };

        struct OutputFileRedirector final {
            explicit OutputFileRedirector( std::FILE* file,
                                           std::string& result );

            OutputFileRedirector( OutputFileRedirector const& ) = delete;
            OutputFileRedirector( OutputFileRedirector&& ) = delete;

            ~OutputFileRedirector() noexcept;

            OutputFileRedirector&
            operator=( OutputFileRedirector const& ) = delete;
            OutputFileRedirector& operator=( OutputFileRedirector&& ) = delete;

        private:
            std::FILE* m_file;
            int m_fd;
            UniqueFileDescriptor m_previous;
            std::thread m_readThread;
        };

        struct PipeRedirect final {
            PipeRedirect( std::string& output, std::string& error ):
                m_output{ stdout, output }, m_error{ stderr, error } {}

        private:
            OutputFileRedirector m_output;
            OutputFileRedirector m_error;
        };

        class PipeRedirectWrapper : public OutputRedirect {
            void activateImpl() override { m_redirect = Detail::make_unique<PipeRedirect>( m_stdout, m_stderr ); }
            void deactivateImpl() override { m_redirect.reset(); }
            std::string getStdout() override { return m_stdout; }
            std::string getStderr() override { return m_stderr; }
            void clearBuffers() override {
                m_stdout.clear();
                m_stderr.clear();
            }
            Detail::unique_ptr<PipeRedirect> m_redirect;
            std::string m_stdout, m_stderr;
        };

        static inline void close_or_throw( int descriptor ) {
            if ( close( descriptor ) ) {
                CATCH_INTERNAL_ERROR( "close-or-throw" );
                //CATCH_SYSTEM_ERROR( errno, std::generic_category() );
            }
        }

        static inline int dup_or_throw( int descriptor ) {
            int result{ dup( descriptor ) };

            if ( result == -1 ) {
                CATCH_INTERNAL_ERROR( "dup-or-throw" );
                //CATCH_SYSTEM_ERROR( errno, std::generic_category() );
            }

            return result;
        }

        static inline int dup2_or_throw( int sourceDescriptor,
                                         int destinationDescriptor ) {
            int result{ dup2( sourceDescriptor, destinationDescriptor ) };

            if ( result == -1 ) {
                CATCH_INTERNAL_ERROR( "dup2-or-throw" );
                //CATCH_SYSTEM_ERROR( errno, std::generic_category() );
            }

            return result;
        }

        static inline int fileno_or_throw( std::FILE* file ) {
            int result{ fileno( file ) };

            if ( result == -1 ) {
                CATCH_INTERNAL_ERROR( "fileno-or-throw" );
                //CATCH_SYSTEM_ERROR( errno, std::generic_category() );
            }

            return result;
        }

        static inline void pipe_or_throw( int descriptors[2] ) {
#    if defined( _MSC_VER )
            constexpr int defaultPipeSize{ 0 };

            int result{ _pipe( descriptors, defaultPipeSize, _O_BINARY ) };
#    else
            int result{ pipe( descriptors ) };
#    endif

            if ( result ) {
                CATCH_INTERNAL_ERROR( "pipe-or-throw" );
                // CATCH_SYSTEM_ERROR( errno, std::generic_category() );
            }
        }

        static inline size_t
        read_or_throw( int descriptor, void* buffer, size_t size ) {
#    if defined( _MSC_VER )
            int result{
                _read( descriptor, buffer, static_cast<unsigned>( size ) ) };
#    else
            ssize_t result{ read( descriptor, buffer, size ) };
#    endif

            if ( result == -1 ) {
                CATCH_INTERNAL_ERROR( "read-or-throw" );
                //CATCH_SYSTEM_ERROR( errno, std::generic_category() );
            }

            return static_cast<size_t>( result );
        }

        static inline void fflush_or_throw( std::FILE* file ) {
            if ( std::fflush( file ) ) {
                CATCH_INTERNAL_ERROR( "fflush-or-throw" );
                // CATCH_SYSTEM_ERROR( errno, std::generic_category() );
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
            if ( m_value == 0 ) { return; }

            close_or_throw(
                m_value ); // std::terminate on failure (due to noexcept)
        }

        UniqueFileDescriptor& UniqueFileDescriptor::operator=(
            UniqueFileDescriptor&& other ) noexcept {
            std::swap( m_value, other.m_value );
            return *this;
        }

        constexpr int UniqueFileDescriptor::get() { return m_value; }

        static inline void
        create_pipe( UniqueFileDescriptor& readDescriptor,
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

            // Anonymous pipes have a limited buffer and require an active
            // reader to ensure the writer does not become blocked. Use a
            // separate thread to ensure the buffer does not get stuck full.
            m_readThread =
                std::thread{ [readDescriptor{ CATCH_MOVE( readDescriptor ) },
                              &result]() mutable {
                    read_thread( CATCH_MOVE( readDescriptor ), result );
                } };

            // Replace the stdout or stderr file descriptor with the write end
            // of the pipe.
            dup2_or_throw( writeDescriptor.get(), m_fd );
        }

        OutputFileRedirector::~OutputFileRedirector() noexcept {
            fflush_or_throw(
                m_file ); // std::terminate on failure (due to noexcept)

            // Restore the original stdout or stderr file descriptor.
            dup2_or_throw(
                m_previous.get(),
                m_fd ); // std::terminate on failure (due to noexcept)

            if ( m_readThread.joinable() ) { m_readThread.join(); }
        }


#endif // CATCH_CONFIG_USE_ASYNC


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
#if defined(CATCH_CONFIG_USE_ASYNC)
        case OutputRedirect::Pipes:
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
            //return Detail::make_unique<StreamRedirect>();
            return Detail::make_unique<PipeRedirectWrapper>();
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
