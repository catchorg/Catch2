/*
 *  Created by Phil on 2/12/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */
#ifndef TWOBLUECUBES_CATCH_STREAM_H_INCLUDED
#define TWOBLUECUBES_CATCH_STREAM_H_INCLUDED

#include "catch_compiler_capabilities.h"
#include "catch_streambuf.h"

#include <streambuf>
#include <ostream>
#include <fstream>

namespace Catch {

    std::ostream& cout();
    std::ostream& cerr();


    struct IStream {
        virtual ~IStream() CATCH_NOEXCEPT;
        virtual std::ostream& stream() const = 0;
    };

    class FileStream : public IStream {
        mutable std::ofstream m_ofs;
    public:
        FileStream( std::string const& filename );
        virtual ~FileStream() CATCH_NOEXCEPT;
    public: // IStream
        virtual std::ostream& stream() const CATCH_OVERRIDE;
    };


    class CoutStream : public IStream {
        mutable std::ostream m_os;
    public:
        CoutStream();
        virtual ~CoutStream() CATCH_NOEXCEPT;

    public: // IStream
        virtual std::ostream& stream() const CATCH_OVERRIDE;
    };


    class DebugOutStream : public IStream {
        std::auto_ptr<StreamBufBase> m_streamBuf;
        mutable std::ostream m_os;
    public:
        DebugOutStream();
        virtual ~DebugOutStream() CATCH_NOEXCEPT;

    public: // IStream
        virtual std::ostream& stream() const CATCH_OVERRIDE;
    };
}

#endif // TWOBLUECUBES_CATCH_STREAM_H_INCLUDED
