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

#include <limits>
#include <algorithm>

namespace Catch {
    inline bool matchesFilters( std::vector<TestCaseFilters> const& filters, TestCase const& testCase ) {
        std::vector<TestCaseFilters>::const_iterator it = filters.begin();
        std::vector<TestCaseFilters>::const_iterator itEnd = filters.end();
        for(; it != itEnd; ++it )
            if( !it->shouldInclude( testCase ) )
                return false;
        return true;
    }

    inline std::size_t listTests( Config const& config ) {
        if( config.filters().empty() )
            std::cout << "All available test cases:\n";
        else
            std::cout << "Matching test cases:\n";
        std::vector<TestCase> const& allTests = getRegistryHub().getTestCaseRegistry().getAllTests();
        std::vector<TestCase>::const_iterator it = allTests.begin(), itEnd = allTests.end();

        // First pass - get max tags
        std::size_t maxTagLen = 0;
        std::size_t maxNameLen = 0;
        for(; it != itEnd; ++it ) {
            if( matchesFilters( config.filters(), *it ) ) {
                maxTagLen = (std::max)( it->getTestCaseInfo().tagsAsString.size(), maxTagLen );
                maxNameLen = (std::max)( it->getTestCaseInfo().name.size(), maxNameLen );
            }
        }

        // Try to fit everything in. If not shrink tag column first, down to 30
        // then shrink name column until it all fits (strings will be wrapped within column)
        while( maxTagLen + maxNameLen > CATCH_CONFIG_CONSOLE_WIDTH-5 ) {
            if( maxTagLen > 30 )
                --maxTagLen;
            else
                --maxNameLen;
        }

        std::size_t matchedTests = 0;
        for( it = allTests.begin(); it != itEnd; ++it ) {
            if( matchesFilters( config.filters(), *it ) ) {
                matchedTests++;
                Text nameWrapper(   it->getTestCaseInfo().name,
                                    TextAttributes()
                                        .setWidth( maxNameLen )
                                        .setInitialIndent(2)
                                        .setIndent(4) );

                Text tagsWrapper(   it->getTestCaseInfo().tagsAsString,
                                    TextAttributes()
                                        .setWidth( maxTagLen )
                                        .setInitialIndent(0)
                                        .setIndent( 2 ) );

                for( std::size_t i = 0; i < (std::max)( nameWrapper.size(), tagsWrapper.size() ); ++i ) {
                    Colour::Code colour = Colour::None;
                    if( it->getTestCaseInfo().isHidden )
                        colour = Colour::SecondaryText;
                    std::string nameCol;
                    if( i < nameWrapper.size() ) {
                        nameCol = nameWrapper[i];
                    }
                    else {
                        nameCol = "    ...";
                        colour = Colour::SecondaryText;
                    }

                    {
                        Colour colourGuard( colour );
                        std::cout << nameCol;
                    }
                    if( i < tagsWrapper.size() && !tagsWrapper[i].empty() ) {
                        if( i == 0 ) {
                            Colour colourGuard( Colour::SecondaryText );
                            std::cout << "  " << std::string( maxNameLen - nameCol.size(), '.' ) << "  ";
                        }
                        else {
                            std::cout << std::string( maxNameLen - nameCol.size(), ' ' ) << "    ";
                        }
                        std::cout << tagsWrapper[i];
                    }
                    std::cout << "\n";
                }
            }
        }
        if( config.filters().empty() )
            std::cout << pluralise( matchedTests, "test case" ) << "\n" << std::endl;
        else
            std::cout << pluralise( matchedTests, "matching test case" ) << "\n" << std::endl;
        return matchedTests;
    }

    inline std::size_t listTags( Config const& config ) {
        if( config.filters().empty() )
            std::cout << "All available tags:\n";
        else
            std::cout << "Matching tags:\n";
        std::vector<TestCase> const& allTests = getRegistryHub().getTestCaseRegistry().getAllTests();
        std::vector<TestCase>::const_iterator it = allTests.begin(), itEnd = allTests.end();

        std::map<std::string, int> tagCounts;

        std::size_t maxTagLen = 0;

        for(; it != itEnd; ++it ) {
            if( matchesFilters( config.filters(), *it ) ) {
                for( std::set<std::string>::const_iterator  tagIt = it->getTestCaseInfo().tags.begin(),
                                                            tagItEnd = it->getTestCaseInfo().tags.end();
                        tagIt != tagItEnd;
                        ++tagIt ) {
                    std::string tagName = *tagIt;
                    maxTagLen = (std::max)( maxTagLen, tagName.size() );
                    std::map<std::string, int>::iterator countIt = tagCounts.find( tagName );
                    if( countIt == tagCounts.end() )
                        tagCounts.insert( std::make_pair( tagName, 1 ) );
                    else
                        countIt->second++;
                }
            }
        }
        maxTagLen +=4;
        if( maxTagLen > CATCH_CONFIG_CONSOLE_WIDTH-10 )
            maxTagLen = CATCH_CONFIG_CONSOLE_WIDTH-10;

        for( std::map<std::string, int>::const_iterator countIt = tagCounts.begin(), countItEnd = tagCounts.end();
                countIt != countItEnd;
                ++countIt ) {
            Text wrapper( "[" + countIt->first + "]", TextAttributes()
                                                        .setIndent(2)
                                                        .setWidth( maxTagLen ) );
            std::cout << wrapper;
            std::size_t dots = 2;
            if( maxTagLen > wrapper.last().size() )
                dots += maxTagLen - wrapper.last().size();
            {
                Colour colourGuard( Colour::SecondaryText );
                std::cout << std::string( dots, '.' );
            }
            std::cout   << countIt->second
                        << "\n";
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
        if( config.listTags() )
            listedCount = listedCount.valueOr(0) + listTags( config );
        if( config.listReporters() )
            listedCount = listedCount.valueOr(0) + listReporters( config );
        return listedCount;
    }

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_LIST_HPP_INCLUDED
