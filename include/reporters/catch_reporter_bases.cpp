/*
 *  Created by Phil on 27/11/2013.
 *  Copyright 2013 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "../internal/catch_interfaces_reporter.h"
#include "../internal/catch_errno_guard.h"
#include "catch_reporter_bases.hpp"

#include <cstring>
#include <cfloat>
#include <cstdio>
#include <cassert>
#include <memory>

namespace Catch {
    void prepareExpandedExpression(AssertionResult& result) {
        result.getExpandedExpression();
    }

    // Because formatting using c++ streams is stateful, drop down to C is required
    // Alternatively we could use stringstream, but its performance is... not good.
    std::string getFormattedDuration( double duration ) {
        // Max exponent + 1 is required to represent the whole part
        // + 1 for decimal point
        // + 3 for the 3 decimal places
        // + 1 for null terminator
        const std::size_t maxDoubleSize = DBL_MAX_10_EXP + 1 + 1 + 3 + 1;
        char buffer[maxDoubleSize];

        // Save previous errno, to prevent sprintf from overwriting it
        ErrnoGuard guard;
#ifdef _MSC_VER
        sprintf_s(buffer, "%.3f", duration);
#else
        std::sprintf(buffer, "%.3f", duration);
#endif
        return std::string(buffer);
    }

    std::string serializeFilters( std::vector<std::string> const& container ) {
        ReusableStringStream oss;
        bool first = true;
        for (auto&& filter : container)
        {
            if (!first)
                oss << ' ';
            else
                first = false;

            oss << filter;
        }
        return oss.str();
    }

    TestEventListenerBase::TestEventListenerBase(ReporterConfig const & _config)
        :StreamingReporterBase(_config) {}

    std::set<Verbosity> TestEventListenerBase::getSupportedVerbosities() {
        return { Verbosity::Quiet, Verbosity::Normal, Verbosity::High };
    }

    void TestEventListenerBase::assertionStarting(AssertionInfo const &) {}

    bool TestEventListenerBase::assertionEnded(AssertionStats const &) {
        return false;
    }


} // end namespace Catch
