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

#include "catch_common.h"

#include <iosfwd>
#include <cstddef>
#include <ostream>

namespace Catch {

    std::ostream& cout();
    std::ostream& cerr();
    std::ostream& clog();

    class StringRef;

    struct IStream {
        virtual ~IStream();
        virtual std::ostream& stream() const = 0;
    };

    auto makeStream( StringRef const &filename ) -> IStream const*;

    class ReusableStringStream : NonCopyable {
        std::size_t m_index;
        std::ostream* m_oss;
    public:
        ReusableStringStream();
        ~ReusableStringStream();

        auto str() const -> std::string;

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
// Old versions of GCC do not understand -Wnonnull-compare
#pragma GCC diagnostic ignored "-Wpragmas"
// Streaming a function pointer triggers Waddress and Wnonnull-compare
// on GCC, because it implicitly converts it to bool and then decides
// that the check it uses (a? true : false) is tautological and cannot
// be null...
#pragma GCC diagnostic ignored "-Waddress"
#pragma GCC diagnostic ignored "-Wnonnull-compare"
#endif

        template<typename T>
        auto operator << ( T const& value ) -> ReusableStringStream& {
            *m_oss << value;
            return *this;
        }

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
        auto get() -> std::ostream& { return *m_oss; }
    };
}

#endif // TWOBLUECUBES_CATCH_STREAM_H_INCLUDED
