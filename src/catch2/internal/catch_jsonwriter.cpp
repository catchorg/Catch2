
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
// Note: swapping these two includes around causes MSVC to error out
//       while in /permissive- mode. No, I don't know why.
//       Tested on VS 2019, 18.{3, 4}.x
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_jsonwriter.hpp>

#include <cstdint>
#include <iomanip>
#include <type_traits>

namespace Catch {
    JsonWriter::JsonWriter( std::ostream& os ): m_os{ os } {}
    JsonWriter::~JsonWriter() {
        while ( m_object_level > 0 ) {
            endObject();
        }
    }

    JsonWriter& JsonWriter::startObject() {
        for ( auto i = m_object_level; i > 0; --i ) {
            m_os << "  ";
        }
        m_os << "{\n";

        ++m_object_level;

        return *this;
    }

    JsonWriter& JsonWriter::endObject() {
        if ( m_object_level == 0 ) { return *this; }

        for ( auto i = m_object_level; i > 1; --i ) {
            m_os << "  ";
        }
        m_os << "}\n";

        --m_object_level;

        return *this;
    }
} // namespace Catch
