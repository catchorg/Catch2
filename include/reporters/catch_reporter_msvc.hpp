/*
 *  Created by Phil on 28/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_REPORTER_MSVC_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_REPORTER_MSVC_HPP_INCLUDED

#include "../internal/catch_capture.hpp"
#include "../internal/catch_interfaces_reporter.h"
#include "../internal/catch_reporter_registrars.hpp"
#include "../internal/catch_console_colour.hpp"
#include "catch_reporter_basic.hpp"

namespace Catch {

    class MsvcReporter : public BasicReporter {

    public:
        MsvcReporter( const ReporterConfig& config )
        :    BasicReporter(config)
        {}

        static std::string getDescription() {
            return "Reports test results as lines of text formatted for Microsoft Visual Studio's output window";
        }

    private:

        virtual const char* GetErrorPrefix() const {
            return "error: ";
        }

        virtual const char* GetWarningPrefix() const {        
            return "warning: ";
        }
    };
        
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_REPORTER_MSVC_HPP_INCLUDED
