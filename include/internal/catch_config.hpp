/*
 *  catch_config.hpp
 *  Catch
 *
 *  Created by Phil on 08/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef TWOBLUECUBES_CATCH_RUNNERCONFIG_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_RUNNERCONFIG_HPP_INCLUDED

#include "catch_interfaces_reporter.h"
#include "catch_hub.h"

#include <memory>
#include <vector>
#include <string>
#include <iostream>

namespace Catch
{
    
    class Config : public IReporterConfig
    {
    private:
        Config( const Config& other );
        Config& operator = ( const Config& other );
    public:
        
        struct Include { enum What
        {
            FailedOnly, 
            SuccessfulResults
        }; };

        struct List{ enum What
        {
            None = 0,
            
            Reports = 1,
            Tests = 2,
            All = 3,
            
            WhatMask = 0xf,
            
            AsText = 0x10,
            AsXml = 0x11,
            
            AsMask = 0xf0
        }; };
        
        
        ///////////////////////////////////////////////////////////////////////////
        Config()
        :   m_reporter( NULL ),
            m_listSpec( List::None ),
            m_shouldDebugBreak( false ),
            m_showHelp( false ),
            m_streambuf( NULL ),
            m_os( std::cout.rdbuf() ),
            m_includeWhat( Include::FailedOnly )
        {}
        
        ///////////////////////////////////////////////////////////////////////////
        ~Config()
        {
            m_os.rdbuf( std::cout.rdbuf() );
            delete m_streambuf;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void setReporter( const std::string& reporterName )
        {
            if( m_reporter.get() )
                return setError( "Only one reporter may be specified" );
            setReporter( Hub::getReporterRegistry().create( reporterName, *this ) );
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void addTestSpec( const std::string& testSpec )
        {
            m_testSpecs.push_back( testSpec );
        }
        
        ///////////////////////////////////////////////////////////////////////////
        bool testsSpecified() const
        {
            return !m_testSpecs.empty();
        }

        ///////////////////////////////////////////////////////////////////////////
        const std::vector<std::string>& getTestSpecs() const
        {
            return m_testSpecs;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        List::What getListSpec( void ) const
        {
            return m_listSpec;
        }

        ///////////////////////////////////////////////////////////////////////////
        void setListSpec( List::What listSpec )
        {
            m_listSpec = listSpec;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void setFilename( const std::string& filename )
        {
            m_filename = filename;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        const std::string& getFilename() const
        {
            return m_filename;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        const std::string& getMessage() const
        {
            return m_message;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void setError( const std::string& errorMessage )
        {
            m_message = errorMessage + "\n\n" + "Usage: ...";
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void setReporter( IReporter* reporter )
        {
            m_reporter = std::auto_ptr<IReporter>( reporter );
        }
        
        ///////////////////////////////////////////////////////////////////////////
        IReporter* getReporter() const
        {
            if( !m_reporter.get() )
                const_cast<Config*>( this )->setReporter( Hub::getReporterRegistry().create( "basic", *this ) );
            return m_reporter.get();
        }
                
        ///////////////////////////////////////////////////////////////////////////
        List::What listWhat() const
        {
            return static_cast<List::What>( m_listSpec & List::WhatMask );
        }        
        
        ///////////////////////////////////////////////////////////////////////////
        List::What listAs() const
        {
            return static_cast<List::What>( m_listSpec & List::AsMask );
        }        
        
        ///////////////////////////////////////////////////////////////////////////
        void setIncludeWhat( Include::What includeWhat )
        {
            m_includeWhat = includeWhat;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void setShouldDebugBreak( bool shouldDebugBreakFlag )
        {
            m_shouldDebugBreak = shouldDebugBreakFlag;
        }

        ///////////////////////////////////////////////////////////////////////////
        void setName( const std::string& name )
        {
            m_name = name;
        }

        ///////////////////////////////////////////////////////////////////////////
        std::string getName() const
        {
            return m_name;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        bool shouldDebugBreak() const
        {
            return m_shouldDebugBreak;
        }

        ///////////////////////////////////////////////////////////////////////////
        void setShowHelp( bool showHelpFlag )
        {
            m_showHelp = showHelpFlag;
        }

        ///////////////////////////////////////////////////////////////////////////
        bool showHelp() const
        {
            return m_showHelp;
        }

        ///////////////////////////////////////////////////////////////////////////
        virtual std::ostream& stream() const
        {
            return m_os;
        }
        
        ///////////////////////////////////////////////////////////////////////////
        void setStreamBuf( std::streambuf* buf )
        {
            m_os.rdbuf( buf ? buf : std::cout.rdbuf() );
        }        

        ///////////////////////////////////////////////////////////////////////////
        void useStream( const std::string& streamName )
        {
            std::streambuf* newBuf = Hub::createStreamBuf( streamName );
            setStreamBuf( newBuf );
            delete m_streambuf;
            m_streambuf = newBuf;
        }        
        
        ///////////////////////////////////////////////////////////////////////////
        virtual bool includeSuccessfulResults() const
        {
            return m_includeWhat == Include::SuccessfulResults;
        }
        
    private:
        std::auto_ptr<IReporter> m_reporter;
        std::string m_filename;
        std::string m_message;
        List::What m_listSpec;
        std::vector<std::string> m_testSpecs;
        bool m_shouldDebugBreak;
        bool m_showHelp;
        std::streambuf* m_streambuf;
        mutable std::ostream m_os;
        Include::What m_includeWhat;
        std::string m_name;
        
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RUNNERCONFIG_HPP_INCLUDED
