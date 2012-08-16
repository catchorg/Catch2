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
        m_output = oss.str();
        return totals;
    }
    
    void MockReporter::Result( const ResultInfo& resultInfo ) {
        if( resultInfo.getResultType() == ResultWas::Ok )
            return;
        
        switch( resultInfo.getResultType() ) {          
            case ResultWas::Info:
                m_log << "Info";
                break;
            case ResultWas::Warning:
                m_log << "Warning";
                break;
            case ResultWas::ExplicitFailure:
                m_log << "ExplicitFailure";
                break;
            case ResultWas::ExpressionFailed:
                m_log << "ExpressionFailed";
                break;
            case ResultWas::Unknown:
                m_log << "Unknown";
                break;
            case ResultWas::ThrewException:
                m_log << "ThrewException";
                break;
            case ResultWas::DidntThrowException:
                m_log << "DidntThrowException";
                break;

            // We shouldn't ever see these
            case ResultWas::Ok:
                m_log << "Ok";
                break;
            case ResultWas::FailureBit:
                m_log << "FailureBit";
                break;
            case ResultWas::Exception:
                m_log << "Exception";
                break;
        }
        
        if( resultInfo.hasExpression() )
            m_log << resultInfo.getExpression();
        
        if( resultInfo.hasMessage() )
            m_log << "'" << resultInfo.getMessage() << "'";
        
        if( resultInfo.hasExpandedExpression() )
            m_log << resultInfo.getExpandedExpression();        
    }    

    void MockReporter::openLabel( const std::string& label, const std::string& arg ) {
        if( shouldRecord( label ) ) {
            m_log << m_indent << "\\" << label;
            if( !arg.empty() )
                m_log << " " << arg;
            m_log << "\n";
            m_indent += " ";
        }
    }
    
    void MockReporter::closeLabel( const std::string& label, const std::string& arg ) {
        if( shouldRecord( label ) ) {
            m_indent = m_indent.substr( 0, m_indent.size()-1 );
            m_log << m_indent << "/" << label;
            if( !arg.empty() )
                m_log << " " << arg;
            m_log << "\n";
        }
    }
        
    const std::string MockReporter::recordGroups = "[g]";
    const std::string MockReporter::recordTestCases = "[tc]";
    const std::string MockReporter::recordSections =" [s]";
    
    INTERNAL_CATCH_REGISTER_REPORTER( "mock", MockReporter )
    
}
