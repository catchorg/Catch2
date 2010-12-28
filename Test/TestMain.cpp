/*
 *  main.cpp
 *  Catch - Test
 *
 *  Created by Phil on 22/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "../catch.hpp"
#include "../catch_runner.hpp"

// This code runs the meta tests and verifies that the failing ones failed and the successful ones succeeded
///*
int main (int argc, char * const argv[])
{
    using namespace Catch;
    
    bool showAllResults = false;
    if( argc > 1 && ( std::string( argv[1] ) == "-s" || std::string( argv[1] ) == "--success" ) )
        showAllResults = true;
        
    std::ostringstream ossSucceeding;
    std::ostringstream ossFailing;

    RunnerConfig config;
    ReporterConfig& reporterConfig = config.getReporterConfig();
    config.setReporter( new BasicReporter(reporterConfig ) );
    reporterConfig.setIncludeWhat( ReporterConfig::Include::SuccessfulResults );
    Runner runner( config );

    reporterConfig.setStreamBuf( ossSucceeding.rdbuf() );
    runner.runMatching( "succeeding/*" );
    std::string succeedingResults = ossSucceeding.str();
    
    reporterConfig.setStreamBuf( ossFailing.rdbuf() );
    runner.runMatching( "failing/*" );
    std::string failingResults = ossFailing.str();

    int result = 0;
    if( succeedingResults.find( "failed" ) != std::string::npos )
    {
        std::cerr << "Some tests that should have succeeded failed:\n\n" << succeedingResults;
        result = 1;
    }
    else if( showAllResults )
    {
        std::cout << succeedingResults << "\n\n";
    }
    if( failingResults.find( "succeeded" ) != std::string::npos )
    {
        std::cerr << "Some tests that should have failed succeeded:\n\n" << failingResults;
        result = 1;
    }
    else if( showAllResults )
    {
        std::cout << failingResults << "\n\n";
    }

    if( result == 0 )
    {
        const size_t expectedTestCaseCount = 106; // !TBD factor this out
        size_t testCaseCount = runner.getSuccessCount() + runner.getFailureCount();
        std::cout << "All " << testCaseCount << " tests completed successfully" << std::endl;
        if( testCaseCount != expectedTestCaseCount )
        {
            std::cerr   << "- but we were expecting " << expectedTestCaseCount
                        << " test to run. Were some added or removed, or were they not compiled in?" 
                        << std::endl;
            return 1;
        }        
        
    }
    return result;
}
//*/
//#include "catch_default_main.hpp"