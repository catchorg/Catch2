/*
 *  Created by Phil on 14/02/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#define CATCH_CONFIG_MAIN
#include "catch_self_test.hpp"

namespace Catch{
    
    NullStreamingReporter::~NullStreamingReporter() {}

    Totals EmbeddedRunner::runMatching( const std::string& rawTestSpec, std::size_t groupIndex, std::size_t groupsCount, const std::string& ) {
        std::ostringstream oss;
        Ptr<Config> config = new Config();
        config->setStreamBuf( oss.rdbuf() );
        
        Totals totals;

        // Scoped because RunContext doesn't report EndTesting until its destructor
        {
            RunContext runner( config.get(), m_reporter.get() );
            totals = runner.runMatching( rawTestSpec, groupIndex, groupsCount );
        }
        return totals;
    }

}
