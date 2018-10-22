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

namespace Catch {

    std::size_t listTests( Config const& config ) {
        TestSpec testSpec = config.testSpec();
        if( config.hasTestFilters() )
            Catch::cout() << "Matching test cases:\n";
        else {
            Catch::cout() << "All available test cases:\n";
        }

        auto matchedTestCases = filterTests( getAllTestCasesSorted( config ), testSpec, config );
        for( auto const& testCaseInfo : matchedTestCases ) {
            Colour::Code colour = testCaseInfo.isHidden()
                ? Colour::SecondaryText
                : Colour::None;
            Colour colourGuard( colour );

            Catch::cout() << Column( testCaseInfo.name ).initialIndent( 2 ).indent( 4 ) << "\n";
            if( config.verbosity() >= Verbosity::High ) {
                Catch::cout() << Column( Catch::Detail::stringify( testCaseInfo.lineInfo ) ).indent(4) << std::endl;
                std::string description = testCaseInfo.description;
                if( description.empty() )
                    description = "(NO DESCRIPTION)";
                Catch::cout() << Column( description ).indent(4) << std::endl;
            }
            if( !testCaseInfo.tags.empty() )
                Catch::cout() << Column( testCaseInfo.tagsAsString() ).indent( 6 ) << "\n";
        }

        if( !config.hasTestFilters() )
            Catch::cout() << pluralise( matchedTestCases.size(), "test case" ) << '\n' << std::endl;
        else
            Catch::cout() << pluralise( matchedTestCases.size(), "matching test case" ) << '\n' << std::endl;
        return matchedTestCases.size();
    }

    std::size_t listTestsNamesOnly( Config const& config ) {
        TestSpec testSpec = config.testSpec();
        std::size_t matchedTests = 0;
        std::vector<TestCase> matchedTestCases = filterTests( getAllTestCasesSorted( config ), testSpec, config );
        for( auto const& testCaseInfo : matchedTestCases ) {
            matchedTests++;
            if( startsWith( testCaseInfo.name, '#' ) )
               Catch::cout() << '"' << testCaseInfo.name << '"';
            else
               Catch::cout() << testCaseInfo.name;
            if ( config.verbosity() >= Verbosity::High )
                Catch::cout() << "\t@" << testCaseInfo.lineInfo;
            Catch::cout() << std::endl;
        }
        return matchedTests;
    }

    void TagInfo::add( std::string const& spelling ) {
        ++count;
        spellings.insert( spelling );
    }

    std::string TagInfo::all() const {
        std::string out;
        for( auto const& spelling : spellings )
            out += "[" + spelling + "]";
        return out;
    }

    std::size_t listTags( Config const& config ) {
        TestSpec testSpec = config.testSpec();
        if( config.hasTestFilters() )
            Catch::cout() << "Tags for matching test cases:\n";
        else {
            Catch::cout() << "All available tags:\n";
        }

        std::map<std::string, TagInfo> tagCounts;

        std::vector<TestCase> matchedTestCases = filterTests( getAllTestCasesSorted( config ), testSpec, config );
        for( auto const& testCase : matchedTestCases ) {
            for( auto const& tagName : testCase.getTestCaseInfo().tags ) {
                std::string lcaseTagName = toLower( tagName );
                auto countIt = tagCounts.find( lcaseTagName );
                if( countIt == tagCounts.end() )
                    countIt = tagCounts.insert( std::make_pair( lcaseTagName, TagInfo() ) ).first;
                countIt->second.add( tagName );
            }
        }

        for( auto const& tagCount : tagCounts ) {
            ReusableStringStream rss;
            rss << "  " << std::setw(2) << tagCount.second.count << "  ";
            auto str = rss.str();
            auto wrapper = Column( tagCount.second.all() )
                                                    .initialIndent( 0 )
                                                    .indent( str.size() )
                                                    .width( CATCH_CONFIG_CONSOLE_WIDTH-10 );
            Catch::cout() << str << wrapper << '\n';
        }
        Catch::cout() << pluralise( tagCounts.size(), "tag" ) << '\n' << std::endl;
        return tagCounts.size();
    }

    std::size_t listReporters() {
        Catch::cout() << "Available reporters:\n";
        IReporterRegistry::FactoryMap const& factories = getRegistryHub().getReporterRegistry().getFactories();
        std::size_t maxNameLen = 0;
        for( auto const& factoryKvp : factories )
            maxNameLen = (std::max)( maxNameLen, factoryKvp.first.size() );

        for( auto const& factoryKvp : factories ) {
            Catch::cout()
                    << Column( factoryKvp.first + ":" )
                            .indent(2)
                            .width( 5+maxNameLen )
                    +  Column( factoryKvp.second->getDescription() )
                            .initialIndent(0)
                            .indent(2)
                            .width( CATCH_CONFIG_CONSOLE_WIDTH - maxNameLen-8 )
                    << "\n";
        }
        Catch::cout() << std::endl;
        return factories.size();
    }

    Option<std::size_t> list( Config const& config ) {
        Option<std::size_t> listedCount;
        if( config.listTests() )
            listedCount = listedCount.valueOr(0) + listTests( config );
        if( config.listTestNamesOnly() )
            listedCount = listedCount.valueOr(0) + listTestsNamesOnly( config );
        if( config.listTags() )
            listedCount = listedCount.valueOr(0) + listTags( config );
        if( config.listReporters() )
            listedCount = listedCount.valueOr(0) + listReporters();
        return listedCount;
    }

} // end namespace Catch
