/*
 *  Created by Phil on 14/02/2012.
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifdef __clang__
#pragma clang diagnostic ignored "-Wpadded"
#endif

#define CATCH_CONFIG_MAIN
#include "catch_self_test.hpp"

namespace Catch{
    
    NullStreamingReporter::~NullStreamingReporter() {}

    Totals EmbeddedRunner::runMatching( const std::string& rawTestSpec, const std::string& ) {
        std::ostringstream oss;
        Config config;
        config.setStreamBuf( oss.rdbuf() );
        
        Totals totals;

        // Scoped because Runner doesn't report EndTesting until its destructor
        {
            Runner runner( config, m_reporter.get() );
            totals = runner.runMatching( rawTestSpec );
        }
        return totals;
    }

}
