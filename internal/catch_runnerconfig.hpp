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

#include "catch_reporter_registry.hpp"

#include <memory>
#include <vector>
#include <string>

namespace Catch
{
    class RunnerConfig
    {
    public:
        
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
            m_shouldDebugBreak( false )
        {}
        
        void setReporterInfo( const std::string& reporterName )
        {
            if( m_reporter.get() )
                return setError( "Only one reporter may be specified" );
            setReporter( ReporterRegistry::instance().create( reporterName, m_reporterConfig ) );
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
        
        void setReporter( ITestReporter* reporter )
        {
            m_reporter = std::auto_ptr<ITestReporter>( reporter );
        }
        
        ITestReporter* getReporter()
        {
            if( !m_reporter.get() )
                setReporter( ReporterRegistry::instance().create( "basic", m_reporterConfig ) );
            return m_reporter.get();
        }
        
        ITestReporter* getReporter() const
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
        
        ReporterConfig& getReporterConfig()
        {
            return m_reporterConfig;
        }
        void setIncludeAll( bool includeAll )
        {
            m_reporterConfig.setIncludeWhat( includeAll ? ReporterConfig::Include::SuccessfulResults : ReporterConfig::Include::FailedOnly );
        }
        void setShouldDebugBreak( bool shouldDebugBreak )
        {
            m_shouldDebugBreak = shouldDebugBreak;
        }
        bool shouldDebugBreak() const
        {
            return m_shouldDebugBreak;
        }
        
        std::auto_ptr<ITestReporter> m_reporter;
        std::string m_filename;
        ReporterConfig m_reporterConfig;
        std::string m_message;
        ListInfo m_listSpec;
        std::vector<std::string> m_testSpecs;
        bool m_shouldDebugBreak;
    };
    
} // end namespace Catch

#endif // TWOBLUECUBES_CATCH_RUNNERCONFIG_HPP_INCLUDED
