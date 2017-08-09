/*
 *  Created by Phil on 17/01/2011.
 *  Copyright 2011 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_STREAM_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_STREAM_HPP_INCLUDED

#include "catch_stream.h"
#include "catch_debugger.h"

#include <stdexcept>
#include <cstdio>
#include <iostream>

namespace Catch {

    template<typename WriterF, size_t bufferSize=256>
    class StreamBufImpl : public StreamBufBase {
        char data[bufferSize];
        WriterF m_writer;

    public:
        StreamBufImpl() {
            setp( data, data + sizeof(data) );
        }

        ~StreamBufImpl() CATCH_NOEXCEPT {
            sync();
        }

    private:
        int overflow( int c ) {
            sync();

            if( c != EOF ) {
                if( pbase() == epptr() )
                    m_writer( std::string( 1, static_cast<char>( c ) ) );
                else
                    sputc( static_cast<char>( c ) );
            }
            return 0;
        }

        int sync() {
            if( pbase() != pptr() ) {
                m_writer( std::string( pbase(), static_cast<std::string::size_type>( pptr() - pbase() ) ) );
                setp( pbase(), epptr() );
            }
            return 0;
        }
    };

    ///////////////////////////////////////////////////////////////////////////


    FileStream::FileStream( std::string const& filename ) {
        m_ofs.open( filename.c_str() );
        if( m_ofs.fail() ) {
            std::ostringstream oss;
            oss << "Unable to open file: '" << filename << '\'';
            throw std::domain_error( oss.str() );
        }
    }

    std::ostream& FileStream::stream() const {
        return m_ofs;
    }

    struct OutputDebugWriter {

        void operator()( std::string const&str ) {
            writeToDebugConsole( str );
        }
    };

    DebugOutStream::DebugOutStream()
    :   m_streamBuf( new StreamBufImpl<OutputDebugWriter>() ),
        m_os( m_streamBuf.get() )
    {}

    std::ostream& DebugOutStream::stream() const {
        return m_os;
    }

    // Store the streambuf from cout up-front because
    // cout may get redirected when running tests
    CoutStream::CoutStream()
    :   m_os( Catch::cout().rdbuf() )
    {}

    std::ostream& CoutStream::stream() const {
        return m_os;
    }

#ifndef CATCH_CONFIG_NOSTDOUT // If you #define this you must implement these functions
    std::ostream& cout() {
        return std::cout;
    }
    std::ostream& cerr() {
        return std::cerr;
    }
    std::ostream& clog() {
        return std::clog;
    }
#endif
}

#endif // TWOBLUECUBES_CATCH_STREAM_HPP_INCLUDED
