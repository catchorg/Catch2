
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_JSONWRITER_HPP_INCLUDED
#define CATCH_JSONWRITER_HPP_INCLUDED

#include <catch2/internal/catch_reusable_string_stream.hpp>
#include <catch2/internal/catch_stringref.hpp>

#include <iosfwd>
#include <vector>

namespace Catch {
    class JsonWriter {
    public:
        JsonWriter( std::ostream& os );
        ~JsonWriter();

        JsonWriter( JsonWriter const& ) = delete;
        JsonWriter& operator=( JsonWriter const& ) = delete;

        JsonWriter& startObject();
        JsonWriter& endObject();

    private:
        std::uint64_t m_object_level = 0;

        std::ostream& m_os;
    };
} // namespace Catch

#endif // CATCH_JSONWRITER_HPP_INCLUDED
