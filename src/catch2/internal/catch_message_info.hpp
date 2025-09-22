
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_MESSAGE_INFO_HPP_INCLUDED
#define CATCH_MESSAGE_INFO_HPP_INCLUDED

#include <catch2/internal/catch_deprecation_macro.hpp>
#include <catch2/internal/catch_result_type.hpp>
#include <catch2/internal/catch_source_line_info.hpp>

#include <string>

namespace Catch {

    struct MessageInfo {
        MessageInfo(    std::string_view _macroName,
                        SourceLineInfo const& _lineInfo,
                        ResultWas::OfType _type );

        std::string_view macroName;
        std::string message;
        SourceLineInfo lineInfo;
        ResultWas::OfType type;
        // The "ID" of the message, used to know when to remove it from reporter context.
        unsigned int sequence;

        DEPRECATED( "Explicitly use the 'sequence' member instead" )
        bool operator == (MessageInfo const& other) const {
            return sequence == other.sequence;
        }
        DEPRECATED( "Explicitly use the 'sequence' member instead" )
        bool operator < (MessageInfo const& other) const {
            return sequence < other.sequence;
        }
    private:
        static thread_local unsigned int globalCount;
    };

} // end namespace Catch

#endif // CATCH_MESSAGE_INFO_HPP_INCLUDED
