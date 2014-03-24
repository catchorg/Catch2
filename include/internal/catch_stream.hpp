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
#include "catch_streambuf.h"
#include "catch_debugger.h"

#include <stdexcept>
#include <cstdio>

namespace Catch {

    template<typename WriterF, size_t bufferSize=256>
    class StreamBufImpl : public StreamBufBase {
        char data[bufferSize];
        WriterF m_writer;

    public:
        StreamBufImpl() {
            setp( data, data + sizeof(data) );
        }

        ~StreamBufImpl() throw() {
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

    struct OutputDebugWriter {

        void operator()( std::string const&str ) {
            writeToDebugConsole( str );
        }
    };

    INTERNAL_CATCH_INLINE Stream::Stream()
    : streamBuf( NULL ), isOwned( false )
    {}

    INTERNAL_CATCH_INLINE Stream::Stream( std::streambuf* _streamBuf, bool _isOwned )
    : streamBuf( _streamBuf ), isOwned( _isOwned )
    {}

    INTERNAL_CATCH_INLINE void Stream::release() {
        if( isOwned ) {
            delete streamBuf;
            streamBuf = NULL;
            isOwned = false;
        }
    }
}

#endif // TWOBLUECUBES_CATCH_STREAM_HPP_INCLUDED
