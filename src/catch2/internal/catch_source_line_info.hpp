
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_SOURCE_LINE_INFO_HPP_INCLUDED
#define CATCH_SOURCE_LINE_INFO_HPP_INCLUDED

#include <cstddef>
#include <iosfwd>

// We need a dummy global operator<< so we can bring it into Catch namespace later
struct Catch_global_namespace_dummy {};
std::ostream& operator<<(std::ostream&, Catch_global_namespace_dummy);

namespace Catch {

    struct SourceLineInfo {

        SourceLineInfo() = delete;
        constexpr SourceLineInfo( char const* _file, std::size_t _line ) noexcept:
            file( _file ),
            line( _line )
        {}

        bool operator == ( SourceLineInfo const& other ) const noexcept;
        bool operator < ( SourceLineInfo const& other ) const noexcept;

        char const* file;
        std::size_t line;

        friend std::ostream& operator << (std::ostream& os, SourceLineInfo const& info);
    };


    // Bring in operator<< from global namespace into Catch namespace
    // This is necessary because the overload of operator<< above makes
    // lookup stop at namespace Catch
    using ::operator<<;
}

#define CATCH_INTERNAL_LINEINFO \
    ::Catch::SourceLineInfo( __FILE__, static_cast<std::size_t>( __LINE__ ) )

#endif // CATCH_SOURCE_LINE_INFO_HPP_INCLUDED
