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
#include <iomanip>
#include <set>

namespace Catch {
    namespace {

        struct TagInfo {
            void add(std::string const& spelling);
            std::string all() const;

            std::set<std::string> spellings;
            std::size_t count = 0;
        };


        void listTests(Config const& config) {
            TestSpec testSpec = config.testSpec();
            if (config.hasTestFilters())
                Catch::cout() << "Matching test cases:\n";
            else {
                Catch::cout() << "All available test cases:\n";
            }

            auto matchedTestCases = filterTests(getAllTestCasesSorted(config), testSpec, config);
            for (auto const& testCaseInfo : matchedTestCases) {
                Colour::Code colour = testCaseInfo.isHidden()
                    ? Colour::SecondaryText
                    : Colour::None;
                Colour colourGuard(colour);

                Catch::cout() << Column(testCaseInfo.name).initialIndent(2).indent(4) << "\n";
                if (config.verbosity() >= Verbosity::High) {
                    Catch::cout() << Column(Catch::Detail::stringify(testCaseInfo.lineInfo)).indent(4) << std::endl;
                    std::string description = testCaseInfo.description;
                    if (description.empty())
                        description = "(NO DESCRIPTION)";
                    Catch::cout() << Column(description).indent(4) << std::endl;
                }
                if (!testCaseInfo.tags.empty())
                    Catch::cout() << Column(testCaseInfo.tagsAsString()).indent(6) << "\n";
            }

            if (!config.hasTestFilters())
                Catch::cout() << pluralise(matchedTestCases.size(), "test case") << '\n' << std::endl;
            else
                Catch::cout() << pluralise(matchedTestCases.size(), "matching test case") << '\n' << std::endl;
        }

        void listTags(Config const& config) {
            TestSpec testSpec = config.testSpec();
            if (config.hasTestFilters())
                Catch::cout() << "Tags for matching test cases:\n";
            else {
                Catch::cout() << "All available tags:\n";
            }

            std::map<std::string, TagInfo> tagCounts;

            std::vector<TestCase> matchedTestCases = filterTests(getAllTestCasesSorted(config), testSpec, config);
            for (auto const& testCase : matchedTestCases) {
                for (auto const& tagName : testCase.getTestCaseInfo().tags) {
                    std::string lcaseTagName = toLower(tagName);
                    auto countIt = tagCounts.find(lcaseTagName);
                    if (countIt == tagCounts.end())
                        countIt = tagCounts.insert(std::make_pair(lcaseTagName, TagInfo())).first;
                    countIt->second.add(tagName);
                }
            }

            for (auto const& tagCount : tagCounts) {
                ReusableStringStream rss;
                rss << "  " << std::setw(2) << tagCount.second.count << "  ";
                auto str = rss.str();
                auto wrapper = Column(tagCount.second.all())
                    .initialIndent(0)
                    .indent(str.size())
                    .width(CATCH_CONFIG_CONSOLE_WIDTH - 10);
                Catch::cout() << str << wrapper << '\n';
            }
            Catch::cout() << pluralise(tagCounts.size(), "tag") << '\n' << std::endl;
        }

        void listReporters() {
            Catch::cout() << "Available reporters:\n";
            IReporterRegistry::FactoryMap const& factories = getRegistryHub().getReporterRegistry().getFactories();
            std::size_t maxNameLen = 0;
            for (auto const& factoryKvp : factories)
                maxNameLen = (std::max)(maxNameLen, factoryKvp.first.size());

            for (auto const& factoryKvp : factories) {
                Catch::cout()
                    << Column(factoryKvp.first + ":")
                    .indent(2)
                    .width(5 + maxNameLen)
                    + Column(factoryKvp.second->getDescription())
                    .initialIndent(0)
                    .indent(2)
                    .width(CATCH_CONFIG_CONSOLE_WIDTH - maxNameLen - 8)
                    << "\n";
            }
            Catch::cout() << std::endl;
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

    bool list( std::shared_ptr<Config> const& config ) {
        bool listed = false;
        getCurrentMutableContext().setConfig( config );
        if (config->listTests()) {
            listed = true;
            listTests(*config);
        }
        if (config->listTags()) {
            listed = true;
            listTags(*config);
        }
        if (config->listReporters()) {
            listed = true;
            listReporters();
        }
        return listed;
    }

} // end namespace Catch
