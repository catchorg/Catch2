/*
 *  Created by Phil on 5/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED

#include "catch_commandline.hpp"
#include "catch_text.h"
#include "catch_console_colour.hpp"
#include "catch_interfaces_reporter.h"

#include <limits>
#include <algorithm>

namespace Catch {

    inline std::size_t listTests( Config const& config ) {
        if( config.filters().empty() )
            std::cout << "All available test cases:\n";
        else
            std::cout << "Matching test cases:\n";

        std::size_t matchedTests = 0;
        TextAttributes nameAttr, tagsAttr;
        nameAttr.setInitialIndent( 2 ).setIndent( 4 );
        tagsAttr.setIndent( 6 );

        std::vector<TestCase> matchedTestCases;
        getRegistryHub().getTestCaseRegistry().getFilteredTests( config, matchedTestCases );
        for( std::vector<TestCase>::const_iterator it = matchedTestCases.begin(), itEnd = matchedTestCases.end();
                it != itEnd;
                ++it ) {
            matchedTests++;
            TestCaseInfo const& testCaseInfo = it->getTestCaseInfo();
            Colour::Code colour = testCaseInfo.isHidden
                ? Colour::SecondaryText
                : Colour::None;
            Colour colourGuard( colour );

            std::cout << Text( testCaseInfo.name, nameAttr ) << std::endl;
            if( !testCaseInfo.tags.empty() )
                std::cout << Text( testCaseInfo.tagsAsString, tagsAttr ) << std::endl;
        }

        if( config.filters().empty() )
            std::cout << pluralise( matchedTests, "test case" ) << "\n" << std::endl;
        else
            std::cout << pluralise( matchedTests, "matching test case" ) << "\n" << std::endl;
        return matchedTests;
    }

    inline std::size_t listTestsNamesOnly( Config const& config ) {
        std::size_t matchedTests = 0;
        std::vector<TestCase> matchedTestCases;
        getRegistryHub().getTestCaseRegistry().getFilteredTests( config, matchedTestCases );
        for( std::vector<TestCase>::const_iterator it = matchedTestCases.begin(), itEnd = matchedTestCases.end();
                it != itEnd;
                ++it ) {
            matchedTests++;
            TestCaseInfo const& testCaseInfo = it->getTestCaseInfo();
            std::cout << testCaseInfo.name << std::endl;
        }
        return matchedTests;
    }

    inline std::size_t listTags( Config const& config ) {
        if( config.filters().empty() )
            std::cout << "All available tags:\n";
        else
            std::cout << "Matching tags:\n";

        std::map<std::string, int> tagCounts;

        std::vector<TestCase> matchedTestCases;
        getRegistryHub().getTestCaseRegistry().getFilteredTests( config, matchedTestCases );
        for( std::vector<TestCase>::const_iterator it = matchedTestCases.begin(), itEnd = matchedTestCases.end();
                it != itEnd;
                ++it ) {
            for( std::set<std::string>::const_iterator  tagIt = it->getTestCaseInfo().tags.begin(),
                                                        tagItEnd = it->getTestCaseInfo().tags.end();
                    tagIt != tagItEnd;
                    ++tagIt ) {
                std::string tagName = *tagIt;
                std::map<std::string, int>::iterator countIt = tagCounts.find( tagName );
                if( countIt == tagCounts.end() )
                    tagCounts.insert( std::make_pair( tagName, 1 ) );
                else
                    countIt->second++;
            }
        }

        for( std::map<std::string, int>::const_iterator countIt = tagCounts.begin(),
                                                        countItEnd = tagCounts.end();
                countIt != countItEnd;
                ++countIt ) {
            std::ostringstream oss;
            oss << "  " << countIt->second << "  ";
            Text wrapper( "[" + countIt->first + "]", TextAttributes()
                                                        .setInitialIndent( 0 )
                                                        .setIndent( oss.str().size() )
                                                        .setWidth( CATCH_CONFIG_CONSOLE_WIDTH-10 ) );
            std::cout << oss.str() << wrapper << "\n";
        }
        std::cout << pluralise( tagCounts.size(), "tag" ) << "\n" << std::endl;
        return tagCounts.size();
    }

    inline std::size_t listReporters( Config const& /*config*/ ) {
        std::cout << "Available reports:\n";
        IReporterRegistry::FactoryMap const& factories = getRegistryHub().getReporterRegistry().getFactories();
        IReporterRegistry::FactoryMap::const_iterator itBegin = factories.begin(), itEnd = factories.end(), it;
        std::size_t maxNameLen = 0;
        for(it = itBegin; it != itEnd; ++it )
            maxNameLen = (std::max)( maxNameLen, it->first.size() );

        for(it = itBegin; it != itEnd; ++it ) {
            Text wrapper( it->second->getDescription(), TextAttributes()
                                                        .setInitialIndent( 0 )
                                                        .setIndent( 7+maxNameLen )
                                                        .setWidth( CATCH_CONFIG_CONSOLE_WIDTH - maxNameLen-8 ) );
            std::cout << "  "
                    << it->first
                    << ":"
                    << std::string( maxNameLen - it->first.size() + 2, ' ' )
                    << wrapper << "\n";
        }
        std::cout << std::endl;
        return factories.size();
    }

    inline Option<std::size_t> list( Config const& config ) {
        Option<std::size_t> listedCount;
        if( config.listTests() )
            listedCount = listedCount.valueOr(0) + listTests( config );
        if( config.listTestNamesOnly() )
            listedCount = listedCount.valueOr(0) + listTestsNamesOnly( config );
        if( config.listTags() )
            listedCount = listedCount.valueOr(0) + listTags( config );
        if( config.listReporters() )
            listedCount = listedCount.valueOr(0) + listReporters( config );
        return listedCount;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED
