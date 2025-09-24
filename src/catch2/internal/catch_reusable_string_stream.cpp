
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#include <catch2/internal/catch_reusable_string_stream.hpp>
#include <catch2/internal/catch_singletons.hpp>
#include <catch2/internal/catch_thread_support.hpp>
#include <catch2/internal/catch_unique_ptr.hpp>

#include <cstdio>
#include <sstream>
#include <tuple>
#include <vector>

namespace Catch {

    // This class encapsulates the idea of a pool of ostringstreams that can be reused.
    struct StringStreams {
        std::vector<Detail::unique_ptr<std::ostringstream>> m_streams;
        std::vector<std::size_t> m_unused;
        std::ostringstream m_referenceStream; // Used for copy state/ flags from
        Detail::Mutex m_mutex;

        auto add() -> std::pair<std::size_t, std::ostringstream*> {
            Detail::LockGuard _( m_mutex );
            if( m_unused.empty() ) {
                m_streams.push_back( Detail::make_unique<std::ostringstream>() );
                return { m_streams.size()-1, m_streams.back().get() };
            }
            else {
                auto index = m_unused.back();
                m_unused.pop_back();
                return { index, m_streams[index].get() };
            }
        }

        void release( std::size_t index, std::ostream* originalPtr ) {
            assert( originalPtr );
            originalPtr->copyfmt( m_referenceStream );  // Restore initial flags and other state

            Detail::LockGuard _( m_mutex );
            assert( originalPtr == m_streams[index].get() && "Mismatch between release index and stream ptr" );
            m_unused.push_back( index );
        }
    };

    ReusableStringStream::ReusableStringStream() {
        std::tie( m_index, m_oss ) =
            Singleton<StringStreams>::getMutable().add();
    }

    ReusableStringStream::~ReusableStringStream() {
        static_cast<std::ostringstream*>( m_oss )->str("");
        m_oss->clear();
        Singleton<StringStreams>::getMutable().release( m_index, m_oss );
    }

    std::string ReusableStringStream::str() const {
        return static_cast<std::ostringstream*>( m_oss )->str();
    }

    void ReusableStringStream::str( std::string const& str ) {
        static_cast<std::ostringstream*>( m_oss )->str( str );
    }


}
