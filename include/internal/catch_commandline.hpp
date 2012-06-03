/*
 *  Created by Phil on 02/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED

#include "catch_config.hpp"
#include "catch_runner_impl.hpp"

namespace Catch {

    class Command {
    public:
        Command(){}
        
        explicit Command( const std::string& name ) : m_name( name ) {}
                
        Command& operator += ( const std::string& arg ) {
            m_args.push_back( arg );
            return *this;
        }
        Command& operator += ( const Command& other ) {
            std::copy( other.m_args.begin(), other.m_args.end(), std::back_inserter( m_args ) );
            if( m_name.empty() )
                m_name = other.m_name;
            return *this;
        }
        Command operator + ( const Command& other ) {
            Command newCommand( *this );
            newCommand += other;
            return newCommand;
        }
        
        operator SafeBool::type() const {
            return SafeBool::makeSafe( !m_name.empty() );
        }
        
        std::string name() const { return m_name; }
        std::string operator[]( std::size_t i ) const { return m_args[i]; }
        std::size_t argsCount() const { return m_args.size(); }
        
        void raiseError( const std::string& message ) const {
            std::ostringstream oss;
            oss << "Error while parsing " << m_name << ". " << message << ".";
            if( m_args.size() > 0 )
                oss << " Arguments where:";
            for( std::size_t i = 0; i < m_args.size(); ++i )
                oss << " " << m_args[i];
            throw std::domain_error( oss.str() );
        }
        
    private:
        
        std::string m_name;
        std::vector<std::string> m_args;
    };
    
    class CommandParser {
    public:
        CommandParser( int argc, char const * const * argv ) : m_argc( static_cast<std::size_t>( argc ) ), m_argv( argv ) {}

        Command find( const std::string& arg1,  const std::string& arg2, const std::string& arg3 ) const {
            return find( arg1 ) + find( arg2 ) + find( arg3 );
        }

        Command find( const std::string& shortArg, const std::string& longArg ) const {
            return find( shortArg ) + find( longArg );
        }
        Command find( const std::string& arg ) const {
            for( std::size_t i = 0; i < m_argc; ++i  )
                if( m_argv[i] == arg )
                    return getArgs( i );
            return Command();
        }
        
    private:
        Command getArgs( std::size_t from ) const {
            Command command( m_argv[from] );
            for( std::size_t i = from+1; i < m_argc && m_argv[i][0] != '-'; ++i  )
                command += m_argv[i];
            return command;
        }
        
        std::size_t m_argc;
        char const * const * m_argv;
    };
    
    inline bool parseIntoConfig( const CommandParser& parser, Config& config ) {

        try {
            if( Command cmd = parser.find( "-l", "--list" ) ) {
                if( cmd.argsCount() > 2 )
                    cmd.raiseError( "Expected upto 2 arguments" );

                List::What listSpec = List::All;
                if( cmd.argsCount() >= 1 ) {
                    if( cmd[0] == "tests" )
                        listSpec = List::Tests;
                    else if( cmd[0] == "reporters" )
                        listSpec = List::Reports;
                    else
                        cmd.raiseError( "Expected [tests] or [reporters]" );
                }
                if( cmd.argsCount() >= 2 ) {
                    if( cmd[1] == "xml" )
                        listSpec = static_cast<List::What>( listSpec | List::AsXml );
                    else if( cmd[1] == "text" )
                        listSpec = static_cast<List::What>( listSpec | List::AsText );
                    else
                        cmd.raiseError( "Expected [xml] or [text]" );
                }
                config.setListSpec( static_cast<List::What>( config.getListSpec() | listSpec ) );
            }
                    
            if( Command cmd = parser.find( "-t", "--test" ) ) {
                if( cmd.argsCount() == 0 )
                    cmd.raiseError( "Expected at least one argument" );
                for( std::size_t i = 0; i < cmd.argsCount(); ++i )
                    config.addTestSpec( cmd[i] );
            }
            
            if( Command cmd = parser.find( "-r", "--reporter" ) ) {
                if( cmd.argsCount() != 1 )
                    cmd.raiseError( "Expected one argument" );
                config.setReporter( cmd[0] );
            }

            if( Command cmd = parser.find( "-o", "--out" ) ) {
                if( cmd.argsCount() == 0 )
                    cmd.raiseError( "Expected filename" );
                if( cmd[0][0] == '%' )
                    config.useStream( cmd[0].substr( 1 ) );
                else
                    config.setFilename( cmd[0] );
            }

            if( Command cmd = parser.find( "-s", "--success" ) ) {
                if( cmd.argsCount() != 0 )
                    cmd.raiseError( "Does not accept arguments" );
                config.setIncludeWhichResults( Include::SuccessfulResults );
            }
            
            if( Command cmd = parser.find( "-b", "--break" ) ) {
                if( cmd.argsCount() != 0 )
                    cmd.raiseError( "Does not accept arguments" );
                config.setShouldDebugBreak( true );
            }

            if( Command cmd = parser.find( "-n", "--name" ) ) {
                if( cmd.argsCount() != 1 )
                    cmd.raiseError( "Expected a name" );
                config.setName( cmd[0] );
            }
            
            if( Command cmd = parser.find( "-h", "-?", "--help" ) ) {
                if( cmd.argsCount() != 0 )
                    cmd.raiseError( "Does not accept arguments" );
                config.setShowHelp( true );
            }

            if( Command cmd = parser.find( "-a", "--abort" ) ) {
                if( cmd.argsCount() > 1 )
                    cmd.raiseError( "Only accepts 0-1 arguments" );
                int threshold = 1;
                if( cmd.argsCount() == 1 )
                {
                    std::stringstream ss;
                    ss << cmd[0];
                    ss >> threshold;
                }
                config.setCutoff( threshold );
            }
        }
        catch( std::exception& ex ) {
            config.setError( ex.what() );
            return false;
        }        
        return true;
    }
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_COMMANDLINE_HPP_INCLUDED
