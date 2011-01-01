/*
 *  catch_runnerconfig.hpp
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

#include <memory>
#include <vector>
#include <string>

namespace Catch
{
    class RunnerConfig : public IReporterConfig
    {
    private:
        RunnerConfig( const RunnerConfig& other );
        RunnerConfig& operator = ( const RunnerConfig& other );
    public:
        
        struct Include { enum What
        {
            FailedOnly, 
            SuccessfulResults
        }; };

        enum ListInfo
        {
            listNone = 0,
            
            listReports = 1,
            listTests = 2,
            listAll = 3,
            
            listWhatMask = 0xf,
            
            listAsText = 0x10,
            listAsXml = 0x11,
            
            listAsMask = 0xf0
        };
        
        
        RunnerConfig()
        :   m_reporter( NULL ),
            m_listSpec( listNone ),
            m_shouldDebugBreak( false ),
            m_showHelp( false ),
            m_os( std::cout.rdbuf() ),
            m_includeWhat( Include::FailedOnly )
        {}
        
        void setReporterInfo( const std::string& reporterName )
        {
            if( m_reporter.get() )
                return setError( "Only one reporter may be specified" );
            setReporter( Hub::getReporterRegistry().create( reporterName, *this ) );
        }
        
        void addTestSpec( const std::string& testSpec )
        {
            m_testSpecs.push_back( testSpec );
        }
        void setListSpec( ListInfo listSpec )
        {
            m_listSpec = listSpec;
        }
        
        void setFilename( const std::string& filename )
        {
            m_filename = filename;
        }
        
        std::string getFilename()
        {
            return m_filename;
        }
        
        void setError( const std::string& errorMessage )
        {
            m_message = errorMessage + "\n\n" + "Usage: ...";
        }
        
        void setReporter( IReporter* reporter )
        {
            m_reporter = std::auto_ptr<IReporter>( reporter );
        }
        
        IReporter* getReporter()
        {
            if( !m_reporter.get() )
                setReporter( Hub::getReporterRegistry().create( "basic", *this ) );
            return m_reporter.get();
        }
        
        IReporter* getReporter() const
        {
            return const_cast<RunnerConfig*>( this )->getReporter();
        }
        
        ListInfo listWhat() const
        {
            return (ListInfo)( m_listSpec & listWhatMask );
        }
        
        ListInfo listAs() const
        {
            return (ListInfo)( m_listSpec & listAsMask );
        }        
        
        void setIncludeWhat( Include::What includeWhat )
        {
            m_includeWhat = includeWhat;
        }
        void setShouldDebugBreak( bool shouldDebugBreak )
        {
            m_shouldDebugBreak = shouldDebugBreak;
        }
        bool shouldDebugBreak() const
        {
            return m_shouldDebugBreak;
        }
        void setShowHelp( bool showHelp )
        {
            m_showHelp = showHelp;
        }
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
            m_os.rdbuf( buf );
        }        

        ///////////////////////////////////////////////////////////////////////////
        virtual bool includeSuccessfulResults() const
        {
            return m_includeWhat == Include::SuccessfulResults;
        }
        
        
        
        std::auto_ptr<IReporter> m_reporter;
        std::string m_filename;
        std::string m_message;
        ListInfo m_listSpec;
        std::vector<std::string> m_testSpecs;
        bool m_shouldDebugBreak;
        bool m_showHelp;
        mutable std::ostream m_os;
        Include::What m_includeWhat;        
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RUNNERCONFIG_HPP_INCLUDED
