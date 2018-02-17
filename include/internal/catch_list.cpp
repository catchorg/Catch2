/*
 *  Created by Phil on 5/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_list.h"

#include "catch_interfaces_registry_hub.h"
#include "catch_interfaces_reporter.h"
#include "catch_interfaces_testcase.h"

#include "catch_stream.h"
#include "catch_text.h"

#include "catch_console_colour.h"
#include "catch_test_spec_parser.h"
#include "catch_tostring.h"
#include "catch_string_manip.h"

#include <limits>
#include <algorithm>
#include <iomanip>
#include <map>


namespace Catch {

    void TagInfo::add( std::string const& spelling ) {
        ++count;
        if (std::find(spellings.begin(), spellings.end(), spelling) == spellings.end()) {
            spellings.push_back(spelling);
        }
    }

    std::string TagInfo::tagsAsString() const {
        std::size_t finalSize = 0;
        for (auto const& sp : spellings) {
            finalSize += sp.size();
        }

        std::string out; out.reserve(finalSize);
        for (auto const& spelling : spellings) {
            out.push_back('[');
            out.append(spelling);
            out.push_back(']');
        }
        return out;
    }

    void listTags( IStreamingReporter& reporter, Config const& config ) {
        std::map<std::string, TagInfo> tagCounts;

        TestSpec testSpec = config.testSpec();
        std::vector<TestCase> matchedTestCases = filterTests( getAllTestCasesSorted( config ), testSpec, config );
        for( auto const& testCase : matchedTestCases ) {
            for( auto const& tagName : testCase.getTestCaseInfo().tags ) {
                std::string lcaseTagName = toLower( tagName );
                tagCounts[lcaseTagName].add(tagName);
            }
        }

        std::vector<TagInfo> tagInfo;
        for (auto const& tagc : tagCounts) {
            tagInfo.emplace_back(std::move(tagc.second));
        }

        reporter.listTags(tagInfo, config);
    }

    void listReporters( IStreamingReporter& reporter, Config const& config ) {
        std::vector<ReporterDescription> descriptions;

        IReporterRegistry::FactoryMap const& factories = getRegistryHub().getReporterRegistry().getFactories();
        descriptions.reserve(factories.size());
        for (const auto& fac : factories) {
            descriptions.push_back({ fac.first, fac.second->getDescription() });
        }

        reporter.listReporters(descriptions, config);
    }

    void listTests( IStreamingReporter& reporter, Config const& config ) {
        TestSpec testSpec = config.testSpec();
        auto matchedTestCases = filterTests(getAllTestCasesSorted(config), testSpec, config);
        reporter.listTests(matchedTestCases, config);
    }

    bool list( IStreamingReporter& reporter, Config const& config ) {
        bool listed = false;
        if (config.listTests()) {
            listTests(reporter, config);
            listed = true;
        }
        if (config.listTags()) {
            listTags(reporter, config);
            listed = true;
        }
        if (config.listReporters()) {
            listReporters(reporter, config);
            listed = true;
        }
        return listed;
    }

} // end namespace Catch
