
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_MATCHERS_CONTAINER_PROPERTIES_HPP_INCLUDED
#define CATCH_MATCHERS_CONTAINER_PROPERTIES_HPP_INCLUDED

#include <catch2/internal/catch_container_nonmembers.hpp>
#include <catch2/internal/catch_dll_public.hpp>
#include <catch2/internal/catch_move_and_forward.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

namespace Catch {
    namespace Matchers {

        class CATCH_DLL_PUBLIC IsEmptyMatcher final
            : public MatcherGenericBase {
        public:
            // todo: Use polyfills
            template <typename RangeLike>
            bool match(RangeLike&& rng) const {
#if defined(CATCH_CONFIG_POLYFILL_NONMEMBER_CONTAINER_ACCESS)
                using Catch::Detail::empty;
#else
                using std::empty;
#endif
                return empty(rng);
            }

            std::string describe() const override;
        };

        class CATCH_DLL_PUBLIC HasSizeMatcher final
            : public MatcherGenericBase {
            std::size_t m_target_size;
        public:
            explicit HasSizeMatcher(std::size_t target_size):
                m_target_size(target_size)
            {}

            template <typename RangeLike>
            bool match(RangeLike&& rng) const {
#if defined(CATCH_CONFIG_POLYFILL_NONMEMBER_CONTAINER_ACCESS)
                using Catch::Detail::size;
#else
                using std::size;
#endif
                return size(rng) == m_target_size;
            }

            std::string describe() const override;
        };

        template <typename Matcher>
        class CATCH_DLL_PUBLIC SizeMatchesMatcher final
            : public MatcherGenericBase {
            Matcher m_matcher;
        public:
            explicit SizeMatchesMatcher(Matcher m):
                m_matcher(CATCH_MOVE(m))
            {}

            template <typename RangeLike>
            bool match(RangeLike&& rng) const {
#if defined(CATCH_CONFIG_POLYFILL_NONMEMBER_CONTAINER_ACCESS)
                using Catch::Detail::size;
#else
                using std::size;
#endif
                return m_matcher.match(size(rng));
            }

            std::string describe() const override {
                return "size matches " + m_matcher.describe();
            }
        };

        //! Creates a matcher that accepts empty ranges/containers
        CATCH_DLL_PUBLIC IsEmptyMatcher IsEmpty();
        //! Creates a matcher that accepts ranges/containers with specific size
        CATCH_DLL_PUBLIC HasSizeMatcher SizeIs( std::size_t sz );
        template <typename Matcher>
        std::enable_if_t<Detail::is_matcher<Matcher>::value,
        SizeMatchesMatcher<Matcher>> SizeIs(Matcher&& m) {
            return SizeMatchesMatcher<Matcher>{CATCH_FORWARD(m)};
        }

    } // end namespace Matchers
} // end namespace Catch

#endif // CATCH_MATCHERS_CONTAINER_PROPERTIES_HPP_INCLUDED
