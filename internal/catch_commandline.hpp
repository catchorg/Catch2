/*
 *  catch_commandline.hpp
 *  Catch
 *
 *  Created by Phil on 02/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED

#include "catch_runnerconfig.hpp"
#include "catch_runner_impl.hpp"

namespace Catch
{
    // -l --list:tests [xml] lists available tests (optionally in xml)
    // -l --list:reports [xml] lists available reports (optionally in xml)
    // -l --list:all [xml] lists available tests and reports (optionally in xml)
    // -t --test "testspec" (any number)
    // -r --report <type>
    // -o --output filename to write to
    // -s --success report successful cases too
    // -b --break breaks into debugger on test failure
	class ArgParser : NonCopyable
    {
        enum Mode
        {
            modeNone,
            modeList,
            modeTest,
            modeReport,
            modeOutput,
            modeSuccess,
            modeBreak,

            modeError
        };
        
    public:
        ArgParser( int argc, char * const argv[], RunnerConfig& config )
        :   m_mode( modeNone ),
            m_config( config )
        {
            for( int i=1; i < argc; ++i )
            {
                if( argv[i][0] == '-' )
                {
                    std::string cmd = ( argv[i] );
                    if( cmd == "-l" || cmd == "--list" )
                        changeMode( cmd, modeList );
                    else if( cmd == "-t" || cmd == "--test" )
                        changeMode( cmd, modeTest );
                    else if( cmd == "-r" || cmd == "--report" )
                        changeMode( cmd, modeReport );
                    else if( cmd == "-o" || cmd == "--output" )
                        changeMode( cmd, modeOutput );
                    else if( cmd == "-s" || cmd == "--success" )
                        changeMode( cmd, modeSuccess );
                    else if( cmd == "-b" || cmd == "--break" )
                        changeMode( cmd, modeBreak );
                }
                else
                {
                    m_args.push_back( argv[i] );
                }
                if( m_mode == modeError )
                    return;
            }
            changeMode( "", modeNone );            
        }
        
    private:
        std::string argsAsString()
        {
            std::ostringstream oss;
            std::vector<std::string>::const_iterator it = m_args.begin();
            std::vector<std::string>::const_iterator itEnd = m_args.end();
            for( bool first = true; it != itEnd; ++it, first = false )
            {
                if( !first )
                    oss << " ";
                oss << *it;
            }
            return oss.str();
        }
        
        void changeMode( const std::string& cmd, Mode mode )
        {
            m_command = cmd;
            switch( m_mode )
            {
                case modeNone:
                    if( m_args.size() > 0 )
                        return setErrorMode( "Unexpected arguments before " + m_command + ": " + argsAsString() );
                    break;
                case modeList:
                    if( m_args.size() > 2 )
                    {
                        return setErrorMode( m_command + " expected upto 2 arguments but recieved: " + argsAsString() );
                    }
                    else
                    {
                        RunnerConfig::ListInfo listSpec = RunnerConfig::listAll;
                        if( m_args.size() >= 1 )
                        {
                            if( m_args[0] == "tests" )
                                listSpec = RunnerConfig::listTests;
                            else if( m_args[0] == "reports" )
                                listSpec = RunnerConfig::listReports;
                            else
                                return setErrorMode( m_command + " expected [tests] or [reports] but recieved: [" + m_args[0] + "]" );                        
                        }
                        if( m_args.size() >= 2 )
                        {
                            if( m_args[1] == "xml" )
                                listSpec = (RunnerConfig::ListInfo)( listSpec | RunnerConfig::listAsXml );
                            else if( m_args[1] == "text" )
                                listSpec = (RunnerConfig::ListInfo)( listSpec | RunnerConfig::listAsText );
                            else
                                return setErrorMode( m_command + " expected [xml] or [text] but recieved: [" + m_args[1] + "]" );                        
                        }
                        m_config.m_listSpec = (RunnerConfig::ListInfo)( m_config.m_listSpec | listSpec );
                    }
                    break;
                case modeTest:
                    if( m_args.size() == 0 )                        
                        return setErrorMode( m_command + " expected at least 1 argument but recieved none" );
                    {
                        std::vector<std::string>::const_iterator it = m_args.begin();
                        std::vector<std::string>::const_iterator itEnd = m_args.end();
                        for(; it != itEnd; ++it )
                            m_config.addTestSpec( *it );
                    }
                    break;
                case modeReport:
                    if( m_args.size() != 1 )
                        return setErrorMode( m_command + " expected one argument, recieved: " +  argsAsString() );
                    m_config.setReporterInfo( m_args[0] );
                    break;
                case modeOutput:
                    if( m_args.size() == 0 )
                        return setErrorMode( m_command + " expected filename" );
                    m_config.setFilename( m_args[0] );
                    break;
                case modeSuccess:
                    if( m_args.size() != 0 )
                        return setErrorMode( m_command + " does not accept arguments" );
                    m_config.setIncludeAll( true );
                    break;
                case modeBreak:
                    if( m_args.size() != 0 )
                        return setErrorMode( m_command + " does not accept arguments" );
                    m_config.setShouldDebugBreak( true );
                    break;
            default:
                break;
            }
            m_args.clear();
            m_mode = mode;
        }
        
        void setErrorMode( const std::string& errorMessage )
        {
            m_mode = modeError;
            m_command = "";
            m_config.setError( errorMessage );
        }
        
    private:
        
        Mode m_mode;
        std::string m_command;
        std::vector<std::string> m_args;
        RunnerConfig& m_config;
    };
    
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED
