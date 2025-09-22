
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_LIST_HPP_INCLUDED
#define CATCH_LIST_HPP_INCLUDED

#include <set>
#include <string>
#include <string_view>


namespace Catch {

    class IEventListener;
    class Config;


    struct ReporterDescription {
        std::string name, description;
    };
    struct ListenerDescription {
        std::string_view name;
        std::string description;
    };

    struct TagInfo {
        void add(std::string_view spelling);
        std::string all() const;

        std::set<std::string_view> spellings;
        std::size_t count = 0;
    };

    bool list( IEventListener& reporter, Config const& config );

} // end namespace Catch

#endif // CATCH_LIST_HPP_INCLUDED
