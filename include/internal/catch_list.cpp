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

#include "catch_context.h"
#include "catch_stream.h"
#include "catch_text.h"

#include "catch_console_colour.h"
#include "catch_test_spec_parser.h"
#include "catch_tostring.h"
#include "catch_string_manip.h"

#include <limits>
#include <algorithm>
#include <set>

namespace Catch {
    namespace {

        void listTests(IStreamingReporter& reporter, Config const& config) {
            TestSpec testSpec = config.testSpec();
            auto matchedTestCases = filterTests(getAllTestCasesSorted(config), testSpec, config);
            reporter.listTests(matchedTestCases, config);
        }

        void listTags(IStreamingReporter& reporter, Config const& config) {
            TestSpec testSpec = config.testSpec();
            std::vector<TestCase> matchedTestCases = filterTests(getAllTestCasesSorted(config), testSpec, config);

            std::map<std::string, TagInfo> tagCounts;
            for (auto const& testCase : matchedTestCases) {
                for (auto const& tagName : testCase.getTestCaseInfo().tags) {
                    std::string lcaseTagName = toLower(tagName);
                    auto countIt = tagCounts.find(lcaseTagName);
                    if (countIt == tagCounts.end())
                        countIt = tagCounts.insert(std::make_pair(lcaseTagName, TagInfo())).first;
                    countIt->second.add(tagName);
                }
            }

            std::vector<TagInfo> infos; infos.reserve(tagCounts.size());
            for (auto const& tagc : tagCounts) {
                infos.push_back(std::move(tagc.second));
            }

            reporter.listTags(infos, config);
        }

        void listReporters(IStreamingReporter& reporter, Config const& config) {
            std::vector<ReporterDescription> descriptions;

            IReporterRegistry::FactoryMap const& factories = getRegistryHub().getReporterRegistry().getFactories();
            descriptions.reserve(factories.size());
            for (auto const& fac : factories) {
                descriptions.push_back({ fac.first, fac.second->getDescription() });
            }

            reporter.listReporters(descriptions, config);
        }

    } // end anonymous namespace

    void TagInfo::add( std::string const& spelling ) {
        ++count;
        spellings.insert( spelling );
    }

    std::string TagInfo::all() const {
        size_t size = 0;
        for (auto const& spelling : spellings) {
            // Add 2 for the brackes
            size += spelling.size() + 2;
        }

        std::string out; out.reserve(size);
        for (auto const& spelling : spellings) {
            out += '[';
            out += spelling;
            out += ']';
        }
        return out;
    }

    bool list( IStreamingReporter& reporter, std::shared_ptr<Config> const& config ) {
        bool listed = false;
        getCurrentMutableContext().setConfig( config );
        if (config->listTests()) {
            listed = true;
            listTests(reporter, *config);
        }
        if (config->listTags()) {
            listed = true;
            listTags(reporter, *config);
        }
        if (config->listReporters()) {
            listed = true;
            listReporters(reporter, *config);
        }
        return listed;
    }

} // end namespace Catch
