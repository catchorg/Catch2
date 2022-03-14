
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE_1_0.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_INTERFACES_REPORTER_REGISTRY_HPP_INCLUDED
#define CATCH_INTERFACES_REPORTER_REGISTRY_HPP_INCLUDED

#include <catch2/internal/catch_case_insensitive_comparisons.hpp>
#include <catch2/internal/catch_unique_ptr.hpp>

#include <string>
#include <vector>
#include <map>

namespace Catch {

    struct IConfig;

    class IEventListener;
    using IStreamingReporterPtr = Detail::unique_ptr<IEventListener>;
    struct IReporterFactory;
    using IReporterFactoryPtr = Detail::unique_ptr<IReporterFactory>;
    struct ReporterConfig;
    class EventListenerFactory;

    struct IReporterRegistry {
        using FactoryMap = std::map<std::string, IReporterFactoryPtr, Detail::CaseInsensitiveLess>;
        using Listeners = std::vector<Detail::unique_ptr<EventListenerFactory>>;

        virtual ~IReporterRegistry(); // = default
        virtual IStreamingReporterPtr create( std::string const& name, ReporterConfig const& config ) const = 0;
        virtual FactoryMap const& getFactories() const = 0;
        virtual Listeners const& getListeners() const = 0;
    };

} // end namespace Catch

#endif // CATCH_INTERFACES_REPORTER_REGISTRY_HPP_INCLUDED
