/*
 *  Created by Phil on 31/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED

#include "catch_commandline.h"
#include "catch_config.hpp"
#include "catch_text.h"

#include <memory>

namespace Catch {

    class Session : NonCopyable {
    public:

        Session();
        ~Session() override;

        void showHelp() const;
        void libIdentify();

        int applyCommandLine( int argc, char* argv[] );

        void useConfigData( ConfigData const& configData );

        int run( int argc, char* argv[] );
    #if defined(WIN32) && defined(UNICODE)
        int run( int argc, wchar_t* const argv[] );
    #endif
        int run();

        clara::Parser const& cli() const;
        void cli( clara::Parser const& newParser );
        ConfigData& configData();
        Config& config();
    private:
        int runInternal();

        clara::Parser m_cli;
        ConfigData m_configData;
        std::shared_ptr<Config> m_config;
        bool m_startupExceptions = false;
    };

} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RUNNER_HPP_INCLUDED
