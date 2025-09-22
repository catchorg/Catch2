
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/catch_message.hpp>
#include <catch2/interfaces/catch_interfaces_capture.hpp>
#include <catch2/internal/catch_enforce.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>

#include <cassert>
#include <stack>

namespace Catch {

    ////////////////////////////////////////////////////////////////////////////


    ScopedMessage::ScopedMessage( MessageBuilder&& builder ):
        m_messageId( builder.m_info.sequence ) {
        MessageInfo info( CATCH_MOVE( builder.m_info ) );
        info.message = builder.m_stream.str();
        getResultCapture().pushScopedMessage( CATCH_MOVE(info) );
    }

    ScopedMessage::ScopedMessage( ScopedMessage&& old ) noexcept:
        m_messageId( old.m_messageId ) {
        old.m_moved = true;
    }

    ScopedMessage::~ScopedMessage() {
        if ( !m_moved ) { getResultCapture().popScopedMessage( m_messageId ); }
    }


    Capturer::Capturer( std::string_view macroName,
                        SourceLineInfo const& lineInfo,
                        ResultWas::OfType resultType,
                        std::string_view names ):
        m_resultCapture( getResultCapture() ) {
        auto trimmed = [&] (size_t start, size_t end) {
            while (names[start] == ',' || isspace(static_cast<unsigned char>(names[start]))) {
                ++start;
            }
            while (names[end] == ',' || isspace(static_cast<unsigned char>(names[end]))) {
                --end;
            }
            return names.substr(start, end - start + 1);
        };
        auto skipq = [&] (size_t start, char quote) {
            for (auto i = start + 1; i < names.size() ; ++i) {
                if (names[i] == quote)
                    return i;
                if (names[i] == '\\')
                    ++i;
            }
            CATCH_INTERNAL_ERROR("CAPTURE parsing encountered unmatched quote");
        };

        size_t start = 0;
        std::stack<char> openings;
        for (size_t pos = 0; pos < names.size(); ++pos) {
            char c = names[pos];
            switch (c) {
            case '[':
            case '{':
            case '(':
            // It is basically impossible to disambiguate between
            // comparison and start of template args in this context
//            case '<':
                openings.push(c);
                break;
            case ']':
            case '}':
            case ')':
//           case '>':
                openings.pop();
                break;
            case '"':
            case '\'':
                pos = skipq(pos, c);
                break;
            case ',':
                if (start != pos && openings.empty()) {
                    m_messages.emplace_back(macroName, lineInfo, resultType);
                    m_messages.back().message += trimmed(start, pos);
                    m_messages.back().message += " := ";
                    start = pos;
                }
                break;
            default:; // noop
            }
        }
        assert(openings.empty() && "Mismatched openings");
        m_messages.emplace_back(macroName, lineInfo, resultType);
        m_messages.back().message += trimmed(start, names.size() - 1);
        m_messages.back().message += " := ";
    }
    Capturer::~Capturer() {
        assert( m_captured == m_messages.size() );
        for (auto const& message : m_messages) {
            m_resultCapture.popScopedMessage( message.sequence );
        }
    }

    void Capturer::captureValue( size_t index, std::string const& value ) {
        assert( index < m_messages.size() );
        m_messages[index].message += value;
        m_resultCapture.pushScopedMessage( CATCH_MOVE(m_messages[index]) );
        m_captured++;
    }

} // end namespace Catch
